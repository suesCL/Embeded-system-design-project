// Piano.c
// Runs on LM4F120 or TM4C123, 
// edX lab 13 
// There are four keys in the piano
// Daniel Valvano
// December 29, 2014

// Port E bits 3-0 have 4 piano keys

#include "Piano.h"
#include "..//tm4c123gh6pm.h"


// **************Piano_Init*********************
// Initialize 4 piano key inputs
// Input: none
// Output: none
void Piano_Init(void){ 
	volatile unsigned long  delay;
  SYSCTL_RCGC2_R |= 0x00000010;     // 1) activate clock for Port E
  delay = SYSCTL_RCGC2_R;           // allow time for clock to start
	GPIO_PORTE_LOCK_R = 0x4C4F434B;   // 2) unlock GPIO Port E
  GPIO_PORTF_CR_R = 0x0F;           // allow changes to PE3-0
  GPIO_PORTE_CR_R = 0x0F;           // allow changes to PE3-0
  GPIO_PORTE_AMSEL_R = 0x00;        // 3) disable analog on PE
  GPIO_PORTE_PCTL_R = 0x00000000;   // 4) PCTL GPIO on PE3-0
  GPIO_PORTE_DIR_R &= ~0x0F;          // 5) PE3-0 input
  GPIO_PORTE_AFSEL_R = 0x00;        // 6) disable alt funct on PE3-0
  GPIO_PORTE_PUR_R = 0x0F;          // enable pull-up on PE3-0
  GPIO_PORTE_DEN_R = 0x0F;          // 7) enable digital I/O on PE3-0
} 

// **************Piano_In*********************
// Input from piano key inputs
// Input: none 
// Output: 0 to 15 depending on keys
// 0x01 is key 0 pressed, 0x02 is key 1 pressed,
// 0x04 is key 2 pressed, 0x08 is key 3 pressed
unsigned long Piano_In(void){
	if(GPIO_PORTE_DATA_R&0x01){
		 return 80000000/16744;
	}else if(GPIO_PORTE_DATA_R&0x02){
		 return 80000000/18794;
	}else if(GPIO_PORTE_DATA_R&0x04){
		 return 80000000/21095;
	}else if(GPIO_PORTE_DATA_R&0x08){
		 return 80000000/25087;
	}else if(GPIO_PORTE_DATA_R&0x00){
		 return 0;
  }
}