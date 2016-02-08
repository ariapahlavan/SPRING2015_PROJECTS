; Print.s
; Student names: change this to your names or look very silly
; Last modification date: change this to the last modification date or look very silly
; Runs on LM4F120 or TM4C123
; EE319K lab 7 device driver for any LCD
;
; As part of Lab 7, students need to implement these LCD_OutDec and LCD_OutFix
; This driver assumes two low-level LCD functions
; ST7735_OutChar   outputs a single 8-bit ASCII character
; ST7735_OutString outputs a null-terminated string 

    IMPORT   ST7735_OutChar
    IMPORT   ST7735_OutString
    EXPORT   LCD_OutDec
    EXPORT   LCD_OutFix

    AREA    |.text|, CODE, READONLY, ALIGN=2
    THUMB

	PRESERVE8

;-----------------------LCD_OutDec-----------------------
; Output a 32-bit number in unsigned decimal format
; Input: R0 (call by value) 32-bit unsigned number
; Output: none
; Invariables: This function must not permanently modify registers R4 to R11
LCD_OutDec

TEN RN 1	;Rename R1 to TEN
	
		PUSH {R0, R4, R5, LR}	;Current value of SP is now of our local variable, the input
		AND R4, #0			;R4 is our counter of how many places away the SP is from the local variable
		MOV R1, #10
loop1	LDR R5, [SP, R4]
		UDIV R2, R5, TEN		;R2 has the input divided by 10 and truncated to a whole number
		MUL R2, TEN			;Dividing by ten, then multiplying by ten, gets rid of the value in the ones' place of the decimal
		SUB R3, R5, R2			;R3 now has the digit in the ones' place
		ADD R3, #0x30			;R3 now has the ASCII value of the digit
		PUSH {R3}			
		ADD R4, #4			;Add 4 to ccounter since SP has just shifted up by 4
		UDIV R5, R5, TEN		;R5 still has local variable in it
		STR R5, [SP, R4]
		LDR R2, [SP, R4]
		CMP R2, #0
		BNE loop1
		
print1	POP {R0}
		BL ST7735_OutChar
		SUB R4, #4
		CMP R4, #0
		BNE print1
	
		POP {R0, R4, R5, LR}
		BX LR
;* * * * * * * * End of LCD_OutDec * * * * * * * *

; -----------------------LCD _OutFix----------------------
; Output characters to LCD display in fixed-point format
; unsigned decimal, resolution 0.001, range 0.000 to 9.999
; Inputs:  R0 is an unsigned 32-bit number
; Outputs: none
; E.g., R0=0,    then output "0.000 "
;       R0=3,    then output "0.003 "
;       R0=89,   then output "0.089 "
;       R0=123,  then output "0.123 "
;       R0=9999, then output "9.999 "
;       R0>9999, then output "*.*** "
; Invariables: This function must not permanently modify registers R4 to R11
LCD_OutFix

		PUSH {R0, R4, R5, R6, LR}
		MOV R6, SP
		MOV R0, #0x30			;ASCII for 0
		STR R0, [SP, #-4]
		STR R0, [SP, #-8]
		STR R0, [SP, #-12]
		STR R0, [SP, #-16]		;Store 0 in 4 memory locations above the stack
		
		AND R4, #0			;R4 is counter as it was in LCD_OutDec
		MOV R1, #10
loop2	LDR R5, [SP, R4]
		UDIV R2, R5, TEN		;R2 has the input divided by 10 and truncated to a whole number
		MUL R2, TEN			;Dividing by ten, then multiplying by ten, gets rid of the value in the ones' place of the decimal
		SUB R3, R5, R2			;R3 now has the digit in the ones' place
		ADD R3, #0x30			;R3 now has the ASCII value of the digit
		PUSH {R3}			
		ADD R4, #4			;Add 4 to ccounter since SP has just shifted up by 4
		UDIV R5, R5, TEN		;R5 still has local variable in it
		STR R5, [SP, R4]
		LDR R2, [SP, R4]
		CMP R2, #0
		BNE loop2
		
		
		CMP R4, #16
		BGT printstar
		BEQ print2
		MOV R0, SP
		ADD R0, R4
		SUB R0, #16
		MOV SP, R0
		MOV R4, #16
		B print2
		
printstar	MOV R0, #0x2A			;0x2A is ASCII for *
		BL ST7735_OutChar
		MOV R0, #0x2E
		BL ST7735_OutChar
		MOV R0, #0x2A
		BL ST7735_OutChar
		MOV R0, #0x2A
		BL ST7735_OutChar
		MOV R0, #0x2A
		BL ST7735_OutChar		;Prints " *.*** "
		B fin

print2	POP {R0}
		BL ST7735_OutChar
		SUB R4, #4
		CMP R4, #12			;If R4 is 12, then we have three characters left
		BNE print2
		
		MOV R0, #0x2E			;0x2E is '.' in ASCII
		BL ST7735_OutChar
print3	POP {R0}
		BL ST7735_OutChar
		SUB R4, #4
		CMP R4, #0
		BNE print3
	
fin		MOV SP, R6
		POP {R0, R4, R5, R6, LR}
		BX  LR
 
     ALIGN
;* * * * * * * * End of LCD_OutFix * * * * * * * *

     ALIGN                           ; make sure the end of this section is aligned
     END                             ; end of file
