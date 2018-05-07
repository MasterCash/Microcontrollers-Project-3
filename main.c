//#include <reg51.h>
#include <reg932.h>
#include "uart.h"

//variables
unsigned char mode = 0;
sbit LED = P2^4;
sbit SPEAKER = P1^7;
sbit BTN1 = P2^0;
sbit BTN2 = P0^1;
sbit BTN3 = P2^3;


sbit LED1 = P3^0;
sbit LED2 = P3^1;

code unsigned char HotxBuns[] = "Hot Cross Buns";
code unsigned char Tetris[] = "Tetris Theme";

code unsigned int notes4[] = 
{
  7045,
  6649,
  6276,
  5924,
  5591,
  5278,
  4981,
  4702,
  4438,
  4189,
  3954,
  3732
};

code unsigned int notes5[] = 
{
  3522, //C 0
  3324, //C# 1
  3138, //D 2
  2962, //D# 3
  2795, //E 4
  2639, //F 5
  2490, //F# 6
  2351, //G 7
  2219, //G# 8
  2094, //A 9
  1977, //A# 10
  1866 //B 11
};


//1 = eighth note, 2 = quarter note, 3 = half note, 4 = whole note, 0 = articuation pause bpm = 120
code unsigned int beats[] =
{
  163,    // articuation pause !!Don't use this!! Internal only
  3932,   // Eighth Note
  8028,  // Quarter Note
  16220,  // Half Note
  32605,   // Whole Note
  163,
  3407,
  6668,
  13500,
  27165
};

unsigned int SEC = 16384;

void updateMode()
{
  LED1 = 0;
  LED2 = 0;
  switch(mode)
  {
    case 1: 
      LED1 = 1;
      break;
    case 2:
      LED2 = 1;
      break;
    case 3:
      LED1 = 1;
      LED2 = 1;
      break;
    case 4:
      break;
  }
}

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

void holdNote(unsigned int note)
{
  SPEAKER = 0;
  // hopefully this works right
  TH1 = -note >> 8;
  TL1 = -note;
  TR1 = 1;
  // toggle speaker
  SPEAKER = 1;
  while(TF1 == 0);
  SPEAKER = 0;
  TR1 = 0;
  TF1 = 0;        
}

/*
 * Uses Timer 0 AND Timer 1 to play a note 
 * assumes that both timers are not in use
 * note is preload for square wave for that frequency of that note
 * type is the type of note: 1 = eighth note, 2 = quarter note, 3 = half note, 4 = whole note
 */
void playNote(unsigned int note, unsigned char type, bit tempo)
{
	// looping over causes imprecise timing due to loop nature 
	unsigned char loop = 225;
	unsigned int timerLoad = (tempo == 0 ? beats[type] : beats[type + 4]);

	//run the pause.
	for(; loop > 0; loop--)
	{
		TH0 = -timerLoad >> 8;
		TL0 = -timerLoad;
		TR0 = 1;
		//run while timer goes, speaker may slightly change.
    SPEAKER = 0;
		while(TF0 == 0)
		{
      if(note > 0)
        holdNote(note);
		}
    TR0 = 0;
		TF0 = 0;
	}
  timerover(beats[0]); //articuation pause
}


void song1Repeat()
{
  playNote(notes4[4], 2, 0); // quarter note
  playNote(notes4[2], 2, 0); // quarter note
  playNote(notes4[0], 3, 0);
}

/* 
 * Song number 1 Hot Cross Buns
 */
void playSong1()
{
	//My attempt to make hot cross buns.
	//timerover(SEC);
  
  song1Repeat();
  
  song1Repeat();
  
  playNote(notes4[0], 1, 0);
  playNote(notes4[0], 1, 0);
  playNote(notes4[0], 1, 0);
  playNote(notes4[0], 1, 0);
  playNote(notes4[2], 1, 0);
  playNote(notes4[2], 1, 0);
  playNote(notes4[2], 1, 0);
  playNote(notes4[2], 1, 0);
  
  song1Repeat();
  
  timerover(SEC);
}

void playSong2()
{
  playNote(notes5[4], 2, 1);
  playNote(notes4[11], 1, 1);
  playNote(notes5[0], 1, 1);
  playNote(notes5[2], 2, 1);
  playNote(notes5[0], 1, 1);
  playNote(notes4[11], 1, 1);

  playNote(notes4[9], 2, 1);
  playNote(notes4[9], 1, 1);
  playNote(notes5[0], 1, 1);
  playNote(notes5[4], 2, 1);
  playNote(notes5[2], 1, 1);
  playNote(notes5[0], 1, 1);

  playNote(notes4[11], 2, 1);
  playNote(0, 1, 1);
  playNote(notes5[0], 1, 1);
  playNote(notes5[2], 2, 1);
  playNote(notes5[4], 2, 1);

  playNote(notes5[0], 2, 1);
  playNote(notes4[9], 2, 1);
  playNote(notes4[9], 3, 1);
  timerover(SEC);
}

void keyboard()
{
  if(BTN1 == 0)
    holdNote(notes4[0]);
  else if(BTN2 == 0)
    holdNote(notes4[2]);
  else if(BTN3 == 0)
    holdNote(notes4[4]);
}

void displaySong(unsigned char song[], unsigned char size)
{
  unsigned char k = 0;
  for(;k < size; k++)
  {
   uart_transmit(song[k]); 
  }
  uart_transmit('\r');
  uart_transmit('\n');       
}

void ExternInterrupt() interrupt 0 
{
    mode++;
    updateMode();
    timerover(SEC);
    timerover(SEC);
}



void main()
{
  
  unsigned char k = 0;
	P1M1 = 0x04;
  //P1M2 = 0x00;
  //P2M2 = 0x00;
  P2M1 = 0x00;
  //P0M2 = 0x00;
  P0M1 = 0x00;
  P3M1 = 0x00;
  TMOD = 0x11;
  EX0 = 1;
  mode = 0;
  
	EA = 1;
  uart_init();
  
	//to remove warnings**
	//uart_get();
	//uart_transmit('a');
	//********************
	/**********************
		TO-DO
		-------------------
		[X]Play two Short Tunes(~4 seconds long)
		[X]Display Song Title of Each on PC Term via serial
		[X]Make keyboard with atleast 3 buttons like keys
		[]Use 1-2 buttons to control mode of opp(options above are modes)
      - must be able to change modes in middle of stored tune is playing
		[X]use 8051 ports to connect a secondary device via breadboard to do some opp
		  -Using seven-segment display to show mode
      - Idea: display of LEDs for different notes4
		[]Each Member had their own feature.
    []
	**********************/
	while(1)
	{
   
    //updateMode LEDs and 7-segment Display
    mode++;
    updateMode();
    //controls what mode we do.
    switch(mode)
    {
      case 0:
        break;
      case 1:
        EX0 = 0;
        displaySong(HotxBuns, 14);
        EX0 = 1;
        playSong1();
       //mode++;
        break;
      case 2:
        break;
      case 3:
        EX0 = 0;
        displaySong(Tetris, 12);
        playSong2();
        //mode++;
        break;
      case 4:
        break;
      case 5:
        keyboard();
        break;
    }
	}
}


