// TuningFork.c Lab 12
// Runs on LM4F120/TM4C123
// Use SysTick interrupts to create a squarewave at 440Hz.  
// There is a positive logic switch connected to PA3, PB3, or PE3.
// There is an output on PA2, PB2, or PE2. The output is 
//   connected to headphones through a 1k resistor.
// The volume-limiting resistor can be any value from 680 to 2000 ohms
// The tone is initially off, when the switch goes from
// not touched to touched, the tone toggles on/off.
//                   |---------|               |---------|     
// Switch   ---------|         |---------------|         |------
//
//                    |-| |-| |-| |-| |-| |-| |-|
// Tone     ----------| |-| |-| |-| |-| |-| |-| |---------------
//
// Daniel Valvano, Jonathan Valvano
// January 15, 2016

/* This example accompanies the book
   "Embedded Systems: Introduction to ARM Cortex M Microcontrollers",
   ISBN: 978-1469998749, Jonathan Valvano, copyright (c) 2015

 Copyright 2016 by Jonathan W. Valvano, valvano@mail.utexas.edu
    You may use, edit, run or distribute this file
    as long as the above copyright notice remains
 THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 VALVANO SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL,
 OR CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 For more information about my classes, my research, and my books, see
 http://users.ece.utexas.edu/~valvano/
 */


#include "TExaS.h"
#include "..//tm4c123gh6pm.h"

#define buzzerStatus (*(volatile unsigned long *)0x40004010) // PA2
#define switchStatus (*(volatile unsigned long *)0x40004020) // PA3


// basic functions defined at end of startup.s
void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
void WaitForInterrupt(void);  // low power mode

unsigned long H, L;  // High and low values for the duty cycle
unsigned long currSwitch; // State of the switch
unsigned long prevSwitch; // Previous state of the switch

// input from PA3, output from PA2, SysTick interrupts
void Sound_Init(void){ 
	SYSCTL_RCGC2_R |= 0x00000001; // activate clock for port A
  L = H = 90908;                // high/low oscillate at 880 Hz - (1/880Hz)/(12.5ns)-1 = 90908
  GPIO_PORTA_AMSEL_R &= ~0x0C;      // disable analog functionality on PA3, PA2
  GPIO_PORTA_PCTL_R &= ~0x0000FF00; // configure PA3, PA2 as GPIO
  GPIO_PORTA_DIR_R &= ~0x08;     // make PA3 input
	GPIO_PORTA_DIR_R |= 0x04;     // make PA2 output
  GPIO_PORTA_AFSEL_R &= ~0x0C;  // disable alt funct on PA3, PA2
  GPIO_PORTA_DEN_R |= 0x0C;     // enable digital I/O on PA3, PA2
  GPIO_PORTA_DATA_R &= ~0x04;   // make PA2 low
  NVIC_ST_CTRL_R = 0;           // disable SysTick during setup
  NVIC_ST_RELOAD_R = L;       // reload value for 880 HZ (1.13636ms)
  NVIC_ST_CURRENT_R = 0;        // write to current to clear it
  NVIC_SYS_PRI3_R = NVIC_SYS_PRI3_R&0x00FFFFFF; // priority 0      
  NVIC_ST_CTRL_R = 0x07;  // enable with core clock and interrupts
}

// called at 880 Hz
void SysTick_Handler(void){
	// Oscillate the signal at 880 Hz if switch is pressed, else output low
	if((prevSwitch == 0x00 && switchStatus != 0x00) || (prevSwitch != 0x00 && switchStatus == 0x00))
		currSwitch ^= 0x08; // Toggle current state
		prevSwitch = currSwitch; // set previous state to current state
	if(currSwitch != 0x00)
		buzzerStatus ^= 0x04; 	// Turn on buzzer
	else
		buzzerStatus = 0x00; // Turn off buzzer
}

int main(void){// activate grader and set system clock to 80 MHz
	DisableInterrupts();   // disabe interrupts before initialization
  TExaS_Init(SW_PIN_PA3, HEADPHONE_PIN_PA2,ScopeOn); 
  Sound_Init();         
  EnableInterrupts();   // enable after all initialization are done
	prevSwitch = 0x00;
	currSwitch = 0x00;
  while(1){
    // main program is free to perform other tasks
    // do not use WaitForInterrupt() here, it may cause the TExaS to crash
  }
}
