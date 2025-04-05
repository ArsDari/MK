reset:
	rjmp main

main:

loop:
	in r16, OCR0A
	lsl r16
	out OCR0A, r16
	rjmp loop