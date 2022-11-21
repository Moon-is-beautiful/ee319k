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

;-----------------------LCD_OutDec-----------------------
; Output a 32-bit number in unsigned decimal format
; Input: R0 (call by value) 32-bit unsigned number
; Output: none
; Invariables: This function must not permanently modify registers R4 to R11
LCD_OutDec
;; --UUU-- Complete this (copy from Lab7-8)
PUSH {LR, R4}
;Allocation Phase
    SUB SP, #8 ; Allocate num

;Access Phase
    CMP R0, #10
    BLO baseOutDec
    MOV R3, #0xA
    UDIV R2, R0, R3
    MLS R4, R2, R3, R0 
    MOV R0, R2
    STR R4, [SP, #num]
    BL LCD_OutDec
    LDR R0, [SP, #num]
    ADD R0, #0x30
    STR R0,[R1]
    B returnOutDec
baseOutDec
    ADD R0, #0x30
    STR R0,[R1]
returnOutDec
    ADD R1,#1
;Deallocation Phase
    ADD SP,#8
    POP {LR, R4}
    BX LR
;* * * * * * * * End of LCD_OutDec * * * * * * * *

; -----------------------LCD _OutFix----------------------
; Output characters to LCD display in fixed-point format
; unsigned decimal, resolution 0.01, range 0.00 to 9.99
; Inputs:  R0 is an unsigned 32-bit number
; Outputs: none
; E.g., R0=0,    then output "0.00 "
;       R0=3,    then output "0.003 "
;       R0=89,   then output "0.89 "
;       R0=123,  then output "1.23 "
;       R0=999,  then output "9.99 "
;       R0>999,  then output "*.** "
; Invariables: This function must not permanently modify registers R4 to R11
; Lab 7 requirement is for at least one local variable on the stack with symbolic binding
LCD_OutFix
;; --UUU-- Complete this (copy from Lab7-8)
    MOV R2, #1
LOOP
    PUSH {LR, R4, R5}

; **Allocation Phase**
    SUB SP, #8 ; Allocate num

; **Access Phase**
    MOV R4, #10000
    CMP R0, R4
    BHS stars
    CMP R2, #5
    BEQ returnOutFixS
    MOV R3, #0xa
    UDIV R5, R0, R3
    MLS R4, R5, R3, R0 
    MOV R0, R5
    STR R4, [SP, #num]
    STR R2, [SP, #length]
    ADD R2, #1
    BL LOOP
    LDR R0, [SP, #num]
    ADD R0, #0x30
    STR R0,[R1]
    LDR R2, [SP, #length]
    CMP R2, #4
    BNE returnOutFix
	ADD R1,#1
    MOV R0, #0x2E
    STR R0,[R1]
    B returnOutFix
stars
c    MOV R0, #0x2A
    STR R0,[R1]
    MOV R0, #0x2E
    STR R0,[R1,#1]
    MOV R0, #0x2A
    STR R0,[R1,#2]
    MOV R0, #0x2A
    STR R0,[R1,#3]
    MOV R0, #0x2A
    STR R0,[R1,#4]
	B DONES
returnOutFix
    ADD R1,#1
returnOutFixS
	LDR R2,[SP,#length]
	CMP R2,#1
	BNE DONE
	MOV R0,#0x20
	STR R0,[R1]
DONE
DONES
;Deallocation Phase
    ADD SP, #8

    POP {LR, R4, R5}
     BX LR
;* * * * * * * * End of LCD_OutFix * * * * * * * *

    ALIGN                           ; make sure the end of this section is aligned
    END                             ; end of file
