// Sound.c
// This module contains the SysTick ISR that plays sound
// Runs on LM4F120 or TM4C123
// Program written by: Aria Pahlavan - Khalid Qarryzada
// Date Created: 8/25/2014 
// Last Modified: 3/6/2015 
// Section 1-2pm     TA: Wooseok Lee
// Lab number: 6
// Hardware connections
// TO STUDENTS "REMOVE THIS LINE AND SPECIFY YOUR HARDWARE********

// Code files contain the actual implemenation for public functions
// this file also contains an private functions and private data
#include <stdint.h>
#include "dac.h"
#include "tm4c123gh6pm.h"


// **************Sound_Init*********************
// Initialize Systick periodic interrupts
// Called once, with sound initially off
// Output: none
const unsigned short SineWave[64] = {32,35,38,41,44,47,49,52,54,56,58,59,61,62,62,63,63,63,62,62,61,59,58,56,54,52,49,47,44,41,38,35,32,29,26,23,20,17,15,12,10,8,6,5,3,2,2,1,1,1,2,2,3,5,6,8,10,12,15,17,20,23,26,29};
//const unsigned short SineWave[32] = { 8,9,11,12,13,14,14,15,15,15,14, 14,13,12,11,9,8,7,5,4,3,2, 2,1,1,1,2,2,3,4,5,7};	
	unsigned char Index=0; 

void Sound_Init(uint32_t period){
  DAC_Init();          // Port B is DAC
  NVIC_ST_CTRL_R = 0;         						 // disable SysTick during setup
  NVIC_ST_RELOAD_R = 2389;								 // reload value
  NVIC_ST_CURRENT_R = 0;      						 // any write to current clears it
  NVIC_SYS_PRI3_R = (NVIC_SYS_PRI3_R&0x00FFFFFF)|0x20000000; // priority 1
  NVIC_ST_CTRL_R = 0x0007; 						 // enable SysTick with core clock and interrupts
}

void SysTick_Handler(void){
	       // toggle PF3
	GPIO_PORTF_DATA_R ^= 0x08;
	DAC_Out(SineWave[Index]);    // output one value each interrupt
	Index = (Index+1)&0x3F;      // 8,9,11,12,13,14,14,15,15,15,14,14,13,12,11,9,8,7,... 
}

// **************Sound_Play*********************
// Start sound output, and set Systick interrupt period 
// Input: interrupt period
// Output: none
void Sound_Play(uint32_t period){
	if(period == 0){
	NVIC_ST_RELOAD_R =0;
	}
	else{
		NVIC_ST_RELOAD_R = period-1;
		
	}
}
