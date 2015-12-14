#include "fighter.h"
#include "resources.h"
#include "values.h"
#include "../Utils/stretchyBuffer.h"
#include "../Utils/helpers.h"
#include "../Math/mathUtil.h"
#include <SDL_log.h>
#include <stdlib.h>

static Fighter fighters[2];
const static float STAMINA_REGEN_PCT_PER_SEC = 0.05f;

static void checkForKO( int idx )
{
	if( fighters[idx].health <= 0.0f ) {
		fighters[idx].stamina = 0.0f;
		fighter_PlayAnim( idx, ANIM_KNOCKED_OUT, 0, 0, KO_INTERRUPT );
	}
}

static int checkForStun( int idx )
{
	if( fighters[idx].stamina <= 0.0f ) {
		fighter_PlayAnim( idx, ANIM_STUNNED, 1, 0, STUNNED_INTERRUPT );
		return 0;
	}

	return -1;
}

static void testAttack( int attackerIdx, int defenderIdx, int attackerDir, float healthDmg, float stamDmg, int blockable )
{
	float recoveryScale = fighters[defenderIdx].recovering ? 1.25f : 1.0f;

	FighterAnimations reactionAnim = ( attackerDir == 1 ) ? ANIM_HIT_FROM_RIGHT : ANIM_HIT_FROM_LEFT;

	if( fighters[defenderIdx].blocking && blockable ) {
		// do stamina damage to the fighter
		// notify AI of block
		fighters[defenderIdx].stamina -= healthDmg * 1.25f;
		checkForStun( defenderIdx );
		return;
	}

	// always able to dodge unblockable attacks
	if( !blockable && fighters[defenderIdx].dodgeDirection != 0 ) {
		return;
	}

	if( ( fighters[defenderIdx].dodgeDirection != 0 ) && ( fighters[defenderIdx].dodgeDirection != attackerDir ) ) {
		return;
	}
	
	fighters[defenderIdx].health -= healthDmg * recoveryScale;
	fighters[defenderIdx].stamina -= stamDmg * recoveryScale;
	fighter_PlayAnim( defenderIdx, reactionAnim, 0, 0, REACTION_INTERRUPT );
	checkForStun( defenderIdx );
	checkForKO( defenderIdx );
}

static void testCrossHit( int idx, int dir )
{
	testAttack( idx, 1 - idx, dir, fighters[idx].baseDamage * 1.2f, 0.0f, 1 );
}

static void testHookHit( int idx, int dir )
{
	testAttack( idx, 1 - idx, dir, fighters[idx].baseDamage * 1.5f, 0.0f, 1 );
}

static void testJabHit( int idx, int dir )
{
	testAttack( idx, 1 - idx, dir, fighters[idx].baseDamage, 0.0f, 1 );
}

static void testUppercutHit( int idx, int dir )
{
	testAttack( idx, 1 - idx, dir, fighters[idx].baseDamage, fighters[idx].baseDamage, 0 );
}

static void animListener( spAnimationState* state, int trackIndex, spEventType type, spEvent* event, int loopCount )
{
	int idx = (int)( state->rendererObject );
	switch( type ) {
	case SP_ANIMATION_START:
		// just to make sure everything is cleaned up correctly
		fighters[idx].recovering = 0;
		fighters[idx].blocking = 0;
		fighters[idx].dodgeDirection = 0;
		break;
	case SP_ANIMATION_END:
		break;
	case SP_ANIMATION_COMPLETE:
		if( !state->tracks[trackIndex]->loop ) {
			if( fighters[idx].lockedIntoAnimation == KO_INTERRUPT ) {
				// game over, notify that it's all over
				fighters[idx].isKnockedOut = 1;
			} else {
				fighters[idx].lockedIntoAnimation = 0;
				fighter_PlayAnim( idx, ANIM_IDLE, 1, 0, ALWAYS_INTERRUPT );
			}
		}
		break;
	case SP_ANIMATION_EVENT:
		// encode the event type in the first character of the event name, avoid strcmps
		if( event->data->name != NULL ) {
			switch( event->data->name[0] ) {
			case '0': // dodging
				fighters[idx].dodgeDirection = event->intValue;
				break;
			case '1': // recovery
				fighters[idx].recovering = event->intValue;
				break;
			case '2': // test cross hit
				testCrossHit( idx, event->intValue );
				break;
			case '3': // test hook hit
				testHookHit( idx, event->intValue );
				break;
			case '4': // test jab hit
				testJabHit( idx, event->intValue );
				break;
			case '5': // test uppercut hit
				testUppercutHit( idx, event->intValue );
				break;
			case '6': // set locked in animation
				fighters[idx].lockedIntoAnimation = event->intValue;
				break;
			case '7': // blocking
				fighters[idx].blocking = event->intValue;
				break;
			}
		}
		break;
	}
}

