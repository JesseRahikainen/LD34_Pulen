#include "advanceScreen.h"

#include "../Graphics/graphics.h"
#include "../Graphics/images.h"
#include "../Graphics/spineGfx.h"
#include "../Graphics/camera.h"
#include "../Graphics/debugRendering.h"
#include "../Graphics/imageSheets.h"
#include "../UI/button.h"
#include "../UI/text.h"
#include "resources.h"
#include "values.h"
#include "../Utils/helpers.h"
#include "../Input/input.h"
#include "gameScreen.h"

static int buttonSysID = -1;

typedef struct {
	int btnId;
	int upBtnIdx;
	int downBtnIdx;
	int leftBtnIdx;
	int rightBtnIdx;
} LayedOutButton;

typedef enum {
	B_INC_STR,
	B_DEC_STR,
	B_INC_SPD,
	B_DEC_SPD,
	B_INC_END,
	B_DEC_END,
	B_INC_CON,
	B_DEC_CON,
	B_FIGHT,
	NUM_ADV_BTNS
} AdvButtons;

static LayedOutButton layedOutButtons[NUM_ADV_BTNS];

AdvButtons focusedBtn;

static void checkAndFocus( int idx )
{
	if( idx != -1 ) {
		btn_SetFocused( layedOutButtons[idx].btnId );
		focusedBtn = idx;
	}
}

static void moveLeft( void )
{
	checkAndFocus( layedOutButtons[focusedBtn].leftBtnIdx );
}

static void moveRight( void )
{
	checkAndFocus( layedOutButtons[focusedBtn].rightBtnIdx );
}

static void moveUp( void )
{
	checkAndFocus( layedOutButtons[focusedBtn].upBtnIdx );
}

static void moveDown( void )
{
	checkAndFocus( layedOutButtons[focusedBtn].downBtnIdx );
}

static void pressButton( void )
{
	btn_PressRespond( layedOutButtons[focusedBtn].btnId );
}

static void setupInputBindings( void )
{
	input_BindOnKeyPress( SDLK_UP, moveUp );
	input_BindOnKeyPress( SDLK_LEFT, moveLeft );
	input_BindOnKeyPress( SDLK_RIGHT, moveRight );
	input_BindOnKeyPress( SDLK_DOWN, moveDown );
	input_BindOnKeyPress( SDLK_SPACE, pressButton );
	input_BindOnKeyPress( SDLK_RETURN, pressButton );
}

static int testAndGiveXP( int* stat )
{
	if( (*stat) <= 0 ) {
		return 0;
	}

	--(*stat);
	playerFT.xp += advanceCostByRank[*stat];
	return 1;
}

static int testAndSpendXP( int* stat)
{
	int cost = advanceCostByRank[*stat];
	if( cost < 0 ) {
		return 0;
	}

	if( playerFT.xp >= cost ) {
		playerFT.xp -= cost;
		++(*stat);
		return 1;
	}

	return 0;
}

static void decStr( void )
{
	testAndGiveXP( &playerFT.strength );
}

static void incStr( void )
{
	testAndSpendXP( &playerFT.strength );
}

static void decSpd( void )
{
	testAndGiveXP( &playerFT.speed );
}

static void incSpd( void )
{
	testAndSpendXP( &playerFT.speed );
}

static void decEnd( void )
{
	testAndGiveXP( &playerFT.endurance );
}

static void incEnd( void )
{
	testAndSpendXP( &playerFT.endurance );
}

static void decCon( void )
{
	testAndGiveXP( &playerFT.conditioning );
}

static void incCon( void )
{
	testAndSpendXP( &playerFT.conditioning );
}

static void nextFight( void )
{
	++currentOpponent;
	gsmEnterState( &globalFSM, &gameScreenState );
}

