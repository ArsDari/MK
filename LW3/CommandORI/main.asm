reset:
	rjmp main

main:

loop:
	in r16, OCR0A
	ori r16, 0x00
	out OCR0A, r16
	rjmp loop