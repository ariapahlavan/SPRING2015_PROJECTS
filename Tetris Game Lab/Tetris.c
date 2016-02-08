// Tetris.c
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
// ******* Possible Hardware I/O connections*******************
// Slide pot pin 1 connected to ground
// Slide pot pin 2 connected to PE2/AIN1
// Slide pot pin 3 connected to +3.3V 
// fire button connected to PE0
// special weapon fire button connected to PE1
// 8*R resistor DAC bit 0 on PB0 (least significant bit)
// 4*R resistor DAC bit 1 on PB1
// 2*R resistor DAC bit 2 on PB2
// 1*R resistor DAC bit 3 on PB3 (most significant bit)
// LED on PB4
// LED on PB5

// Backlight (pin 10) connected to +3.3 V
// MISO (pin 9) unconnected
// SCK (pin 8) connected to PA2 (SSI0Clk)
// MOSI (pin 7) connected to PA5 (SSI0Tx)
// TFT_CS (pin 6) connected to PA3 (SSI0Fss)
// CARD_CS (pin 5) unconnected
// Data/Command (pin 4) connected to PA6 (GPIO), high for data, low for command
// RESET (pin 3) connected to PA7 (GPIO)
// VCC (pin 2) connected to +3.3 V
// Gnd (pin 1) connected to ground

#include <stdint.h>
#include "tm4c123gh6pm.h"
#include "ST7735.h"
#include "Sound.h"
#include "Random.h"
#include "TExaS.h"
#include "ADC.h"
#include "ExtraFunctions.h"
#include "DAC.h"
#include "PLL.h"
#include "Images.h"
#include "Timer0.h"
#include "Timer1.h"
#include "Timer2.h"
#define PF1       (*((volatile uint32_t *)0x40025008))
#define PF2       (*((volatile uint32_t *)0x40025010))
#define PF3       (*((volatile uint32_t *)0x40025020))
void IO_Init(void);
void IO_Touch(void);
void drop1block(void);
void Random_Init(uint32_t);
void play_sound(uint32_t count);
void Mod_line(uint32_t count);
void Mod_level(void);
uint32_t Random(void);
void endNow(void);
void Mod_score(uint32_t count);
void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
//void Delay100ms(uint32_t count); // time delay in 0.1 seconds


//**************************** Game Subroutines ****************************
signed long x=37, y=33, speed= 70000000,ADCspeed=30000000;
char currentshape;
char currentrotate;
char next;
int levelcnt = 0;
int line100 = 0, line10 = 0, line1=0, level10 = 0,level1=0;				//global variable for # lines
int score1 = 0,score10 = 0,score100 = 0,score1000 = 0, score10000 = 0;				//global variable for score
 char array[20] = {'0',0,'1',0,'2',0,'3',0,'4',0,'5',0,'6',0,'7',0,'8',0,'9',0};
 
 
int r;					//r is for the Draw and Draw,No_Int functions, which allow the draw bitmap function
						//to access the tetStateMod function
extern long TimerCount;

char slideflag;
uint32_t tetState[16][10];			//Current state of the game
void tetState_Init(void){
	uint32_t i, j;
	for (i = 0; i < 16; i++){
		for (j = 0; j < 10; j++){
			tetState[i][j]=0;
      }
   }	
}



uint32_t findHeight(char shape, char rot){
	uint32_t h = 0;
			switch(shape){
				case 'O':
						h = 18;
					break;
				case 'L':
						switch(rot){
						case 0:
							h = 27;
							break;
						case 1:
							h = 18;
							break;
						case 2:
							h = 27;
							break;
						case 3:
							h = 18;
							break;
						}
					break;
				case 'J':
					switch(rot){
						case 0:
							h = 27;
							break;
						case 1:
							h = 18;
							break;
						case 2:
							h = 27;
							break;
						case 3:
							h = 18;
							break;
						}
					break;
				case 'T':
					switch(rot){
						case 0:
							h = 18;
							break;
						case 1:
							h = 27;
							break;
						case 2:
							h = 18;
							break;
						case 3:
							h = 27;
							break;
						}
					break;
				case 'I':
					switch(rot){
						case 0:
							h = 36;
							break;
						case 1:
							h = 9;
							break;
						}
					break;
				case 'Z':
					switch(rot){
						case 0:
							h = 18;
							break;
						case 1:
							h = 27;
							break;
						}
					break;
				case 'S':
					switch(rot){
						case 0:
							h = 18;
							break;
						case 1:
							h = 27;
							break;
						}
					break;
			}
			return h;
} 




void tetStateMod(uint32_t a, uint32_t b, char next, char boxflag){
	DisableInterrupts();
	//a, b - coordinates of the block being printed
		a = ((a-1) / 9);														//a = ((a-1) / 9) -1;
		b = ((b-15) / 9) - 1;
	switch(next){
		case 0:
				tetState[b][a] = boxflag;
			break;
		case 1:
			break;
	}
	EnableInterrupts();
}
//
//
char CheckBoundary(char a, char b, char c){		//If function returns 0, move is invalid. If function returns 1, move is valid.
	
	
	//a is the type of shape and b is its orientation, char c checks if we want to print in the "next" box
	if (c == 1){
		return 1;
	}
	if (a == 'O'){
		if (x+18 > 91){return 0;}
		else if (x < 1){return 0;}
		else {return 1;}}
	
	else if (a == 'I'){
		if (b ==0){
			if(x+27 > 91){return 0;}
			else if (x-9 < 1){return 0;}
			else return 1;
		}
		else if (b ==1){
			if(x+9 > 91){return 0;}
			else if (x < 1){return 0;}
			else return 1;
		}
	}
	else if (a == 'S'){
		if (b ==0){
			if(x+27 > 91){return 0;}
			else if (x < 1){return 0;}
			else return 1;
		}

		else if(b == 1){
			if(x+18 > 91){return 0;}
			else if (x < 1){return 0;}
			else return 1;
		}
	}
	else if (a == 'Z'){
		if (b == 0){
			if(x+27 > 91){return 0;}
			else if (x < 1){return 0;}
			else return 1;
		}
		
		else if (b == 1){
			if(x+18 > 91){return 0;}
			else if (x < 1){return 0;}
			else return 1;
		}
	}
	else if (a == 'T'){
		if (b == 0){
			if(x+27 > 91){return 0;}
				else if (x < 1){return 0;}
					else return 1;
		}
		else if (b == 1){
			if(x+18 > 91){return 0;}
				else if (x < 1){return 0;}
					else return 1;
		}
		else if (b == 2){
			if(x+27 > 91){return 0;}
				else if (x < 1){return 0;}
					else return 1;
		}
		else if (b ==3){
			if(x+18 > 91){return 0;}
				else if (x < 1){return 0;}
					else return 1;
		}
	}
	else if (a == 'L'){
		if (b ==0){
			if(x+27 > 91){return 0;}
				else if (x < 1){return 0;}
					else return 1;
		}
		else if (b ==1){
			if(x+18 > 91){return 0;}
				else if (x < 1){return 0;}
					else return 1;
		}
		else if (b ==2){
			if(x+18 > 91){return 0;}
			else if (x-9 < 1){return 0;}
			else return 1;
		}
		else if (b ==3){
			if(x+18 > 91){return 0;}
			else if (x < 1){return 0;}
			else return 1;
		}		
	}
	else if (a == 'J'){
		if (b ==0){
			if(x+27 > 91){return 0;}
			else if (x < 1){return 0;}
			else return 1;
		}
		else if (b ==1){
			if(x+18 > 91){return 0;}
			else if (x < 1){return 0;}
			else return 1;
		}
		else if (b ==2){
			if(x+18 > 91){return 0;}
			else if (x-9 < 1){return 0;}
			else return 1;
		}
		else if (b ==3){
			if(x+18 > 91){return 0;}
			else if (x < 1){return 0;}
			else return 1;
		}		
	}
	return 2;	//if value is 2, function is broken
}													 



