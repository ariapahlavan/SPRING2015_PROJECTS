/*
 *Fifo.c
 *Student name: Aria Pahlavan - Khalid Qarryzada
 *EE 319k
 *Lab 9
*/
#include <stdint.h>
#define SIZE 21		//For 4 elemnts (5 memory locations each), plus one to signify we are at the end
static uint32_t PutI;
static uint32_t GetI;
static int32_t FIFO[SIZE];

void Fifo_Init(void){
	PutI = GetI = 0;
}
	
int Fifo_Put(unsigned char input){
	if(((PutI+1)%SIZE) == GetI){		//fail if empty
		return 0;						
	}
	FIFO[PutI] = input;				//retrieve data
	PutI = (PutI+1)%SIZE;			//next place to get
	return 1;
		
}	

int Fifo_Get (unsigned char* datapt){
	if (PutI == GetI){
		return 0;
	}
	*datapt = FIFO[GetI];
	GetI = (GetI+1)%SIZE;
	return 1;
}
