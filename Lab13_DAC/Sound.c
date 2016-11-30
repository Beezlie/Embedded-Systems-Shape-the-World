// Sound.c
// Runs on LM4F120 or TM4C123, 
// edX lab 13 
// Use the SysTick timer to request interrupts at a particular period.
// Daniel Valvano, Jonathan Valvano
// December 29, 2014
// This routine calls the 4-bit DAC

#include "Sound.h"
#include "DAC.h"
#include "..//tm4c123gh6pm.h"

// basic functions defined at end of startup.s
void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts

// Initialize global variables
const int SineWave[64] = // sine wave ranging for 0 to 15 with 256 samples
{8,8,9,10,10,11,12,12,13,13,14,14,14,15,15,15,15,15,15,15,14,14,14,13,13,12,11,11,10,9,
9,8,7,6,6,5,4,4,3,2,2,1,1,1,0,0,0,0,0,0,0,1,1,1,2,2,3,3,4,5,5,6,7,8};
unsigned int Index=0;           // Index varies from 0 to 64

// **************Sound_Init*********************
// Initialize Systick periodic interrupts
// Also calls DAC_Init() to initialize DAC
// Input: none
// Output: none
void Sound_Init(void){
  DAC_Init();          // Port B is DAC
	NVIC_ST_CTRL_R = 0;         // disable SysTick during setup
  NVIC_ST_RELOAD_R = 0;// reload value
  NVIC_ST_CURRENT_R = 0;      // any write to current clears it
  NVIC_SYS_PRI3_R = NVIC_SYS_PRI3_R&0x00FFFFFF; // priority 0
  NVIC_ST_CTRL_R = 0x0007; // enable,core clock, and interrupts
}

// **************Sound_Tone*********************
// Change Systick periodic interrupts to start sound output
// Input: interrupt period
//           Units of period are 12.5ns
//           Maximum is 2^24-1
//           Minimum is determined by length of ISR
// Output: none
void Sound_Tone(unsigned long period){
// this routine sets the RELOAD and starts SysTick
	EnableInterrupts();
	NVIC_ST_CTRL_R = 0; // disable SysTick
	Index = 0; // reset sine wave
  NVIC_ST_RELOAD_R = period-1;// reload value
  NVIC_ST_CTRL_R = 0x07; // enable SysTick
}


// **************Sound_Off*********************
// stop outputing to DAC
// Output: none
void Sound_Off(void){
 // this routine stops the sound output
	DisableInterrupts();
}


// Interrupt service routine
// Executed every 12.5ns*(period)
void SysTick_Handler(void){
  Index = (Index+1)%64;      // 4,5,6,7,7,7,6,5,4,3,2,1,1,1,2,3... 
  DAC_Out(SineWave[Index]);    // output one value each interrupt
}
