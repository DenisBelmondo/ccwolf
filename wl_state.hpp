#ifndef WL_STATE_H
#define WL_STATE_H

#include "wl_def.h"

#include "cc_types.hpp"
using namespace CcWolf::Types;

#define TURNTICS 10
#define SPDPATROL 512
#define SPDDOG 1500

void InitHitRect(objtype *ob, unsigned radius);
void SpawnNewObj(unsigned tilex, unsigned tiley, statetype *state);
void NewState(objtype *ob, statetype *state);

bool TryWalk(objtype *ob);
void SelectChaseDir(objtype *ob);
void SelectDodgeDir(objtype *ob);
void SelectRunDir(objtype *ob);
void MoveObj(objtype *ob, int32_t move);
bool SightPlayer(objtype *ob);

void KillActor(objtype *ob);
void DamageActor(objtype *ob, unsigned damage);

bool CheckLine(objtype *ob);
bool CheckSight(objtype *ob);

#endif // WL_STATE_H
