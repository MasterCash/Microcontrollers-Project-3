//include <reg51.h>
#include <reg932.h>
#include "uart.h"


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
	
}