//include <reg51.h>
#include <reg932.h>
#include "uart.h"

//current state of the project
// mode = 0 - tune playing mode
// mode = 1 - display songs
// mode = 2 - keyboard mode
char mode = 0;

//Node Chart
unsigned int notes[] = 
{
	0x06DE, // C
	0x067F, // C#
	0x061F, // D
	0x05C9, // D#
	0x0574, // E
	0x0528, // F
	0x04DD, // F#
	0x0497, // G
	0x0456, // G#
	0x0417, // A
	0x03DC, // A#
	0x03A4, // B
}

void main()
{
	P1M1 = 0x00;
	EA = 1;
	//to remove warnings**
	uart_init();
	uart_transmit('a');
	uart_get();
	//********************
	/**********************
		TO-DO
		-------------------
		Play two Short Tunes(~4 seconds long)
		Display Song Title of Each on PC Term via serial
		Make keyboard with atleast 3 buttons like keys
		Use 1-2 buttons to control mode of opp(options above are modes)
		must be able to change modes in middle of stored tune is playing
		use 8051 ports to connect a secondary device via breadboard to do some opp
		 - Idea: display of LEDs for different notes
		Each Member had their own feature.
	**********************/
	while(1)
	{
		switch(mode)
		{
			case 0:
				break;
			case 1:
				break;
			case 2:
				break;
		}
	}
}