//
char check_bot_border(char shapeType, char orientation){
int temp=0;
		switch(shapeType){
			case 'O':
				if (y+9 > 159) temp = 0;else temp=1;
				break;
			case 'I':
				switch(orientation){
					case 0:
						if (y == 159) temp = 2;else temp=1;
						if (y > 159) temp = 0;else temp=1;				
						break;
					case 1:
						if (y+27 > 159) temp = 0;		else temp=1;				
						break;
				};
				break;
			case 'Z':
				switch(orientation){
					case 0:
						if (y+9 > 159) temp = 0;else temp=1;						
						break;
					case 1:
						if (y+18 > 159) temp = 0;	else temp=1;					
						break;
				};
				break;
			case 'S':
				switch(orientation){
					case 0:
						if (y+9 > 159) temp = 0;else temp=1;						
						break;
					case 1:
						if (y+18 > 159) temp = 0;else temp=1;						
						break;
				};
				break;
			case 'J':
				switch(orientation){
					case 0:
						if (y+9 > 159) temp = 0;else temp=1;						
						break;
					case 1:
						if (y+18 > 159) temp = 0;else temp=1;						
						break;
					case 2:
						if (y+9 > 159) temp = 0;else temp=1;						
						break;
					case 3:
						if (y+18 > 159) temp = 0;else temp=1;						
						break;
				};
				break;
			case 'L':
				switch(orientation){
					case 0:
						if (y+9 > 159) temp = 0;else temp=1;						
						break;
					case 1:
						if (y+18 > 159) temp = 0;else temp=1;						
						break;
					case 2:
						if (y+9 > 159) temp = 0;else temp=1;					
						break;
					case 3:
						if (y+18 > 159) temp = 0;	else temp=1;					
						break;
				};
				break;
			case 'T':
				switch(orientation){
					case 0:
						if (y+9 > 159) temp = 0;else temp=1;						
						break;
					case 1:
						if (y+18 > 159) temp = 0;	else temp=1;					
						break;
					case 2:
						if (y+9 > 159) temp = 0;	else temp=1;					
						break;
					case 3:
						if (y+18 > 159) temp = 0;else temp=1;						
						break;
				};
				break;
			default:
				temp = 1;
		};
		return temp;
}
//
int check_vertical_move (char shapeType, char orientation, uint8_t next){//shapeType- is it an O? a T? etc. 
DisableInterrupts();	
	
int temp = check_bot_border(shapeType, orientation);
				
		if (temp == 0){
//			EnableInterrupts();		//enable interrupts before return
			return 0;				// 0- invalid
		}

	int one,two,three,four;
		y += 9;		//we need to check the place the shape is gonna be next
	if (shapeType == 'O'){
		one = tetState[(((y-9)-15) / 9)][((x-1) / 9)];
		two = tetState[(((y-9)-15) / 9)][((x-1) / 9)+1];
		three = tetState[(((y-9)-15) / 9)-1][((x-1) / 9)];
		four = tetState[(((y-9)-15) / 9)-1][((x-1) / 9)+1];
	}
	else if (shapeType == 'I'){
		if (orientation ==0){
			
				one = tetState[(((y-9)-15) / 9)-1][((x-1) / 9)-1];
				two = tetState[(((y-9)-15) / 9)-1][((x-1) / 9)];
				three = tetState[(((y-9)-15) / 9)-1][((x-1) / 9)+1];
				four = tetState[(((y-9)-15) / 9)-1][((x-1) / 9)+2];
		}
				
		else if (orientation ==1){
		one = tetState[(((y-9)-15) / 9)-1][((x-1) / 9)];
		two = tetState[(((y-9)-15) / 9)][((x-1) / 9)];
		three = tetState[(((y-9)-15) / 9)+1][((x-1) / 9)];
		four = tetState[(((y-9)-15) / 9)+2][((x-1) / 9)];
		}
	}
	else if (shapeType == 'S'){
		if (orientation ==0){
		one = tetState[(((y-9)-15) / 9)][((x-1) / 9)];
		two = tetState[(((y-9)-15) / 9)][((x-1) / 9)+1];
		three = tetState[(((y-9)-15) / 9)-1][((x-1) / 9)+1];
		four = tetState[(((y-9)-15) / 9)-1][((x-1) / 9)+2];
		}

		else if(orientation == 1){
		one = tetState[(((y-9)-15) / 9)-1][((x-1) / 9)];
		two = tetState[(((y-9)-15) / 9)][((x-1) / 9)];
		three = tetState[(((y-9)-15) / 9)][((x-1) / 9)+1];
		four = tetState[(((y-9)-15) / 9)+1][((x-1) / 9)+1];
		}
	}
	else if (shapeType == 'Z'){
		if (orientation == 0){
		one = tetState[(((y-9)-15) / 9)-1][((x-1) / 9)];
		two = tetState[(((y-9)-15) / 9)-1][((x-1) / 9)+1];
		three = tetState[(((y-9)-15) / 9)][((x-1) / 9)+1];
		four = tetState[(((y-9)-15) / 9)][((x-1) / 9)+2];
		
		}
		
		else if (orientation == 1){
		one = tetState[(((y-9)-15) / 9)-1][((x-1) / 9)+1];
		two = tetState[(((y-9)-15) / 9)][((x-1) / 9)+1];
		three = tetState[(((y-9)-15) / 9)][((x-1) / 9)];
		four = tetState[(((y-9)-15) / 9)+1][((x-1) / 9)];
		}
	}
	else if (shapeType == 'T'){
		if (orientation == 0){
		one = tetState[(((y-9)-15) / 9)-1][((x-1) / 9)];
		two = tetState[(((y-9)-15) / 9)-1][((x-1) / 9)+1];
		three = tetState[(((y-9)-15) / 9)][((x-1) / 9)+1];
		four = tetState[(((y-9)-15) / 9)-1][((x-1) / 9)+2];
		}
		else if (orientation == 1){
		one = tetState[(((y-9)-15) / 9)][((x-1) / 9)];
		two = tetState[(((y-9)-15) / 9)][((x-1) / 9)+1];
		three = tetState[(((y-9)-15) / 9)-1][((x-1) / 9)+1];
		four = tetState[(((y-9)-15) / 9)+1][((x-1) / 9)+1];
		}
		else if (orientation == 2){
		one = tetState[(((y-9)-15) / 9)][((x-1) / 9)];
		two = tetState[(((y-9)-15) / 9)][((x-1) / 9)+1];
		three = tetState[(((y-9)-15) / 9)][((x-1) / 9)+2];
		four = tetState[(((y-9)-15) / 9)-1][((x-1) / 9)+1];
		}
		else if (orientation ==3){
		one = tetState[(((y-9)-15) / 9)-1][((x-1) / 9)];
		two = tetState[(((y-9)-15) / 9)][((x-1) / 9)];
		three = tetState[(((y-9)-15) / 9)+1][((x-1) / 9)];
		four = tetState[(((y-9)-15) / 9)][((x-1) / 9)+1];
		}
	}
	else if (shapeType == 'L'){
		if (orientation ==0){
		one = tetState[(((y-9)-15) / 9)][((x-1) / 9)];
		two = tetState[(((y-9)-15) / 9)-1][((x-1) / 9)];
		three = tetState[(((y-9)-15) / 9)-1][((x-1) / 9)+1];
		four = tetState[(((y-9)-15) / 9)-1][((x-1) / 9)+2];
		}
		else if (orientation ==1){
		one = tetState[(((y-9)-15) / 9)-1][((x-1) / 9)];
		two = tetState[(((y-9)-15) / 9)-1][((x-1) / 9)+1];
		three = tetState[(((y-9)-15) / 9)][((x-1) / 9)+1];
		four = tetState[(((y-9)-15) / 9)+1][((x-1) / 9)+1];
		}
		else if (orientation ==2){
		one = tetState[(((y-9)-15) / 9)][((x-1) / 9)-1];
		two = tetState[(((y-9)-15) / 9)][((x-1) / 9)];
		three = tetState[(((y-9)-15) / 9)][((x-1) / 9)+1];
		four = tetState[(((y-9)-15) / 9)-1][((x-1) / 9)+1];
		}
		else if (orientation ==3){
		one = tetState[(((y-9)-15) / 9)-1][((x-1) / 9)];
		two = tetState[(((y-9)-15) / 9)][((x-1) / 9)];
		three = tetState[(((y-9)-15) / 9)+1][((x-1) / 9)];
		four = tetState[(((y-9)-15) / 9)+1][((x-1) / 9)+1];
		}		
	}
	else if (shapeType == 'J'){
		if (orientation ==0){
		one = tetState[(((y-9)-15) / 9)-1][((x-1) / 9)];
		two = tetState[(((y-9)-15) / 9)-1][((x-1) / 9)+1];
		three = tetState[(((y-9)-15) / 9)-1][((x-1) / 9)+2];
		four = tetState[(((y-9)-15) / 9)][((x-1) / 9)+2];
		}
		else if (orientation ==1){
		one = tetState[(((y-9)-15) / 9)-1][((x-1) / 9)+1];
		two = tetState[(((y-9)-15) / 9)][((x-1) / 9)+1];
		three = tetState[(((y-9)-15) / 9)+1][((x-1) / 9)+1];
		four = tetState[(((y-9)-15) / 9)+1][((x-1) / 9)];
		}
		else if (orientation ==2){
		one = tetState[(((y-9)-15) / 9)-1][((x-1) / 9)-1];
		two = tetState[(((y-9)-15) / 9)][((x-1) / 9)-1];
		three = tetState[(((y-9)-15) / 9)][((x-1) / 9)];
		four = tetState[(((y-9)-15) / 9)][((x-1) / 9)+1];
		}
		else if (orientation ==3){
		one = tetState[(((y-9)-15) / 9)-1][((x-1) / 9)+1];
		two = tetState[(((y-9)-15) / 9)-1][((x-1) / 9)];
		three = tetState[(((y-9)-15) / 9)][((x-1) / 9)];
		four = tetState[(((y-9)-15) / 9)+1][((x-1) / 9)];
		}		
	}
	else{
		y-=9;  return 0;
	}
	if(one == 1|| two == 1|| three ==1|| four ==1){
		//EnableInterrupts();		//enable interrupts before return
		y-=9;  return 0;		//0-invalid
		}
		else{
	//		EnableInterrupts();
			y-=9;  return 1;		//1-valid
		}

}	

