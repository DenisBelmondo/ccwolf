// ID_VL.C

#include <string.h>

#include "wl_def.h"
#include "wl_draw.hpp"


bool fullscreen = true;

bool usedoublebuffering = true;
unsigned screenWidth = 640;
unsigned screenHeight = 400;
unsigned screenBits = -1; // use "best" color depth according to libSDL

SDL_Window *window;
static SDL_Renderer *renderer;
static SDL_Texture  *texture;

SDL_Surface *screen = NULL;
unsigned screenPitch;

SDL_Surface *screenBuffer = NULL;
unsigned bufferPitch;

SDL_Surface *curSurface = NULL;
unsigned curPitch;

unsigned scaleFactor;

bool screenfaded;
unsigned bordercolor;

SDL_Color palette1[256], palette2[256];
SDL_Color curpal[256];

#define CASSERT(x) extern int ASSERT_COMPILE[((x) != 0) * 2 - 1];
#define RGB(r, g, b)                                                                                                   \
    {                                                                                                                  \
        (r) * 255 / 63, (g)*255 / 63, (b)*255 / 63, 255                                                                \
    }

SDL_Color gamepal[] = {
#ifdef SPEAR
#include "sodpal.inc"
#else
#include "wolfpal.inc"
#endif
};

CASSERT(lengthof(gamepal) == 256)

//===========================================================================

/*
=======================
=
= VL_Shutdown
=
=======================
*/

void VL_Shutdown(void)
{
    // VL_SetTextMode ();
}

/*
=======================
=
= VL_SetVGAPlaneMode
=
=======================
*/

void VL_SetVGAPlaneMode(void)
{
    const char *title;
#ifdef SPEAR
    title = "Spear of Destiny";
#else
    title = "Wolfenstein 3D";
#endif

    SDL_DisplayMode dm;
    SDL_GetDesktopDisplayMode(0, &dm);

    screenWidth  = dm.w;
    screenHeight = dm.h;

    uint32_t window_flags = SDL_WINDOW_ALLOW_HIGHDPI;

    if (fullscreen)
    {
        window_flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
    }

    window = SDL_CreateWindow(
        title,
        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
        screenWidth, screenHeight,
        window_flags
    );

    if (!window)
    {
        printf("Unable to create %ix%ix%i window: %s\n", screenWidth, screenHeight, screenBits, SDL_GetError());
        exit(1);
    }

    renderer = SDL_CreateRenderer(window, -1, 0);

    if (!renderer)
    {
        printf("Unable to create renderer: %s\n", SDL_GetError());
        exit(1);
    }

    SDL_RendererInfo info;

    if (SDL_GetRendererInfo(renderer, &info) < 0)
    {
        printf("Unable to get renderer info: %s\n", SDL_GetError());
        exit(1);
    }

    for (Uint32 i = 0; i < info.num_texture_formats; ++i)
    {
        // TODO: debug this
        if (screenBits == -1)
        {
            screenBits = SDL_BITSPERPIXEL(info.texture_formats[i]);
            break;
        }
    }

    texture = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_ABGR8888,
        SDL_TEXTUREACCESS_STREAMING,
        screenWidth, screenHeight
    );

    if (!texture)
    {
        printf("Unable to create texture: %s\n", SDL_GetError());
        exit(1);
    }

    screen = SDL_CreateRGBSurface(0, screenWidth, screenHeight, 32, 0, 0, 0, 0);

    if (!screen)
    {
        printf("Unable to set %ix%ix%i video mode: %s\n", screenWidth, screenHeight, screenBits, SDL_GetError());
        exit(1);
    }

    SDL_ShowCursor(SDL_DISABLE);

    memcpy(curpal, gamepal, sizeof(SDL_Color) * 256);

    screenBuffer = SDL_CreateRGBSurface(
        SDL_SWSURFACE,
        screenWidth, screenHeight, 8, 0, 0, 0, 0
    );

    if (!screenBuffer)
    {
        printf("Unable to create screen buffer surface: %s\n", SDL_GetError());
        exit(1);
    }

    SDL_Palette *sdlpal = SDL_AllocPalette(256);

    if (!sdlpal)
    {
        printf("Unable to allocate palette.\n");
        exit(1);
    }

    if (SDL_SetPaletteColors(sdlpal, gamepal, 0, 256) < 0)
    {
        printf("Unable to set palette colors: %s\n", SDL_GetError());
        exit(1);
    }

    if (SDL_SetSurfacePalette(screenBuffer, sdlpal) < 0)
    {
        printf("Unable to set surface palette: %s\n", SDL_GetError());
        exit(1);
    }

    screenPitch = screen->pitch;
    bufferPitch = screenBuffer->pitch;

    curSurface = screenBuffer;
    curPitch   = bufferPitch;

    scaleFactor = screenWidth / 320;

    if (screenHeight / 200 < scaleFactor)
    {
        scaleFactor = screenHeight / 200;
    }

    pixelangle = (short *)malloc(screenWidth * sizeof(short));
    CHECKMALLOCRESULT(pixelangle);

    wallheight = (int *)malloc(screenWidth * sizeof(int));
    CHECKMALLOCRESULT(wallheight);
}

