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

res		EQU 4
div		EQU 0
val		EQU	0
  

;-----------------------LCD_OutDec-----------------------
; Output a 32-bit number in unsigned decimal format
; Input: R0 (call by value) 32-bit unsigned number
; Output: none
; Invariables: This function must not permanently modify registers R4 to R11
LCD_OutDec
	PUSH {R5-R11, LR}			;keep these save
	MOV R11, SP
	MOV R5, R0										;save input
	CMP R0, #10
	BLO ascii										;see if you can output the value yet
	MOV R1, #10										;move divisor of 10 in R1
	UDIV R2, R0, R1									;divide the input by 10
	MUL R3, R1, R2									;get the result and divisor and sub
	SUB R3, R0, R3									;subtract the input by what you got out
	STR R3, [R11, #val]
	MOVS R0, R2										;move the results as a new input
	BL LCD_OutDec									;input that back into the function
	LDR R2, [R11, #val]
	MOVS R0, R2
ascii
	ADD R0, R0, #0x30
	BL ST7735_OutChar
	POP {R5-R11, LR}			;return to origional
      BX  LR
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
	PUSH {R5-R11, LR}			;keep these saved
	MOV R11, SP										;save the SP
	MOV R5, R0										;save initial value
	MOV R2, #1000									;store the divisor
	UDIV R1, R0, R2
	MOV R3, R1
	CMP R1, #10										;see if >10000 then all astriks ouputted
	BHS greater10k
	MOV R0, R1										;move result in dummy register
	ADD R0, R0, #0x30					
	STR R3, [R11, #res]
	STR R2, [R11, #div]
	BL ST7735_OutChar								;output first result
	MOV R0, #0x2E
	BL ST7735_OutChar							;output "."
	LDR R3, [R11, #res]
	LDR R2, [R11, #div]
	MUL R1, R3, R2									;get value to subtract
	SUBS R5, R5, R1
	MOV R0, R5										;new value to work with after divide by 1000
	MOV R2, #100									;new divisor of 100
	UDIV R1, R0, R2									;new value
	MOV R3, R1
	ADD R1, #0x30									;ascii value
	MOV R0, R1										;copy to output to function
	STR R3, [R11, #res]
	STR R2, [R11, #div]
	BL ST7735_OutChar
	LDR R3, [R11, #res]
	LDR R2, [R11, #div]
	MUL R1, R3, R2									;value to sutract
	SUB R5, R5, R1
	MOV R0, R5
	MOV R2, #10										;new divisor of 10
	UDIV R1, R0, R2									;new value
	MOV R3, R1
	ADD R1, #0x30									;ascii value
	MOV R0, R1
	STR R3, [R11, #res]
	STR R2, [R11, #div]
	BL ST7735_OutChar
	LDR R3, [R11, #res]
	LDR R2, [R11, #div]
	MUL R1, R3, R2
	SUB R5, R5, R1									;should give you the last value to work with
	MOV R0, R5
	ADD R0, #0x30
	BL ST7735_OutChar								;4th and final output
done	
	POP {R5-R11, LR}
     BX   LR
greater10k
	MOV R0, #0x2A						;ascii for "*"
	BL ST7735_OutChar					;output the *
	MOV R0, #0x2E
	BL ST7735_OutChar					;output "."
	MOV R1, #3							;counter to output 3 more *
loop10
	MOV R0, #0x2A						;move "*" into R0
	PUSH {R1, R2}						;save R1 which is the counte
	BL ST7735_OutChar
	POP {R1, R2}						;reload the counter
	SUBS R1, #1							;decrament the counter
	BNE loop10							;makes sure to output only 3 "*"
	B done								;return to where you came from
	
	
	
	
     ALIGN
;* * * * * * * * End of LCD_OutFix * * * * * * * *

     ALIGN                           ; make sure the end of this section is aligned
     END                             ; end of file
