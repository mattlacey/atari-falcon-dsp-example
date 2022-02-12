#ifndef ATARI_INTERNAL_H
#define ATARI_INTERNAL_H

#include <stdint.h>

#include "atari.h"
#include "dsp.h"
#include "graphics.h"
#include "atari_global.h"

// these signatures are hardcoded in atarithread.cpp, musn't change!
ATARISHARED_EXPORT void     SysExit( int32_t code );
ATARISHARED_EXPORT void     TimerDCallback( void );
ATARISHARED_EXPORT void     TimerVblCallback( void );
ATARISHARED_EXPORT Bitmap*  ScreenGetPhysical( void );

#endif // ATARI_INTERNAL_H