/*
=============================================================================

                        PALETTE OPS

        To avoid snow, do a WaitVBL BEFORE calling these

=============================================================================
*/

/*
=================
=
= VL_ConvertPalette
=
=================
*/

void VL_ConvertPalette(byte *srcpal, SDL_Color *destpal, int numColors)
{
    for (int i = 0; i < numColors; i++)
    {
        destpal[i].r = *srcpal++ * 255 / 63;
        destpal[i].g = *srcpal++ * 255 / 63;
        destpal[i].b = *srcpal++ * 255 / 63;
    }
}

/*
=================
=
= VL_FillPalette
=
=================
*/

void VL_FillPalette(int red, int green, int blue)
{
    int i;
    SDL_Color pal[256];

    for (i = 0; i < 256; i++)
    {
        pal[i].r = red;
        pal[i].g = green;
        pal[i].b = blue;
    }

    VL_SetPalette(pal, true);
}

//===========================================================================

/*
=================
=
= VL_GetColor
=
=================
*/

void VL_GetColor(int color, int *red, int *green, int *blue)
{
    SDL_Color *col = &curpal[color];
    *red = col->r;
    *green = col->g;
    *blue = col->b;
}

//===========================================================================

/*
 =================
 =
 = VL_Flip
 = For SDL2
 =
 =================
 */

// IOANCH: major thanks to http://sandervanderburg.blogspot.ro/2014/05/rendering-8-bit-palettized-surfaces-in.html
void VL_Flip()
{
    void *pixels;
    int pitch;
    SDL_LockTexture(texture, NULL, &pixels, &pitch);
    SDL_ConvertPixels(screen->w, screen->h, screen->format->format, screen->pixels, screen->pitch,
                      SDL_PIXELFORMAT_ABGR8888, pixels, pitch);

    SDL_UnlockTexture(texture);

    SDL_RenderCopy(renderer, texture, NULL, NULL);
    SDL_RenderPresent(renderer);
}

//===========================================================================

/*
=================
=
= VL_SetPalette
=
=================
*/

void VL_SetPalette(SDL_Color *palette, bool forceupdate)
{
    memcpy(curpal, palette, sizeof(SDL_Color) * 256);

    SDL_Palette *pal = SDL_AllocPalette(256);
    SDL_SetPaletteColors(pal, curpal, 0, 256);
    SDL_SetSurfacePalette(curSurface, pal);
    SDL_FreePalette(pal);

    // This trick is used due to a SDL 2.0.3 bug, to invalidate the palette
    // cache blit map.
    SDL_SetSurfaceRLE(curSurface, 1);
    SDL_SetSurfaceRLE(curSurface, 0);

    if (forceupdate)
    {
        SDL_BlitSurface(curSurface, NULL, screen, NULL);
        VL_Flip();
    }
}

//===========================================================================

/*
=================
=
= VL_GetPalette
=
=================
*/

void VL_GetPalette(SDL_Color *palette)
{
    memcpy(palette, curpal, sizeof(SDL_Color) * 256);
}

//===========================================================================

/*
=================
=
= VL_FadeOut
=
= Fades the current palette to the given color in the given number of steps
=
=================
*/