int check_horiz_move(char shapeType, char orientation, uint8_t next, char num){//shapeType- is it an O? a T? etc. 
DisableInterrupts();	
							//image- whether to print the reg. block or the clearing block
	
	int one,two,three,four;
		x += num;		//we need to check the place the shape is gonna be next
	if (shapeType == 'O'){
		one = tetState[(((y-9)-15) / 9)][((x-1) / 9)];
		two = tetState[(((y-9)-15) / 9)][((x-1) / 9)+1];
		three = tetState[(((y-9)-15) / 9)-1][((x-1) / 9)];
		four = tetState[(((y-9)-15) / 9)-1][((x-1) / 9)+1];
	}
	else if (shapeType == 'I'){
		if (orientation ==0){
		one = tetState[(((y-9)-15) / 9)-1][((x-1) / 9)-1];
		two = tetState[(((y-9)-15) / 9)-1][((x-1) / 9)];
		three = tetState[(((y-9)-15) / 9)-1][((x-1) / 9)+1];
		four = tetState[(((y-9)-15) / 9)-1][((x-1) / 9)+2];
		}
		
		else if (orientation ==1){
		one = tetState[(((y-9)-15) / 9)-1][((x-1) / 9)];
		two = tetState[(((y-9)-15) / 9)][((x-1) / 9)];
		three = tetState[(((y-9)-15) / 9)+1][((x-1) / 9)];
		four = tetState[(((y-9)-15) / 9)+2][((x-1) / 9)];
		}
	}
	else if (shapeType == 'S'){
		if (orientation ==0){
		one = tetState[(((y-9)-15) / 9)][((x-1) / 9)];
		two = tetState[(((y-9)-15) / 9)][((x-1) / 9)+1];
		three = tetState[(((y-9)-15) / 9)-1][((x-1) / 9)+1];
		four = tetState[(((y-9)-15) / 9)-1][((x-1) / 9)+2];
		}

		else if(orientation == 1){
		one = tetState[(((y-9)-15) / 9)-1][((x-1) / 9)];
		two = tetState[(((y-9)-15) / 9)][((x-1) / 9)];
		three = tetState[(((y-9)-15) / 9)][((x-1) / 9)+1];
		four = tetState[(((y-9)-15) / 9)+1][((x-1) / 9)+1];
		}
	}
	else if (shapeType == 'Z'){
		if (orientation == 0){
		one = tetState[(((y-9)-15) / 9)-1][((x-1) / 9)];
		two = tetState[(((y-9)-15) / 9)-1][((x-1) / 9)+1];
		three = tetState[(((y-9)-15) / 9)][((x-1) / 9)+1];
		four = tetState[(((y-9)-15) / 9)][((x-1) / 9)+2];
		
		}
		
		else if (orientation == 1){
		one = tetState[(((y-9)-15) / 9)-1][((x-1) / 9)+1];
		two = tetState[(((y-9)-15) / 9)][((x-1) / 9)+1];
		three = tetState[(((y-9)-15) / 9)][((x-1) / 9)];
		four = tetState[(((y-9)-15) / 9)+1][((x-1) / 9)];
		}
	}
	else if (shapeType == 'T'){
		if (orientation == 0){
		one = tetState[(((y-9)-15) / 9)-1][((x-1) / 9)];
		two = tetState[(((y-9)-15) / 9)-1][((x-1) / 9)+1];
		three = tetState[(((y-9)-15) / 9)][((x-1) / 9)+1];
		four = tetState[(((y-9)-15) / 9)-1][((x-1) / 9)+2];
		}
		else if (orientation == 1){
		one = tetState[(((y-9)-15) / 9)][((x-1) / 9)];
		two = tetState[(((y-9)-15) / 9)][((x-1) / 9)+1];
		three = tetState[(((y-9)-15) / 9)-1][((x-1) / 9)+1];
		four = tetState[(((y-9)-15) / 9)+1][((x-1) / 9)+1];
		}
		else if (orientation == 2){
		one = tetState[(((y-9)-15) / 9)][((x-1) / 9)];
		two = tetState[(((y-9)-15) / 9)][((x-1) / 9)+1];
		three = tetState[(((y-9)-15) / 9)][((x-1) / 9)+2];
		four = tetState[(((y-9)-15) / 9)-1][((x-1) / 9)+1];
		}
		else if (orientation ==3){
		one = tetState[(((y-9)-15) / 9)-1][((x-1) / 9)];
		two = tetState[(((y-9)-15) / 9)][((x-1) / 9)];
		three = tetState[(((y-9)-15) / 9)+1][((x-1) / 9)];
		four = tetState[(((y-9)-15) / 9)][((x-1) / 9)+1];
		}
	}
	else if (shapeType == 'L'){
		if (orientation ==0){
		one = tetState[(((y-9)-15) / 9)][((x-1) / 9)];
		two = tetState[(((y-9)-15) / 9)-1][((x-1) / 9)];
		three = tetState[(((y-9)-15) / 9)-1][((x-1) / 9)+1];
		four = tetState[(((y-9)-15) / 9)-1][((x-1) / 9)+2];
		}
		else if (orientation ==1){
		one = tetState[(((y-9)-15) / 9)-1][((x-1) / 9)];
		two = tetState[(((y-9)-15) / 9)-1][((x-1) / 9)+1];
		three = tetState[(((y-9)-15) / 9)][((x-1) / 9)+1];
		four = tetState[(((y-9)-15) / 9)+1][((x-1) / 9)+1];
		}
		else if (orientation ==2){
		one = tetState[(((y-9)-15) / 9)][((x-1) / 9)-1];
		two = tetState[(((y-9)-15) / 9)][((x-1) / 9)];
		three = tetState[(((y-9)-15) / 9)][((x-1) / 9)+1];
		four = tetState[(((y-9)-15) / 9)-1][((x-1) / 9)+1];
		}
		else if (orientation ==3){
		one = tetState[(((y-9)-15) / 9)-1][((x-1) / 9)];
		two = tetState[(((y-9)-15) / 9)][((x-1) / 9)];
		three = tetState[(((y-9)-15) / 9)+1][((x-1) / 9)];
		four = tetState[(((y-9)-15) / 9)+1][((x-1) / 9)+1];
		}		
	}
	else if (shapeType == 'J'){
		if (orientation ==0){
		one = tetState[(((y-9)-15) / 9)-1][((x-1) / 9)];
		two = tetState[(((y-9)-15) / 9)-1][((x-1) / 9)+1];
		three = tetState[(((y-9)-15) / 9)-1][((x-1) / 9)+2];
		four = tetState[(((y-9)-15) / 9)][((x-1) / 9)+2];
		}
		else if (orientation ==1){
		one = tetState[(((y-9)-15) / 9)-1][((x-1) / 9)+1];
		two = tetState[(((y-9)-15) / 9)][((x-1) / 9)+1];
		three = tetState[(((y-9)-15) / 9)+1][((x-1) / 9)+1];
		four = tetState[(((y-9)-15) / 9)+1][((x-1) / 9)];
		}
		else if (orientation ==2){
		one = tetState[(((y-9)-15) / 9)-1][((x-1) / 9)-1];
		two = tetState[(((y-9)-15) / 9)][((x-1) / 9)-1];
		three = tetState[(((y-9)-15) / 9)][((x-1) / 9)];
		four = tetState[(((y-9)-15) / 9)][((x-1) / 9)+1];
		}
		else if (orientation ==3){
		one = tetState[(((y-9)-15) / 9)-1][((x-1) / 9)+1];
		two = tetState[(((y-9)-15) / 9)-1][((x-1) / 9)];
		three = tetState[(((y-9)-15) / 9)][((x-1) / 9)];
		four = tetState[(((y-9)-15) / 9)+1][((x-1) / 9)];
		}		
	}
	else{
	//	EnableInterrupts();
		x -= num;  return 0;
	}
	if(one == 1|| two == 1|| three ==1|| four ==1){
	//	EnableInterrupts();		//enable interrupts before return
		x -= num;  return 0;		//0-invalid
		}
		else{
		//	EnableInterrupts();
			x -= num;;  return 1;		//1-valid
		}

}	
//
//
void Draw (char shapeType, char orientation, const unsigned short *image, char next, char boxflag){//shapeType- is it an O? a T? etc. 
	int temp;								//orientation- how much to rotate it by? 0 = none, 1 = 90°, etc. 
										//image- whether to print the reg. block or the clearing block
											//right now the function only draws O-tetromino
	if (shapeType == 'C'){
		ST7735_DrawBitmap(92, 44, image, 36,26);
	}
	
	else if (shapeType == 'O'){
		temp = CheckBoundary(shapeType, orientation, next);
		if (temp == 0){ return;}	//if CheckBoundary returns a 0, the move is invalid so don't draw.
		ST7735_DrawBitmap(x, y, image, 9,9);
		tetStateMod(x, y, next, boxflag);
		ST7735_DrawBitmap(x+9, y, image, 9,9);
		tetStateMod(x+9, y, next, boxflag);
		ST7735_DrawBitmap(x, y-9, image, 9,9);
		tetStateMod(x, y-9, next, boxflag);
		ST7735_DrawBitmap(x+9, y-9, image, 9,9);
		tetStateMod(x+9, y-9, next, boxflag);
	}
	else if (shapeType == 'I'){
		if (orientation ==0){
		temp = CheckBoundary(shapeType, orientation, next);
		if (temp == 0){return;}
			ST7735_DrawBitmap(x-9, y-9, image, 9,9);
		tetStateMod(x-9, y-9, next, boxflag);
			ST7735_DrawBitmap(x, y-9, image, 9,9);	
		tetStateMod(x, y-9, next, boxflag);
			ST7735_DrawBitmap(x+9, y-9, image, 9,9);
		tetStateMod(x+9, y-9, next, boxflag);
			ST7735_DrawBitmap(x+18, y-9, image, 9,9);
		tetStateMod(x+18, y-9, next, boxflag);
		}
		
		else if (orientation ==1){
		temp = CheckBoundary(shapeType, orientation, next);
		if (temp == 0){return;}
			ST7735_DrawBitmap(x, y-9, image, 9,9);
		tetStateMod(x, y-9, next, boxflag);
			ST7735_DrawBitmap(x, y, image, 9,9);
		tetStateMod(x, y, next, boxflag);
			ST7735_DrawBitmap(x, y+9, image, 9,9);
		tetStateMod(x, y+9, next, boxflag);
			ST7735_DrawBitmap(x, y+18, image, 9,9);
		tetStateMod(x, y+18, next, boxflag);
		}
	}
	else if (shapeType == 'S'){
		if (orientation ==0){
		temp = CheckBoundary(shapeType, orientation, next);
		if (temp == 0){return;}
			ST7735_DrawBitmap(x, y, image, 9,9);
		tetStateMod(x, y, next, boxflag);
			ST7735_DrawBitmap(x+9, y, image, 9,9);
		tetStateMod(x+9, y, next, boxflag);
			ST7735_DrawBitmap(x+9, y-9, image, 9,9);
		tetStateMod(x+9, y-9, next, boxflag);
			ST7735_DrawBitmap(x+18, y-9, image, 9,9);
		tetStateMod(x+18, y-9, next, boxflag);
		}

		else if(orientation == 1){
		temp = CheckBoundary(shapeType, orientation, next);
		if (temp == 0){return;}
			ST7735_DrawBitmap(x, y-9, image, 9,9);
		tetStateMod(x, y-9, next, boxflag);
			ST7735_DrawBitmap(x, y, image, 9,9);
		tetStateMod(x, y, next, boxflag);
			ST7735_DrawBitmap(x+9, y, image, 9,9);
		tetStateMod(x+9, y, next, boxflag);
			ST7735_DrawBitmap(x+9, y+9, image, 9,9);
		tetStateMod(x+9, y+9, next, boxflag);
		}
	}
	else if (shapeType == 'Z'){
		if (orientation == 0){
		temp = CheckBoundary(shapeType, orientation, next);
		if (temp == 0){return;}
			ST7735_DrawBitmap(x, y-9, image, 9,9);
		tetStateMod(x, y-9, next, boxflag);
			ST7735_DrawBitmap(x+9, y-9, image, 9,9);
		tetStateMod(x+9, y-9, next, boxflag);
			ST7735_DrawBitmap(x+9, y, image, 9,9);
		tetStateMod(x+9, y, next, boxflag);
			ST7735_DrawBitmap(x+18, y, image, 9,9);
		tetStateMod(x+18, y, next, boxflag);
		
		}
		
		else if (orientation == 1){
		temp = CheckBoundary(shapeType, orientation, next);
		if (temp == 0){return;}
			ST7735_DrawBitmap(x+9, y-9, image, 9,9);
		tetStateMod(x+9, y-9, next, boxflag);
			ST7735_DrawBitmap(x+9, y, image, 9,9);
		tetStateMod(x+9, y, next, boxflag);
			ST7735_DrawBitmap(x, y, image, 9,9);
		tetStateMod(x, y, next, boxflag);
			ST7735_DrawBitmap(x, y+9, image, 9,9);
		tetStateMod(x, y+9, next, boxflag);
		}
	}
	else if (shapeType == 'T'){
		if (orientation == 0){
		temp = CheckBoundary(shapeType, orientation, next);
		if (temp == 0){return;}
			ST7735_DrawBitmap(x, y-9, image, 9,9);
		tetStateMod(x, y-9, next, boxflag);
			ST7735_DrawBitmap(x+9, y-9, image, 9,9);
		tetStateMod(x+9, y-9, next, boxflag);
			ST7735_DrawBitmap(x+9, y, image, 9,9);
		tetStateMod(x+9, y, next, boxflag);
			ST7735_DrawBitmap(x+18, y-9, image, 9,9);
		tetStateMod(x+18, y-9, next, boxflag);
		}
		else if (orientation == 1){
		temp = CheckBoundary(shapeType, orientation, next);
		if (temp == 0){return;}
			ST7735_DrawBitmap(x, y, image, 9,9);
		tetStateMod(x, y, next, boxflag);
			ST7735_DrawBitmap(x+9, y, image, 9,9);
		tetStateMod(x+9, y, next, boxflag);
			ST7735_DrawBitmap(x+9, y-9, image, 9,9);
		tetStateMod(x+9, y-9, next, boxflag);
			ST7735_DrawBitmap(x+9, y+9, image, 9,9);
		tetStateMod(x+9, y+9, next, boxflag);
		}
		else if (orientation == 2){
		temp = CheckBoundary(shapeType, orientation, next);
		if (temp == 0){return;}
			ST7735_DrawBitmap(x, y, image, 9,9);
		tetStateMod(x, y, next, boxflag);
			ST7735_DrawBitmap(x+9, y, image, 9,9);
		tetStateMod(x+9, y, next, boxflag);
			ST7735_DrawBitmap(x+18, y, image, 9,9);
		tetStateMod(x+18, y, next, boxflag);
			ST7735_DrawBitmap(x+9, y-9, image, 9,9);
		tetStateMod(x+9, y-9, next, boxflag);
		}
		else if (orientation ==3){
		temp = CheckBoundary(shapeType, orientation, next);
		if (temp == 0){return;}
			ST7735_DrawBitmap(x, y-9, image, 9,9);
		tetStateMod(x, y-9, next, boxflag);
			ST7735_DrawBitmap(x, y, image, 9,9);
		tetStateMod(x, y, next, boxflag);
			ST7735_DrawBitmap(x, y+9, image, 9,9);
		tetStateMod(x, y+9, next, boxflag);
			ST7735_DrawBitmap(x+9, y, image, 9,9);
		tetStateMod(x+9, y, next, boxflag);
		}
	}
	else if (shapeType == 'L'){
		if (orientation ==0){
		temp = CheckBoundary(shapeType, orientation, next);
		if (temp == 0){return;}
			ST7735_DrawBitmap(x, y, image, 9,9);
		tetStateMod(x, y, next, boxflag);
			ST7735_DrawBitmap(x, y-9, image, 9,9);
		tetStateMod(x, y-9, next, boxflag);
			ST7735_DrawBitmap(x+9, y-9, image, 9,9);
		tetStateMod(x+9, y-9, next, boxflag);
			ST7735_DrawBitmap(x+18, y-9, image, 9,9);
		tetStateMod(x+18, y-9, next, boxflag);
		}
		else if (orientation ==1){
		temp = CheckBoundary(shapeType, orientation, next);
		if (temp == 0){return;}
			ST7735_DrawBitmap(x, y-9, image, 9,9);
			tetStateMod(x, y-9, next, boxflag);
			ST7735_DrawBitmap(x+9, y-9, image, 9,9);
			tetStateMod(x+9, y-9, next, boxflag);
			ST7735_DrawBitmap(x+9, y, image, 9,9);
			tetStateMod(x+9, y, next, boxflag);
			ST7735_DrawBitmap(x+9, y+9, image, 9,9);
			tetStateMod(x+9, y+9, next, boxflag);
		}
		else if (orientation ==2){
		temp = CheckBoundary(shapeType, orientation, next);
		if (temp == 0){return;}
			ST7735_DrawBitmap(x-9, y, image, 9,9);
			tetStateMod(x-9, y, next, boxflag);
			ST7735_DrawBitmap(x, y, image, 9,9);
			tetStateMod(x, y, next, boxflag);
			ST7735_DrawBitmap(x+9, y, image, 9,9);
			tetStateMod(x+9, y, next, boxflag);
			ST7735_DrawBitmap(x+9, y-9, image, 9,9);
			tetStateMod(x+9, y-9, next, boxflag);
		}
		else if (orientation ==3){
		temp = CheckBoundary(shapeType, orientation, next);
		if (temp == 0){return;}
			ST7735_DrawBitmap(x, y-9, image, 9,9);
			tetStateMod(x, y-9, next, boxflag);
			ST7735_DrawBitmap(x, y, image, 9,9);
			tetStateMod(x, y, next, boxflag);
			ST7735_DrawBitmap(x, y+9, image, 9,9);
			tetStateMod(x, y+9, next, boxflag);
			ST7735_DrawBitmap(x+9, y+9, image, 9,9);
			tetStateMod(x+9, y+9, next, boxflag);
		}		
	}
	else if (shapeType == 'J'){
		if (orientation ==0){
		temp = CheckBoundary(shapeType, orientation, next);
		if (temp == 0){return;}
			ST7735_DrawBitmap(x, y-9, image, 9,9);
			tetStateMod(x, y-9, next, boxflag);
			ST7735_DrawBitmap(x+9, y-9, image, 9,9);
			tetStateMod(x+9, y-9, next, boxflag);
			ST7735_DrawBitmap(x+18, y-9, image, 9,9);
			tetStateMod(x+18, y-9, next, boxflag);
			ST7735_DrawBitmap(x+18, y, image, 9,9);
			tetStateMod(x+18, y, next, boxflag);
		}
		else if (orientation ==1){
		temp = CheckBoundary(shapeType, orientation, next);
		if (temp == 0){return;}
			ST7735_DrawBitmap(x+9, y-9, image, 9,9);
			tetStateMod(x+9, y-9, next, boxflag);
			ST7735_DrawBitmap(x+9, y, image, 9,9);
			tetStateMod(x+9, y, next, boxflag);
			ST7735_DrawBitmap(x+9, y+9, image, 9,9);
			tetStateMod(x+9, y+9, next, boxflag);
			ST7735_DrawBitmap(x, y+9, image, 9,9);
			tetStateMod(x, y+9, next, boxflag);
		}
		else if (orientation ==2){
		temp = CheckBoundary(shapeType, orientation, next);
		if (temp == 0){return;}
			ST7735_DrawBitmap(x-9, y-9, image, 9,9);
			tetStateMod(x-9, y-9, next, boxflag);
			ST7735_DrawBitmap(x-9, y, image, 9,9);
			tetStateMod(x-9, y, next, boxflag);
			ST7735_DrawBitmap(x, y, image, 9,9);
			tetStateMod(x, y, next, boxflag);
			ST7735_DrawBitmap(x+9, y, image, 9,9);
			tetStateMod(x+9, y, next, boxflag);
		}
		else if (orientation ==3){
		temp = CheckBoundary(shapeType, orientation, next);
		if (temp == 0){return;}
			ST7735_DrawBitmap(x+9, y-9, image, 9,9);
			tetStateMod(x+9, y-9, next, boxflag);
			ST7735_DrawBitmap(x, y-9, image, 9,9);
			tetStateMod(x, y-9, next, boxflag);
			ST7735_DrawBitmap(x, y, image, 9,9);
			tetStateMod(x, y, next, boxflag);
			ST7735_DrawBitmap(x, y+9, image, 9,9);
			tetStateMod(x, y+9, next, boxflag);
		}		
	}
	else{
		return;
	}
}	


