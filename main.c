//#include <reg51.h>
#include <reg932.h>
#include "uart.h"



//**************GLOBAL*VARIABLES*****************
// Current mode of device.
unsigned char mode = 0;
// The variable of the speaker for sending sounds.
sbit SPEAKER = P1^7;
// First button of the Keyboard.
sbit BTN1 = P2^0;
// Second button of the Keyboard.
sbit BTN2 = P0^1;
// Third button of the Keyboard.
sbit BTN3 = P2^3;
// Flag to controll when to switch modes. (set by External Interrupt).
bit flag = 0;

sbit LEDC = P2^4;
sbit LEDD = P0^5;
sbit LEDE = P2^7;
sbit LEDF = P0^6;
sbit LEDG = P1^6;
sbit LEDA = P0^4;
sbit LEDB = P2^5;



// Control the mode displayed on the 7-segment display two bit number.
// Bit 1 of the number.
sbit DISPLAYCONTROL1 = P3^0;
// Bit 2 of the number.
sbit DISPLAYCONTROL2 = P3^1;

// Arrays of the character that make up the song names.
// Name for Song 1.
code unsigned char HotxBuns[] = "Hot Cross Buns";
// Name for Song 2.
code unsigned char Tetris[] = "Tetris Theme";

// Array of Arrays of characters for each note in an octative.
code unsigned char notes[12][2] ={"C ", "C#", "D ", "D#", "E ", "F ", "F#", "G ", "G#", "A ", "A#", "B "};

// Notes Preload values for Octative 4, C4-B4 (includes sharps).
code unsigned int notes4[] = {
  7045, // C   0
  6649, // C#  1
  6276, // D   2
  5924, // D#  3 
  5591, // E   4 
  5278, // F   5
  4981, // F#  6
  4702, // G   7
  4438, // G#  8
  4189, // A   9
  3954, // A# 10
  3732  // B  11
};

// Notes Preload values for Octative 5, C5-B5 (includes sharps).
code unsigned int notes5[] = {
  3522, // C   0
  3324, // C#  1
  3138, // D   2
  2962, // D#  3
  2795, // E   4
  2639, // F   5
  2490, // F#  6
  2351, // G   7
  2219, // G#  8
  2094, // A   9
  1977, // A# 10
  1866  // B  11
};

// Tempo Control Preload values to "Play" a note for a certain type of note. AKA a quarter note.
// Add 4 to index to switch from bps = 120 to bps = 144.
//1 = Eighth note, 2 = Quarter note, 3 = Half note, 4 = Whole note, 0 = Articuation pause.
code unsigned int beats[] = {
  163,    // Articuation pause !!Don't use this!! Internal only.
  3932,   // Eighth Note.
  8028,   // Quarter Note.
  16220,  // Half Note.
  32605,  // Whole Note.
  163,    // Articuation pause !!Don't use this!! Internal only.
  3407,   // Eighth Note.
  6668,   // Quarter Note.
  13500,  // Half Note.
  27165   // Whole Note.
};

// Timer Wait Values.
// One Second Wait Preload Value.
unsigned int SEC = 16384;
// 0.2 Second Wait Preload Value for Multiple Button Press Shielding.
unsigned int BUTTONPAUSE = 3276;
//**************END*OF*VARIABLES*****************


//*****************FUNCTIONS*********************

/*
 * Discription: Updates the 7-Segment Display to the new mode. 
 * Requires: None.
 * Returns: None.
 */
void updateMode() {
  // Start off at Zero.
  DISPLAYCONTROL1 = 0;
  DISPLAYCONTROL2 = 0;
  // Turn on bits to make number in binary.
  switch(mode) {
    case 1: 
      DISPLAYCONTROL1 = 1;
      break;
    case 2:
      DISPLAYCONTROL2 = 1;
      break;
    case 3:
      DISPLAYCONTROL1 = 1;
      DISPLAYCONTROL2 = 1;
      break;
    default:
      // If it is not Case 1-3 Then it should be Zero (this keeps the mode in a loop).
      mode = 0;
      break;
  }
}

/*
 * Discription: Sends a string via Serial Communication via UART to be displayed on terminal. 
 * Requires: Array of characters to be sent and size of that array.
 * Returns: None.
 */