void VL_FadeOut(int start, int end, int red, int green, int blue, int steps)
{
    int i, j, orig, delta;
    SDL_Color *origptr, *newptr;

    red = red * 255 / 63;
    green = green * 255 / 63;
    blue = blue * 255 / 63;

    VL_WaitVBL(1);
    VL_GetPalette(palette1);
    memcpy(palette2, palette1, sizeof(SDL_Color) * 256);

    //
    // fade through intermediate frames
    //
    for (i = 0; i < steps; i++)
    {
        origptr = &palette1[start];
        newptr = &palette2[start];
        for (j = start; j <= end; j++)
        {
            orig = origptr->r;
            delta = red - orig;
            newptr->r = orig + delta * i / steps;
            orig = origptr->g;
            delta = green - orig;
            newptr->g = orig + delta * i / steps;
            orig = origptr->b;
            delta = blue - orig;
            newptr->b = orig + delta * i / steps;
            origptr++;
            newptr++;
        }

        if (!usedoublebuffering || screenBits == 8)
            VL_WaitVBL(1);
        VL_SetPalette(palette2, true);
    }

    //
    // final color
    //
    VL_FillPalette(red, green, blue);

    screenfaded = true;
}

/*
=================
=
= VL_FadeIn
=
=================
*/

void VL_FadeIn(int start, int end, SDL_Color *palette, int steps)
{
    int i, j, delta;

    VL_WaitVBL(1);
    VL_GetPalette(palette1);
    memcpy(palette2, palette1, sizeof(SDL_Color) * 256);

    //
    // fade through intermediate frames
    //
    for (i = 0; i < steps; i++)
    {
        for (j = start; j <= end; j++)
        {
            delta = palette[j].r - palette1[j].r;
            palette2[j].r = palette1[j].r + delta * i / steps;
            delta = palette[j].g - palette1[j].g;
            palette2[j].g = palette1[j].g + delta * i / steps;
            delta = palette[j].b - palette1[j].b;
            palette2[j].b = palette1[j].b + delta * i / steps;
        }

        if (!usedoublebuffering || screenBits == 8)
            VL_WaitVBL(1);
        VL_SetPalette(palette2, true);
    }

    //
    // final color
    //
    VL_SetPalette(palette, true);
    screenfaded = false;
}

/*
=============================================================================

                            PIXEL OPS

=============================================================================
*/

byte *VL_LockSurface(SDL_Surface *surface)
{
    if (SDL_MUSTLOCK(surface))
    {
        if (SDL_LockSurface(surface) < 0)
            return NULL;
    }
    return (byte *)surface->pixels;
}

void VL_UnlockSurface(SDL_Surface *surface)
{
    if (SDL_MUSTLOCK(surface))
    {
        SDL_UnlockSurface(surface);
    }
}

/*
=================
=
= VL_Plot
=
=================
*/

void VL_Plot(int x, int y, int color)
{
    byte *ptr;

    assert(x >= 0 && (unsigned)x < screenWidth && y >= 0 && (unsigned)y < screenHeight &&
           "VL_Plot: Pixel out of bounds!");

    ptr = VL_LockSurface(curSurface);
    if (ptr == NULL)
        return;

    ptr[y * curPitch + x] = color;

    VL_UnlockSurface(curSurface);
}

/*
=================
=
= VL_GetPixel
=
=================
*/

byte VL_GetPixel(int x, int y)
{
    byte *ptr;
    byte col;

    ptr = VL_LockSurface(curSurface);
    if (ptr == NULL)
        return 0;

    col = ((byte *)curSurface->pixels)[y * curPitch + x];

    VL_UnlockSurface(curSurface);

    return col;
}

/*
=================
=
= VL_Hlin
=
=================
*/

void VL_Hlin(unsigned x, unsigned y, unsigned width, int color)
{
    byte *ptr;

    assert(x >= 0 && x + width <= screenWidth && y >= 0 && y < screenHeight &&
           "VL_Hlin: Destination rectangle out of bounds!");

    ptr = VL_LockSurface(curSurface);
    if (ptr == NULL)
        return;

    memset(ptr + y * curPitch + x, color, width);

    VL_UnlockSurface(curSurface);
}