//
//
void Draw_No_Int (char shapeType, char orientation, const unsigned short *image, char next, char boxflag){//shapeType- is it an O? a T? etc.
	//int temp;								//orientation- how much to rotate it by? 0 = none, 1 = 90°, etc. 
	DisableInterrupts();					//image- whether to print the reg. block or the clearing block
											//right now the function only draws O-tetromino
	
	if (shapeType == 'C'){
		ST7735_DrawBitmap(92, 44, image, 36,26);
	}
	
	else if (shapeType == 'O'){
		//temp = CheckBoundary(shapeType, orientation, next);
		//if (temp == 0){ return;}	//if CheckBoundary returns a 0, the move is invalid so don't draw.
		ST7735_DrawBitmap(x, y, image, 9,9);
		tetStateMod(x, y, next, boxflag);
		ST7735_DrawBitmap(x+9, y, image, 9,9);
		tetStateMod(x+9, y, next, boxflag);
		ST7735_DrawBitmap(x, y-9, image, 9,9);
		tetStateMod(x, y-9, next, boxflag);
		ST7735_DrawBitmap(x+9, y-9, image, 9,9);
		tetStateMod(x+9, y-9, next, boxflag);
	}
	else if (shapeType == 'I'){
		if (orientation ==0){
//		temp = CheckBoundary(shapeType, orientation, next);
//		if (temp == 0){return;}
			ST7735_DrawBitmap(x-9, y-9, image, 9,9);
		tetStateMod(x-9, y-9, next, boxflag);
			ST7735_DrawBitmap(x, y-9, image, 9,9);	
		tetStateMod(x, y-9, next, boxflag);
			ST7735_DrawBitmap(x+9, y-9, image, 9,9);
		tetStateMod(x+9, y-9, next, boxflag);
			ST7735_DrawBitmap(x+18, y-9, image, 9,9);
		tetStateMod(x+18, y-9, next, boxflag);
		}
		
		else if (orientation ==1){
//		temp = CheckBoundary(shapeType, orientation, next);
//		if (temp == 0){return;}
			ST7735_DrawBitmap(x, y-9, image, 9,9);
		tetStateMod(x, y-9, next, boxflag);
			ST7735_DrawBitmap(x, y, image, 9,9);
		tetStateMod(x, y, next, boxflag);
			ST7735_DrawBitmap(x, y+9, image, 9,9);
		tetStateMod(x, y+9, next, boxflag);
			ST7735_DrawBitmap(x, y+18, image, 9,9);
		tetStateMod(x, y+18, next, boxflag);
		}
	}
	else if (shapeType == 'S'){
		if (orientation ==0){
//		temp = CheckBoundary(shapeType, orientation, next);
//		if (temp == 0){return;}
			ST7735_DrawBitmap(x, y, image, 9,9);
		tetStateMod(x, y, next, boxflag);
			ST7735_DrawBitmap(x+9, y, image, 9,9);
		tetStateMod(x+9, y, next, boxflag);
			ST7735_DrawBitmap(x+9, y-9, image, 9,9);
		tetStateMod(x+9, y-9, next, boxflag);
			ST7735_DrawBitmap(x+18, y-9, image, 9,9);
		tetStateMod(x+18, y-9, next, boxflag);
		}

		else if(orientation == 1){
//		temp = CheckBoundary(shapeType, orientation, next);
//		if (temp == 0){return;}
			ST7735_DrawBitmap(x, y-9, image, 9,9);
		tetStateMod(x, y-9, next, boxflag);
			ST7735_DrawBitmap(x, y, image, 9,9);
		tetStateMod(x, y, next, boxflag);
			ST7735_DrawBitmap(x+9, y, image, 9,9);
		tetStateMod(x+9, y, next, boxflag);
			ST7735_DrawBitmap(x+9, y+9, image, 9,9);
		tetStateMod(x+9, y+9, next, boxflag);
		}
	}
	else if (shapeType == 'Z'){
		if (orientation == 0){
//		temp = CheckBoundary(shapeType, orientation, next);
//		if (temp == 0){return;}
			ST7735_DrawBitmap(x, y-9, image, 9,9);
		tetStateMod(x, y-9, next, boxflag);
			ST7735_DrawBitmap(x+9, y-9, image, 9,9);
		tetStateMod(x+9, y-9, next, boxflag);
			ST7735_DrawBitmap(x+9, y, image, 9,9);
		tetStateMod(x+9, y, next, boxflag);
			ST7735_DrawBitmap(x+18, y, image, 9,9);
		tetStateMod(x+18, y, next, boxflag);
		
		}
		
		else if (orientation == 1){
//		temp = CheckBoundary(shapeType, orientation, next);
//		if (temp == 0){return;}
			ST7735_DrawBitmap(x+9, y-9, image, 9,9);
		tetStateMod(x+9, y-9, next, boxflag);
			ST7735_DrawBitmap(x+9, y, image, 9,9);
		tetStateMod(x+9, y, next, boxflag);
			ST7735_DrawBitmap(x, y, image, 9,9);
		tetStateMod(x, y, next, boxflag);
			ST7735_DrawBitmap(x, y+9, image, 9,9);
		tetStateMod(x, y+9, next, boxflag);
		}
	}
	else if (shapeType == 'T'){
		if (orientation == 0){
//			temp = CheckBoundary(shapeType, orientation, next);
//			if (temp == 0){return;}
			ST7735_DrawBitmap(x, y-9, image, 9,9);
			tetStateMod(x, y-9, next, boxflag);
			ST7735_DrawBitmap(x+9, y-9, image, 9,9);
			tetStateMod(x+9, y-9, next, boxflag);
			ST7735_DrawBitmap(x+9, y, image, 9,9);
			tetStateMod(x+9, y, next, boxflag);
			ST7735_DrawBitmap(x+18, y-9, image, 9,9);
			tetStateMod(x+18, y-9, next, boxflag);
		}
		else if (orientation == 1){
//		temp = CheckBoundary(shapeType, orientation, next);
//		if (temp == 0){return;}
			ST7735_DrawBitmap(x, y, image, 9,9);
		tetStateMod(x, y, next, boxflag);
			ST7735_DrawBitmap(x+9, y, image, 9,9);
		tetStateMod(x+9, y, next, boxflag);
			ST7735_DrawBitmap(x+9, y-9, image, 9,9);
		tetStateMod(x+9, y-9, next, boxflag);
			ST7735_DrawBitmap(x+9, y+9, image, 9,9);
		tetStateMod(x+9, y+9, next, boxflag);
		}
		else if (orientation == 2){
//		temp = CheckBoundary(shapeType, orientation, next);
//		if (temp == 0){return;}
			ST7735_DrawBitmap(x, y, image, 9,9);
		tetStateMod(x, y, next, boxflag);
			ST7735_DrawBitmap(x+9, y, image, 9,9);
		tetStateMod(x+9, y, next, boxflag);
			ST7735_DrawBitmap(x+18, y, image, 9,9);
		tetStateMod(x+18, y, next, boxflag);
			ST7735_DrawBitmap(x+9, y-9, image, 9,9);
		tetStateMod(x+9, y-9, next, boxflag);
		}
		else if (orientation ==3){
//		temp = CheckBoundary(shapeType, orientation, next);
//		if (temp == 0){return;}
			ST7735_DrawBitmap(x, y-9, image, 9,9);
		tetStateMod(x, y-9, next, boxflag);
			ST7735_DrawBitmap(x, y, image, 9,9);
		tetStateMod(x, y, next, boxflag);
			ST7735_DrawBitmap(x, y+9, image, 9,9);
		tetStateMod(x, y+9, next, boxflag);
			ST7735_DrawBitmap(x+9, y, image, 9,9);
		tetStateMod(x+9, y, next, boxflag);
		}
	}
	else if (shapeType == 'L'){
		if (orientation ==0){
//		temp = CheckBoundary(shapeType, orientation, next);
//		if (temp == 0){return;}
			ST7735_DrawBitmap(x, y, image, 9,9);
		tetStateMod(x, y, next, boxflag);
			ST7735_DrawBitmap(x, y-9, image, 9,9);
		tetStateMod(x, y-9, next, boxflag);
			ST7735_DrawBitmap(x+9, y-9, image, 9,9);
		tetStateMod(x+9, y-9, next, boxflag);
			ST7735_DrawBitmap(x+18, y-9, image, 9,9);
		tetStateMod(x+18, y-9, next, boxflag);
		}
		else if (orientation ==1){
//		temp = CheckBoundary(shapeType, orientation, next);
//		if (temp == 0){return;}
			ST7735_DrawBitmap(x, y-9, image, 9,9);
			tetStateMod(x, y-9, next, boxflag);
			ST7735_DrawBitmap(x+9, y-9, image, 9,9);
			tetStateMod(x+9, y-9, next, boxflag);
			ST7735_DrawBitmap(x+9, y, image, 9,9);
			tetStateMod(x+9, y, next, boxflag);
			ST7735_DrawBitmap(x+9, y+9, image, 9,9);
			tetStateMod(x+9, y+9, next, boxflag);
		}
		else if (orientation ==2){
//		temp = CheckBoundary(shapeType, orientation, next);
//		if (temp == 0){return;}
			ST7735_DrawBitmap(x-9, y, image, 9,9);
			tetStateMod(x-9, y, next, boxflag);
			ST7735_DrawBitmap(x, y, image, 9,9);
			tetStateMod(x, y, next, boxflag);
			ST7735_DrawBitmap(x+9, y, image, 9,9);
			tetStateMod(x+9, y, next, boxflag);
			ST7735_DrawBitmap(x+9, y-9, image, 9,9);
			tetStateMod(x+9, y-9, next, boxflag);
		}
		else if (orientation ==3){
//		temp = CheckBoundary(shapeType, orientation, next);
//		if (temp == 0){return;}
			ST7735_DrawBitmap(x, y-9, image, 9,9);
			tetStateMod(x, y-9, next, boxflag);
			ST7735_DrawBitmap(x, y, image, 9,9);
			tetStateMod(x, y, next, boxflag);
			ST7735_DrawBitmap(x, y+9, image, 9,9);
			tetStateMod(x, y+9, next, boxflag);
			ST7735_DrawBitmap(x+9, y+9, image, 9,9);
			tetStateMod(x+9, y+9, next, boxflag);
		}		
	}
	else if (shapeType == 'J'){
		if (orientation ==0){
//		temp = CheckBoundary(shapeType, orientation, next);
//		if (temp == 0){return;}
			ST7735_DrawBitmap(x, y-9, image, 9,9);
			tetStateMod(x, y-9, next, boxflag);
			ST7735_DrawBitmap(x+9, y-9, image, 9,9);
			tetStateMod(x+9, y-9, next, boxflag);
			ST7735_DrawBitmap(x+18, y-9, image, 9,9);
			tetStateMod(x+18, y-9, next, boxflag);
			ST7735_DrawBitmap(x+18, y, image, 9,9);
			tetStateMod(x+18, y, next, boxflag);
		}
		else if (orientation ==1){
//		temp = CheckBoundary(shapeType, orientation, next);
//		if (temp == 0){return;}
			ST7735_DrawBitmap(x+9, y-9, image, 9,9);
			tetStateMod(x+9, y-9, next, boxflag);
			ST7735_DrawBitmap(x+9, y, image, 9,9);
			tetStateMod(x+9, y, next, boxflag);
			ST7735_DrawBitmap(x+9, y+9, image, 9,9);
			tetStateMod(x+9, y+9, next, boxflag);
			ST7735_DrawBitmap(x, y+9, image, 9,9);
			tetStateMod(x, y+9, next, boxflag);
		}
		else if (orientation ==2){
//		temp = CheckBoundary(shapeType, orientation, next);
//		if (temp == 0){return;}
			ST7735_DrawBitmap(x-9, y-9, image, 9,9);
			tetStateMod(x-9, y-9, next, boxflag);
			ST7735_DrawBitmap(x-9, y, image, 9,9);
			tetStateMod(x-9, y, next, boxflag);
			ST7735_DrawBitmap(x, y, image, 9,9);
			tetStateMod(x, y, next, boxflag);
			ST7735_DrawBitmap(x+9, y, image, 9,9);
			tetStateMod(x+9, y, next, boxflag);
		}
		else if (orientation ==3){
//		temp = CheckBoundary(shapeType, orientation, next);
//		if (temp == 0){return;}
			ST7735_DrawBitmap(x+9, y-9, image, 9,9);
			tetStateMod(x+9, y-9, next, boxflag);
			ST7735_DrawBitmap(x, y-9, image, 9,9);
			tetStateMod(x, y-9, next, boxflag);
			ST7735_DrawBitmap(x, y, image, 9,9);
			tetStateMod(x, y, next, boxflag);
			ST7735_DrawBitmap(x, y+9, image, 9,9);
			tetStateMod(x, y+9, next, boxflag);
		}		
	}
	else{
		EnableInterrupts();
		return;
	}
	EnableInterrupts();
}		



