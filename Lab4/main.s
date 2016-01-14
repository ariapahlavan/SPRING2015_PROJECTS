;****************** main.s ***************
; Program written by: Aria Pahlavan and Khalid Qarryzada
; Date Created: 1/24/2015 
; Last Modified: 3/1/15 
; Section 1-2pm     TA: Wooseok Lee
; Lab number: 4
; Brief description of the program
;   If the switch is presses, the LED toggles at 8 Hz
; Hardware connections
;  PE0 is switch input  (1 means pressed, 0 means not pressed)
;  PE1 is LED output (1 activates external LED on protoboard) 
;Overall functionality of this system is the similar to Lab 3, with four changes:
;1-  activate the PLL to run at 80 MHz (12.5ns bus cycle time) 
;2-  initialize SysTick with RELOAD 0x00FFFFFF 
;3-  add a heartbeat to PF2 that toggles every time through loop 
;4-  add debugging dump of input, output, and time
; Operation
;	1) Make PE1 an output and make PE0 an input. 
;	2) The system starts with the LED on (make PE1 =1). 
;   3) Wait about 62 ms
;   4) If the switch is pressed (PE0 is 1), then toggle the LED once, else turn the LED on. 
;   5) Steps 3 and 4 are repeated over and over


SWITCH                  EQU 0x40024004  ;PE0
LED                     EQU 0x40024008  ;PE1
SYSCTL_RCGCGPIO_R       EQU 0x400FE608
SYSCTL_RCGC2_GPIOE      EQU 0x00000010   ; port E Clock Gating Control
SYSCTL_RCGC2_GPIOF      EQU 0x00000020   ; port F Clock Gating Control
GPIO_PORTE_DATA_R       EQU 0x400243FC
GPIO_PORTE_DIR_R        EQU 0x40024400
GPIO_PORTE_AFSEL_R      EQU 0x40024420
GPIO_PORTE_PUR_R        EQU 0x40024510
GPIO_PORTE_DEN_R        EQU 0x4002451C
GPIO_PORTF_DATA_R       EQU 0x400253FC
GPIO_PORTF_DIR_R        EQU 0x40025400
GPIO_PORTF_AFSEL_R      EQU 0x40025420
GPIO_PORTF_DEN_R        EQU 0x4002551C
NVIC_ST_CTRL_R          EQU 0xE000E010
NVIC_ST_RELOAD_R        EQU 0xE000E014
NVIC_ST_CURRENT_R       EQU 0xE000E018
           THUMB
           AREA    DATA, ALIGN=4
SIZE       EQU    50
;You MUST use these two buffers and two variables
;You MUST not change their names
;These names MUST be exported
           EXPORT DataBuffer  
           EXPORT TimeBuffer  
           EXPORT DataPt [DATA,SIZE=4] 
           EXPORT TimePt [DATA,SIZE=4]
DataBuffer SPACE  SIZE*4
TimeBuffer SPACE  SIZE*4
DataPt     SPACE  4
TimePt     SPACE  4

	ALIGN          
	AREA    |.text|, CODE, READONLY, ALIGN=2
	THUMB
	EXPORT  Start
	IMPORT  TExaS_Init

Start BL   TExaS_Init  ; running at 80 MHz, scope voltmeter on PD3
		LDR R1, =SYSCTL_RCGCGPIO_R
		LDR R0, [R1]
		ORR R0, #0x30					;Initialize Port E and Port F
		STR R0, [R1]                  
		NOP
		NOP  							; allow time for clock to finish
		LDR R1, =GPIO_PORTE_DIR_R       ; Set direction register
		ORR R0, #0x02
		BIC R0, #0x01
		STR R0, [R1]           
		LDR R1, =GPIO_PORTE_DEN_R       ; 7) enable Port E digital port
		ORR R0, #0x03                   
		STR R0, [R1]                    
		LDR R1, =GPIO_PORTE_DATA_R
		LDR R0, [R1]
		ORR R0, #0x02
		STR R0, [R1]
	
		LDR R1, =GPIO_PORTF_DIR_R
		LDR R0, [R1]
		ORR R0, #0x04
		STR R0, [R1]
		LDR R1, =GPIO_PORTF_DEN_R
		LDR R0, [R1]
		ORR R0, #0x04
		STR R0, [R1]

	

	CPSIE  I    ; TExaS voltmeter, scope runs on interrupts
		BL Debug_Init
loop 	BL Debug_Capture

