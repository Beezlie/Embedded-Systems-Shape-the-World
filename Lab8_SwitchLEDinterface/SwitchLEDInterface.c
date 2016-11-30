// ***** 0. Documentation Section *****
// SwitchLEDInterface.c for Lab 8
// Runs on LM4F120/TM4C123
// Use simple programming structures in C to toggle an LED
// while a button is pressed and turn the LED on when the
// button is released.  This lab requires external hardware
// to be wired to the LaunchPad using the prototyping board.
// January 15, 2016
//      Jon Valvano and Ramesh Yerraballi

// ***** 1. Pre-processor Directives Section *****
#include "TExaS.h"
#include "tm4c123gh6pm.h"
#define GPIO_PORTE_DATA_R       (*((volatile unsigned long *)0x400243FC))
#define GPIO_PORTE_DIR_R        (*((volatile unsigned long *)0x40024400))
#define GPIO_PORTE_AFSEL_R      (*((volatile unsigned long *)0x40024420))
#define GPIO_PORTE_PUR_R        (*((volatile unsigned long *)0x40024510))
#define GPIO_PORTE_DEN_R        (*((volatile unsigned long *)0x4002451C))
#define GPIO_PORTE_LOCK_R       (*((volatile unsigned long *)0x40024520))
#define GPIO_PORTE_CR_R         (*((volatile unsigned long *)0x40024524))
#define GPIO_PORTE_AMSEL_R      (*((volatile unsigned long *)0x40024528))
#define GPIO_PORTE_PCTL_R       (*((volatile unsigned long *)0x4002452C))
#define PE0							        (*((volatile unsigned long *)0x40024004))
#define PE1							        (*((volatile unsigned long *)0x40024008))
#define SYSCTL_RCGC2_R          (*((volatile unsigned long *)0x400FE108))
#define SYSCTL_RCGC2_GPIOE      0x00000010  // port E Clock Gating Control
unsigned long delay;
	
// ***** 2. Global Declarations Section *****

// FUNCTION PROTOTYPES: Each subroutine defined
void PortE_Init(void); // Initialize Port E
void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
void DelayMs(unsigned long msec); // Delay for a specified number of miliseconds

// ***** 3. Subroutines Section *****

// PE0 connected to positive logic momentary switch using 10k ohm pull down resistor
// PE1 connected to positive logic LED through 470 ohm current limiting resistor
// To avoid damaging your hardware, ensure that your circuits match the schematic
// shown in Lab8_artist.sch (PCB Artist schematic file) or 
// Lab8_artist.pdf (compatible with many various readers like Adobe Acrobat).
int main(void){ 
//**********************************************************************
// The following version tests input on PE0 and output on PE1
//**********************************************************************
  TExaS_Init(SW_PIN_PE0, LED_PIN_PE1, ScopeOn);  // activate grader and set system clock to 80 MHz
	PortE_Init();
	PE1 |= 0x02;		// turn the LED on
	
  EnableInterrupts();           // enable interrupts for the grader
  while(1){
    DelayMs(100);
		if (PE0 != 0x00) {
			PE1 ^= 0x02;							// toggle the LED if switch pressed
		} else {
			PE1 |= 0x02;							// turn on the LED if switch not pressed
		}
  }
}

void PortE_Init(void) {
	SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOE;     // Port E clock
  delay = SYSCTL_RCGC2_R;           				// wait 3-5 bus cycles
  GPIO_PORTE_DIR_R |= 0x02;        				  // PE1 output
  GPIO_PORTE_DIR_R &= ~0x01;        			  // PE0 input 
  GPIO_PORTE_AFSEL_R &= ~0x03;      				// not alternative
  GPIO_PORTE_AMSEL_R &= ~0x03;     				  // no analog
  GPIO_PORTE_PCTL_R &= ~0x000000FF; 				// bits for PE1,PE0
  GPIO_PORTE_DEN_R |= 0x03;         				// enable PE1,PE0
}

void DelayMs(unsigned long msec) {
	unsigned long i;
  while(msec > 0){
    i = 13333;  // approximately 1 ms (1ms/12.5ns/6)
    while(i > 0){
      i = i - 1;
    }
    msec = msec - 1; // decrements every 100 ms
  }
}