//
//
void Rotate(void){
	DisableInterrupts();
	if(currentshape == 'O'){EnableInterrupts(); return;}
	Draw(currentshape, currentrotate, Black_Block, 0, 0);	//First get rid of the shape
	currentrotate++;		//current rotation switched to next rotation	
	if (currentshape == 'S' || currentshape == 'Z' || currentshape == 'I'){
		if (currentrotate == 2){
			currentrotate = 0;
		}
	}
		else {
			if (currentrotate == 4){
				currentrotate = 0;
			}
		}
		
		if (CheckBoundary(currentshape, currentrotate, 0)){
				if(check_horiz_move(currentshape,currentrotate,next,0)){
					y-=9;	//move the shape up bexause chech vertical will move it down automatically
					if(check_vertical_move(currentshape, currentrotate, next)){	
						y+=9; //put y in its original position
						Draw(currentshape, currentrotate, Block, 0, 1);	
						RotateSound();		//play the rotate sound

					}else {currentrotate --; Draw(currentshape, currentrotate, Block, 0, 1); EnableInterrupts(); return;}
				}else{currentrotate --; Draw(currentshape, currentrotate, Block, 0, 1); EnableInterrupts(); return;}	
			}else{currentrotate --; Draw(currentshape, currentrotate, Block, 0, 1); EnableInterrupts(); return;}
	
	EnableInterrupts();
}
//
//

