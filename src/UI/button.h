/*
Simple button handling. Assume we're using a 3x3 image as the background (or really just 9 images), and some
text on the button itself. Will only handle a click event on release.
Need: Normal, focused, clicked; images, sounds, and functions
Don't manage resources used, expect them to be loaded somewhere else.
*/

#ifndef ENGINE_BUTTON_H
#define ENGINE_BUTTON_H

#include "../Math/Vector2.h"
#include "../Graphics/color.h"
#include <SDL_events.h>

typedef void (*ButtonResponse)(void);

/* Call this before trying to use any buttons. */
void btn_Init( int useMouse );

/*
Creates a button. All image ids must be valid.
*/
int btn_Create( Vector2 position, Vector2 size, const char* text,
	int fontID, Color normalTextClr, Color focusedTextClr, Color clickedTextClr,
	int normalImg, int focusedImg, int clickedImg,
	unsigned int camFlags, char layer, ButtonResponse pressResponse, ButtonResponse releaseResponse );
void btn_Destroy( int buttonIdx );
void btn_DestroyAll( void );

/*
Handles the system level stuff.
 Returns a number < 0 if there was a problem.
*/
int btn_RegisterSystem( void );
void btn_UnRegisterSystem( void );

void btn_Draw( void );
void btn_Process( void );
void btn_ProcessEvents( SDL_Event* sdlEvent );

/*
For when you want to control the button via keyboard
*/
void btn_SetFocused( int id );
void btn_PressRespond( int id );
void btn_ReleaseRespond( int id );

#endif
