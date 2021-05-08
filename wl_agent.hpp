#ifndef WL_AGENT_H
#define WL_AGENT_H

#include "wl_def.h"

extern short anglefrac;
extern int facecount, facetimes;
extern word plux, pluy; // player coordinates scaled to unsigned
extern int32_t thrustspeed;
extern objtype *LastAttacker;

void Thrust(int angle, int32_t speed);
void SpawnPlayer(int tilex, int tiley, int dir);
void TakeDamage(int points, objtype *attacker);
void GivePoints(int32_t points);
void GetBonus(statobj_t *check);
void GiveWeapon(int weapon);
void GiveAmmo(int ammo);
void GiveKey(int key);

//
// player state info
//

void StatusDrawFace(unsigned picnum);
void DrawFace(void);
void DrawHealth(void);
void HealSelf(int points);
void DrawLevel(void);
void DrawLives(void);
void GiveExtraMan(void);
void DrawScore(void);
void DrawWeapon(void);
void DrawKeys(void);
void DrawAmmo(void);

#endif // WL_AGENT_H