Hdelay	EQU 124000
heartb	LDR R0, =Hdelay			;percentage overhead required to run the debugging instrument: 
hagain	SUBS R0, #1						; (900 ns)/(62.5 ms)*100% = 0.00144%
		BNE hagain
		LDR R5, =GPIO_PORTF_DATA_R
		LDR R6, [R5]
		EOR R6, #0x04
		STR R6, [R5]
		
		
		
		BL	delay
		LDR R1, =GPIO_PORTE_DATA_R
		LDR R0, [R1]					;R1 has GPIO_PORTE_DATA_R
		AND R2, R0, #0x01
		CMP R2, #0
		BEQ	turnon
		B	toggle

toggle	LDR R0, [R1]
		EOR R0, #0x02
		STR R0, [R1]

		B loop

turnon	LDR R0, [R1]
		ORR R0, #0x02
		STR R0, [R1]

		B loop

MILLISEC	EQU	1240000
delay	LDR R0, =MILLISEC				;percentage overhead required to run the debugging instrument: 
again	SUBS R0, #1						; (900 ns)/(62.5 ms)*100% = 0.00144%
		BNE again
		BX LR							
; Delay
;input PE0 test output PE1
	  B    loop


;------------Debug_Init------------
; Initializes the debugging instrument
; Input: none
; Output: none
; Modifies: none
; Note: push/pop an even number of registers so C compiler is happy
Debug_Init
	PUSH {R0-R6, LR}
		LDR R0, =DataBuffer					;Put the address of DataBuffer into DataPt
		LDR R1, =DataPt
		STR R0, [R1]
	
		LDR R2, =TimeBuffer
		LDR R3, =TimePt
		STR R2, [R3]
		
		MVN R5, #0
		MOV R4, #50	
dloop1	STR R5, [R0]
		ADD R0, #4
		SUB R4, #1
		CMP R4, #0
		BNE dloop1
		
		MOV R4, #50
dloop2	STR R5, [R2]
		ADD R2, #4
		SUB R4, #1
		CMP R4, #0
		BNE dloop2

	BL SysTick_Init
	POP {R0-R6,LR}
	BX	LR
; init SysTick
SysTick_Init
	PUSH{R0,R1}
	LDR R1, =NVIC_ST_CTRL_R
	MOV R0, #0						;First disable SysTick
	STR R0, [R1]
	LDR R1, =NVIC_ST_RELOAD_R		;R1 = &NVIC_ST_RELOAD_R
	LDR R0, =0xFFFFFF				;maximum value that we can hold
	STR R0, [R1]
	LDR R1, =NVIC_ST_CURRENT_R
	MOV R0, #0
	STR R0, [R1]
	LDR R1, =NVIC_ST_CTRL_R
	MOV R0, #0x05
	STR R0, [R1]
	POP{R0,R1}
	BX LR

;------------Debug_Capture------------
; Dump Port E and time into buffers
; Input: none
; Output: none
; Modifies: none
; Note: push/pop an even number of registers so C compiler is happy
Debug_Capture
		PUSH {R0-R6, LR}					;Estimated time to process: 900 nanoseconds
		LDR R0, =DataBuffer
		LDR R1, =TimeBuffer
		ADD R4, R0, #200					;R0 now points to the location right after the end of the array
		ADD R5, R1, #200
		LDR R2, =DataPt
		LDR R2, [R2]
		LDR R3, =TimePt
		LDR R3, [R3]
		CMP R2, R4
		BEQ	fin
		CMP R3, R5
		BEQ fin
		
		LDR R0, =GPIO_PORTE_DATA_R
		LDR R0, [R0]
		AND R2, R0, #1
		AND R3, R0, #2
		LSR R3, #1
		LSL R2, #4
		ADD R4, R3, R2					
		LDR R5, =DataPt
		LDR R5, [R5]
		STR R4, [R5]					;Dump port info into DataBuffer
		ADD R5, #4
		LDR R6, =DataPt
		STR R5, [R6]
		
		LDR R1, =NVIC_ST_CURRENT_R						;Dump time info into TimeBuffer (?)
		LDR R1, [R1]
		LDR R2, =TimePt
		LDR R2, [R2]
		STR R1, [R2]
		ADD R2, #4
		LDR R6, =TimePt
		STR R2, [R6]
		
fin		POP {R0-R6, LR}
		BX LR
		



    ALIGN                           ; make sure the end of this section is aligned
    END                             ; end of file
        