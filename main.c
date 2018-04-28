//include <reg51.h>
#include <reg932.h>
#include "uart.h"

//current state of the project
// mode = 0 - tune playing mode
// mode = 1 - display songs
// mode = 2 - keyboard mode
char mode = 0;
sbit SPEAKER = P1^7;
//Note Chart
unsigned int notes[] = 
{
	1758, // C
	1663, // C#
	1567, // D
	1481, // D#
	1396, // E
	1320, // F
	1245, // F#
	1175, // G
	1110, // G#
	1047, // A
	988, // A#
	932, // B
};

/*
 * Expects Timer 0 to not be in use.
 * For Timer Load Values greater than 65535
 */
void timerover(unsigned int t)
{
	// looping over causes imprecise timing due to loop nature 
	unsigned char loop = 100;
	// make sure not to overide the Timer1 settings
	// (hopefully this makes sure it doesn't mess up if it is running)
	unsigned int currentTMOD = TMOD >> 4 << 4;
	// attach the timer 0 mode we want
	currentTMOD += 0x01;
	// set tmod to that mode.
	TMOD = currentTMOD;
	// get the preload val
	t = t / loop;
	
	//run the pause.
	for(; loop > 0; loop--)
	{
		TH0 = -t >> 8;
		TL0 = -t;
		TR0 = 1;
		while(TF0 == 0);
		TF0 = 0;
	}
}


/*
 * Uses Timer 0 AND Timer 1 to play a note for .5 sec
 * assumes that both timers are not in use
 * t is preload for note square wave
 */
void playNote(unsigned int t)
{
	// looping over causes imprecise timing due to loop nature 
	unsigned char loop = 100;
	
	// set TMOD.
	TMOD = 0x11;
	SPEAKER = 0;
	
	//run the pause.
	for(; loop > 0; loop--)
	{
		TH0 = -4608 >> 8;
		TL0 = -4608;
		TR0 = 1;
		//run while timer goes, speaker may slightly change.
		while(TF0 == 0)
		{
			// hopefully this works right
			TH1 = -t >> 8;
			TL1 = -t;
			TR1 = 1;
			// toggle speaker
			SPEAKER = ~SPEAKER;
			while(TF1 == 0);
			TF1 = 0;
		}
		
		TF0 = 0;
	}
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

