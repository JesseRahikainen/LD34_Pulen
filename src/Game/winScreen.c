#include "winScreen.h"

#include "../Graphics/graphics.h"
#include "../Graphics/images.h"
#include "../Graphics/spineGfx.h"
#include "../Graphics/camera.h"
#include "../Graphics/debugRendering.h"
#include "../Graphics/imageSheets.h"
#include "../UI/text.h"
#include "values.h"
#include "resources.h"
#include "startScreen.h"

static float timeUp;

static int winScreen_Enter( void )
{
	timeUp = -0.75f;
	cam_TurnOnFlags( 0, 1 );
	
	gfx_SetClearColor( CLR_BLACK );

	return 1;
}

static int winScreen_Exit( void )
{
	return 1;
}

static void winScreen_ProcessEvents( SDL_Event* e )
{
	if( e->type == SDL_KEYDOWN ) {
		if( timeUp >= 0.0f ) {
			gsmEnterState( &globalFSM, &startScreenState );
		}
	}
}

static void winScreen_Process( void )
{
}

static void winScreen_Draw( void )
{
	Vector2 pos = { 400.0f, 300.0f };
	txt_DisplayString( "You've Won!", pos, TEXT_COLOR, HORIZ_ALIGN_CENTER, VERT_ALIGN_CENTER, font, 1, 0 );

	if( timeUp >= 0.0f ) {
		pos.y = 500.0f;
		txt_DisplayString( "Press Any Key To Restart", pos, TEXT_COLOR, HORIZ_ALIGN_CENTER, VERT_ALIGN_CENTER, font, 1, 0 );
	}
}

static void winScreen_PhysicsTick( float dt )
{
	if( timeUp < 0.0f ) {
		timeUp += dt;
	}
}

struct GameState winScreenState = { winScreen_Enter, winScreen_Exit, winScreen_ProcessEvents,
	winScreen_Process, winScreen_Draw, winScreen_PhysicsTick };