//uint32_t ADCMail, ADCStatus;
//
void Timer2A_Handler(void){
	TIMER2_ICR_R = 0x01;
	TIMER0_CTL_R = 0x0000000;    //enable TIMER0A
	
	//horizontal move
	int a = ADC_In();
	if (a >= 3000){
			x+= 9;
			int temp1 = CheckBoundary(currentshape, currentrotate, 0);
			//int temp2	= check_horiz_move(currentshape, currentrotate, next, 9);
		
				if (temp1 == 0 ){
					x-=9;		//if CheckBoundary is 0, move is invalid, so return the coordinates to what they were
					TIMER0_CTL_R = 0x00000001; 
					return;
					}else{
							x-=9;		//put the shape to where it was
							Draw_No_Int(currentshape, currentrotate, Black_Block, 0, 0);		//first git rid of the shape
						//x+=9;
							if(check_horiz_move(currentshape, currentrotate, next,9)){			//then check to see if move down  box valid
							x+=9;
							Draw_No_Int(currentshape, currentrotate, Block, 0, 1);		//if so move box horizantally
								VertSound();	//play the soundeffect
							}
								else{
									//x-=9;
									Draw_No_Int(currentshape, currentrotate, Block, 0, 1);		//if not valid put the shappe back where it was
									}
		

						}
	}
	else if (a <= 1200){

			x-= 9;
			int temp1 = CheckBoundary(currentshape, currentrotate, 0);
			//int temp2	= check_horiz_move(currentshape, currentrotate, next, 9);
		
				if (temp1 == 0 ){
					x+=9;		//if CheckBoundary is 0, move is invalid, so return the coordinates to what they were
					TIMER0_CTL_R = 0x00000001; 
					return;
					}else
							{x+=9;		//put the shape to where it was
							Draw_No_Int(currentshape, currentrotate, Black_Block, 0, 0);		//first git rid of the shape
							//x-=9;
							if(check_horiz_move(currentshape, currentrotate, next,-9)){			//then check to see if move down  box valid
							x-=9;
							Draw_No_Int(currentshape, currentrotate, Block, 0, 1);		//if so move box horizantally
								VertSound();	//play the soundeffect
							}
							else{
								//x+=9;				
								Draw_No_Int(currentshape, currentrotate, Block, 0, 1);		//if not valid put the shappe back where it was
								}
					//else{//TIMER0_CTL_R = 0x0000000;  
						//return;}
					//TIMER0_CTL_R = 0x0000001; 
					//slideflag = 1;			//set slideflag for drop1block function
					}
	}
	TIMER0_CTL_R = 0x0000001;
}
// *************************** Capture image dimensions out of BMP**********

