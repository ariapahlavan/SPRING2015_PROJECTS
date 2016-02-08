; IO.s
; Student names: change this to your names or look very silly
; Last modification date: change this to the last modification date or look very silly
; Runs on LM4F120/TM4C123

; EE319K lab 7 device driver for the switch and LED.
; You are allowed to use any switch and any LED,
; although the Lab suggests the SW1 switch PF4 and Red LED PF1

; As part of Lab 7, students need to implement these three functions

; negative logic SW2 connected to PF0 on the Launchpad
; red LED connected to PF1 on the Launchpad
; blue LED connected to PF2 on the Launchpad
; green LED connected to PF3 on the Launchpad
; negative logic SW1 connected to PF4 on the Launchpad

        EXPORT   IO_Init
        EXPORT   IO_Touch
        EXPORT   IO_HeartBeat

GPIO_PORTF_DATA_R  EQU 0x400253FC
GPIO_PORTF_DIR_R   EQU 0x40025400
GPIO_PORTF_AFSEL_R EQU 0x40025420
GPIO_PORTF_PUR_R   EQU 0x40025510
GPIO_PORTF_DEN_R   EQU 0x4002551C
GPIO_PORTF_LOCK_R  EQU 0x40025520
GPIO_PORTF_CR_R    EQU 0x40025524
GPIO_PORTF_AMSEL_R EQU 0x40025528
GPIO_PORTF_PCTL_R  EQU 0x4002552C
GPIO_LOCK_KEY      EQU 0x4C4F434B  ; Unlocks the GPIO_CR register
PF0       EQU 0x40025004
PF1       EQU 0x40025008
PF2       EQU 0x40025010
PF3       EQU 0x40025020
PF4       EQU 0x40025040
LEDS      EQU 0x40025038
RED       EQU 0x02
BLUE      EQU 0x04
GREEN     EQU 0x08
SWITCHES  EQU 0x40025044
SW1       EQU 0x10                 ; on the left side of the Launchpad board
SW2       EQU 0x01                 ; on the right side of the Launchpad board
SYSCTL_RCGCGPIO_R  EQU 0x400FE608
    
        AREA    |.text|, CODE, READONLY, ALIGN=2
        THUMB



;------------IO_Init------------
; Initialize GPIO Port for a switch and an LED
; Input: none
; Output: none
; This is a public function
; Invariables: This function must not permanently modify registers R4 to R11
IO_Init
	LDR R0, =SYSCTL_RCGCGPIO_R
	LDR R1, [R0]
	ORR R1, #0x20		;Turn on clock for bit 5, which corresponds to Port F
	STR R1, [R0]
	NOP
	NOP
	
	LDR R0, =GPIO_PORTF_DIR_R
	LDR R1, [R0]
	ORR R1, #0x02		;PF1 set as output for heartbeat (PF1 connected to red LED on launchpad)
	BIC R1, #0x10		;PF4 set as an input from a switch
	STR R1, [R0]
    
	LDR R0, =GPIO_PORTF_DEN_R
	LDR R1, [R0]
	ORR R1, #0x12
	STR R1, [R0]
	
	LDR R0, =GPIO_PORTF_PUR_R
	LDR R1, [R0]
	ORR R1, #0x10
	STR R1, [R0]
	
	BX  LR
;* * * * * * * * End of IO_Init * * * * * * * *

;------------IO_HeartBeat------------
; Toggle the output state of the LED.
; Input: none
; Output: none
; This is a public function
; Invariables: This function must not permanently modify registers R4 to R11
IO_HeartBeat
	LDR R0, =GPIO_PORTF_DATA_R
	LDR R1, [R0]
	EOR R1, #0x02
	STR R1, [R0]
	
	BX  LR                          ; return
;* * * * * * * * End of IO_HeartBeat * * * * * * * *

;------------IO_Touch------------
; First: wait for the release of the switch
; and then: wait for the touch of the switch
; Input: none
; Output: none
; This is a public function
; Invariables: This function must not permanently modify registers R4 to R11
IO_Touch
loop1	LDR R0, =GPIO_PORTF_DATA_R
		LDR R1, [R0]
		AND R2, R1, #0x10		;Only the value of PF4 is placed into R2 using a bit mask.
		CMP R2, #0			;R2 is 0x10 if PF4 is high and 0 if PF4 is low.
							;First we check if the switch is released, i.e., that PF4 is low.
		BEQ loop1				
		
loop2	LDR R3, =GPIO_PORTF_DATA_R
		LDR R1, [R3]
		AND R2, R1, #0x10
		CMP R2, #0			;If the switch is not high, go back and check again.
		BNE loop2
	
		BX  LR                          ; return
;* * * * * * * * End of IO_Touch * * * * * * * *

    ALIGN                           ; make sure the end of this section is aligned
    END                             ; end of file