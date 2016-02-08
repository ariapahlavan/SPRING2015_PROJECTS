// Runs on LM4F120 or TM4C123
// Program written by: Aria Pahlavan and Khalid Qarryzada
// Lab number: 10
// Hardware connections
// TO STUDENTS "REMOVE THIS LINE AND SPECIFY YOUR HARDWARE********


#include <stdbool.h>
#include "ExtraFunctions.h"
#include "tm4c123gh6pm.h"

uint32_t all_1s(uint32_t array[], int size){
    for(int i = 0; i < size; i++)
        if(array[i] != 1)
            return 0;
    return 1;
}

//int map[16][10];

//uint32_t ArrayShift(int size1, int size2, uint32_t map[][size2]){
//    for(int row = 15; row >= 0; row--){
//        if(all_1s(map[row], 10)){					//calls all_1s to check that the current row is all ones
//            for(int i = 0; i < 10; i++)			
//                map[row][i] = 0;

//            for(int i = row - 1; i >= 0; i--){			//goes row by row	
//                for(int i2 = 0; i2 < 10; i2++){			//drops elements one by one
//                    map[i+1][i2] = map[i][i2];
//                    map[i][i2] = 0;
//                }
//            }
//        }
//    }

//    return 0;
//}