static int advanceScreen_Enter( void )
{
	cam_TurnOnFlags( 0, 1 );
	cam_SetNextState( 0, VEC2_ZERO );
	
	gfx_SetClearColor( CLR_BLACK );

	btn_Init( 0 );
	btn_RegisterSystem( );

	// create add and subtract buttons
	Vector2 size;
	img_GetSize( unfocusedAdvImg, &size );

	Vector2 pos = { 422.0f, 162.0f };

	layedOutButtons[B_DEC_STR].btnId = btn_Create( pos, size,
		"-", font, TEXT_OFF_COLOR, TEXT_ON_COLOR, TEXT_ON_COLOR,
		-1, -1, -1, 1, 0, decStr, NULL );
	pos.x += 40.0f;
	layedOutButtons[B_INC_STR].btnId = btn_Create( pos, size,
		"+", font, TEXT_OFF_COLOR, TEXT_ON_COLOR, TEXT_ON_COLOR,
		-1, -1, -1, 1, 0, incStr, NULL );

	pos.y += 38.0f;
	pos.x = 422.0f;
	layedOutButtons[B_DEC_SPD].btnId = btn_Create( pos, size,
		"-", font, TEXT_OFF_COLOR, TEXT_ON_COLOR, TEXT_ON_COLOR,
		-1, -1, -1, 1, 0, decSpd, NULL );
	pos.x += 40.0f;
	layedOutButtons[B_INC_SPD].btnId = btn_Create( pos, size,
		"+", font, TEXT_OFF_COLOR, TEXT_ON_COLOR, TEXT_ON_COLOR,
		-1, -1, -1, 1, 0, incSpd, NULL );

	pos.y += 38.0f;
	pos.x = 422.0f;
	layedOutButtons[B_DEC_END].btnId = btn_Create( pos, size,
		"-", font, TEXT_OFF_COLOR, TEXT_ON_COLOR, TEXT_ON_COLOR,
		-1, -1, -1, 1, 0, decEnd, NULL );
	pos.x += 40.0f;
	layedOutButtons[B_INC_END].btnId = btn_Create( pos, size,
		"+", font, TEXT_OFF_COLOR, TEXT_ON_COLOR, TEXT_ON_COLOR,
		-1, -1, -1, 1, 0, incEnd, NULL );

	pos.y += 38.0f;
	pos.x = 422.0f;
	layedOutButtons[B_DEC_CON].btnId = btn_Create( pos, size,
		"-", font, TEXT_OFF_COLOR, TEXT_ON_COLOR, TEXT_ON_COLOR,
		-1, -1, -1, 1, 0, decCon, NULL );
	pos.x += 40.0f;
	layedOutButtons[B_INC_CON].btnId = btn_Create( pos, size,
		"+", font, TEXT_OFF_COLOR, TEXT_ON_COLOR, TEXT_ON_COLOR,
		-1, -1, -1, 1, 0, incCon, NULL );

	pos.x = 350.0f;
	pos.y = 325.0f;
	layedOutButtons[B_FIGHT].btnId = btn_Create( pos, size,
		"FIGHT!", font, TEXT_OFF_COLOR, TEXT_ON_COLOR, TEXT_ON_COLOR,
		-1, -1, -1, 1, 0, nextFight, NULL );


	layedOutButtons[B_INC_STR].upBtnIdx		= -1;
	layedOutButtons[B_INC_STR].rightBtnIdx	= -1;
	layedOutButtons[B_INC_STR].leftBtnIdx	= B_DEC_STR;
	layedOutButtons[B_INC_STR].downBtnIdx	= B_INC_SPD;

	layedOutButtons[B_DEC_STR].upBtnIdx		= -1;
	layedOutButtons[B_DEC_STR].rightBtnIdx	= B_INC_STR;
	layedOutButtons[B_DEC_STR].leftBtnIdx	= -1;
	layedOutButtons[B_DEC_STR].downBtnIdx	= B_DEC_SPD;

	layedOutButtons[B_INC_SPD].upBtnIdx		= B_INC_STR;
	layedOutButtons[B_INC_SPD].rightBtnIdx	= -1;
	layedOutButtons[B_INC_SPD].leftBtnIdx	= B_DEC_SPD;
	layedOutButtons[B_INC_SPD].downBtnIdx	= B_INC_END;

	layedOutButtons[B_DEC_SPD].upBtnIdx		= B_DEC_STR;
	layedOutButtons[B_DEC_SPD].rightBtnIdx	= B_INC_SPD;
	layedOutButtons[B_DEC_SPD].leftBtnIdx	= -1;
	layedOutButtons[B_DEC_SPD].downBtnIdx	= B_DEC_END;

	layedOutButtons[B_INC_END].upBtnIdx		= B_INC_SPD;
	layedOutButtons[B_INC_END].rightBtnIdx	= -1;
	layedOutButtons[B_INC_END].leftBtnIdx	= B_DEC_END;
	layedOutButtons[B_INC_END].downBtnIdx	= B_INC_CON;

	layedOutButtons[B_DEC_END].upBtnIdx		= B_DEC_SPD;
	layedOutButtons[B_DEC_END].rightBtnIdx	= B_INC_END;
	layedOutButtons[B_DEC_END].leftBtnIdx	= -1;
	layedOutButtons[B_DEC_END].downBtnIdx	= B_DEC_CON;

	layedOutButtons[B_INC_CON].upBtnIdx		= B_INC_END;
	layedOutButtons[B_INC_CON].rightBtnIdx	= -1;
	layedOutButtons[B_INC_CON].leftBtnIdx	= B_DEC_CON;
	layedOutButtons[B_INC_CON].downBtnIdx	= B_FIGHT;

	layedOutButtons[B_DEC_CON].upBtnIdx		= B_DEC_END;
	layedOutButtons[B_DEC_CON].rightBtnIdx	= B_INC_CON;
	layedOutButtons[B_DEC_CON].leftBtnIdx	= -1;
	layedOutButtons[B_DEC_CON].downBtnIdx	= B_FIGHT;

	layedOutButtons[B_FIGHT].upBtnIdx		= B_DEC_CON;
	layedOutButtons[B_FIGHT].rightBtnIdx	= -1;
	layedOutButtons[B_FIGHT].leftBtnIdx		= -1;
	layedOutButtons[B_FIGHT].downBtnIdx		= -1;

	checkAndFocus( B_DEC_STR );

	setupInputBindings( );

	return 1;
}

