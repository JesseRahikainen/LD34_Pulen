#ifndef FIGHTER_H
#define FIGHTER_H

#include "../Graphics/spineGfx.h"
#include "../Graphics/color.h"

typedef enum {
	ANIM_BLOCK,
	ANIM_CROSS_LEFT,
	ANIM_CROSS_RIGHT,
	ANIM_DODGE_LEFT,
	ANIM_DODGE_RIGHT,
	ANIM_HIT_FROM_LEFT,
	ANIM_HIT_FROM_RIGHT,
	ANIM_HOOK_LEFT,
	ANIM_HOOK_RIGHT,
	ANIM_IDLE,
	ANIM_JAB_LEFT,
	ANIM_JAB_RIGHT,
	ANIM_KNOCKED_OUT,
	ANIM_STUNNED,
	ANIM_UPPERCUT,
	NUM_ANIMS
} FighterAnimations;

typedef struct {
	int strength;
	int speed;
	int endurance;
	int conditioning;
	int xp;
	Color color;
} FighterTemplate;

typedef struct {
	FighterAnimations* moves;
} FighterCombo;

typedef struct {
	float minActionWait;
	float maxActionWait;
	float defenseChance;
	float counterChance;
	float comboChance;
	FighterCombo* combos;
} FighterAITemplate;

typedef struct {
	float maxHealth;
	float health;
	float maxStamina;
	float stamina;
	float baseDamage;
	float speedMod;

	spAnimationState* animState;
	spAnimation* animations[NUM_ANIMS];
	spBone* cameraBone;

	int blocking;
	int dodgeDirection;
	int recovering;
	int lockedIntoAnimation;

	int instanceID;

	float actionWaitLeft;
	int currentCombo;
	uint32_t currentComboIdx;

	int isKnockedOut;

	FighterAITemplate* aiTemplate;
} Fighter;

int fighter_Create( FighterTemplate* template, FighterAITemplate* aiTemplate );
void fighter_Destroy( int idx );
void fighter_Update( float dt );
void fighter_Adjust( int idx, FighterTemplate* template, FighterAITemplate* aiTemplate );
void fighter_GetCameraPos( int idx, Vector2* outPos );
void fighter_PlayAnim( int idx, FighterAnimations anim, int loops, int isPunch, int interruptionLevel );
void fighter_Jab( int idx, int side );
void fighter_Cross( int idx, int side );
void fighter_Hook( int idx, int side );
void fighter_Dodge( int idx, int side );
void fighter_GetHealthAndStamina( int idx, float* outHealth, float* outStamina );
int fighter_KnockedOut( int idx );

#endif