/*
=================
=
= VL_Vlin
=
=================
*/

void VL_Vlin(int x, int y, int height, int color)
{
    byte *ptr;

    assert(x >= 0 && (unsigned)x < screenWidth && y >= 0 && (unsigned)y + height <= screenHeight &&
           "VL_Vlin: Destination rectangle out of bounds!");

    ptr = VL_LockSurface(curSurface);
    if (ptr == NULL)
        return;

    ptr += y * curPitch + x;

    while (height--)
    {
        *ptr = color;
        ptr += curPitch;
    }

    VL_UnlockSurface(curSurface);
}

/*
=================
=
= VL_Bar
=
=================
*/

void VL_BarScaledCoord(int scx, int scy, int scwidth, int scheight, int color)
{
    byte *ptr;

    assert(scx >= 0 && (unsigned)scx + scwidth <= screenWidth && scy >= 0 && (unsigned)scy + scheight <= screenHeight &&
           "VL_BarScaledCoord: Destination rectangle out of bounds!");

    ptr = VL_LockSurface(curSurface);
    if (ptr == NULL)
        return;

    ptr += scy * curPitch + scx;

    while (scheight--)
    {
        memset(ptr, color, scwidth);
        ptr += curPitch;
    }
    VL_UnlockSurface(curSurface);
}

/*
============================================================================

                            MEMORY OPS

============================================================================
*/

/*
=================
=
= VL_MemToLatch
=
=================
*/

void VL_MemToLatch(byte *source, int width, int height, SDL_Surface *destSurface, int x, int y)
{
    byte *ptr;
    int xsrc, ysrc, pitch;

    assert(x >= 0 && (unsigned)x + width <= screenWidth && y >= 0 && (unsigned)y + height <= screenHeight &&
           "VL_MemToLatch: Destination rectangle out of bounds!");

    ptr = VL_LockSurface(destSurface);
    if (ptr == NULL)
        return;

    pitch = destSurface->pitch;
    ptr += y * pitch + x;
    for (ysrc = 0; ysrc < height; ysrc++)
    {
        for (xsrc = 0; xsrc < width; xsrc++)
        {
            ptr[ysrc * pitch + xsrc] = source[(ysrc * (width >> 2) + (xsrc >> 2)) + (xsrc & 3) * (width >> 2) * height];
        }
    }
    VL_UnlockSurface(destSurface);
}

//===========================================================================

/*
=================
=
= VL_MemToScreenScaledCoord
=
= Draws a block of data to the screen with scaling according to scaleFactor.
=
=================
*/

void VL_MemToScreenScaledCoord(byte *source, int width, int height, int destx, int desty)
{
    byte *ptr;
    int i, j, sci, scj;
    unsigned m, n;

    assert(destx >= 0 && destx + width * scaleFactor <= screenWidth && desty >= 0 &&
           desty + height * scaleFactor <= screenHeight &&
           "VL_MemToScreenScaledCoord: Destination rectangle out of bounds!");

    ptr = VL_LockSurface(curSurface);
    if (ptr == NULL)
        return;

    for (j = 0, scj = 0; j < height; j++, scj += scaleFactor)
    {
        for (i = 0, sci = 0; i < width; i++, sci += scaleFactor)
        {
            byte col = source[(j * (width >> 2) + (i >> 2)) + (i & 3) * (width >> 2) * height];
            for (m = 0; m < scaleFactor; m++)
            {
                for (n = 0; n < scaleFactor; n++)
                {
                    ptr[(scj + m + desty) * curPitch + sci + n + destx] = col;
                }
            }
        }
    }
    VL_UnlockSurface(curSurface);
}

/*
=================
=
= VL_MemToScreenScaledCoord
=
= Draws a part of a block of data to the screen.
= The block has the size origwidth*origheight.
= The part at (srcx, srcy) has the size width*height
= and will be painted to (destx, desty) with scaling according to scaleFactor.
=
=================
*/

