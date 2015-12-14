#include "gameScreen.h"

#include <stdarg.h>
#include <math.h>

#include "../Utils/helpers.h"
#include "../Utils/stretchyBuffer.h"
#include "../Graphics/spineGfx.h"
#include "../Graphics/camera.h"
#include "../Graphics/graphics.h"
#include "../Graphics/images.h"
#include "../Input/input.h"
#include "fighter.h"
#include "values.h"
#include "../UI/text.h"
#include "resources.h"
#include "advanceScreen.h"
#include "winScreen.h"

int aiFighter = -1;
int playerFighter = -1;

// *****************************************************
static float timeLeftPressed;
static float timeRightPressed;

const float DODGE_TIME = 0.15f;
const float UPPERCUT_DELAY = 0.1f;

static void createOpponent( void )
{
	if( aiFighter < 0 ) {
		aiFighter = fighter_Create( &( opponentTemplates[currentOpponent] ), &( opponentAITemplates[currentOpponent] ) );
	} else {
		fighter_Adjust( aiFighter,  &( opponentTemplates[currentOpponent] ), &( opponentAITemplates[currentOpponent] ) );
	}

	if( playerFighter < 0 ) {
		playerFighter = fighter_Create( &playerFT, NULL );
	} else {
		fighter_Adjust( playerFighter, &playerFT, NULL );
	}
}

static void resetBothTimes( void )
{
	timeLeftPressed = -1.0f;
	timeRightPressed = -1.0f;
}

static void inputUpdate( float dt )
{
	int dodgedLeft = 0;
	int dodgedRight = 0;

	if( timeLeftPressed >= 0.0f ) {
		float prevTime = timeLeftPressed;
		timeLeftPressed += dt;
		if( ( prevTime < DODGE_TIME ) && ( timeLeftPressed >= DODGE_TIME ) ) {
			fighter_PlayAnim( playerFighter, ANIM_DODGE_LEFT, 1, 0, ALWAYS_INTERRUPT );
			dodgedLeft = 1;
		}
	}

	if( timeRightPressed >= 0.0f ) {
		float prevTime = timeRightPressed;
		timeRightPressed += dt;
		if( ( prevTime < DODGE_TIME ) && ( timeRightPressed >= DODGE_TIME ) ) {
			fighter_PlayAnim( playerFighter, ANIM_DODGE_RIGHT, 1, 0, ALWAYS_INTERRUPT );
			dodgedRight = 1;
		}
	}

	if( ( timeLeftPressed >= 0.0f ) && ( timeRightPressed >= 0.0f ) ) {
		if( dodgedRight || dodgedLeft ) {
			fighter_PlayAnim( playerFighter, ANIM_BLOCK, 1, 0, ALWAYS_INTERRUPT );
		}
	}
}

static void testCrossOrUppercut( void )
{
	// the addition gives a little bit more lenience
	if( ( timeLeftPressed >= ( DODGE_TIME + 0.05f ) ) || ( timeRightPressed >= ( DODGE_TIME + 0.05f ) ) ) {
		return;
	}
	int side = 0;
	float diff = fabsf( timeRightPressed - timeLeftPressed );
	if( diff < UPPERCUT_DELAY ) {
		fighter_PlayAnim( playerFighter, ANIM_UPPERCUT, 0, 1, ACTION_INTERRUPT );
	} else {
		if( timeRightPressed > timeLeftPressed ) {
			side = 1;
		} else {
			side = -1;
		}
		fighter_Cross( playerFighter, side );
	}
	resetBothTimes( );
}

static void leftPressed( void )
{
	if( timeRightPressed > DODGE_TIME ) {
		fighter_Hook( playerFighter, 1 );
		resetBothTimes( );
		return;
	}

	timeLeftPressed = 0.0f;
}

static void leftRelease( void )
{
	if( timeLeftPressed < 0.0f ) {
		return;
	} else if( timeRightPressed >= 0.0f ) {
		testCrossOrUppercut( );
	} else if( timeLeftPressed < DODGE_TIME ) {
		fighter_Jab( playerFighter, -1 );
	} else {
		fighter_PlayAnim( playerFighter, ANIM_IDLE, 1, 0, ALWAYS_INTERRUPT );
	}

	timeLeftPressed = -1.0f;
}

static void rightPressed( void )
{
	if( timeLeftPressed > DODGE_TIME ) {
		fighter_Hook( playerFighter, -1 );
		resetBothTimes( );
		return;
	}

	timeRightPressed = 0.0f;
}

static void rightRelease( void )
{
	if( timeRightPressed < 0.0f ) {
		return;
	} else if( timeLeftPressed >= 0.0f ) {
		testCrossOrUppercut( );
	} else if( timeRightPressed < DODGE_TIME ) {
		fighter_Jab( playerFighter, 1 );
	} else {
		fighter_PlayAnim( playerFighter, ANIM_IDLE, 1, 0, ALWAYS_INTERRUPT );
	}

	timeRightPressed = -1.0f;
}

static void advanceOpponent( void )
{
	++currentOpponent;
	if( currentOpponent >= MAX_OPPONENTS ) {
		currentOpponent = MAX_OPPONENTS - 1;
	}

	createOpponent( );
}

static void decrementOpponent( void )
{
	--currentOpponent;
	if( currentOpponent < 0 ) {
		currentOpponent = 0;
	}

	createOpponent( );
}

