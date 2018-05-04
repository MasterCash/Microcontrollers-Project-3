//#include <reg51.h>
#include <reg932.h>
#include "uart.h"

//current state of the project
// mode = 0 - tune playing mode
// mode = 1 - display songs
// mode = 2 - keyboard mode
char mode = 0;
sbit LED = P2^4;
sbit SPEAKER = P1^7;


//Note Chart C4 - B4
unsigned int notes[] = 
{
	14090, // C
	13300, // C#
	12553, // D
	11848, // D#
	11183, // E
	10556, // F
	9963, // F#
	9404, // G
	8876, // G#
	8378, // A
	7908, // A#
	7464, // B
};

//1 = eighth note, 2 = quarter note, 3 = half note, 4 = whole note, 0 = articuation pause
unsigned int beats[] =
{
  163,    // articuation pause !!Don't use this!! Internal only
  3932,   // Eighth Note
  8028,  // Quarter Note
  16220,  // Half Note
  32605   // Whole Note
};
/*
unsigned int beats2[] =
{
  61,
  1529,
  3010,
  6082,
  12226
};
*/
unsigned int SEC = 16384;



/*
 * Expects Timer 0 to not be in use.
 * For Timer Load Values less than 256
 */
 /*
void time8b(unsigned char t)
{
  unsigned int currentTMOD = TMOD >> 4 << 4;
	// attach the timer 0 mode we want
	currentTMOD += 0x01;
	// set tmod to that mode.
	TMOD = currentTMOD;
  TH0 = -t;
  TL0 = -t;
  TR0 = 1;
  while(TF0 == 0);
  TR0 = 0;
  TF0 = 0;
}
*/

/*
 * Expects Timer 0 to not be in use.
 * For Timer Load Values greater than 65535
 */
void timerover(unsigned int t)
{
	// looping over causes imprecise timing due to loop nature 
	unsigned char loop = 225;
	// make sure not to overide the Timer1 settings
	// (hopefully this makes sure it doesn't mess up if it is running)
	unsigned int currentTMOD = TMOD >> 4 << 4;
	// attach the timer 0 mode we want
	currentTMOD += 0x01;
	// set tmod to that mode.
	TMOD = currentTMOD;
	// get the preload val
	
	//run the pause.
	for(; loop > 0; loop--)
	{
		TH0 = -t >> 8;
		TL0 = -t;
		TR0 = 1;
		while(TF0 == 0);
    TR0 = 0;
		TF0 = 0;
	}
}


/*
 * Uses Timer 0 AND Timer 1 to play a note 
 * assumes that both timers are not in use
 * note is preload for square wave for that frequency of that note
 * type is the type of note IE: 1 = eighth note, 2 = quarter note, 3 = half note, 4 = whole note
 */
void playNote(unsigned int note, unsigned char type)
{
	// looping over causes imprecise timing due to loop nature 
	unsigned char loop = 225;
	unsigned int timerLoad = beats[type];
  unsigned int 
	// set TMOD.
	TMOD = 0x11;
	SPEAKER = 0;
	//run the pause.
	for(; loop > 0; loop--)
	{
		TH0 = -timerLoad >> 8;
		TL0 = -timerLoad;
		TR0 = 1;
		//run while timer goes, speaker may slightly change.
    LED =0;
		while(TF0 == 0)
		{
      if(note > 0)
      {
        // hopefully this works right
        TH1 = -note >> 8;
        TL1 = -note;
        TR1 = 1;
        // toggle speaker
        SPEAKER = ~SPEAKER;
        while(TF1 == 0);
        TR1 = 0;
        TF1 = 0;        
      }
		}
    TR0 = 0;
		TF0 = 0;
    LED =1;
	}
  timerover(beats[0]); //articuation pause
}

/* 
 * Song number 1 Hot Cross Buns
 */
void playSong1()
{
	//My attempt to make hot cross buns.
	timerover(SEC);
		playNote(notes[4], 2); // quarter note
    playNote(notes[2], 2); // quarter note
    playNote(notes[0], 3);
    
    playNote(notes[4], 2);
    playNote(notes[2], 2);
    playNote(notes[0], 3);
    
    playNote(notes[0], 1);
    playNote(notes[0], 1);
    playNote(notes[0], 1);
    playNote(notes[0], 1);
    playNote(notes[2], 1);
    playNote(notes[2], 1);
    playNote(notes[2], 1);
    playNote(notes[2], 1);
    
    playNote(notes[4], 2);
    playNote(notes[2], 2);
    playNote(notes[0], 3);
    timerover(SEC);
}


void main()
{
	P1M1 = 0x00;
  P2M1 = 0x00;
	EA = 1;
	//to remove warnings**
	//uart_init();
	//uart_transmit('a');
	//uart_get();
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
    playSong1();
    
    switch(mode)
    {
      case 0:
        break;
      case 1:
        break;
      case 2:
        break;
      case 3:
        break;
      case 4:
        break;
    }
	}
}

