// Timer2.c
// Runs on LM4F120 or TM4C123
// Program written by: Aria Pahlavan and Khalid Qarryzada
// Lab number: 10
// Hardware connections
// TO STUDENTS "REMOVE THIS LINE AND SPECIFY YOUR HARDWARE********

// http://www.spaceinvaders.de/
// sounds at http://www.classicgaming.cc/classics/spaceinvaders/sounds.php
// http://www.classicgaming.cc/classics/spaceinvaders/playguide.php

/*
 Copyright 2015 by Aria Pahlavan, apahlavan1@utexas.edu 
		   Khalid Qarryzada
    You may use, edit, run or distribute this file
    as long as the above copyright notice remains
 THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 ARIAPAHLAVAN OR KHALID QARRYZADA SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL,
 OR CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 */
#include <stdint.h>

#include "Timer2.h"
#include "tm4c123gh6pm.h"
#include "ADC.h"
extern int x;
extern int y;

// ***************** TIMER2_Init ****************
// Activate TIMER2 interrupts to run user task periodically
// Inputs:  task is a pointer to a user function
//          period in units (1/clockfreq)
// Outputs: none
unsigned long TimerCount;
void Timer2_Init(uint32_t period){ 
  unsigned long volatile delay;
  SYSCTL_RCGCTIMER_R |= 0x04;   // 0) activate timer2
  delay = SYSCTL_RCGCTIMER_R;
  TimerCount = 0;
  TIMER2_CTL_R = 0x00000000;   // 1) disable timer2A
  TIMER2_CFG_R = 0x00000000;   // 2) 32-bit mode
  TIMER2_TAMR_R = 0x00000002;  // 3) periodic mode
  TIMER2_TAILR_R = period-1;   // 4) reload value
  TIMER2_TAPR_R = 0;           // 5) clock resolution
  TIMER2_ICR_R = 0x00000001;   // 6) clear timeout flag
  TIMER2_IMR_R = 0x00000001;   // 7) arm timeout
  NVIC_PRI5_R = (NVIC_PRI5_R&0x00FFFFFF)|0x80000000; 
// 8) priority 4
  NVIC_EN0_R = 1<<23;          // 9) enable IRQ 23 in
  TIMER2_CTL_R = 0x00000001;   // 10) enable timer2A
}