static int advanceScreen_Exit( void )
{
	cam_TurnOffFlags( 0, 1 );

	btn_DestroyAll( );
	btn_UnRegisterSystem( );
	return 1;
}

static void advanceScreen_ProcessEvents( SDL_Event* e )
{
}

static void advanceScreen_Process( void )
{
}

static void drawRating( int rating, Vector2 basePos )
{
	Color clr;
	for( int i = 0; i < 4; ++i ) {
		if( rating >= i ) {
			clr = CLR_YELLOW;
		} else {
			clr = CLR_WHITE;
		}

		img_Draw_c( starImg, 1, basePos, basePos, clr, clr, 0 );

		basePos.x += 38.0f;
	}
}

static void advanceScreen_Draw( void )
{
	char str[256] = { 0 };
	Vector2 pos;

	pos.x = 350.0f;
	pos.y = 100.0f;
	txt_DisplayString( "Allocate Points", pos, TEXT_COLOR, HORIZ_ALIGN_CENTER, VERT_ALIGN_BASE_LINE, font, 1, 0 );

	pos.y += 24.0f;
	SDL_snprintf( str, ARRAY_SIZE( str ) - 1, "    Points: %i", playerFT.xp );
	txt_DisplayString( str, pos, TEXT_COLOR, HORIZ_ALIGN_CENTER, VERT_ALIGN_BASE_LINE, font, 1, 0 );

	pos.x = 250.0f;
	pos.y += 38.0f;
	txt_DisplayString( "Strength", pos, TEXT_COLOR, HORIZ_ALIGN_RIGHT, VERT_ALIGN_CENTER, font, 1, 0 );
	pos.x += 20.0f;
	drawRating( playerFT.strength, pos );

	pos.x = 250.0f;
	pos.y += 38.0f;
	txt_DisplayString( "Speed", pos, TEXT_COLOR, HORIZ_ALIGN_RIGHT, VERT_ALIGN_CENTER, font, 1, 0 );
	pos.x += 20.0f;
	drawRating( playerFT.speed, pos );

	pos.x = 250.0f;
	pos.y += 38.0f;
	txt_DisplayString( "Endurance", pos, TEXT_COLOR, HORIZ_ALIGN_RIGHT, VERT_ALIGN_CENTER, font, 1, 0 );
	pos.x += 20.0f;
	drawRating( playerFT.endurance, pos );

	pos.x = 250.0f;
	pos.y += 38.0f;
	txt_DisplayString( "Conditioning", pos, TEXT_COLOR, HORIZ_ALIGN_RIGHT, VERT_ALIGN_CENTER, font, 1, 0 );
	pos.x += 20.0f;
	drawRating( playerFT.conditioning, pos );
}

static void advanceScreen_PhysicsTick( float dt )
{
}

struct GameState advanceScreenState = { advanceScreen_Enter, advanceScreen_Exit, advanceScreen_ProcessEvents,
	advanceScreen_Process, advanceScreen_Draw, advanceScreen_PhysicsTick };