void Tetris_Init(void){
	ST7735_InitR(INITR_REDTAB);
	ST7735_DrawBitmap(0, 159, Tetris_Menu, 128,160);
	IO_Touch();
	ST7735_DrawBitmap(0, 159, TetrisBG, 128,160);
	//ST7735_DrawBitmap(column (px), row(px), img, width(px), height(px));
	ST7735_DrawString(0,0, "Score:", ST7735_BLACK, ST7735_GREEN);
	ST7735_DrawString(16, 1, "Next", ST7735_BLACK, ST7735_GREEN);
	ST7735_DrawString(16, 6, "Level", ST7735_BLACK, ST7735_GREEN);
	ST7735_DrawString(16, 11, "Lines", ST7735_BLACK, ST7735_GREEN);
	ST7735_DrawString(17, 8, &array[(level10) *2], ST7735_BLACK, ST7735_MAGENTA);	//Initialize level to 0	tens
	ST7735_DrawString(18, 8, &array[(level1) *2], ST7735_BLACK, ST7735_MAGENTA);	//Initialize level to 0		ones
	ST7735_DrawString(17, 13, &array[(line100) *2], ST7735_BLACK, ST7735_MAGENTA);	//Initialize #lines to 0 tens	
	ST7735_DrawString(18, 13, &array[(line10) *2], ST7735_BLACK, ST7735_MAGENTA);	//Initialize #lines to 0 tens
	ST7735_DrawString(19, 13, &array[(line1) *2], ST7735_BLACK, ST7735_MAGENTA);	//Initialize #lines to 0	ones
	ST7735_DrawString(7, 00, &array[(score10000) *2], ST7735_BLACK, ST7735_MAGENTA);		//Initialize score to 0	thosnds	
	ST7735_DrawString(8, 00, &array[(score1000) *2], ST7735_BLACK, ST7735_MAGENTA);		//Initialize score to 0	thosnds
	ST7735_DrawString(9, 00, &array[(score100) *2], ST7735_BLACK, ST7735_MAGENTA);		//Initialize score to 0	hunds
	ST7735_DrawString(10, 00, &array[(score10) *2], ST7735_BLACK, ST7735_MAGENTA);		//Initialize score to 0	tens
	ST7735_DrawString(11,00, &array[(score1) *2], ST7735_BLACK, ST7735_MAGENTA);		//Initialize score to 0	ones
	
//Initialize the Tetris state to zeros
		
	
}

//******************************** Handlers****************************
void Timer0A_Handler(void){			//periodic interrupt that gets the current tetromino to fall
	TIMER0_ICR_R = TIMER_ICR_TATOCINT;// acknowledge TIMER0A timeout
	TIMER2_CTL_R = 0x00000000;
	
	drop1block();
	
	TIMER2_CTL_R = 0x00000001;
}


//
char Random_Letter(void){		
	//int input = Random();
	//input = input % 7;
	int input = (Random())%60; // a number from 0 to 59
	if (input  >= 0 && input <9){				
		return 'O';
	}
	else if (input  >= 9 && input <17){
		return 'I';
	}
	else if (input  >= 17 && input <25){
		return 'S';
	}
	else if (input  >= 25 && input <33){
		return 'Z';
	}
	else if (input  >= 33 && input <41){
		return 'T';
	}
	else if (input  >= 41 && input <49){
		return 'L';
	}
	else if (input  >= 49 && input <=59){
		return 'J';
	}
		return Random_Letter();	
}	

//
void CycleShape(void){			
	currentshape = next;		//set the shape that is said to be next as the current one
	currentrotate = 0;		//All initial outputs have rotation value 0
	next = Random_Letter();
	x = 92, y = 44;
	Draw('C',0,NextBox,1,2);	//Clears next box before 
	next = Random_Letter();
	if (next == 'I'){
		x = 101, y = 39;}
	else{
		x = 98, y = 39;}
	Draw(next, 0, Block, 1, 1);				//display next shape in correct place, reset x and y
	x=37, y=33;
}

//
void drop1block(void){
	DisableInterrupts();
	
		Draw_No_Int(currentshape, currentrotate, Black_Block, 0, 0);		//first git rid of the shape
		if(check_vertical_move(currentshape, currentrotate, next)){			//then check to see if move down  box valid
			y+=9;
			Draw_No_Int(currentshape, currentrotate, Block, 0, 1);		//if so drop by  box
		}
		else{
			Draw_No_Int(currentshape, currentrotate, Block, 0, 1);		//if not valid put the shappe back where it was
			{//let horizontally move for 100ms
				{EnableInterrupts();
				TIMER2_CTL_R = 0x00000001;
					Delay10ms(500);
				TIMER2_CTL_R = 0x00000000;
			  DisableInterrupts();}
					//done; next shape; increase speed
									if(y <= 33 ){	
								TIMER0_ICR_R = 0x01;
								TIMER1_ICR_R = 0x01;
								TIMER2_ICR_R = 0x01;
								TIMER0_CTL_R = 0x0000000;    //disable TIMER0
								TIMER1_CTL_R = 0x0000001;    //disable TIMER1
								TIMER2_CTL_R = 0x0000000;    //disable TIMER2 
								EnableInterrupts();
								LostSound();
								endNow(); 
					}
					if(speed > 60000000){
							speed -= 2500000;
							ADCspeed-=2500000;
							Timer0_Init(speed);}//*************************
						else if(speed > 5000000){
								speed -= 500000;
								ADCspeed -= 500000;
								Timer0_Init(speed);}//*************************
							else if(speed > 4000000){
									speed -= 70000;
									ADCspeed -= 70000;
									Timer0_Init(speed);}//*************************
					levelcnt++;
uint32_t temp = ArrayShift(16, 10, tetState);
					play_sound(temp);
					Mod_score(temp);
					Mod_line(temp);
					Mod_level();

					CycleShape();
					x=37;
					y=33;
			}
		}
	
	EnableInterrupts();
}


//
void play_sound(uint32_t count){
	switch(count){
		case 0:
			NextSound();
			break;
		case 1:
			LineSound();
			break;
		case 2:
			MultiLineSound();
			break;
		case 3:
			MultiLineSound();
			break;
		case 4:
			FourLinesSound();
			break;
		default:
			break;
	};
		
	
}

//
void Mod_line(uint32_t count){
uint32_t tempL = 0;//temporary score 
	tempL = ((line100 + tempL)*10 +line10)*10 +line1;
		switch(count){
		case 0:
			return;
		case 1:
			tempL += 1;//add 1 line
			break;
		case 2:
			tempL += 2;//add 2 lines
			break;
		case 3:
			tempL += 3;//add 3 lines
			break;
		case 4:
			tempL += 4;//add 4 lines
			break;
		default:
			break;
	};		
	//Update the score variables
	line1 = tempL % 10;//get the ones
		tempL /= 10;
	line10 = tempL % 10;// get the tens
		tempL /= 10;
	line100 = tempL % 10;// get the hundreds
	//update the score on the LCD
	ST7735_DrawString(17, 13, &array[(line100) *2], ST7735_BLACK, ST7735_MAGENTA);	//Initialize #lines to 0 tens	
	ST7735_DrawString(18, 13, &array[(line10) *2], ST7735_BLACK, ST7735_MAGENTA);	//Initialize #lines to 0 tens
	ST7735_DrawString(19, 13, &array[(line1) *2], ST7735_BLACK, ST7735_MAGENTA);	//Initialize #lines to 0	ones

}
//
void Mod_level(void){
uint32_t tempL = 0;//temporary score 
	tempL = (level10 + tempL)*10 +level1;

	if( levelcnt >= 15){
		levelcnt = 0;
		tempL++;
		//Update the score variables
		level1 = tempL % 10;//get the ones
			tempL /= 10;
		level10 = tempL % 10;// get the tens
		//update the score on the LCD
	ST7735_DrawString(17, 8, &array[(level10) *2], ST7735_BLACK, ST7735_MAGENTA);	//Initialize level to 0	tens
	ST7735_DrawString(18, 8, &array[(level1) *2], ST7735_BLACK, ST7735_MAGENTA);	//Initialize level to 0		ones
	}
}
//
void Mod_score(uint32_t count){
uint32_t tempS = 0;//temporary score 
	tempS = ((((score10000 + tempS)*10 +score1000)*10 +score100)*10 +score10)*10 +score1;
		switch(count){
		case 0:
			return;
		case 1:
			tempS += 10;//add 10 pts
			break;
		case 2:
			tempS += 25;//add 25 pts
			break;
		case 3:
			tempS += 40;//add 40 pts
			break;
		case 4:
			tempS += 65;//add 65 pts
			break;
		default:
			break;
	};		
	//Update the score variables
	score1 = tempS % 10;//get the ones
		tempS /= 10;
	score10 = tempS % 10;// get the tens
		tempS /= 10;
	score100 = tempS % 10;//get the hundreds
		tempS /= 10;
	score1000 = tempS % 10;// get the thousands
		tempS /= 10;
	score10000 = tempS % 10;//get the millions
	//update the score on the LCD
	ST7735_DrawString(7, 0, &array[(score10000) *2], ST7735_BLACK, ST7735_MAGENTA);		//Initialize score to 0	thosnds	
	ST7735_DrawString(8, 0, &array[(score1000) *2], ST7735_BLACK, ST7735_MAGENTA);		//Initialize score to 0	thosnds
	ST7735_DrawString(9, 0, &array[(score100) *2], ST7735_BLACK, ST7735_MAGENTA);		//Initialize score to 0	hunds
	ST7735_DrawString(10,0, &array[(score10) *2], ST7735_BLACK, ST7735_MAGENTA);		//Initialize score to 0	tens
	ST7735_DrawString(11,0, &array[(score1) *2], ST7735_BLACK, ST7735_MAGENTA);		//Initialize score to 0	ones

}

