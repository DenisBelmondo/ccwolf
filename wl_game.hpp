#ifndef WL_GAME_H
#define WL_GAME_H

#include <SDL.h>

#include "wl_def.h"

#include "cc_types.hpp"
using namespace CcWolf::Types;

extern Game gamestate;
extern byte bordercol;
extern SDL_Surface *latchpics[NUMLATCHPICS];
extern char demoname[13];

void SetupGameLevel(void);
void GameLoop(void);
void DrawPlayBorder(void);
void DrawStatusBorder(byte color);
void DrawPlayScreen(void);
void DrawPlayBorderSides(void);
void ShowActStatus();

void PlayDemo(int demonumber);
void RecordDemo(void);

#ifdef SPEAR
extern int32_t spearx, speary;
extern unsigned spearangle;
extern bool spearflag;
#endif

#define ClearMemory SD_StopDigitized

// JAB
#define PlaySoundLocTile(s, tx, ty)                                                                                    \
    PlaySoundLocGlobal(s, (((int32_t)(tx) << TILESHIFT) + (1L << (TILESHIFT - 1))),                                    \
                       (((int32_t)ty << TILESHIFT) + (1L << (TILESHIFT - 1))))
#define PlaySoundLocActor(s, ob) PlaySoundLocGlobal(s, (ob)->x, (ob)->y)
void PlaySoundLocGlobal(word s, fixed gx, fixed gy);
void UpdateSoundLoc(void);

#endif // WL_GAME_H
