reset:
    rjmp main

main:
	ldi r16, 0xF0
	ldi r17, 0x0F

loop:
	brlt condition_true
	rjmp condition_false

condition_true:
	cls
	out OCR0A, r16
	rjmp loop

condition_false:
	ses
	out OCR0A, r17
	rjmp loop