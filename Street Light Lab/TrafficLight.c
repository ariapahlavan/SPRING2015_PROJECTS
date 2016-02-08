// ***** 0. Documentation Section *****
// TrafficLight.c for Lab 5
// Runs on LM4F120/TM4C123
// Implementation of a Moore finite state machine to operate a traffic light.  
// Daniel Valvano, Jonathan Valvano
// Feb 24, 2015

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

// FUNCTION PROTOTYPES: Each subroutine defined
void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts

// ***** 3. Subroutines Section *****
void SysTick_Init(void){
  NVIC_ST_CTRL_R = 0;               // disable SysTick during setup
  NVIC_ST_CTRL_R = 0x00000005;      // enable SysTick with core clock
}
void SysTick_Wait(unsigned long delay){
  NVIC_ST_RELOAD_R = delay-1;  // number of counts to wait
  NVIC_ST_CURRENT_R = 0;       // any value written to CURRENT clears
  while((NVIC_ST_CTRL_R&0x00010000)==0){ // wait for count flag
  }
}
// 10000us equals 10ms
void SysTick_Wait10ms(unsigned long delay){
  unsigned long i;
  for(i=0; i<delay; i++){
    SysTick_Wait(800000);  // wait 10ms
  }
}
	struct state{
		unsigned long	out1;
		unsigned long	out2;
		unsigned long	wait;
		unsigned long	next[8];		
	};
#define	goW			0
#define	waitW		1
#define	goS			2
#define	waitS		3
#define	walk		4
#define	tglOn1	5
#define	tglOff1	6
#define	tglOn2	7
#define	tglOff2	8
	typedef	struct state	stype;
	stype	FSM[9]={
		{0x0C,0x2,150,{goW,goW,waitW,waitW,waitW,waitW,waitW,waitW}},
		{0x14,0x2,100,{walk,goS,goS,goS,walk,walk,walk,walk}},
		{0x21,0x2,150,{goS,waitS,goS,waitS,waitS,waitS,waitS,waitS}},
		{0x22,0x2,100,{walk,goW,goW,goW,walk,goW,walk,goW}},
		{0x24,0x8,125,{walk,tglOn1,tglOn1,tglOn1,walk,tglOn1,tglOn1,tglOn1}},
		{0x24,0x2,100,{tglOff1,tglOff1,tglOff1,tglOff1,tglOff1,tglOff1,tglOff1,tglOff1}},
		{0x24,0x0,100,{tglOn2,tglOn2,tglOn2,tglOn2,tglOn2,tglOn2,tglOn2,tglOn2}},
		{0x24,0x2,100,{tglOff2,tglOff2,tglOff2,tglOff2,tglOff2,tglOff2,tglOff2,tglOff2}},
		{0x24,0x0,100,{goW,goW,goS,goS,walk,goW,goS,goS}}
	};
		uint16_t	S;
		uint32_t	input;
	
int main(void){
	volatile unsigned long delay;
	SysTick_Init();
  TExaS_Init(SW_PIN_PE210, LED_PIN_PB543210); // activate grader and set system clock to 80 MHz
	SYSCTL_RCGC2_R |= 0x32;      			// 1) B E F
  delay = SYSCTL_RCGC2_R;     			// 2) no need to unlock
  GPIO_PORTE_AMSEL_R &= ~0x07; 			// 3) disable analog function on PE2-0
  GPIO_PORTE_PCTL_R &= ~0x000000FF; // 4) enable regular GPIO
  GPIO_PORTE_DIR_R &= ~0x07;   			// 5) inputs on PE2-0
  GPIO_PORTE_AFSEL_R &= ~0x07; 			// 6) regular function on PE2-0
  GPIO_PORTE_DEN_R |= 0x07;    			// 7) enable digital on PE2-0
  GPIO_PORTB_AMSEL_R &= ~0x3F; 			// 3) disable analog function on PB5-0
  GPIO_PORTB_PCTL_R &= ~0x00FFFFFF; // 4) enable regular GPIO
  GPIO_PORTB_DIR_R |= 0x3F;    			// 5) outputs on PB5-0
  GPIO_PORTB_AFSEL_R &= ~0x3F; 			// 6) regular function on PB5-0
  GPIO_PORTB_DEN_R |= 0x3F;    			// 7) enable digital on PB5-0
	GPIO_PORTF_AMSEL_R &= ~0x0F; 			// 8) disable analog function on PF3-0
  GPIO_PORTF_PCTL_R &= ~0x00FFFFFF; // 9) enable regular GPIO
  GPIO_PORTF_DIR_R |= 0x0F;    			// 10) outputs on PF3-0
  GPIO_PORTF_AFSEL_R &= ~0x0F; 			// 11) regular function on PF3-0
  GPIO_PORTF_DEN_R |= 0x0F;    			// 12) enable digital on PF3-0
	
  S	= goW;
  
  
  EnableInterrupts();
  while(1){
		GPIO_PORTB_DATA_R &= ~0xFF;
		GPIO_PORTF_DATA_R &= ~0xFF;
    GPIO_PORTB_DATA_R |= FSM[S].out1;	// set car lights
		GPIO_PORTF_DATA_R	|= FSM[S].out2;	// set walk lights
		
		
		SysTick_Wait10ms(FSM[S].wait);
		input = (GPIO_PORTE_DATA_R & 0x7);     // read sensors
		S = FSM[S].next[input]; 
	}
}

