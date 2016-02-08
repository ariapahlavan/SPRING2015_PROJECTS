// UART1.c
// Runs on LM4F120/TM4C123
//NAME: Aria Pahlavan, Khalid Qarryzada
#include "UART1.h"
#include "fifo.h"
#include "tm4c123gh6pm.h"

#define NVIC_EN0_INT5           0x00000020  // Interrupt 5 enable

#define UART_FR_RXFF            0x00000040  // UART Receive FIFO Full
#define UART_FR_TXFF            0x00000020  // UART Transmit FIFO Full
#define UART_FR_RXFE            0x00000010  // UART Receive FIFO Empty
#define UART_LCRH_WLEN_8        0x00000060  // 8 bit word length
#define UART_LCRH_FEN           0x00000010  // UART Enable FIFOs
#define UART_CTL_UARTEN         0x00000001  // UART Enable
#define UART_IFLS_RX1_8         0x00000000  // RX FIFO >= 1/8 full
#define UART_IFLS_TX1_8         0x00000000  // TX FIFO <= 1/8 full
#define UART_IM_RTIM            0x00000040  // UART Receive Time-Out Interrupt
                                            // Mask
#define UART_IM_TXIM            0x00000020  // UART Transmit Interrupt Mask
#define UART_IM_RXIM            0x00000010  // UART Receive Interrupt Mask
#define UART_RIS_RTRIS          0x00000040  // UART Receive Time-Out Raw
                                            // Interrupt Status
#define UART_RIS_TXRIS          0x00000020  // UART Transmit Raw Interrupt
                                            // Status
#define UART_RIS_RXRIS          0x00000010  // UART Receive Raw Interrupt
                                            // Status
#define UART_ICR_RTIC           0x00000040  // Receive Time-Out Interrupt Clear
#define UART_ICR_TXIC           0x00000020  // Transmit Interrupt Clear
#define UART_ICR_RXIC           0x00000010  // Receive Interrupt Clear



int ErrCnt;			//Global error count



void UART1_Init(void){            // should be called only once
  SYSCTL_RCGCUART_R |= 0x00000002;  	// activate UART1
  SYSCTL_RCGCGPIO_R |= 0x00000004; 	// activate port C
	Fifo_Init(); // initializes a FIFO that holds 6 elements 
	volatile uint32_t delay;				//declare variable
	delay = SYSCTL_RCGCGPIO_R;			//after setting SYSCTL_RCGADC_R, wait for stablization
	delay = SYSCTL_RCGCUART_R;			//after setting SYSCTL_RCGADC_R, wait for stablization
  UART1_CTL_R &= ~UART_CTL_UARTEN;      // disable UART
  UART1_IBRD_R = 50;    				 	// IBRD = int(80,000,000/(16*100,000)) =50.00 
  UART1_FBRD_R = 0;    					 	// FBRD = round(0.00 * 64) = 0
  UART1_LCRH_R = 0x70;
												//UART1_IFLS_R &= ~0x3F;						//
	UART1_IFLS_R |= 0x10;	//1/2 full
	UART1_IM_R |= 0x10;						// Enable interrupt for RXIM & RTIM
	NVIC_PRI1_R &= 0xFF1FFFFF;
	NVIC_EN0_R = 0x40;	
		  UART1_CTL_R |= 0x301;						//Enable Interrupts
  GPIO_PORTC_AFSEL_R |= 0x30;    	// enable alt funct on PC5-4
  GPIO_PORTC_DEN_R |= 0x30;      	// configure PC5-4 as UART1
  GPIO_PORTC_PCTL_R = (GPIO_PORTC_PCTL_R&0xFF00FFFF)+0x00220000;
  GPIO_PORTC_AMSEL_R &= ~0x30;   	// disable analog on PC5-4

	

	
	ErrCnt = 0;		//Clear Global Error Count

}



// Wait for new input, then return ASCII code
unsigned char UART_InChar(void){
  while((UART1_FR_R&0x0010) != 0);      // wait until RXFE is 0
  return((unsigned char)(UART1_DR_R&0xFF));
}
// Wait for buffer to be not full, then output
void UART_OutChar(unsigned char data){
  while(((UART1_FR_R&UART_FR_TXFF) != 0));      // wait until TXFF is 0
  UART1_DR_R = data;
}
// Immediately return input or 0 if no input
unsigned char UART_InCharNonBlocking(void){
  if((UART1_FR_R&UART_FR_RXFE) == 0){
    return((unsigned char)(UART1_DR_R&0xFF));
  } else{
    return 0;
  }
}