void display(unsigned char text[], unsigned char size) {
  unsigned char k = 0;
  // Loop through each character in the array.
  if(flag) return;
  for(;k < size; k++) {
    // Transmit the character.
    uart_transmit(text[k]); 
  } 
}

/*
 * Discription: Display Function That will send the Song name given via UART to Terminal.
 * Requires: Array of characters of song name and size of the array.
 * Returns: None.
 */
void displaySong(unsigned char song[], unsigned char size) {
  // Display Song
  display(song, size);
  // Move to the next line
  uart_transmit('\r');
  uart_transmit('\n');       
}

/*
 * Discription: Turns off all the LEDs for notes.
 * Requires: None.
 * Returns: None.
 */
void shutOffLED() { LEDA = LEDB = LEDC = LEDD = LEDE = LEDF = LEDG = 1; }

/*
 * Discription: Turns on the LED corresponding to the note being played.
 * Requires: The index of the note.
 * Returns: None.
 */
void turnOnLED(unsigned char note) {
  // Turn off LEDs.
  shutOffLED();
  // Turn on the light corresponding to the note.
  switch(note) {
    case 0:
    case 1:
      LEDC = 0;
      break;
    case 2:
    case 3:
      LEDD = 0;
      break;
    case 4:
      LEDE = 0;
      break;
    case 5:
    case 6:
      LEDF = 0;
      break;
    case 7:
    case 8:
      LEDG = 0;
      break;
    case 9:
    case 10:
      LEDA = 0;
      break;
    case 11:
      LEDB = 0;
      break;
  }
}

/*
 * Discription: Display Function to display a note being played via UART to Terminal.
 * Requires: index of the note being played. valid if between 0 - 11.
 * Returns: None.
 */
void displayNote(unsigned char note) { 
  
  display(notes[note], 2); 
  turnOnLED(note);
  
}

/*
 * Discription: Expects Timer 0 to not be in use. For Timer Load Values greater than 65535.
 * Requires: Timer Load Value that the actual disired value is 225 * value but value is less than 16 bit size.
 * Returns: None.
 */
void timerover(unsigned int t) {
	// Looping over causes inexact timing due to loop nature. 
	unsigned char loop = 225;
	// Make sure not to overide the Timer1 settings.
	//(Hopefully this makes sure it doesn't mess up if it is running).
	unsigned int currentTMOD = TMOD >> 4 << 4;
	// Attach the timer 0 mode we want.
	currentTMOD += 0x01;
	// Set TMOD to that mode.
	TMOD = currentTMOD;
	// Run the pause.
	for(; loop > 0; loop--) {
		TH0 = -t >> 8;
		TL0 = -t;
		TR0 = 1;
		while(TF0 == 0);
    TR0 = 0;
		TF0 = 0;
	}
}

/*
 * Discription: Does one Cycle of playing a note at a Frequency. Repeat to create a audiable sound.
 * Requires: A Preload Value for a note that is less than 16 bit and is already set to 1/2 Duty.
 * Returns: None.
 */
void holdNote(unsigned int note) {
  // Make sure speaker is off.
  SPEAKER = 0;
  // Load the note value up.
  TH1 = -note >> 8;
  TL1 = -note;
  TR1 = 1;
  // Turn On SPEAKER.
  SPEAKER = 1;
  while(TF1 == 0);
  // Turn Off SPEAKER.
  SPEAKER = 0;
  TR1 = 0;
  TF1 = 0;        
}

/*
 * Discription: Uses Timer 0 AND Timer 1 to play a note.
 *** assumes that both timers are not in use.
 *** note is preload for square wave for that frequency of that note.
 *** type is the type of note: 1 = eighth note, 2 = quarter note, 3 = half note, 4 = whole note.
 * Requires: A Note Preload Value that is less than 16 bit and is already set to 1/2 Duty.
 *** The type of note to be playing and the tempo to be set at.
 * Returns: None.
 */
