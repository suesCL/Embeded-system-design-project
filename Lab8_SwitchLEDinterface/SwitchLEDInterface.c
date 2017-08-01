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

// ***** 2. Global Declarations Section *****
// Global variables
unsigned long SWITCH;
// FUNCTION PROTOTYPES: Each subroutine defined
void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
void PortE_Init(void);
void Delay1ms(unsigned long msec);

// ***** 3. Subroutines Section *****

// PE0, PB0, or PA2 connected to positive logic momentary switch using 10k ohm pull down resistor
// PE1, PB1, or PA3 connected to positive logic LED through 470 ohm current limiting resistor
// To avoid damaging your hardware, ensure that your circuits match the schematic
// shown in Lab8_artist.sch (PCB Artist schematic file) or 
// Lab8_artist.pdf (compatible with many various readers like Adobe Acrobat).
int main(void){ 
//**********************************************************************
// The following version tests input on PE0 and output on PE1
//**********************************************************************
  TExaS_Init(SW_PIN_PE0, LED_PIN_PE1, ScopeOn);  // activate grader and set system clock to 80 MHz
	PortE_Init();
  EnableInterrupts();           // enable interrupts for the grader
  GPIO_PORTE_DATA_R |= 0x02;
	
	while(1){
		SWITCH = GPIO_PORTE_DATA_R&0x00000001;
		Delay1ms(100);
		if ( SWITCH == 0x01){
			  GPIO_PORTE_DATA_R ^= 0x02;
		}else{
			  GPIO_PORTE_DATA_R |= 0x02;
		}
	}
}


// Subroutine to initialize port E pins for input and output
// PE1 is output and PE0 is input switch
void PortE_Init(void) {volatile unsigned long delay;
  SYSCTL_RCGC2_R |= 0x00000010;      // 1) initialize clock in port E
  delay = SYSCTL_RCGC2_R;            // delay to allow clock to stabilize     
  GPIO_PORTE_AMSEL_R &= 0x00;        // 2) disable analog function
  GPIO_PORTE_PCTL_R &= ~0xFC;        // 3) GPIO clear bit PE1,PE0 at PCTL  
  GPIO_PORTE_DIR_R &= ~0xFE;         // 4.1) PE0 is input
  GPIO_PORTE_DIR_R |= 0x02;          // 4.2) PE1 is output  
  GPIO_PORTE_AFSEL_R &= 0x00;        // 5) no alternate function     
  GPIO_PORTE_DEN_R |= 0x17;          // 7) enable digital pins PE4-PE1
}

//subroutine to delay ms
//input = # ms
//output None
void Delay1ms(unsigned long msec){
   unsigned long count;
   while(msec > 0 ) { // repeat while still msec to delay
     count = 15500; 
     while (count > 0) { 
       count--;
     } // This while loop takes approximately 3 cycles
     msec--;
    }
}