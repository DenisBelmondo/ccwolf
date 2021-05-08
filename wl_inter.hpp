#ifndef WL_INTER_H
#define WL_INTER_H

#include <stdint.h>

#include "cc_types.hpp"
using namespace CcWolf::Types;

void IntroScreen(void);
void PG13(void);
void DrawHighScores(void);
void CheckHighScore(int32_t score, word other);
void Victory(void);
void LevelCompleted(void);
void ClearSplitVWB(void);

void PreloadGraphics(void);

#endif // WL_INTER_H