void setSlotColor( spSkeleton* skeleton, const char* slotName, Color* clr )
{
	spSlot* slot = spSkeleton_findSlot( skeleton, slotName );
	if( slot == NULL ) {
		return;
	}
	slot->r = clr->r;
	slot->g = clr->g;
	slot->b = clr->b;
}

int fighter_Create( FighterTemplate* template, FighterAITemplate* aiTemplate )
{
	int idx = 0;
	int spineTemplate = playerTemplate;
	Vector2 pos = { 400.0f, 300.0f };
	if( aiTemplate != NULL ) {
		idx = 1;
		spineTemplate = opponentTemplate;
		pos.x = 400.0f;
		pos.y = 300.0f;
	}

	// setup all the animations
	
	fighters[idx].instanceID = spine_CreateInstance( spineTemplate, pos, 1, 0, animListener );
	if( fighters[idx].instanceID < 0 ) {
		return -1;
	}

	spSkeleton* skeleton = spine_GetInstanceSkeleton( fighters[idx].instanceID );
	if( skeleton == NULL ) {
		return -1;
	}

	fighters[idx].animState = spine_GetInstanceAnimState( fighters[idx].instanceID );
	if( fighters[idx].animState == NULL ) {
		return -1;
	}
	fighters[idx].animState->rendererObject = (void*)idx;

	fighters[idx].animations[ANIM_BLOCK] = spSkeletonData_findAnimation( skeleton->data, "block" );
	fighters[idx].animations[ANIM_CROSS_LEFT] = spSkeletonData_findAnimation( skeleton->data, "cross_left" );
	fighters[idx].animations[ANIM_CROSS_RIGHT] = spSkeletonData_findAnimation( skeleton->data, "cross_right" );
	fighters[idx].animations[ANIM_DODGE_LEFT] = spSkeletonData_findAnimation( skeleton->data, "dodge_left" );
	fighters[idx].animations[ANIM_DODGE_RIGHT] = spSkeletonData_findAnimation( skeleton->data, "dodge_right" );
	fighters[idx].animations[ANIM_HIT_FROM_LEFT] = spSkeletonData_findAnimation( skeleton->data, "hitFromLeft" );
	fighters[idx].animations[ANIM_HIT_FROM_RIGHT] = spSkeletonData_findAnimation( skeleton->data, "hitFromRight" );
	fighters[idx].animations[ANIM_HOOK_LEFT] = spSkeletonData_findAnimation( skeleton->data, "hook_left" );
	fighters[idx].animations[ANIM_HOOK_RIGHT] = spSkeletonData_findAnimation( skeleton->data, "hook_right" );
	fighters[idx].animations[ANIM_IDLE] = spSkeletonData_findAnimation( skeleton->data, "idle" );
	fighters[idx].animations[ANIM_JAB_LEFT] = spSkeletonData_findAnimation( skeleton->data, "jab_left" );
	fighters[idx].animations[ANIM_JAB_RIGHT] = spSkeletonData_findAnimation( skeleton->data, "jab_right" );
	fighters[idx].animations[ANIM_KNOCKED_OUT] = spSkeletonData_findAnimation( skeleton->data, "knockedOut" );
	fighters[idx].animations[ANIM_STUNNED] = spSkeletonData_findAnimation( skeleton->data, "stunned" );
	fighters[idx].animations[ANIM_UPPERCUT] = spSkeletonData_findAnimation( skeleton->data, "uppercut" );

	fighters[idx].cameraBone = spSkeleton_findBone( skeleton, "camera" );

	fighter_Adjust( idx, template, aiTemplate );

	return idx;
}

