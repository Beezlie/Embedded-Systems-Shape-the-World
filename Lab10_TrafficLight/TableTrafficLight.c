// ***** 0. Documentation Section *****
// TableTrafficLight.c for Lab 10
// Runs on LM4F120/TM4C123
// Index implementation of a Moore finite state machine to operate a traffic light.  
// Daniel Valvano, Jonathan Valvano
// January 15, 2016

// east/west red light connected to PB5
// east/west yellow light connected to PB4
// east/west green light connected to PB3
// north/south facing red light connected to PB2
// north/south facing yellow light connected to PB1
// north/south facing green light connected to PB0
// pedestrian detector connected to PE2 (1=pedestrian present)
// north/south car detector connected to PE1 (1=car present)
// east/west car detector connected to PE0 (1=car present)
// "walk" light connected to PF3 (built-in green LED)
// "don't walk" light connected to PF1 (built-in red LED)

// ***** 1. Pre-processor Directives Section *****
#include "TExaS.h"
#include "tm4c123gh6pm.h"

// ***** 2. Global Declarations Section *****
#define TRAFFICLIGHTS   (*((volatile unsigned long *)0x400050FC)) // PB5-PB0
#define SWITCHES 			  (*((volatile unsigned long *)0x4002401C)) // PE2-PE0
#define CROSSINGLIGHTS  (*((volatile unsigned long *)0x40025028)) // PF3 and PF1
#define goWest   0
#define waitWest 1
#define goSouth   2
#define waitSouth 3
#define walk   4
#define hurryFlashOne 5
#define hurryOffOne   6
#define hurryFlashTwo 7
#define hurryOffTwo   8

struct TrafficState {
	unsigned long trafficOutput;
  unsigned long crossingOutput;
	unsigned long wait;
	unsigned long next[10];
};

typedef const struct TrafficState StateType;

// state transition table consisting of all outputs, delays, and state transitions
StateType fsm[9] = {
	{0x0C, 0x02, 100, {goWest, goWest, waitWest, waitWest, waitWest, waitWest, waitWest, waitWest}},
	{0x14, 0x02, 100, {goWest, goWest, goSouth, goSouth, walk, goSouth, goSouth, goSouth}},
	{0x21, 0x02, 100, {goSouth, waitSouth, goSouth, waitSouth, waitSouth, waitSouth, waitSouth, waitSouth}},
	{0x22, 0x02, 100, {goSouth, goWest, goSouth, goWest, walk, walk, walk, walk}},
	{0x24, 0x08, 100, {walk, hurryFlashOne, hurryFlashOne, hurryFlashOne, walk, hurryFlashOne, hurryFlashOne, hurryFlashOne}},
	{0x24, 0x02, 50, {hurryOffOne, hurryOffOne, hurryOffOne, hurryOffOne, hurryOffOne, hurryOffOne, hurryOffOne, hurryOffOne}},
	{0x24, 0x00, 50, {hurryFlashTwo, hurryFlashTwo, hurryFlashTwo, hurryFlashTwo, hurryFlashTwo, hurryFlashTwo, hurryFlashTwo, hurryFlashTwo}},
	{0x24, 0x02, 50, {hurryOffTwo, hurryOffTwo, hurryOffTwo, hurryOffTwo, hurryOffTwo, hurryOffTwo, hurryOffTwo, hurryOffTwo}},
	{0x24, 0x00, 50, {walk, goWest, goSouth, goWest, walk, goWest, goSouth, goWest}}
};

unsigned long S;  // index to the current state 
unsigned long Input; 

// FUNCTION PROTOTYPES: Each subroutine defined
void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
void PortB_Init(void); // Initialize PortB
void PortE_Init(void); // Initialize PortE
void PortF_Init(void); // Initialize PortF
void SysTick_Init(void); // Initialize SysTick timer
void SysTick_Wait(unsigned long delay); // SysTick Counter loaded with delay value
void SysTick_Wait10ms(unsigned long delay);

// ***** 3. Subroutines Section *****

