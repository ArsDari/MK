reset:
    rjmp main

main:
	ldi r16, 0xF0
	ldi r17, 0x0F

loop:
	brsh condition_true
	rjmp condition_false

condition_true:
	sec
	out OCR0A, r16
	rjmp loop

condition_false:
	clc
	out OCR0A, r17
	rjmp loop