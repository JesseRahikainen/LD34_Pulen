#include "resources.h"
#include "../Graphics/spineGfx.h"
#include "../UI/text.h"
#include "../System/memory.h"
#include "../Graphics/images.h"
#include "../Utils/stretchyBuffer.h"

#include "values.h"

static void addComboToTemplate( FighterAITemplate* aiTemplate, int count, ... )
{
	FighterCombo combo = { NULL };
	va_list val;

	va_start( val, count );
	for( int i = 0; i < count; ++i ) {
		FighterAnimations anim = va_arg( val, FighterAnimations );
		sb_Push( combo.moves, anim );
	}

	sb_Push( aiTemplate->combos, combo );
}

static void setAICombos( void )
{
	addComboToTemplate( &( opponentAITemplates[0] ), 3, ANIM_JAB_RIGHT, ANIM_JAB_RIGHT, ANIM_HOOK_RIGHT );
	addComboToTemplate( &( opponentAITemplates[0] ), 3, ANIM_JAB_LEFT, ANIM_JAB_LEFT, ANIM_HOOK_LEFT );

	addComboToTemplate( &( opponentAITemplates[1] ), 3, ANIM_JAB_RIGHT, ANIM_HOOK_LEFT, ANIM_JAB_RIGHT );
	addComboToTemplate( &( opponentAITemplates[1] ), 2, ANIM_JAB_LEFT, ANIM_JAB_RIGHT );
	addComboToTemplate( &( opponentAITemplates[1] ), 3, ANIM_CROSS_RIGHT, ANIM_JAB_LEFT, ANIM_UPPERCUT );

	addComboToTemplate( &( opponentAITemplates[2] ), 3, ANIM_JAB_LEFT, ANIM_JAB_RIGHT, ANIM_JAB_LEFT );
	addComboToTemplate( &( opponentAITemplates[2] ), 2, ANIM_JAB_LEFT, ANIM_CROSS_RIGHT );
	addComboToTemplate( &( opponentAITemplates[2] ), 2, ANIM_JAB_RIGHT, ANIM_CROSS_LEFT );

	addComboToTemplate( &( opponentAITemplates[3] ), 4, ANIM_JAB_LEFT, ANIM_JAB_RIGHT, ANIM_JAB_LEFT, ANIM_JAB_RIGHT );
	addComboToTemplate( &( opponentAITemplates[3] ), 4, ANIM_JAB_LEFT, ANIM_JAB_LEFT, ANIM_JAB_RIGHT, ANIM_JAB_RIGHT );
	addComboToTemplate( &( opponentAITemplates[3] ), 4, ANIM_UPPERCUT, ANIM_BLOCK, ANIM_CROSS_LEFT, ANIM_HOOK_LEFT );

	addComboToTemplate( &( opponentAITemplates[4] ), 3, ANIM_UPPERCUT, ANIM_JAB_LEFT, ANIM_JAB_RIGHT );
	addComboToTemplate( &( opponentAITemplates[4] ), 3, ANIM_JAB_LEFT, ANIM_CROSS_RIGHT, ANIM_UPPERCUT );
	addComboToTemplate( &( opponentAITemplates[4] ), 3, ANIM_JAB_LEFT, ANIM_CROSS_RIGHT, ANIM_CROSS_LEFT );
	addComboToTemplate( &( opponentAITemplates[4] ), 3, ANIM_JAB_LEFT, ANIM_CROSS_RIGHT, ANIM_JAB_RIGHT );
}

int loadResources( void )
{
	opponentTemplate = spine_LoadTemplate( "Images/opponent" );
	if( opponentTemplate < 0 ) {
		return -1;
	}

	playerTemplate = spine_LoadTemplate( "Images/player" );
	if( playerTemplate < 0 ) {
		return -1;
	}

	txt_Init( );
	font = txt_LoadFont( "Fonts/kenpixel_mini.ttf", 32 );
	if( font < 0 ) {
		return -1;
	}

	barBaseImg = img_Load( "Images/bar_base.png", ST_DEFAULT );
	if( barBaseImg < 0 ) {
		return -1;
	}

	starImg = img_Load( "Images/star.png", ST_DEFAULT );
	if( starImg < 0 ) {
		return -1;
	}

	focusedAdvImg = img_Load( "Images/focused_advancement.png", ST_DEFAULT );
	unfocusedAdvImg = img_Load( "Images/unfocused_advancement.png", ST_DEFAULT );

	setAICombos( );

	return 0;
}