void fighter_Destroy( int idx )
{
	spine_CleanInstance( fighters[idx].instanceID );
	fighters[idx].instanceID = -1;
	fighters[idx].animState = NULL;
	fighters[idx].cameraBone = NULL;
	fighters[idx].aiTemplate = NULL;
	for( int i = 0; i < NUM_ANIMS; ++i ) {
		fighters[idx].animations[i] = NULL;
	}
}

void fighter_Adjust( int idx, FighterTemplate* ft, FighterAITemplate* aiTemplate )
{
	fighters[idx].maxHealth = maxHealthByRank[ ft->conditioning ];
	fighters[idx].health = fighters[idx].maxHealth;

	fighters[idx].maxStamina = maxStaminaByRank[ ft->endurance ];
	fighters[idx].stamina = fighters[idx].maxStamina;

	fighters[idx].baseDamage = damageByRank[ ft->strength ];
	fighters[idx].speedMod = speedByRank[ ft->speed ];

	if( aiTemplate != NULL ) {
		fighters[idx].actionWaitLeft = randFloat( aiTemplate->minActionWait, aiTemplate->maxActionWait );
	}
	fighters[idx].recovering = 0;
	fighters[idx].blocking = 0;
	fighters[idx].dodgeDirection = 0;
	fighters[idx].lockedIntoAnimation = 0;
	fighters[idx].isKnockedOut = 0;

	fighters[idx].currentCombo = -1;
	fighters[idx].currentComboIdx = 0;

	fighters[idx].aiTemplate = aiTemplate;

	spSkeleton* skeleton = spine_GetInstanceSkeleton( fighters[idx].instanceID );
	if( skeleton == NULL ) {
		return;
	}

	// set color for all the slots
	setSlotColor( skeleton, "torso", &( ft->color ) );
	setSlotColor( skeleton, "head", &( ft->color ) );
	setSlotColor( skeleton, "leftHand", &( ft->color ) );
	setSlotColor( skeleton, "rightHand", &( ft->color ) );

	spTrackEntry* track = spAnimationState_setAnimation( fighters[idx].animState, 0, fighters[idx].animations[ANIM_IDLE], 1 );
	track->timeScale = fighters[idx].speedMod;
}

typedef struct {
	FighterAnimations anim;
	float weight;
} WeightedAction;

WeightedAction randomActions[] = {
	{ ANIM_BLOCK, 1.0f },
	{ ANIM_CROSS_LEFT, 1.0f },
	{ ANIM_CROSS_RIGHT, 1.0f },
	{ ANIM_DODGE_LEFT, 2.0f },
	{ ANIM_DODGE_RIGHT, 2.0f },
	{ ANIM_HOOK_LEFT, 1.0f },
	{ ANIM_HOOK_RIGHT, 1.0f },
	{ ANIM_JAB_LEFT, 4.0f },
	{ ANIM_JAB_RIGHT, 4.0f },
	{ ANIM_UPPERCUT, 1.0f }
};

WeightedAction counterActions[] = {
	{ ANIM_CROSS_LEFT, 4.0f },
	{ ANIM_CROSS_RIGHT, 4.0f },
	{ ANIM_HOOK_LEFT, 1.0f },
	{ ANIM_HOOK_RIGHT, 1.0f },
	{ ANIM_JAB_LEFT, 4.0f },
	{ ANIM_JAB_RIGHT, 4.0f },
	{ ANIM_UPPERCUT, 1.0f }
};