//
void PortF_Init(void){
	volatile uint32_t delay;
	SYSCTL_RCGCGPIO_R |=0x20;			//activate port F clock	
	delay = SYSCTL_RCGCGPIO_R;			//delay
	GPIO_PORTF_DIR_R |= 0x0E;			//Make PF1-3 outputs
	GPIO_PORTF_DEN_R |= 0x0E;			//Enable the diigital function for PF1-3
	GPIO_PORTF_AFSEL_R &= ~0x0E;		//Disable Alternate function
}

//

void TestCycleShapes(void){			//function that tests the current and next shape and clearing
									//make sure to set currentshape = Random_Letter(); before calling the function
	//NOTE: The if-else statements are only because the I block is uncentered when you use x = 99.
	//if statement only there to center the block when it's an I.
		Draw(currentshape, 0, Block, 0, 1);		//currentshape is a global variable
		next = Random_Letter();
		if (next == 'I'){
			x = 101, y = 39;}
		else{
			x = 98, y = 39;}
		Draw(next, 0, Block, 1, 1);				//display next shape in correct place, reset x and y
		x = 37, y = 33;
		IO_Touch();
		Draw(currentshape, 0, Black_Block, 0, 0);
		if (next == 'I'){
			x = 101, y = 39;}
		else{
			x = 98, y = 39;}
		Draw(next, 0, Purple_Block, 1, 2);		//clear "Next" box before printing the next block
		currentshape = next;
		x=37, y = 33;
}


void TestRotation(void){
	DisableInterrupts();
	Draw_No_Int(currentshape, currentrotate, Block, 0, 1);		//currentshape is a global variable
	Rotate();
	EnableInterrupts();
	
}
void TestAllShapes(void){
	Draw('O', 0, Block, 0, 1);
	IO_Touch();
	ST7735_DrawBitmap(0, 159, TetrisBG, 128,160);
	Draw('I', 0, Block, 0, 1);
	IO_Touch();
	ST7735_DrawBitmap(0, 159, TetrisBG, 128,160);
	Draw('I', 1, Block, 0, 1);
	IO_Touch();
	ST7735_DrawBitmap(0, 159, TetrisBG, 128,160);
	Draw('S', 0, Block, 0, 1);
	IO_Touch();
	ST7735_DrawBitmap(0, 159, TetrisBG, 128,160);
	Draw('S', 1, Block, 0, 1);
	IO_Touch();
	ST7735_DrawBitmap(0, 159, TetrisBG, 128,160);
	Draw('Z', 0, Block, 0, 1);
	IO_Touch();
	ST7735_DrawBitmap(0, 159, TetrisBG, 128,160);
	Draw('Z', 1, Block, 0, 1);
	IO_Touch();
	ST7735_DrawBitmap(0, 159, TetrisBG, 128,160);
	Draw('T', 0, Block, 0, 1);
	IO_Touch();
	ST7735_DrawBitmap(0, 159, TetrisBG, 128,160);
	Draw('T', 1, Block, 0, 1);
	IO_Touch();
	ST7735_DrawBitmap(0, 159, TetrisBG, 128,160);
	Draw('T', 2, Block, 0,1);
	IO_Touch();
	ST7735_DrawBitmap(0, 159, TetrisBG, 128,160);
	Draw('T', 3, Block, 0, 1);
	IO_Touch();
	ST7735_DrawBitmap(0, 159, TetrisBG, 128,160);
	Draw('L', 0, Block, 0, 1);
	IO_Touch();
	ST7735_DrawBitmap(0, 159, TetrisBG, 128,160);
	Draw('L', 1, Block, 0, 1);
	IO_Touch();
	ST7735_DrawBitmap(0, 159, TetrisBG, 128,160);
	Draw('L', 2, Block, 0, 1);
	IO_Touch();
	ST7735_DrawBitmap(0, 159, TetrisBG, 128,160);
	Draw('L', 3, Block, 0, 1);
	IO_Touch();
	ST7735_DrawBitmap(0, 159, TetrisBG, 128,160);
	Draw('J', 0, Block, 0, 1);
	IO_Touch();
	ST7735_DrawBitmap(0, 159, TetrisBG, 128,160);
	Draw('J', 1, Block, 0, 1);
	IO_Touch();
	ST7735_DrawBitmap(0, 159, TetrisBG, 128,160);
	Draw('J', 2, Block, 0, 1);
	IO_Touch();
	ST7735_DrawBitmap(0, 159, TetrisBG, 128,160);
	Draw('J', 3, Block, 0, 1);
	IO_Touch();
	ST7735_DrawBitmap(0, 159, TetrisBG, 128,160);	
}
//
//
uint32_t ArrayShift(int size1, int size2, uint32_t map[][size2]){
	DisableInterrupts();
	uint32_t linecnt=0;
    for(int row = 15; row >= 0; row--){
        if(all_1s(map[row], 10)){					//calls all_1s to check that the current row is all ones
            for(int ones = 0; ones < 10; ones++){			
                map[row][ones] = 0;
								ST7735_DrawBitmap(((ones*9)+1), ((row*9)+24), Black_Block, 9,9);
						}
            for(int top = row - 1; top >= 0; top--){			//goes row by row	
                for(int col = 0; col < 10; col++){			//drops elements one by one
                    map[top+1][col] = map[top][col];     //map[i+1][i2] = bottom el ; map[i][i2] = top el
										if(map[top][col]){
												ST7735_DrawBitmap(((col*9)+1), (((top+1)*9)+24), Block, 9,9);
											}else ST7735_DrawBitmap(((col*9)+1), (((top+1)*9)+24), Black_Block, 9,9);

                    map[top][col] = 0;
										ST7735_DrawBitmap(((col*9)+1), ((top*9)+24), Black_Block, 9,9);
											
                }
							if(top == 0) {
								row ++; 
								linecnt++;				//inc linecnt after eahc line has been remove (in every drop)
									
								}
						}
        }	
    }
			///	

		////
		EnableInterrupts();
    return linecnt;
}
//
void endNow(void){
		DisableInterrupts();
	ST7735_DrawBitmap(0, 159, GameOver, 128,160);
///	ST7735_DrawString(30,60, "test score", ST7735_BLACK, ST7735_BLACK);
	ST7735_DrawString(6, 6, &array[(score10000) *2], ST7735_BLACK, ST7735_BLACK);		//Initialize score to 0	thosnds	
	ST7735_DrawString(7, 6, &array[(score1000) *2], ST7735_BLACK, ST7735_BLACK);		//Initialize score to 0	thosnds
	ST7735_DrawString(8, 6, &array[(score100) *2], ST7735_BLACK, ST7735_BLACK);		//Initialize score to 0	hunds
	ST7735_DrawString(9,6, &array[(score10) *2], ST7735_BLACK, ST7735_BLACK);		//Initialize score to 0	tens
	ST7735_DrawString(10,6, &array[(score1) *2], ST7735_BLACK, ST7735_BLACK);		//Initialize score to 0	ones
	IO_Touch();
	Tetris_Init();
	tetState_Init();
	line10 = 0, line1=0, level10 = 0,level1=0;				//global variable for # lines
	score1 = 0,score10 = 0,score100 = 0,score1000 = 0, score10000 = 0;				//global variable for score
  x=37, y=33, speed= 70000000,ADCspeed=30000000;
//	Timer0_Init(speed);				
//	Timer1_Init(7255);
//	Timer2_Init(30000000);
	Timer3_Init();
	Sound_Init();
	currentshape = Random_Letter();
	next = Random_Letter();
	if (next == 'I'){
		x = 101, y = 39;}
	else{
		x = 98, y = 39;}
	Draw(currentshape, 0, Block, 0, 1);
	Draw(next, 0, Block, 1, 1);				//display next shape in correct place
	x = 37, y = 33; 
	currentrotate = 0;
	EnableInterrupts();

		/*added end function
added game over screen with print coordinates
redid menu screen


Notes: Have an infinite while loop at the end of main- let's have it to where at the end of the game, the user
	can hit the button (activating IO_Touch();) and start the game all over again
	Make sure to clear lines and score global variables at the beginning of this loop^ */
}



//******************************* Main**********************************
int main(void){
	DisableInterrupts();
	PLL_Init();  // set system clock to 80 MHz
	IO_Init();
	Tetris_Init();
	tetState_Init();
	Random_Init(1);
	ADC_Init();
//	Timer0_Init(speed);				//1000000
//	Timer1_Init(7255);
	Timer3_Init();
//	Timer2_Init(30000000);
	Sound_Init();
	currentshape = Random_Letter();
	next = Random_Letter();
	if (next == 'I'){
		x = 101, y = 39;}
	else{
		x = 98, y = 39;}
	Draw(currentshape, 0, Block, 0, 1);
	Draw(next, 0, Block, 1, 1);				//display next shape in correct place
	x = 37, y = 33; 
	currentrotate = 0;
	EnableInterrupts();
	while (1){
		IO_Touch();
		Rotate();
		
	}

}

