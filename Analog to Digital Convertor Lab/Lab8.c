// Lab8.c
// Runs on LM4F120 or TM4C123
// Student names: Aria Pahlavan and Khalid Qarryzada
// Last modification date: 4/13/15
// Last Modified: 3/6/2015 

// Analog Input connected to PE2=ADC1
// displays on Sitronox ST7735
// PF3, PF2, PF1 are heartbeats

#include <stdint.h>

#include "ST7735.h"
#include "TExaS.h"
#include "ADC.h"
#include "print.h"
#include "tm4c123gh6pm.h"
#include "math.h"

//*****the first three main programs are for debugging *****
// main1 tests just the ADC and slide pot, use debugger to see data
// main2 adds the LCD to the ADC and slide pot, ADC data is on Nokia
// main3 adds your convert function, position data is no Nokia

void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts

#define PF1       (*((volatile uint32_t *)0x40025008))
#define PF2       (*((volatile uint32_t *)0x40025010))
#define PF3       (*((volatile uint32_t *)0x40025020))
// Initialize Port F so PF1, PF2 and PF3 are heartbeats
void PortF_Init(void){
	volatile uint32_t delay;
	SYSCTL_RCGCGPIO_R |=0x20;			//activate port F clock	
	delay = SYSCTL_RCGCGPIO_R;			//delay
	GPIO_PORTF_DIR_R |= 0x0E;			//Make PF1-3 outputs
	GPIO_PORTF_DEN_R |= 0x0E;			//Enable the diigital function for PF1-3
	GPIO_PORTF_AFSEL_R &= ~0x0E;			//Disable Alternate function
	GPIO_PORTF_PUR_R |= 0x0E;
}
//Initialize SysTick
void SysTick_Init(void){
	NVIC_ST_CTRL_R = 0;               // disable SysTick during setup
	NVIC_ST_CTRL_R = 0x07;      // enable SysTick with core clock & enable interrupt
	NVIC_ST_RELOAD_R = 2000000;  // Set to this value to make interrupts occur every 25 ms
	NVIC_ST_CURRENT_R = 0;       // any value written to CURRENT clears

}


uint32_t ADCMail, ADCStatus;
int ADCTemp;
int thres=100;
long delay;
void SysTick_Handler(void){
	PF2 ^= 0x04;
	PF2 ^= 0x04;
		ADCTemp=ADC_In();
	if(fabs((ADCTemp-ADCMail))>20){
		ADCMail = ADCTemp; 
		ADCStatus= 1;
	}
	PF2 ^=0x04;
	return;
	
}
uint32_t Data;        // 12-bit ADC
uint32_t Position;    // 32-bit fixed-point 0.001 cm
int main1(void){      // single step this program and look at Data
  TExaS_Init();       // Bus clock is 80 MHz 
  ADC_Init();         // turn on ADC, set channel to 1
  while(1){                
    Data = ADC_In();  // sample 12-bit channel 1
  }
}

int main2(void){
  TExaS_Init();       // Bus clock is 80 MHz 
  ADC_Init();         // turn on ADC, set channel to 1
  ST7735_InitR(INITR_REDTAB); //Initialize LCD
  PortF_Init();								//Initialize PortF2 heartbeat
	SysTick_Init();			//Initialize SysTick
  while(1){           // use scope to measure execution time for ADC_In and LCD_OutDec   
    PF2 = 0x04;       // Profile ADC
    Data = ADC_In();  // sample 12-bit channel 1
    PF2 = 0x00;       // end of ADC Profile
    ST7735_SetCursor(0,0);
    PF1 = 0x02;       // Profile LCD
    LCD_OutDec(Data); 					//Output Data
    ST7735_OutString("    ");  // these spaces are used to coverup characters from last output
    PF1 = 0;          // end of LCD Profile
  }
}

uint32_t Convert(uint32_t input){
	
	if(input < 50) input=0.0386*input + 0.7857;		// for distance less than 0.2 cm
		else if(input < 1400)  input = 0.381*input + 205.97;		//if less than 0.7cm
				else if(input <= 4030 && input ) input = 0.4198*input + 156.37;	//if less than 1.7cm
					else if(input <= 4090) input = 1.0274*input - 2207.2;		//if greater than 1.7cm
						else input = 2000;							//if greater then 2cm
	return input;
}
int main3(void){ 
  TExaS_Init();         // Bus clock is 80 MHz 
  ST7735_InitR(INITR_REDTAB); 
  PortF_Init();
  ADC_Init();         // turn on ADC, set channel to 1
	SysTick_Init();			//Initialize SysTick
  while(1){  
    PF2 ^= 0x04;      // Heartbeat
    Data = ADC_In();  // sample 12-bit channel 1
    PF3 = 0x08;       // Profile Convert
    Position = Convert(Data); 
    PF3 = 0;          // end of Convert Profile
    PF1 = 0x02;       // Profile LCD
    ST7735_SetCursor(0,0);
    LCD_OutDec(Data); ST7735_OutString("    "); 
    ST7735_SetCursor(6,0);
    LCD_OutFix(Position);
    PF1 = 0;          // end of LCD Profile
  }
}   
int main(void){
  TExaS_Init();         // Bus clock is 80 MHz 
  ST7735_InitR(INITR_REDTAB); 
  PortF_Init();
  ADC_Init();         // turn on ADC, set channel to 1
  SysTick_Init();			//Initialize SysTick
  for(;;){
	  while(ADCStatus == 0){}	//Poll ADCStatus flag 
	  uint32_t x = ADCMail;		//read ADCMail (input)
	  ADCStatus = 0;			//clear flag
	  x = Convert(x);			//convert the input
	  ST7735_SetCursor(1,7);		
    ST7735_OutString("D = "); 	//print "D = "
    ST7735_SetCursor(5,7);
    LCD_OutFix(x);				// print the fixed point value 
		ST7735_SetCursor(10,7);	
		ST7735_OutString(" cm");	// print " cm"
		/////////////////////
		ST7735_SetCursor(1,2);
			ST7735_OutString("Lab 8:");	
		ST7735_SetCursor(1,3);
		ST7735_OutString("Measurment of");
		ST7735_SetCursor(1,4);
		ST7735_OutString("Distance :)");
		
			
	}
	
}