WeightedAction defenseActions[] = {
	{ ANIM_BLOCK, 1.0f },
	{ ANIM_DODGE_LEFT, 2.0f },
	{ ANIM_DODGE_RIGHT, 2.0f }
};

static int isAnimPunch( FighterAnimations anim )
{
	switch( anim ) {			
	case ANIM_CROSS_LEFT:
	case ANIM_CROSS_RIGHT:
	case ANIM_HOOK_LEFT:
	case ANIM_HOOK_RIGHT:
	case ANIM_JAB_LEFT:
	case ANIM_JAB_RIGHT:
	case ANIM_UPPERCUT:
		return 1;
	}

	return 0;
}

static FighterAnimations chooseWeightedAction( WeightedAction* actions, size_t numActions )
{
	float weightTotal = 0.0f;
	for( size_t i = 0; i < numActions; ++i ) {
		weightTotal += actions[i].weight;
	}

	float choice = randFloat( 0.0f, weightTotal );

	for( size_t i = 0; i < numActions; ++i ) {
		choice -= actions[i].weight;
		if( choice <= 0.0f ) {
			return actions[i].anim;
		}
	}

	return ANIM_UPPERCUT;
}

static FighterAnimations chooseCounterAction( void )
{
	return chooseWeightedAction( counterActions, ARRAY_SIZE( counterActions ) );
}

static FighterAnimations chooseRandomAction( void )
{
	return chooseWeightedAction( randomActions, ARRAY_SIZE( randomActions ) );
}

static FighterAnimations chooseDefenseAction( void )
{
	return chooseWeightedAction( defenseActions, ARRAY_SIZE( defenseActions ) );
}

static int shouldSkipAttack( int idx )
{
	// at 5 stamina and below attacks should never be done
	// after that it scales up to 100% at 15 stamina
	float chance = inverseLerp( 5.0f, 15.0f, fighters[idx].stamina );
	return ( randFloat( 0.0f, 1.0f ) > chance );
}

static void fighterAIUpdate( int idx, float dt )
{
	Fighter* f = &( fighters[idx] );
	Fighter* opponent = &( fighters[1-idx] );
	if( f->lockedIntoAnimation ) {
		return;
	}

	f->actionWaitLeft -= dt;
	if( f->actionWaitLeft <= 0.0f ) {
		FighterAnimations anim = NUM_ANIMS;

		// once a fighter is in a combo they're locked into it until they finish
		if( f->currentCombo >= 0 ) {
			// advance to the next move
			//  if it's the last one then set to no combo
			anim = f->aiTemplate->combos[f->currentCombo].moves[f->currentComboIdx];
			++( f->currentComboIdx );
			if( f->currentComboIdx >= sb_Count( f->aiTemplate->combos[f->currentCombo].moves ) ) {
				f->currentCombo = -1;
			}
			f->actionWaitLeft = f->aiTemplate->minActionWait;
		} else {
			// check to see if the other fighter is attacking (lockedIntoAnimation == 1)
			//  if they are choose to possibly defend or counter

			if( opponent->lockedIntoAnimation == ACTION_INTERRUPT) {
				if( randFloat( 0.0f, 1.0f ) < f->aiTemplate->counterChance ) {
					anim = chooseCounterAction( );
				} else if( randFloat( 0.0f, 1.0f ) < f->aiTemplate->defenseChance ) {
					anim = chooseDefenseAction( );
				}
			}

			if( anim == NUM_ANIMS ) {
				if( ( f->aiTemplate->combos != NULL ) && ( randFloat( 0.0f, 1.0f ) < f->aiTemplate->comboChance ) ) {
					// assuming the combo won't be empty
					f->currentCombo = rand( ) % sb_Count( f->aiTemplate->combos );
					f->currentComboIdx = 1;
					anim = f->aiTemplate->combos[f->currentCombo].moves[0];
					f->actionWaitLeft = f->aiTemplate->minActionWait;
				} else {
					// not doing anything else, choose a random move
					anim = chooseRandomAction( );
					f->actionWaitLeft = randFloat( f->aiTemplate->minActionWait, f->aiTemplate->maxActionWait );
				}
			}
		}

		int isPunch = isAnimPunch( anim );
		if( !isPunch || !shouldSkipAttack( idx ) ) {
			fighter_PlayAnim( idx, anim, 0, isPunch, ACTION_INTERRUPT );
		}
	}
}

