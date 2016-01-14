// dac.c
// This software configures DAC output
// Runs on LM4F120 or TM4C123
// Program written by: Aria Pahlavan - Khalid Qarryzada
// Date Created: 8/25/2014 
// Last Modified: 3/6/2015 
// Section 1-2pm     TA: Wooseok Lee
// Lab number: 6
// Hardware connections
// TO STUDENTS "REMOVE THIS LINE AND SPECIFY YOUR HARDWARE********

#include <stdint.h>
#include "tm4c123gh6pm.h"
// Code files contain the actual implemenation for public functions
// this file also contains an private functions and private data

// **************DAC_Init*********************
// Initialize 4-bit DAC, called once 
// Input: none
// Output: none
void DAC_Init(void){
  unsigned long volatile delay;
  SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOB; // activate port B
  delay = SYSCTL_RCGC2_R;    						// allow time to finish activating
  GPIO_PORTB_DR8R_R |= 0x0F;      			//can drive up to 8mA out
  GPIO_PORTB_DIR_R |= 0x0F;      				// make PB5-0 output
  GPIO_PORTB_DEN_R |= 0x0F;      				// enable digital I/O on PB5-0
}

// **************DAC_Out*********************
// output to DAC
// Input: 4-bit data, 0 to 15 
// Output: none
void DAC_Out(uint32_t data){
	GPIO_PORTB_DATA_R &= ~0x0F;
	GPIO_PORTB_DATA_R |= data;								// Set data equal to PB DATA register
}
