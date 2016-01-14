// ADC.c
// Runs on LM4F120/TM4C123
// Provide functions that initialize ADC0
// Last Modified: 3/6/2015 
// Student names: Aria Pahlavan and Khalid Qarryzada
// Last modification date: 4/13/15

#include <stdint.h>
#include "tm4c123gh6pm.h"

// ADC initialization function 
// Input: none
// Output: none
void ADC_Init(void){//*********channel: PE2 = Ain1*********)
	SYSCTL_RCGCGPIO_R|= 0x10;				//Enable the port clock for the ADC input pin
	while ((SYSCTL_PRGPIO_R & 0x10) ==0){};		//Delay, wait for clock to stabilize
	GPIO_PORTE_DEN_R &= ~0x04;								//Disable the digital function
	GPIO_PORTE_AFSEL_R |= 0x04;								//Enable the alternative function
	GPIO_PORTE_AMSEL_R |=0x04;								//Enable the analog function
	GPIO_PORTE_DIR_R &= ~0x04;								//Make the pin an input (clear the bit)
	SYSCTL_RCGCADC_R |= 0x01;									//Enable the ADC clock
	volatile uint32_t delay;									//declare variable
	delay = SYSCTL_RCGCADC_R;									//after setting SYSCTL_RCGADC_R, wait for stablization
	delay = SYSCTL_RCGCADC_R;									//after setting SYSCTL_RCGADC_R, wait for stablization
	delay = SYSCTL_RCGCADC_R;									//after setting SYSCTL_RCGADC_R, wait for stablization  
	ADC0_PC_R =0x01;													//Configure for 125 kHz sampling rate
	ADC0_SSPRI_R = 0x0123;										//Set Sequencer 3 to highest priority(SS3 SS2 SS1 SS0 = 0 1 2 3)
	ADC0_ACTSS_R &= ~0x08;										//Disable sample sequencer 3 bifore configuring it
	ADC0_EMUX_R &= ~0xF000;										//Sequencer 3 software trigger(software start)
	ADC0_SSMUX3_R=(ADC0_SSMUX3_R & ~0xF)+1;	// 	//Clear SS3 field and set channel Ain1 (PE2 = Ain1)
	ADC0_SSCTL3_R = 0x06;											//TS0:Temperature(N). IE0:INR3 flag(Y). END0:One sample(Y). D0: Differential sampling(N) 
	ADC0_IM_R &= ~0x08;												//Disable SS3 interrupts
	ADC0_ACTSS_R |= 0x08;											//Enable sample sequencer 3
}

//------------ADC_In------------
// Busy-wait Analog to digital conversion
// Input: none
// Output: 12-bit result of ADC conversion
uint32_t ADC_In(void){  
	uint32_t result = 0;								//create a result var
	ADC0_PSSI_R = 0x8;									//Initiate SS3
	while((ADC0_RIS_R & 0x8)==0){};			//Wait for conversion done
	result = ADC0_SSFIFO3_R & 0xFFF;		//Read 12-bit result
	ADC0_ISC_R = 0x8;										// Acknowledge completion
	return result;
}


