reset:
    rjmp main

main:

loop:
	sbrs r16, 0
	rjmp condition_false
	rjmp condition_true

condition_true:
	cbr r16, 0x01
	out OCR0A, r16
	rjmp loop

condition_false:
	sbr r16, 0x01
	out OCR0A, r16
	rjmp loop