static void setupInput( void )
{
	input_BindOnKeyPress( SDLK_LEFT, leftPressed );
	input_BindOnKeyRelease( SDLK_LEFT, leftRelease );
	input_BindOnKeyPress( SDLK_RIGHT, rightPressed );
	input_BindOnKeyRelease( SDLK_RIGHT, rightRelease );

	input_BindOnKeyRelease( SDLK_w, advanceOpponent );
	input_BindOnKeyRelease( SDLK_s, decrementOpponent );

	timeLeftPressed = -1.0f;
	timeRightPressed = -1.0f;
}

// *****************************************************
static int gameScreen_Enter( void )
{
	cam_TurnOnFlags( 0, 1 );
	cam_TurnOnFlags( 1, 2 );
	
	gfx_SetClearColor( CLR_BLACK );

	createOpponent( );

	setupInput( );

	return 1;
}

static int gameScreen_Exit( void )
{
	cam_TurnOffFlags( 0, 1 );
	cam_TurnOffFlags( 1, 2 );

	input_ClearAllKeyBinds( );
	fighter_Destroy( playerFighter );
	fighter_Destroy( aiFighter );

	playerFighter = -1;
	aiFighter = -1;

	return 1;
}

static void gameScreen_ProcessEvents( SDL_Event* e )
{
}

static void gameScreen_Process( void )
{
	if( fighter_KnockedOut( playerFighter ) ) {
		// continue or quit
		//  just continue for now
		gsmEnterState( &globalFSM, &advanceScreenState );
	} else if( fighter_KnockedOut( aiFighter ) ) {
		if( ( currentOpponent + 1 ) >= MAX_OPPONENTS ) {
			gsmEnterState( &globalFSM, &winScreenState );
		} else {
			// advance
			playerFT.xp += 4; // 4 xp per match
			gsmEnterState( &globalFSM, &advanceScreenState );
		}
	}
}

static void gameScreen_Draw( void )
{
	float health;
	float stamina;

	Vector2 pos = { 10.0f, 10.0f };
	Vector2 scale = { 1.0f, 1.0f };
	Vector2 size;
	img_GetSize( barBaseImg, &size );

	// player
	fighter_GetHealthAndStamina( playerFighter, &health, &stamina );
	if( health < 0.0f ) { health = 0.0f; }
	if( stamina < 0.0f ) { stamina = 0.0f; }
	scale.x = ( 150.0f * ( health / maxHealthByRank[3] ) ) / ( size.x / 2.0f );
	pos.x = 8.0f + ( ( size.x / 2.0f ) * scale.x );
	pos.y = 16.0f;
	img_Draw_sv_c( barBaseImg, 2, pos, pos, scale, scale, CLR_RED, CLR_RED, 0 );
	

	scale.x = ( 150.0f * ( stamina / maxStaminaByRank[3] ) ) / ( size.x / 2.0f );
	pos.x = 8.0f + ( ( size.x / 2.0f ) * scale.x );
	pos.y += 20.0f;
	img_Draw_sv_c( barBaseImg, 2, pos, pos, scale, scale, CLR_GREEN, CLR_GREEN, 0 );

	scale.x = ( 150.0f * ( PUNCH_COST / maxStaminaByRank[3] ) ) / ( size.x / 2.0f );
	pos.x = 8.0f + ( ( size.x / 2.0f ) * scale.x );
	img_Draw_sv_c( barBaseImg, 2, pos, pos, scale, scale, CLR_BLUE, CLR_BLUE, -1 );

	pos.x = 16.0f;
	pos.y += 8.0f;
	txt_DisplayString( "YOU", pos, TEXT_COLOR, HORIZ_ALIGN_LEFT, VERT_ALIGN_TOP, font, 2, 0 );

	// computer
	fighter_GetHealthAndStamina( aiFighter, &health, &stamina );
	if( health < 0.0f ) { health = 0.0f; }
	if( stamina < 0.0f ) { stamina = 0.0f; }
	scale.y = 1.0f;
	scale.x = ( 150.0f * ( health / maxHealthByRank[3] ) ) / ( size.x / 2.0f );
	pos.x = 792.0f - ( ( size.x / 2.0f ) * scale.x );
	pos.y = 16.0f;
	img_Draw_sv_c( barBaseImg, 2, pos, pos, scale, scale, CLR_RED, CLR_RED, 0 );
	
	scale.x = ( 150.0f * ( stamina / maxStaminaByRank[3] ) ) / ( size.x / 2.0f );
	pos.x = 792.0f - ( ( size.x / 2.0f ) * scale.x );
	pos.y += 20.0f;
	img_Draw_sv_c( barBaseImg, 2, pos, pos, scale, scale, CLR_GREEN, CLR_GREEN, 0 );

	pos.x = 784.0f;
	pos.y += 8.0f;
	txt_DisplayString( "COMP", pos, TEXT_COLOR, HORIZ_ALIGN_RIGHT, VERT_ALIGN_TOP, font, 2, 0 );
}

static void gameScreen_PhysicsTick( float dt )
{
	Vector2 camPos;
	fighter_GetCameraPos( playerFighter, &camPos );
	cam_SetNextState( 0, camPos );
	inputUpdate( dt );
	fighter_Update( dt );
}

struct GameState gameScreenState = { gameScreen_Enter, gameScreen_Exit, gameScreen_ProcessEvents,
	gameScreen_Process, gameScreen_Draw, gameScreen_PhysicsTick };