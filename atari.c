#include "atari_internal.h"

#include <stdlib.h>
#include <mint/osbind.h>
#include <mint/falcon.h>

#include "screen-asm.h"
#include "timerd-asm.h"
#include "timervbl-asm.h"

#include "dsp_internal.h"
#include "graphics.h"

static volatile int isVblSet;

static Bitmap screen = { 320, 240, 16, BitmapTypeHighColor, NULL, { 0 } };
static HighColor col = 0x0000;

int main( int argc, char* argv[] )
{
	if( loadDspBinary( "calc.p56" ) == 0 )
	{
		return EXIT_FAILURE;
	}

	int32_t oldSSP = Super( 0L );

	asm_screen_save();

	// do this before we kill VBL
	(void)VsetMode( BPS16 | COL40 | VGA | VERTFLAG );

	asm_timerd_init();
	asm_timervbl_init();
	size_t screenSize = ( screen.width * screen.height * screen.depth / 8 ) + 15;

	screen.pixels.pHc = (HighColor*)Mxalloc( screenSize, MX_STRAM );

	if( screen.pixels.pHc == NULL )
	{
		asm_timervbl_deinit();
		asm_timerd_deinit();
		asm_screen_restore();
		Super( oldSSP );
		return EXIT_FAILURE;
	}

	// align on 16 bytes, beware you can't use (M)free anymore
	intptr_t ptr = (intptr_t)screen.pixels.pHc;
	ptr = ( ptr + 15 ) & ~15;
    screen.pixels.pHc = (HighColor*)ptr;

	asm_screen_setvram( screen.pixels.pHc );

	/*
	 * Main demo loop. This is where you want to place your code.
	 */
	for( ; ; )
	{
		if( isVblSet )
		{
			// race condition as hell for HOST
			isVblSet = 0;

			HighColor* p = screen.pixels.pHc;
			// wait for vbl
			for( size_t i = 0; i < screenSize / 2; ++i )
			{
				*p++ = col;
			}

			dspSendUnsignedWord( col );
			// do the math :)
			col = dspReceiveWord();
		}

		if( *(volatile uint8_t*)0xfffffc02 == 0x39 )
		{
			break;
		}
	}

	asm_timervbl_deinit();
	asm_timerd_deinit();
	asm_screen_restore();
	Super( oldSSP );

	return EXIT_SUCCESS;
}

void SysExit( int32_t code )
{
	exit( code );
}

void TimerDCallback( void )
{
}

void TimerVblCallback( void )
{
	isVblSet = 1;
}

Bitmap* ScreenGetPhysical( void )
{
	return &screen;
}
