#include "dsp_internal.h"

#include <stdio.h>
#include "dsp_fix-asm.h"

#define DSP_HOST_LONG (volatile int32_t*)0xffffa204
#define DSP_HOST_WORD (volatile int16_t*)0xffffa206
#define DSP_HOST_BYTE (volatile int8_t*)0xffffa207
#define DSP_ISR	      (volatile uint8_t*)0xffffa202

#define WAIT_RX \
	while( ( *(DSP_ISR) & 0x01 ) == 0 );

#define WAIT_TX \
	while( ( *(DSP_ISR) & 0x02 ) == 0 );

int loadDspBinary( const char* sPath )
{
	static char buf[32768 * 3];
	FILE* f = fopen( sPath, "rb" );
	if( f == NULL )
	{
		return 0;
	}

	int len = fread( buf, 1, sizeof( buf ), f );
	fclose( f );

	return asm_dsp_load_program( buf, len / 3 );
}

int8_t dspReceiveByte( void )
{
	WAIT_RX;
	return *(DSP_HOST_BYTE);
}

int16_t dspReceiveWord( void )
{
	WAIT_RX;
	return *(DSP_HOST_WORD);
}

int32_t dspReceiveSignedLong( void )
{
	WAIT_RX;
	return ( *(DSP_HOST_LONG) << 8 ) >> 8;
}

uint32_t dspReceiveUnsignedLong( void )
{
	WAIT_RX;
	return *(DSP_HOST_LONG) & 0x00ffffff;
}

void dspSendLong( int32_t value )
{
	WAIT_TX;
	*(DSP_HOST_LONG) = value;
}


void dspSendSignedByte( int8_t value )
{
	int32_t dspWord = value;

	dspSendLong( dspWord );
}

void dspSendUnsignedByte( uint8_t value )
{
	uint32_t dspWord = value;

	dspSendLong( dspWord );
}

void dspSendSignedWord( int16_t value )
{
	int32_t dspWord = value;

	dspSendLong( dspWord );
}

void dspSendUnsignedWord( uint16_t value )
{
	uint32_t dspWord = value;

	dspSendLong( dspWord );
}
