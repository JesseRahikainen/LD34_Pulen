#ifndef VALUES_H
#define VALUES_H

#include "fighter.h"

extern float maxHealthByRank[];
extern float damageByRank[];
extern float speedByRank[];
extern float maxStaminaByRank[];
extern int advanceCostByRank[];

extern const float PUNCH_COST;

extern const int ALWAYS_INTERRUPT;
extern const int ACTION_INTERRUPT;
extern const int REACTION_INTERRUPT;
extern const int STUNNED_INTERRUPT;
extern const int KO_INTERRUPT;

extern const Color TEXT_COLOR;
extern const Color TEXT_ON_COLOR;
extern const Color TEXT_OFF_COLOR;
extern const Color BG_COLOR;

/* Fighter Stats */
extern FighterTemplate opponentTemplates[];
extern FighterAITemplate opponentAITemplates[];
extern FighterTemplate playerFT;
extern int currentOpponent;
extern const int MAX_OPPONENTS;

void val_ResetPlayer( void );

#endif