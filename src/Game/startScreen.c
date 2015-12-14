#include "startScreen.h"

#include "../Graphics/graphics.h"
#include "../Graphics/images.h"
#include "../Graphics/spineGfx.h"
#include "../Graphics/camera.h"
#include "../Graphics/debugRendering.h"
#include "../Graphics/imageSheets.h"
#include "../UI/text.h"
#include "values.h"
#include "resources.h"
#include "gameScreen.h"

static int startScreen_Enter( void )
{
	cam_TurnOnFlags( 0, 1 );
	
	gfx_SetClearColor( CLR_BLACK );

	return 1;
}

static int startScreen_Exit( void )
{
	return 1;
}

static void startScreen_ProcessEvents( SDL_Event* e )
{
	if( e->type == SDL_KEYDOWN ) {
		val_ResetPlayer( );
		gsmEnterState( &globalFSM, &gameScreenState );
	}
}

static void startScreen_Process( void )
{
}

static void startScreen_Draw( void )
{
	Vector2 pos = { 400.0f, 250.0f };

	txt_DisplayString( "Instructions\n"
		"While fighting use the left and right arrow keys to punch\n"
		"and move around.\n"
		"Tap one and release - Jab\n"
		"Tap one and hold - Dodge\n"
		"Tap both - Uppercut\n"
		"Tap both and hold - Guard\n"
		"Once dodging, press other - Hook\n"
		"Quickly tap one, then the other, and release - Cross\n"
		"\n"
		"In menus use the arrow key to move around and space or\n"
		"enter to use that button."
		, pos, TEXT_COLOR, HORIZ_ALIGN_CENTER, VERT_ALIGN_CENTER, font, 1, 0 );

	pos.y = 500.0f;
	txt_DisplayString( "Press any key to start", pos, TEXT_COLOR, HORIZ_ALIGN_CENTER, VERT_ALIGN_CENTER, font, 1, 0 );
}

static void startScreen_PhysicsTick( float dt )
{
}

struct GameState startScreenState = { startScreen_Enter, startScreen_Exit, startScreen_ProcessEvents,
	startScreen_Process, startScreen_Draw, startScreen_PhysicsTick };