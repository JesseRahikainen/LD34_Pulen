#include "values.h"

float maxHealthByRank[] = { 40.0f, 60.0f, 80.0f, 100.0f };
float damageByRank[] = { 4.0f, 6.0f, 8.0f, 10.0f };
float speedByRank[] = { 1.0f, 1.25f, 1.5f, 1.75f };
float maxStaminaByRank[] = { 20.0f, 30.0f, 40.0f, 50.0f };
int advanceCostByRank[] = { 1, 2, 3, -1 };

const float PUNCH_COST = 4.0f;

const int ALWAYS_INTERRUPT = 0;
const int ACTION_INTERRUPT = 1;
const int REACTION_INTERRUPT = 2;
const int STUNNED_INTERRUPT = 3;
const int KO_INTERRUPT = 4;

const Color TEXT_COLOR = { 0.71f, 0.88f, 0.87f, 1.0f };
const Color TEXT_ON_COLOR = { 0.53f, 0.66f, 0.65f, 1.0f };
const Color TEXT_OFF_COLOR = { 0.03f, 0.22f, 0.22f, 1.0f };
const Color BG_COLOR = { 0.16f, 0.15f, 0.16f, 1.0f };

/* Fighter Stats */
FighterTemplate opponentTemplates[] = {
	{ 0, 0, 0, 0, 0, { 0.8f, 0.62f, 0.53f } },
	{ 1, 1, 1, 1, 0, { 0.62f, 0.49f, 0.34f } },
	{ 2, 0, 2, 1, 0, { 0.43f, 0.29f, 0.21f } },
	{ 1, 1, 3, 1, 0, { 0.82f, 0.73f, 0.7f } },
	{ 2, 2, 3, 2, 0, { 0.3f, 0.18f, 0.14f } }
};

FighterAITemplate opponentAITemplates[] = {
	{ 1.5f, 3.0f, 0.5f, 0.5f, 0.75f, NULL },
	{ 1.5f, 2.0f, 0.5f, 0.5f, 0.75f, NULL },
	{ 1.0f, 2.0f, 0.6f, 0.5f, 0.75f, NULL },
	{ 0.5f, 1.5f, 0.7f, 0.5f, 0.5f, NULL },
	{ 0.5f, 1.0f, 0.8f, 0.5f, 0.5f, NULL }
};

FighterTemplate playerFT = { 0, 0, 0, 0, 0, { 0.0f, 0.75f, 0.0f } };

int currentOpponent = 0;
const int MAX_OPPONENTS = 5;

void val_ResetPlayer( void )
{
	playerFT.conditioning = 0;
	playerFT.endurance = 0;
	playerFT.speed = 0;
	playerFT.strength = 0;
	playerFT.xp = 0;
}