// Lab6.c
// Runs on LM4F120 or TM4C123
// Use SysTick interrupts to implement a 4-key digital piano
// MOOC lab 13 or EE319K lab6 starter
// Program written by: Aria Pahlavan and Khalid Qarryzada
// Date Created: 1/24/2015 
// Last Modified: 3/6/2015 
// Section 1-2pm     TA: Wooseok Lee
// Lab number: 6
// Hardware connections
// TO STUDENTS "REMOVE THIS LINE AND SPECIFY YOUR HARDWARE********


#include <stdint.h>
#include "tm4c123gh6pm.h"
#include "Sound.h"
#include "Piano.h"
#include "TExaS.h"
#include "dac.h"

// Period = 80000000/64/Freq=1250000/Freq
#define C1 597 // 2093 Hz
#define B1 633 // 1975.5 Hz
#define BF1 670 // 1864.7 Hz
#define A1 710 // 1760 Hz
#define AF1 752 // 1661.2 Hz
#define G1 797 // 1568 Hz
#define GF1 845 // 1480 Hz
#define F1 895 // 1396.9 Hz
#define E1 948 // 1318.5 Hz
#define EF1 1004 // 1244.5 Hz
#define D1 1064 // 1174.7 Hz
#define DF1 1127 // 1108.7 Hz
#define C 1194 // 1046.5 Hz
#define B 1265 // 987.8 Hz
#define BF 1341 // 932.3 Hz
#define A 1420 // 880 Hz
#define AF 1505 // 830.6 Hz
#define G 1594 // 784 Hz***********
#define GF 1689 // 740 Hz
#define F 1790 // 698.5 Hz
#define E 1896 // 659.3 Hz*********
#define EF 2009 // 622.3 Hz
#define D 2128 // 587.3 Hz**********
#define DF 2255 // 554.4 Hz
#define C0 2389 // 523.3 Hz***********
#define B0 2531 // 493.9 Hz
#define BF0 2681 // 466.2 Hz
#define A0 2841 // 440 Hz
#define AF0 3010 // 415.3 Hz
#define G0 3189 // 392 Hz
#define GF0 3378 // 370 Hz
#define F0 3579 // 349.2 Hz
#define E0 3792 // 329.6 Hz
#define EF0 4018 // 311.1 Hz
#define D0 4257 // 293.7 Hz
#define DF0 4510 // 277.2 Hz
#define C7 4778 // 261.6 Hz
#define B7 5062 // 246.9 Hz
#define BF7 5363 // 233.1 Hz
#define A7 5682 // 220 Hz
#define AF7 6020 // 207.7 Hz
#define G7 6378 // 196 Hz
#define GF7 6757 // 185 Hz
#define F7 7159 // 174.6 Hz
#define E7 7584 // 164.8 Hz
#define EF7 8035 // 155.6 Hz
#define D7 8513 // 146.8 Hz
#define DF7 9019 // 138.6 Hz
#define C6 9556 // 130.8 Hz
// basic functions defined at end of startup.s
void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts


	          // Index varies from 0 to 15
void switch_Init(void){
  unsigned long volatile delay;
  SYSCTL_RCGC2_R |= 0x00000020; 		// (a) activate clock for PortB and PortF
  delay = SYSCTL_RCGC2_R;						//delay for clock to become enabl
  GPIO_PORTF_LOCK_R = 0x4C4F434B; // unlock GPIO Port F
  GPIO_PORTF_CR_R = 0x08;         // allow changes to PF3
  GPIO_PORTF_DIR_R |= 0x08;    // (c) make PF3 LED
  GPIO_PORTF_AFSEL_R &= ~0x08;  //     disable alt funct on PF3
  GPIO_PORTF_DEN_R |= 0x08;     //     enable digital I/O on PF3
  GPIO_PORTF_PCTL_R &= ~0xFFFFFFFF; //  configure PF3 as GPIO
  GPIO_PORTF_AMSEL_R &= ~0x08;  //     disable analog functionality 																// enable interrupts after all initialization is finished
}



int main(void){
	//unsigned long input=0;
	
	TExaS_Init(SW_PIN_PE3210,DAC_PIN_PB3210,ScopeOn);    // bus clock at 80 MHz
	switch_Init();
	Piano_Init();
	Sound_Init(0);
//	SysTick_Init();
  // other initialization
	EnableInterrupts();
  while(1){
		//Sound_Play(C0);
//	input = Piano_In();
//	 if (input == 0x01){
//		 Sound_Play(C0);}
//	 else if (input == 0x02){
//		 Sound_Play(D);}
//	 else if (input == 0x04){
//		 Sound_Play(E);}
//	 else if (input == 0x08){
//		 Sound_Play(G);}
//	else{
//		NVIC_ST_RELOAD_R =0;
//		GPIO_PORTF_DATA_R &= ~0x08;  // turn off heartbeat
	}
  } 

