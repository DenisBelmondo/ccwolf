#ifndef WL_DRAW_H
#define WL_DRAW_H

//
// math tables
//
extern short *pixelangle;
extern int32_t finetangent[FINEANGLES / 4];
extern fixed sintable[];
extern fixed *costable;
extern int *wallheight;
extern word horizwall[], vertwall[];
extern int32_t lasttimecount;
extern int32_t frameon;

extern unsigned screenloc[3];

extern bool fizzlein, fpscounter;

extern fixed viewx, viewy; // the focal point
extern fixed viewsin, viewcos;

void ThreeDRefresh(void);
void CalcTics(void);

struct t_compshape
{
    word leftpix, rightpix;
    word dataofs[64];
    // table data after dataofs[rightpix-leftpix+1]
};

#endif // WL_DRAW_H
