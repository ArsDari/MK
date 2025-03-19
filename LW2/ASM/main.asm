;
; ASM.asm
;
; Created: 10.03.2025 16:51:51
; Author : User
;

reset:
	jmp main

main:
	eor r1, r1
	out SREG, r1; 0x3F <- r1; 0x3F = SREG
	ser r28
	ldi r29, 0x08
	out SPH, r29; 0x3E <- r29; 0x3E = SPH
	out SPL, r28; 0x3D <- r28; 0x3D = SPL
	call loop

loop:
	sbi DDRB, 5; 0x04 = DDRB
	ldi r24, 0x01
	ldi r25, 0x01

led:
	eor r24, r25
	breq led_off; if Z == 1: led_off else led_on

led_on:
	sbi PORTB, 5; 0x05 = PORTB
	rjmp setup_delay; 8C + 2 + 2 = 90

led_off:
	cbi PORTB, 5; 0x05 = PORTB

setup_delay:
	ldi r18, 0xE7
	ldi r19, 0x71
	ldi r20, 0x1A

loop_delay:
	subi r18, 0x01
	sbci r19, 0x00
	sbci r20, 0x00
	brne loop_delay
	rjmp led

turn_off_interruptions:
	cli

deprecated:
	rjmp deprecated
	