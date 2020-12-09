	.cdecls "bocas.h"
	.cdecls "iicFunctions.h"
	.def iicSendBit
	.def iicSendStartBit
	.def iicSendStopBit
	.def iicReceiveBit
	.def iicInit

	.ref writeError

SDA .equ 0x01
SCL .equ 0x02


	.ref delay

;	send single bit
;this function sends single bits via IIC
;data to send in low byte of r12, bit 0
;returns success (0) or error (-1)
;IIC connected to port 2, pin 0 and 1
;SDA and SCL are configured as output
;C- prototype: int iicSendBit (char send_bit);

iicSendBit:
	bit.b #0x01, R12
	jz iicSendBit_low
; send high
	bis.b #SDA, &P2OUT
	jmp iicSendBit_setSCL
iicSendBit_low:
	bic.b #SDA, &P2OUT
iicSendBit_setSCL:
	bis.b #SCL, &P2OUT
; test scl line - may be hold down by slave
	bic.b #SCL, &P2DIR	; scl port now input
	bit.b #SCL, &P2IN
	bis.b #SCL, &P2DIR	; scl port now output
	jnz iicSendBit_cont	; rest result of bittest
	mov #-1, R12
	jmp iicSendBit_done
iicSendBit_cont:
	call #delay
	bic.b #SCL, &P2OUT
	mov #0, R12
iicSendBit_done:
	ret


;	send start bit
; this function sends the start bit of a data transmission
;
; returns success (0), or error (-1)
; IIC connected to port 2, pin 0 and 1
; SDA and SCL are configured as output
; C- prototype: int iicSendStartBit (void);

iicSendStartBit:
	; test scl line
	bic.b #SCL, &P2DIR	; scl port now input
	bit.b #SCL, &P2IN
	bis.b #SCL, &P2DIR	; scl port now output
	jz iicSendStartBit_error	; rest result of bittest
	; test sda line
	bic.b #SDA, &P2DIR	; sda port now input
	bit.b #SDA, &P2IN
	bis.b #SDA, &P2DIR	; sda port now output
	jz iicSendStartBit_error
	bic.b #SDA, &P2OUT	; falling edge in sda
	bic.b #SCL, &P2OUT	; clock to low
	mov #0, R12
	jmp iicSendStartBit_done
iicSendStartBit_error:
	mov.b #0x02, R12	; error number
	call #writeError
	mov #-1, R12
iicSendStartBit_done:
	ret


;	send stop bit
; this function sends the stop bit of a data transmission
;
; returns success (0), or error (-1)
; IIC connected to port 2, pin 0 and 1
; SDA and SCL are configured as output
; C- prototype: int iicSendStartBit(void);

iicSendStopBit:
	bic.b #SDA, &P2OUT
	bis.b #SCL, &P2OUT
	bis.b #SDA, &P2OUT
	mov #0, R12
iicSendStopBit_done:
	ret

;	init IIC
; this function initializes the IIC
;
; returns success (0), or error (-1)
; C-prototype: int iicInit(void);

iicInit:
	push R13
	bic.b #(SDA|SCL), &P2SEL
; run up to 8 scl cycles
	mov #8, R13
	bis.b #(SDA|SCL), &P2OUT
	bis.b #SCL, &P2DIR			; SCL output
	bic.b #SDA, &P2DIR			; SDA input
iicInit_loop:
	bit.b #SDA, &P2IN
	jnz iicInit_cont
	bic.b #SCL, &P2OUT
	bis.b #SCL, &P2OUT
	dec R13
	cmp #0, R13
	jnz iicInit_loop
	jmp iicInit_error
iicInit_cont:
	bis.b #SDA, &P2DIR			; now SDA output
; run stop to force slaves into init state
	bic.b #SCL, &P2OUT
	bic.b #SDA, &P2OUT
	bis.b #SCL, &P2OUT
	bis.b #SDA, &P2OUT
; test init state of lines
	bic.b #(SDA|SCL), &P2DIR
	mov #0, R12
	bit.b #SDA, &P2IN
	jz iicInit_error
	bit.b #SCL, &P2IN
	jnz iicInit_done
iicInit_error:
	mov.b #0x01, R12	; error number
	call #writeError
	mov #-1, R12
iicInit_done:
	pop R13
	ret

;	receive bit
; this function recieves a bit
;
; returns success (0), or error (-1)
; IIC connected to port 2, pin 0 and 1
; SDA and SCL are configured as output
; C- prototype: int iicReceiveBit (void);

; Ist die clock auf 0, falls der Slave senden möchte?

iicReceiveBit:
	mov.w R12, R13	; save pointer from R12 in R13
	mov.b #0, R12	; if no error, success in R12
	; test scl line - may be hold up by slave
	;bic.b #SCL, &P2DIR	; scl port now input
	;bit.b #SCL, &P2IN
	;bis.b #SCL, &P2DIR ; scl port now output
	;jnz iicReceiveBit_cont
	;bis.b #-1, R12
	;jmp iicReceiveBit_done
iicReceiveBit_cont:
	bis.b #SDA, &P2OUT
	bis.b #SCL, &P2OUT
	bic.b #SDA, &P2DIR	; sda port now input
	bit.b #SDA, &P2IN
	bic.b #SCL, &P2OUT
	bis.b #SDA, &P2DIR
	jz iicReceiveBit_low
; iic ReceiveBit_high:
	mov.b #1, 0(R13)	; high in ptr pos 0
	jmp iicReceiveBit_done
iicReceiveBit_low:
	mov.b #0, 0(R13)	; low in ptr pos 0
iicReceiveBit_done:
	ret