void VL_MemToScreenScaledCoord(byte *source, int origwidth, int origheight, int srcx, int srcy, int destx, int desty,
                               int width, int height)
{
    byte *ptr;
    int i, j, sci, scj;
    unsigned m, n;

    assert(destx >= 0 && destx + width * scaleFactor <= screenWidth && desty >= 0 &&
           desty + height * scaleFactor <= screenHeight &&
           "VL_MemToScreenScaledCoord: Destination rectangle out of bounds!");

    ptr = VL_LockSurface(curSurface);
    if (ptr == NULL)
        return;

    for (j = 0, scj = 0; j < height; j++, scj += scaleFactor)
    {
        for (i = 0, sci = 0; i < width; i++, sci += scaleFactor)
        {
            byte col = source[((j + srcy) * (origwidth >> 2) + ((i + srcx) >> 2)) +
                              ((i + srcx) & 3) * (origwidth >> 2) * origheight];

            for (m = 0; m < scaleFactor; m++)
            {
                for (n = 0; n < scaleFactor; n++)
                {
                    ptr[(scj + m + desty) * curPitch + sci + n + destx] = col;
                }
            }
        }
    }
    VL_UnlockSurface(curSurface);
}

//==========================================================================

/*
=================
=
= VL_LatchToScreen
=
=================
*/

void VL_LatchToScreenScaledCoord(SDL_Surface *source, int xsrc, int ysrc, int width, int height, int scxdest,
                                 int scydest)
{
    assert(scxdest >= 0 && scxdest + width * scaleFactor <= screenWidth && scydest >= 0 &&
           scydest + height * scaleFactor <= screenHeight &&
           "VL_LatchToScreenScaledCoord: Destination rectangle out of bounds!");

    if (scaleFactor == 1)
    {
        // HACK: If screenBits is not 8 and the screen is faded out, the
        //       result will be black when using SDL_BlitSurface. The reason
        //       is that the logical palette needed for the transformation
        //       to the screen color depth is not equal to the logical
        //       palette of the latch (the latch is not faded). Therefore,
        //       SDL tries to map the colors...
        //       The result: All colors are mapped to black.
        //       So, we do the blit on our own...
        if (screenBits != 8)
        {
            byte *src, *dest;
            unsigned srcPitch;
            int i, j;

            src = VL_LockSurface(source);
            if (src == NULL)
                return;

            srcPitch = source->pitch;

            dest = VL_LockSurface(curSurface);
            if (dest == NULL)
                return;

            for (j = 0; j < height; j++)
            {
                for (i = 0; i < width; i++)
                {
                    byte col = src[(ysrc + j) * srcPitch + xsrc + i];
                    dest[(scydest + j) * curPitch + scxdest + i] = col;
                }
            }
            VL_UnlockSurface(curSurface);
            VL_UnlockSurface(source);
        }
        else
        {
            SDL_Rect srcrect = {xsrc, ysrc, width, height};
            SDL_Rect destrect = {scxdest, scydest, 0, 0}; // width and height are ignored
            SDL_BlitSurface(source, &srcrect, curSurface, &destrect);
        }
    }
    else
    {
        byte *src, *dest;
        unsigned srcPitch;
        int i, j, sci, scj;
        unsigned m, n;

        src = VL_LockSurface(source);
        if (src == NULL)
            return;

        srcPitch = source->pitch;

        dest = VL_LockSurface(curSurface);
        if (dest == NULL)
            return;

        for (j = 0, scj = 0; j < height; j++, scj += scaleFactor)
        {
            for (i = 0, sci = 0; i < width; i++, sci += scaleFactor)
            {
                byte col = src[(ysrc + j) * srcPitch + xsrc + i];
                for (m = 0; m < scaleFactor; m++)
                {
                    for (n = 0; n < scaleFactor; n++)
                    {
                        dest[(scydest + scj + m) * curPitch + scxdest + sci + n] = col;
                    }
                }
            }
        }
        VL_UnlockSurface(curSurface);
        VL_UnlockSurface(source);
    }
}

//===========================================================================

/*
=================
=
= VL_ScreenToScreen
=
=================
*/

void VL_ScreenToScreen(SDL_Surface *source, SDL_Surface *dest)
{
    SDL_BlitSurface(source, NULL, dest, NULL);
}
