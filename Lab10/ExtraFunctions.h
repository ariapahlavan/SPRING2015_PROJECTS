// Runs on LM4F120 or TM4C123
// Program written by: Aria Pahlavan and Khalid Qarryzada
// Lab number: 10
// Hardware connections
// TO STUDENTS "REMOVE THIS LINE AND SPECIFY YOUR HARDWARE********

#include <stdbool.h>
#include "tm4c123gh6pm.h"

uint32_t all_1s(uint32_t array[], int size);

uint32_t ArrayShift(int size1, int size2, uint32_t map[][size2]);