void fighter_Update( float dt )
{
	// process any fighter ai
	for( int i = 0; i < ARRAY_SIZE( fighters ); ++i ) {
		if( fighters[i].health <= 0.0f ) {
			continue;
		}

		if( fighters[i].stamina < fighters[i].maxStamina ) {
			float prevStam = fighters[i].stamina;
			fighters[i].stamina = MIN( fighters[i].maxStamina, ( fighters[i].stamina + ( dt * STAMINA_REGEN_PCT_PER_SEC * fighters[i].maxStamina ) ) );

			// check for leaving stunned
			if( ( prevStam <= 0.0f ) && ( fighters[i].stamina > 0.0f ) ) {
				fighters[i].lockedIntoAnimation = ALWAYS_INTERRUPT;
				fighter_PlayAnim( i, ANIM_IDLE, 1, 0, ALWAYS_INTERRUPT );
			}
		}

		if( fighters[i].aiTemplate != NULL ) {
			fighterAIUpdate( i, dt );
		}
	}
}

void fighter_GetCameraPos( int idx, Vector2* outPos )
{
	if( fighters[idx].cameraBone != NULL ) {
		// assuming the base position will always be the zero vector
		outPos->x = fighters[idx].cameraBone->worldX;
		outPos->y = fighters[idx].cameraBone->worldY;
	}
}

void fighter_PlayAnim( int idx, FighterAnimations anim, int loops, int isPunch, int interruptionLevel )
{
	if( ( fighters[idx].lockedIntoAnimation != 0 ) && ( interruptionLevel <= fighters[idx].lockedIntoAnimation ) ) {
		return;
	}

	if( isPunch ) {
		fighters[idx].stamina -= PUNCH_COST;
		if( checkForStun( idx ) == 0 ) {
			return;
		}
	}

	spTrackEntry* track = spAnimationState_setAnimation( fighters[idx].animState, 0, fighters[idx].animations[anim], loops );
	track->timeScale = fighters[idx].speedMod;
}

void fighter_Jab( int idx, int side )
{
	FighterAnimations anim = ( side == -1 ) ? ANIM_JAB_LEFT : ANIM_JAB_RIGHT;
	fighter_PlayAnim( idx, anim, 0, 1, ACTION_INTERRUPT );
}

void fighter_Cross( int idx, int side )
{
	FighterAnimations anim = ( side == -1 ) ? ANIM_CROSS_LEFT : ANIM_CROSS_RIGHT;
	fighter_PlayAnim( idx, anim, 0, 1, ACTION_INTERRUPT );
}

void fighter_Hook( int idx, int side )
{
	FighterAnimations anim = ( side == -1 ) ? ANIM_HOOK_LEFT : ANIM_HOOK_RIGHT;
	fighter_PlayAnim( idx, anim, 0, 1, ACTION_INTERRUPT );
}

void fighter_Dodge( int idx, int side )
{
	FighterAnimations anim = ( side == -1 ) ? ANIM_DODGE_LEFT : ANIM_DODGE_RIGHT;
	fighter_PlayAnim( idx, anim, 0, 0, ACTION_INTERRUPT );
}

void fighter_GetHealthAndStamina( int idx, float* outHealth, float* outStamina )
{
	(*outHealth) = fighters[idx].health;
	(*outStamina) = fighters[idx].stamina;
}

int fighter_KnockedOut( int idx )
{
	return fighters[idx].isKnockedOut;
}