int main(void){ 
  TExaS_Init(SW_PIN_PE210, LED_PIN_PB543210,ScopeOff); // activate grader and set system clock to 80 MHz
	PortB_Init();
	PortE_Init();
	PortF_Init();
	SysTick_Init();
 
  EnableInterrupts();
	S = goWest;		// set initial state
	
  while(1){
    TRAFFICLIGHTS = fsm[S].trafficOutput;  // set traffic lights PB5-PB0
		
		CROSSINGLIGHTS = fsm[S].crossingOutput;  // set crossing lights PF3 (green) and PF1 (red)
		SysTick_Wait10ms(fsm[S].wait); 
    Input = SWITCHES;     // read switches
    S = fsm[S].next[Input];  // transition to next state
  }
}

void PortB_Init(void) {
	volatile unsigned long delay;
	SYSCTL_RCGC2_R |= 0x00000002;     // Activate clock for Port F
  delay = SYSCTL_RCGC2_R;           // Allow time for clock to start
	GPIO_PORTB_AMSEL_R &= ~0x3F; // 3) disable analog function on PB5-0
  GPIO_PORTB_PCTL_R &= ~0x00FFFFFF; // 4) enable regular GPIO
  GPIO_PORTB_DIR_R |= 0x3F;    // 5) outputs on PB5-0
  GPIO_PORTB_AFSEL_R &= ~0x3F; // 6) regular function on PB5-0
  GPIO_PORTB_DEN_R |= 0x3F;    // 7) enable digital on PB5-0
}

void PortE_Init(void) {
	volatile unsigned long delay;
	SYSCTL_RCGC2_R |= 0x00000010;     // Activate clock for Port F
  delay = SYSCTL_RCGC2_R;           // Allow time for clock to start
	GPIO_PORTE_AMSEL_R &= ~0x07; // 3) disable analog function on PE2-0
  GPIO_PORTE_PCTL_R &= ~0x00000FFF; // 4) enable regular GPIO
  GPIO_PORTE_DIR_R &= ~0x07;   // 5) inputs on PE2-0
  GPIO_PORTE_AFSEL_R &= ~0x07; // 6) regular function on PE2-0
  GPIO_PORTE_DEN_R |= 0x07;    // 7) enable digital on PE2-0
}

void PortF_Init(void) {
	volatile unsigned long delay;
	SYSCTL_RCGC2_R |= 0x00000020;     // Activate clock for Port F
  delay = SYSCTL_RCGC2_R;           // Allow time for clock to start
  GPIO_PORTF_AMSEL_R = 0x00;        // Disable analog on PF3 and PF1
  GPIO_PORTF_PCTL_R = 0x00000000;   // PCTL GPIO on PF4-0
  GPIO_PORTF_DIR_R |= 0x08;        //   PF3 output
	GPIO_PORTF_DIR_R |= 0x02;         //   PF1 output
  GPIO_PORTF_AFSEL_R &= ~0x0A;      // Disable alt funct on PF3 and PF1
  GPIO_PORTF_DEN_R |= 0x0A;         // Enable digital I/O on PF3 and PF1
}

void SysTick_Init(void) {
	NVIC_ST_CTRL_R = 0;                   // disable SysTick during setup
  NVIC_ST_RELOAD_R = 0x00FFFFFF;        // maximum reload value
  NVIC_ST_CURRENT_R = 0;                // any write to current clears it             
  NVIC_ST_CTRL_R = 0x00000005;          // enable SysTick with core clock
}

void SysTick_Wait(unsigned long delay) {
	NVIC_ST_RELOAD_R = delay-1;  // number of counts to wait
  NVIC_ST_CURRENT_R = 0;       // any value written to CURRENT clears
  while((NVIC_ST_CTRL_R&0x00010000)==0); // wait for count flag
}

// 800000*12.5ns equals 10ms
void SysTick_Wait10ms(unsigned long delay){
  unsigned long i;
  for(i=0; i<delay; i++){
    SysTick_Wait(800000);  // wait 10ms
  }
}


