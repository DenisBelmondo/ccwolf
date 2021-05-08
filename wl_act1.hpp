#ifndef WL_ACT1_H
#define WL_ACT1_H

#include "wl_def.h"

extern doorobj_t doorobjlist[MAXDOORS];
extern doorobj_t *lastdoorobj;
extern short doornum;

extern word doorposition[MAXDOORS];

extern byte areaconnect[NUMAREAS][NUMAREAS];

extern boolean areabyplayer[NUMAREAS];

extern word pwallstate;
extern word pwallpos; // amount a pushable wall has been moved (0-63)
extern word pwallx, pwally;
extern byte pwalldir, pwalltile;

void InitDoorList(void);
void InitStaticList(void);
void SpawnStatic(int tilex, int tiley, int type);
void SpawnDoor(int tilex, int tiley, boolean vertical, int lock);
void MoveDoors(void);
void MovePWalls(void);
void OpenDoor(int door);
void PlaceItemType(int itemtype, int tilex, int tiley);
void PushWall(int checkx, int checky, int dir);
void OperateDoor(int door);
void InitAreas(void);

#endif // WL_ACT1_H
