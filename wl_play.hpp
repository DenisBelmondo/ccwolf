#ifndef WL_PLAY_H
#define WL_PLAY_H

#include "wl_def.h"

#include "cc_types.hpp"
using namespace CcWolf::Types;

#define BASEMOVE 35
#define RUNMOVE 70
#define BASETURN 35
#define RUNTURN 70

#define JOYSCALE 2

extern byte tilemap[MAPSIZE][MAPSIZE]; // wall values only
extern byte spotvis[MAPSIZE][MAPSIZE];
extern objtype *actorat[MAPSIZE][MAPSIZE];

extern objtype *player;

extern unsigned tics;
extern int viewsize;

extern int lastgamemusicoffset;

//
// current user input
//
extern int controlx, controly; // range from -100 to 100
extern bool buttonstate[NUMBUTTONS];
extern objtype objlist[MAXACTORS];
extern bool buttonheld[NUMBUTTONS];
extern ExitType playstate;
extern bool madenoise;
extern statobj_t statobjlist[MAXSTATS];
extern statobj_t *laststatobj;
extern objtype *newobj, *killerobj;
extern doorobj_t doorobjlist[MAXDOORS];
extern doorobj_t *lastdoorobj;
extern int godmode;

extern bool demorecord, demoplayback;
extern int8_t *demoptr, *lastdemoptr;
extern memptr demobuffer;

//
// control info
//
extern bool mouseenabled, joystickenabled;
extern int dirscan[4];
extern int buttonscan[NUMBUTTONS];
extern int buttonmouse[4];
extern int buttonjoy[32];

void InitActorList(void);
void GetNewActor(void);
void PlayLoop(void);

void CenterWindow(word w, word h);

void InitRedShifts(void);
void FinishPaletteShifts(void);

void RemoveObj(objtype *gone);
void PollControls(void);
int StopMusic(void);
void StartMusic(void);
void ContinueMusic(int offs);
void StartDamageFlash(int damage);
void StartBonusFlash(void);

#ifdef SPEAR
extern int32_t funnyticount; // FOR FUNNY BJ FACE
#endif

extern objtype *objfreelist; // *obj,*player,*lastobj,

extern bool noclip, ammocheat;
extern int singlestep, extravbls;

#endif // WL_PLAY_H
