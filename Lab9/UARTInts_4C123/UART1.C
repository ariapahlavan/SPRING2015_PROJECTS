// UART1.c
// Runs on LM4F120/TM4C123
//NAME: Aria Pahlavan, Khalid Qarryzada


void UART1_Init(void){            // should be called only once
  SYSCTL_RCGC1_R |= 0x00000002;  // activate UART1
  SYSCTL_RCGC2_R |= 0x00000004;  // activate port C
  UART1_CTL_R &= ~0x00000001;    // disable UART
  UART1_IBRD_R = 50;     // IBRD = int(80,000,000/(16*115,200)) = 
  UART1_FBRD_R = 0;     // FBRD = round(0.40278 * 64) = 
  UART1_LCRH_R = 0x00000070;  // 8 bit, no parity bits, one stop, FIFOs
  UART1_CTL_R |= 0x00000001;     // enable UART
  GPIO_PORTC_AFSEL_R |= 0x30;    // enable alt funct on PC5-4
  GPIO_PORTC_DEN_R |= 0x30;      // configure PC5-4 as UART1
  GPIO_PORTC_PCTL_R = (GPIO_PORTC_PCTL_R&0xFF00FFFF)+0x00220000;
  GPIO_PORTC_AMSEL_R &= ~0x30;   // disable analog on PC5-4
}

void UART1_OutChar(char data){
  while((UART1_FR_R&0x0020) != 0);      // wait until TXFF is 0
  UART1_DR_R = data;
}