// ADC.h
// Runs on LM4F120/TM4C123
// Provide functions that initialize ADC0

// Student names: Aria Pahlavan and Khalid Qarryzada
// Last modification date: change this to the last modification date or look very silly
#include "tm4c123gh6pm.h"

// ADC initialization function 
// Input: none
// Output: none
void ADC_Init(void);

//------------ADC_In------------
// Busy-wait Analog to digital conversion
// Input: none
// Output: 12-bit result of ADC conversion
uint32_t ADC_In(void);
