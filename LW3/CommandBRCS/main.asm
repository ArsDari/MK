reset:
    rjmp main

main:
	ldi r16, 0xF0
	ldi r17, 0x0F

loop:
	brcs condition_true
	rjmp condition_false

condition_true:
	clc
	out OCR0A, r16
	rjmp loop

condition_false:
	sec
	out OCR0A, r17
	rjmp loop