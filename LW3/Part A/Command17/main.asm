reset:
	rjmp main

main:

loop:
	in r16, OCR0A
	dec r16
	out OCR0A, r16
	rjmp loop