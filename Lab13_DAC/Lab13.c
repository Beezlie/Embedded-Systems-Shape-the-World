// Lab13.c
// Runs on LM4F120 or TM4C123
// Use SysTick interrupts to implement a 4-key digital piano
// edX Lab 13 
// Daniel Valvano, Jonathan Valvano
// December 29, 2014
// Port B bits 3-0 have the 4-bit DAC
// Port E bits 3-0 have 4 piano keys

#include "..//tm4c123gh6pm.h"
#include "Sound.h"
#include "Piano.h"
#include "TExaS.h"

// Initialize global variables
const unsigned long pianoInputs[4] = {0x01, 0x02, 0x04, 0x08};		// input values of piano keys

// frequencies associated with notes to be set as Systick reload value
//Systick reload value = (bus freq / (64 DAC outputs per cycle)x(desired frequency))
// For example: 80 MHz / (64 x 523 Hz) = 2390
const int frequencies[4] = {2390, 2129, 1897, 1594};   // frequencies associated with notes to be set as Systick reload value

// basic functions defined at end of startup.s
void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
void delay(unsigned long msec);

int main(void){ // Real Lab13 
	// for the real board grader to work 
	// you must connect PD3 to your DAC output
	
	//initialize variables
	unsigned long prevNote;
	unsigned long note;
	unsigned long freqIndex;
	
  TExaS_Init(SW_PIN_PE3210, DAC_PIN_PB3210,ScopeOn); // activate grader and set system clock to 80 MHz
// PortE used for piano keys, PortB used for DAC        
  Sound_Init(); // initialize SysTick timer and DAC
  Piano_Init();
  EnableInterrupts();  // enable after all initialization are done
	
  while(1){                
// input from keys to select tone
		note = Piano_In(); // get input from keys
		if(prevNote != note) {
			if(note == 0x00) {
					Sound_Off();
			} else {
				freqIndex = 0;
				// get index associated with selected note
				while(pianoInputs[freqIndex] != note) {
					freqIndex++;
				}
				// output frequency associated with note
				Sound_Tone(frequencies[freqIndex]);
				delay(10);
			} 
		prevNote = note;
		}         
	}
}

// Inputs: Number of msec to delay
// Outputs: None
void delay(unsigned long msec){ 
  unsigned long count;
  while(msec > 0 ) {  // repeat while there are still delay
    count = 16000;    // about 1ms
    while (count > 0) { 
      count--;
    } // This while loop takes approximately 3 cycles
    msec--;
  }
}