void playNote(unsigned int note, unsigned char type, bit tempo) {
	// Looping over causes imprecise timing due to loop nature.
	unsigned char loop = 225;
  // Set the tempo.
	unsigned int timerLoad = (tempo == 0 ? beats[type] : beats[type + 4]);

	// Run the pause.
	for(; loop > 0; loop--) {
		TH0 = -timerLoad >> 8;
		TL0 = -timerLoad;
		TR0 = 1;
		// Run while timer goes, speaker may slightly change.
		while(TF0 == 0)	{
      // Check to see if we are playing a rest ( note = 0) or if the mode change flag has been set.
      if(note > 0 && !flag)
        holdNote(note);
      // If the mode change flag has been set, leave the playNote Function.
      if(flag)
        return;
		}
    TR0 = 0;
		TF0 = 0;
	}
  
  // Articulation Pause.
  timerover(beats[0]); 
}

/*
 * Discription: Function to play redunant tune for Song 1.
 * Requires: None.
 * Returns: None.
 */
void song1Repeat() {
  displayNote(4);
  playNote(notes4[4], 2, 0); 
  displayNote(2);
  playNote(notes4[2], 2, 0); 
  displayNote(0);
  playNote(notes4[0], 3, 0);
}

/*
 * Discription: Song #1 - Hot Cross Buns.
 * Requires: None.
 * Returns: None.
 */
void playSong1() { 
  // Measure 1.
  song1Repeat();
  // Measure 2.
  song1Repeat();
  // Measure 3.
  displayNote(0);
  playNote(notes4[0], 1, 0);
  displayNote(0);
  playNote(notes4[0], 1, 0);
  displayNote(0);
  playNote(notes4[0], 1, 0);
  displayNote(0);
  playNote(notes4[0], 1, 0);
  displayNote(2);
  playNote(notes4[2], 1, 0);
  displayNote(2);
  playNote(notes4[2], 1, 0);
  displayNote(2);
  playNote(notes4[2], 1, 0);
  displayNote(2);
  playNote(notes4[2], 1, 0);
  // Measure 4.
  song1Repeat();
  // Only Delay the Next Play of the song if We are not switching modes.
  if(!flag)
    timerover(SEC);
}


 /*
 * Discription: Song #2 - Tetris Theme.
 * Requires: None.
 * Returns: None.
 */
void playSong2() {
  // Measure 1.
  displayNote(4);
  playNote(notes5[4], 2, 1);
  displayNote(11);
  playNote(notes4[11], 1, 1);
  displayNote(0);
  playNote(notes5[0], 1, 1);
  displayNote(2);
  playNote(notes5[2], 2, 1);
  displayNote(0);
  playNote(notes5[0], 1, 1);
  displayNote(11);
  playNote(notes4[11], 1, 1);
  // Measure 2.
  displayNote(9);
  playNote(notes4[9], 2, 1);
  displayNote(9);
  playNote(notes4[9], 1, 1);
  displayNote(0);
  playNote(notes5[0], 1, 1);
  displayNote(4);
  playNote(notes5[4], 2, 1);
  displayNote(2);
  playNote(notes5[2], 1, 1);
  displayNote(0);
  playNote(notes5[0], 1, 1);
  // Measure 3.
  displayNote(11);
  playNote(notes4[11], 2, 1);
  playNote(0, 1, 1);
  displayNote(0);
  playNote(notes5[0], 1, 1);
  displayNote(2);
  playNote(notes5[2], 2, 1);
  displayNote(4);
  playNote(notes5[4], 2, 1);
  // Measure 4.
  displayNote(0);
  playNote(notes5[0], 2, 1);
  displayNote(9);
  playNote(notes4[9], 2, 1);
  displayNote(9);
  playNote(notes4[9], 3, 1);
  // Measure 5.
  playNote(0, 1, 1);
  displayNote(11);
  playNote(notes4[11], 2, 1);
  displayNote(5);
  playNote(notes5[5], 1, 1);
  displayNote(9);
  playNote(notes5[9], 2, 1);
  displayNote(7);
  playNote(notes5[7], 1, 1);
  displayNote(5);
  playNote(notes5[5], 1, 1);
  // Measure 6.
  displayNote(4);
  playNote(notes5[4], 2, 1);
  playNote(0, 1, 1);
  displayNote(0);
  playNote(notes5[0], 1, 1);
  displayNote(4);
  playNote(notes5[4], 2, 1);
  displayNote(2);
  playNote(notes5[2], 1, 1);
  displayNote(0);
  playNote(notes5[0], 1, 1);
  // Measure 7.
  displayNote(11);
  playNote(notes4[11], 2, 1);
  displayNote(11);
  playNote(notes4[11], 1, 1);
  displayNote(0);
  
  playNote(notes5[0], 1, 1);
  // Only Delay the Next Play of the song if We are not switching modes.
  if(!flag)
    timerover(SEC);
}

