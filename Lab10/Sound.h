// Sound.h
// Runs on TM4C123 or LM4F120
// Prototypes for basic functions to play sounds from the
// Runs on LM4F120 or TM4C123
// Program written by: Aria Pahlavan and Khalid Qarryzada
// Lab number: 10

#include "tm4c123gh6pm.h"

void Timer3_Init(void);
void Sound_Init(void);
void Sound_Play1(const unsigned char *sound, uint32_t length);

void Delay10ms(uint32_t count);
void Sound_Play2(uint32_t period);

void VertSound(void);
void FourLinesSound(void);
void LostSound(void);
void HardDropSound(void);
void MultiLineSound(void);
void LineSound(void);
void NextSound(void);
void RotateSound(void);
