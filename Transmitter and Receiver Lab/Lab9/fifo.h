/*
 *FIFO.h
 *Student name: Aria Pahlavan - Khalid Qarryzada
 *EE 319k
 *Lab 9
*/

#include "tm4c123gh6pm.h"

#ifndef __FIFO_H__
#define __FIFO_H__

long StartCritical (void);    // previous I bit, disable interrupts
void EndCritical(long sr);    // restore I bit to previous value

// Two-index implementation of the transmit FIFO
// can hold 0 to TXFIFOSIZE elements
#define TXFIFOSIZE 16 // must be a power of 2
#define TXFIFOSUCCESS 1
#define TXFIFOFAIL    0
typedef char txDataType;




/*
// initialize index FIFO
void TxFifo_Init(void);
// add element to end of index FIFO
// return TXFIFOSUCCESS if successful
int TxFifo_Put(txDataType data);
// remove element from front of index FIFO
// return TXFIFOSUCCESS if successful
int TxFifo_Get(txDataType *datapt);
// number of elements in index FIFO
// 0 to TXFIFOSIZE-1
uint32_t TxFifo_Size(void);

// Two-pointer implementation of the receive FIFO
// can hold 0 to RXFIFOSIZE-1 elements
#define RXFIFOSIZE 16 // can be any size
#define RXFIFOSUCCESS 1
#define RXFIFOFAIL    0

typedef char rxDataType;
*/
// initialize pointer FIFO
void Fifo_Init(void);
// add element to end of pointer FIFO
// return RXFIFOSUCCESS if successful
int Fifo_Put(int32_t input);
// remove element from front of pointer FIFO
// return RXFIFOSUCCESS if successful
int Fifo_Get(unsigned char *datapt);
// number of elements in pointer FIFO
// 0 to RXFIFOSIZE-1
uint32_t Fifo_Size(void);

#endif //  __FIFO_H__