/*
 * Discription: Keyboard Function. Allows user to use the first row of buttons to play notes. 
 * Requires: None.
 * Returns: None.
 */
void keyboard() {
  // Keep looping till we change modes.
  while(!flag) {
    // Trigger note for corresponding button press. (Doesn't allow two to try and run at the same time).
    if(!BTN1)
    {
      turnOnLED(0);
      holdNote(notes5[0]);
    }
    else if(!BTN2)
    {
      turnOnLED(2);
      holdNote(notes5[2]);
    }
    else if(!BTN3)
    {
      turnOnLED(4);
      holdNote(notes5[4]);
    }
    shutOffLED();
  }
}

/*
 * Discription: Function to handle when the mode switch button has been pressed.
 *** Is wired up to recieve press signal from SW9 on the board.
 * Requires: EX1 to be enabled.
 * Returns: None.
 */
void ExternInterrupt() interrupt 2 {
    // Set our Global Flag to let everything else to gracefully switch modes.
    flag = 1;
    // Multi Button Protection.
    timerover(BUTTONPAUSE);
}

/*
 * Discription: Clears the Terminal Via UART Excape characters.
 * Requires: None.
 * Returns: None.
 */
void clearScreen() {
  // Google is my Friend.
  // Excape Character.
  uart_transmit(27);
  // Clear Screen Code.
  display("[1J", 3);
  // Excape Character.
  uart_transmit(27);
  // Curser to top Code.
  display("[H", 2);
}

/*
 * Discription: Keyboard Function 2. Allows User to use 'ASDF' as buttons to play notes via Serial Communication Via UART.
 * Requires: None.
 * Returns: None.
 */
void keyboard2() {
  // Stays here till mode changes.
  while(!flag) {
    // Get note to play and play the assigned note.
    switch(uart_get()) {
      case 'a': 
        turnOnLED(11);
        holdNote(notes4[11]);
        break;
      case 's':
        turnOnLED(0);
        holdNote(notes5[0]);
        break;
      case 'd':
        turnOnLED(2);
        holdNote(notes5[2]);
        break;
      case 'f':
        turnOnLED(4);
        holdNote(notes5[4]);
      default:
        break;
    }
    shutOffLED();
  }    
}
//*************END*OF*FUNCTIONS******************

//*******************MAIN************************
void main() {
  // Loop vairable.
  unsigned char k = 0;
  
  // Initialization.
	P1M1 = 0x00;
  P2M1 = 0x00;
  P0M1 = 0x00;
  P3M1 = 0x00;
  TMOD = 0x11;
  EX1 = 1;
  IT1 = 1;
  mode = 0;
	EA = 1;
  uart_init();
  
	/**********************
		TO-DO List
		-------------------
		[X]Play two Short Tunes(~4 seconds long).
		[X]Display Song Title of Each on PC Term via serial.
		[X]Make keyboard with atleast 3 buttons like keys.
		[X]Use 1-2 buttons to control mode of opp(options above are modes).
      - must be able to change modes in middle of stored tune is playing.
		[X]use 8051 ports to connect a secondary device via breadboard to do some opperation.
		  - Using seven-segment display to show mode.
		[X]Each Member had their own feature.
    [X] Comment Code.
	**********************/
  
	while(1) {
    // Handle flag triggered.
    if(flag) {
      flag = 0;
      mode++;  
    }
    // Update Mode and 7-segment Display.
    updateMode();
    // Clear Screen before each mode.
    clearScreen();
    // Controls what mode we do.
    switch(mode) {
      case 0:
        keyboard();
        break;
      case 1:
        displaySong(HotxBuns, 14);
        playSong1();
        shutOffLED();
        break;
      case 2:
        displaySong(Tetris, 12);
        playSong2();
        shutOffLED();     
        break;
      case 3:
        keyboard2();
        break;
    }
	}
}
//*******************EOF*************************
