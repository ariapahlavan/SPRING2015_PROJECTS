// Sound.c
// Runs on any computer
// Runs on LM4F120 or TM4C123
// Program written by: Aria Pahlavan and Khalid Qarryzada
// Lab number: 10
// Hardware connections
// TO STUDENTS "REMOVE THIS LINE AND SPECIFY YOUR HARDWARE********
// November 17, 2014
#include <stdint.h>
#include "Sound.h"
#include "SoundFiles.h"
#include "DAC.h"
#include "tm4c123gh6pm.h"
#include "SoundFiles.c"
#include "Timer0.h"
#include "Timer1.h"
#include "Timer2.h"

// Period =  80000000/32/Freq=2500000/Freq
void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
const unsigned char* CurrSound;
unsigned long SoundIndex = 0;
unsigned long SoundCount = 0;
unsigned int Index=0;
int i=3;													//specifies the period of delay
int	num =0;

void Timer3_Init(void){
 	unsigned long volatile delay;
	SYSCTL_RCGCTIMER_R |= 0x08;
	delay = SYSCTL_RCGCTIMER_R;   // 0) activate TIMER1
  //PeriodicTask1 = task;          // user function
  TIMER3_CTL_R = 0x00000000;    // 1) disable TIMER1A during setup
  TIMER3_CFG_R = 0x00000000;    // 2) configure for 32-bit mode
  TIMER3_TAMR_R = 0x00000002;   // 3) configure for periodic mode, default down-count settings
  TIMER3_TAILR_R = 29999999;    // 4) reload value
  TIMER3_TAPR_R = 0;            // 5) bus clock resolution
  TIMER3_ICR_R = 0x00000001;    // 6) clear TIMER1A timeout flag
  TIMER3_IMR_R = 0x00000001;    // 7) arm timeout interrupt
  NVIC_PRI8_R = (NVIC_PRI5_R&0x00FFFFFF)|0xA0000000; // 8) priority 5
// interrupts enabled in the main program after all devices initialized
// vector number 37, interrupt number 21
  NVIC_EN1_R = 1<<3;           // 9) enable IRQ 21 in NVIC
  TIMER3_CTL_R = 0x00000001;    // 10) enable TIMER1A
}
void Delay10ms(uint32_t count){
	uint32_t volatile time;
	while(count>0){
		time = 7272;  // 0.1sec at 80 MHz
		while(time){
			time--;
		}
		count--;
	}
}
void Sound_Init(void){
  DAC_Init();          // Port B is DAC
  NVIC_ST_CTRL_R = 0;         						 // disable SysTick during setup
  NVIC_ST_RELOAD_R = 0;								 // reload value
  NVIC_ST_CURRENT_R = 0;      						 // any write to current clears it
  NVIC_SYS_PRI3_R = (NVIC_SYS_PRI3_R&0x00FFFFFF)|0x60000000; // priority 6
  NVIC_ST_CTRL_R = 0x0007; 						 // enable SysTick with core clock and interrupts
}

void SysTick_Handler(void){
	
	DAC_Out(SineCurrSound[Index]);    // output one value each interrupt
	Index = (Index+1)&0x3F;      // 8,9,11,12,13,14,14,15,15,15,14,14,13,12,11,9,8,7,... 
}

// **************Sound_Play2*********************
// Start sound output, and set Systick interrupt period 
// Input: interrupt period
// Output: none
void Sound_Play2(uint32_t period){
	if(period == 0){
	NVIC_ST_RELOAD_R =0;
	}
	else{
		NVIC_ST_RELOAD_R = period-1;
		
	}
}
//***************Timer3A_Handler****************
//Plays the Tetris Sound onces at the beginning of each round
//Then it deactivates itself until the next round begins 
//Input: none
//Output: none
void Timer3A_Handler(void){
	TIMER3_ICR_R = 0x01;
	TIMER3_CTL_R = 0x0000000;    //disable TIMER0
//	TIMER1_CTL_R = 0x0000000;    //disable TIMER1
//	TIMER2_CTL_R = 0x0000000;    //disable TIMER2
			for(int num =0; num  < 133; num++){//133
					if(TETRIS[num] == Z1) i=5;else i =150;	//check if Z1 change the period to 5
			Sound_Play2(TETRIS[num]);						//play the note
			Delay10ms(i);												//delay after each note usign the value of i
			}
		NVIC_ST_CTRL_R =0;					 //disable SysTick after the song has been played 
//		TIMER0_CTL_R = 0x0000001;    //enable TIMER0
//		TIMER2_CTL_R = 0x0000001;    //enable TIMER2
			DisableInterrupts();
		Timer0_Init(70000000);				//1000000
		Timer1_Init(7255);
		Timer2_Init(30000000);
			EnableInterrupts();			
		TIMER3_CTL_R = 0x0000000;		 //disable TIMER1A 
}

//****************Timer1A_Handler***************
//Plays different sound effects depending on the *sound
//Input: none
//Output: none
void Timer1A_Handler(void){
	TIMER1_ICR_R = TIMER_ICR_TATOCINT;// acknowledge TIMER1A timeout
	TIMER0_CTL_R = 0x0000000;    //disable TIMER0A
	TIMER2_CTL_R = 0x0000000;    //disable TIMER2
  if(SoundCount > 0){ 
    //GPIO_PORTB_DATA_R = CurrSound[SoundIndex]>>4; // DAC out
		DAC_Out(CurrSound[SoundIndex]);
    SoundIndex += 1;
    SoundCount -= 1;
		
//		DAC_Out(CurrSound[Index]);    // output one value each interrupt
//	if((Index+ 1) == 0x20) 	{Index= 0; }
//	Index = (Index+1);//&0x1F;      // 8,9,11,12,13,14,14,15,15,15,14,14,13,12,11,9,8,7,... 
  }
	else // If sound ends, disable timer0 & stop sound
		{TIMER0_CTL_R = 0x0000001;    //enable TIMER0A
		TIMER2_CTL_R = 0x0000001;    //enable TIMER2
		TIMER1_CTL_R = 0x00000000;}

	TIMER0_CTL_R = 0x0000001;    //enable TIMER0A
	TIMER2_CTL_R = 0x0000001;    //enable TIMER2
}


// **************Sound_Play1*********************
// Start sound output, and set Timer1 periodic interrupts
// Input: the sound array, interrupt period
// Output: none
void Sound_Play1(const unsigned char *sound, uint32_t length){
	CurrSound = sound;						//array of the sound				
	SoundIndex = 0;					//index = 0
	SoundCount = length;		//the length of the sound effect 
	TIMER1_CTL_R = 1;				//enable Timer1
}

// **************Sound Functions*********************
// Once called, passes into the Sound_Play1 function the appropriate array and period
// Input: none
// Output: none
void VertSound(void){
	Sound_Play1(ADC, 3286);
}

void RotateSound(void){
	Sound_Play1(Rotation, 8768);
}

void NextSound(void){
	Sound_Play1(Drop, 3832);
}


void LineSound(void){
	Sound_Play1(Line, 6086);
}

void MultiLineSound(void){
	Sound_Play1(Multi_line, 8748);
}

void HardDropSound(void){
	Sound_Play1(Hard_Drop, 4285);
}

void LostSound(void){
	Sound_Play1(Lost, 10688);
}

void FourLinesSound(void){
	Sound_Play1(Four_lines, 20347);
}
//
