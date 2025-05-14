.EQU F_CPU = 0x00F42400
.EQU DELAY = 300
.EQU TIMER_DELAY = F_CPU / 1024 * DELAY / 1000

.EQU LATCH = PINB1
.EQU SS = PINB2
.EQU DATA = PINB3
.EQU SCLK = PINB5

.EQU TIMER_PRESCALER = (1 << CS12) | (1 << CS10)
.EQU TIMER_INTERRUPT = (1 << OCIE1A)
.EQU DDRB_SPI = (1 << LATCH) | (1 << SS) | (1 << DATA) | (1 << SCLK)
.EQU SPI_SETTINGS = (1 << SPE) | (1 << MSTR) | (1 << SPR0)

.EQU ARRAY_SIZE = 16
.EQU ELEMENT_SIZE = 2
.EQU FLAG_SIZE = 1

.EQU UPDATE_ALLOWED = 1
.EQU UPDATE_DISALLOWED = 0
.EQU SHOW_ALLOWED = 1
.EQU SHOW_DISALLOWED = 0

.DEF temp = r0
.DEF temp_word_low = r24
.DEF temp_word_high = r25
.DEF led = r1

.MACRO prepare_pointer_to_obj
	ldi @0H, HIGH(@1)
	ldi @0L, LOW(@1)
.ENDMACRO

.MACRO sts_SRAM
	ldi @2, @1
	sts @0, @2
.ENDMACRO

.DSEG
array: .BYTE ARRAY_SIZE * ELEMENT_SIZE
allow_update: .BYTE FLAG_SIZE

.CSEG
ISR_vectors:
	jmp reset
	jmp ISR_vectors
	jmp ISR_vectors
	jmp ISR_vectors
	jmp ISR_vectors
	jmp ISR_vectors
	jmp ISR_vectors
	jmp ISR_vectors
	jmp ISR_vectors
	jmp ISR_vectors
	jmp ISR_vectors
	jmp timer_ISR
	jmp ISR_vectors
	jmp ISR_vectors
	jmp ISR_vectors
	jmp ISR_vectors
	jmp ISR_vectors
	jmp ISR_vectors
	jmp ISR_vectors
	jmp ISR_vectors
	jmp ISR_vectors
	jmp ISR_vectors
	jmp ISR_vectors
	jmp ISR_vectors
	jmp ISR_vectors
	jmp ISR_vectors

reset:
	prepare_pointer_to_obj X, array
	prepare_pointer_to_obj Z, src * 2

load_array:
	lpm temp, Z+
	st X+, temp
	cpi XL, ARRAY_SIZE * ELEMENT_SIZE
	brne load_array

load_flag:
	clr temp
	st X+, temp

call_main:
	call main
	jmp reset

main:
	ldi temp_word_low, DDRB_SPI
    out DDRB, temp_word_low
	sbi DDRD, PIND1
	call initializeTimer
	ldi temp_word_low, SPI_SETTINGS
    out SPCR, temp_word_low
	sei
	clr ZL
	clr led

loop:
	lds temp_word_low, allow_update
	cpi temp_word_low, UPDATE_DISALLOWED
	breq loop

flag_update_allowed:
	sts_SRAM allow_update, UPDATE_DISALLOWED, temp_word_low
	call send_word
	cpi ZL, (ARRAY_SIZE - 1) * ELEMENT_SIZE
	breq clear_counter
	subi ZL, -ELEMENT_SIZE
	rjmp continue_cycle

clear_counter:
	clr ZL

continue_cycle:
	ldi temp_word_low, 1
	eor led, temp_word_low
	breq led_off

led_on:
	sbi PORTD, PIND1
	rjmp loop

led_off:
	cbi PORTD, PIND1
	rjmp loop
	
timer_ISR:
	push temp
	in temp, SREG
	push temp
	push temp_word_low
	push temp_word_high
	push ZL
	push ZH
	sts_SRAM allow_update, UPDATE_ALLOWED, temp_word_low
	prepare_pointer_to_obj Z, OCR1AL
	ld temp_word_low, Z
	ldd temp_word_high, Z+1
	subi temp_word_low, LOW(-TIMER_DELAY)
	sbci temp_word_high, HIGH(-TIMER_DELAY)
	std Z+1, temp_word_high
	st Z, temp_word_low
	pop ZH
	pop ZL
	pop temp_word_high
	pop temp_word_low
	pop temp
	out SREG, temp
	pop temp
	reti
	
initializeTimer:
	sts_SRAM TCCR1B, TIMER_PRESCALER, temp_word_low
	sts_SRAM TIMSK1, TIMER_INTERRUPT, temp_word_low
	sts_SRAM OCR1AH, HIGH(TIMER_DELAY), temp_word_low
	sts_SRAM OCR1AL, LOW(TIMER_DELAY), temp_word_low
	ret

send_byte:
	out SPDR, temp_word_low

send_in_progress:
	in r0, SPSR
	sbrs r0, SPIF
	rjmp send_in_progress
	ret
	
send_word:
	ldd temp_word_low, Z+1
	call send_byte
	ld temp_word_low, Z
	call send_byte
	sbi PORTB, PINB1
	cbi PORTB, PINB1
	ret

src: .DW 0x0001, 0x0002, 0x0004, 0x0008, 0x0010, 0x0020, 0x0040, 0x0080, \
		 0x0101, 0x0202, 0x0404, 0x0808, 0x1010, 0x2020, 0x4040, 0x8080