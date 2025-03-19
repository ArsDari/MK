reset:
	jmp startup

startup:
	eor r1, r1
	out SREG, r1
	ser r28
	ldi r29, 0x08
	out SPH, r29
	out SPL, r28
	call main

main:
	sbi DDRB, 5
	ldi r24, 0x01
	ldi r25, 0x01

loop:
	eor r24, r25
	breq led_off
	sbi PORTB, 5
	rjmp setup_delay

led_off:
	cbi PORTB, 5

setup_delay:
	ldi r18, 0xE7
	ldi r19, 0x71
	ldi r20, 0x1A

loop_delay:
	subi r18, 0x01
	sbci r19, 0x00
	sbci r20, 0x00
	brne loop_delay
	rjmp loop