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
unsigned long S; //index of current state
unsigned long Input;

// FUNCTION PROTOTYPES: Each subroutine defined
void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
void PLL_Init(void);

// ***** 3. Subroutines Section *****
#define Sensor (*((volatile unsigned long *)0x4002401C))
	
//linked data structurs 
struct State {
	unsigned long Out1; // 6 bit traffic light pattern 
	unsigned long Out2; // walk light 
	unsigned long Time; // delay in 10ms units
	unsigned long Next[8];};// next 8 states for 8 inputs 0 to 7

typedef const struct State SType; 
#define goW   0
#define waitW 1
#define goS   2
#define waitS 3
#define Walk  4
#define NWalk 5
#define Off   6
#define NWalk2 7
#define Off2   8
	
SType FSM[9]= {
	{0x0C,0x02,5,{goW,goW,waitW,waitW,waitW,waitW,waitW,waitW}},
	{0x14,0x02,5,{waitW,goS,goS,goS,Walk,Walk,goS,goS}},
	{0x21,0x02,5,{goS,waitS,goS,waitS,waitS,waitS,waitS,waitS}},
	{0x22,0x02,5,{waitS,goW,goW,goW,Walk,goW,Walk,Walk}},
	{0x24,0x08,5,{Walk,NWalk,NWalk,NWalk,Walk,NWalk,NWalk,NWalk}},
	{0x24,0x02,5,{NWalk,Off,Off,Off,Walk,Off,Off,Off}},
	{0x24,0x00,5,{Off,NWalk2,NWalk2,NWalk2,Walk,NWalk2,NWalk2,NWalk2}},
	{0x24,0x02,5,{NWalk2,Off2,Off2,Off2,Walk,Off2,Off2,Off2}},
	{0x24,0x00,5,{Off2,goW,goS,goW,Walk,goW,goS,goW}}
};
	

//subroutine of SysTick_Init 
#define NVIC_ST_CTRL_R      (*((volatile unsigned long *)0xE000E010))
#define NVIC_ST_RELOAD_R    (*((volatile unsigned long *)0xE000E014))
#define NVIC_ST_CURRENT_R   (*((volatile unsigned long *)0xE000E018))
void SysTick_Init(void){
  NVIC_ST_CTRL_R = 0;               // disable SysTick during setup
  NVIC_ST_CTRL_R = 0x00000005;      // enable SysTick with core clock
}

// The delay parameter is in units of the 80 MHz core clock. (12.5 ns)
void SysTick_Wait(unsigned long delay){
  NVIC_ST_RELOAD_R = delay-1;  // number of counts to wait
  NVIC_ST_CURRENT_R = 0;       // any value written to CURRENT clears
  while((NVIC_ST_CTRL_R&0x00010000)==0){ // wait for count flag
  }
}

// 800000*12.5ns equals 10ms
void SysTick_Wait10ms(unsigned long delay){
  unsigned long i;
  for(i=0; i<delay; i++){
    SysTick_Wait(800000);  // wait 10ms
  }
}


//main subroubtine  	
int main(void){ 
	volatile unsigned long delay;
  TExaS_Init(SW_PIN_PE210, LED_PIN_PB543210,ScopeOff); // activate grader and set system clock to 80 MHz
  PLL_Init(); 
  SysTick_Init();
  EnableInterrupts();
	
	//initialize I/O port
	SYSCTL_RCGC2_R |= 0x32;      // 1) B E F
  delay = SYSCTL_RCGC2_R;      // 2) no need to unlock
	//PE2-0
  GPIO_PORTE_AMSEL_R &= ~0x07; // 3) disable analog function on PE2-0
  GPIO_PORTE_PCTL_R &= ~0x000000FF; // 4) enable regular GPIO
  GPIO_PORTE_DIR_R &= ~0x07;   // 5) inputs on PE2-0
  GPIO_PORTE_AFSEL_R &= ~0x07; // 6) regular function on PE2-0
  GPIO_PORTE_DEN_R |= 0x07;    // 7) enable digital on PE2-0
	//PB5-0
  GPIO_PORTB_AMSEL_R &= ~0x3F; // 3) disable analog function on PB5-0
  GPIO_PORTB_PCTL_R &= ~0x00FFFFFF; // 4) enable regular GPIO
  GPIO_PORTB_DIR_R |= 0x3F;    // 5) outputs on PB5-0
  GPIO_PORTB_AFSEL_R &= ~0x3F; // 6) regular function on PB5-0
  GPIO_PORTB_DEN_R |= 0x3F;    // 7) enable digital on PB5-0
	//PF1,3
  GPIO_PORTF_LOCK_R = 0x4C4F434B;   // 2) unlock GPIO Port F
  GPIO_PORTF_CR_R = 0x1F;           // allow changes to PF4-0
  // only PF0 needs to be unlocked, other bits can't be locked
  GPIO_PORTF_AMSEL_R = 0x00;        // 3) disable analog on PF
  GPIO_PORTF_PCTL_R = 0x00000000;   // 4) PCTL GPIO on PF4-0
  GPIO_PORTF_DIR_R = 0x0A;          // 5) PF4,PF0 in, PF3-1 out
  GPIO_PORTF_AFSEL_R = 0x00;        // 6) disable alt funct on PF7-0
  GPIO_PORTF_PUR_R = 0x11;          // enable pull-up on PF0 and PF4
  GPIO_PORTF_DEN_R = 0x1A;          // 7) enable digital I/O on PF4-0
	
  
	//Engine controller 
	S = goW;
	while(1){
		GPIO_PORTB_DATA_R = FSM[S].Out1;  
    GPIO_PORTF_DATA_R = FSM[S].Out2;  
		SysTick_Wait10ms(FSM[S].Time);
		Input = Sensor ; //read sensor info 
		S = FSM[S].Next[Input];
  }
}

