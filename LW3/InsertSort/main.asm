.EQU ARRAY_SIZE = 6
.EQU BYTE_LONG = 4

.DSEG
array: .BYTE ARRAY_SIZE * BYTE_LONG

.DEF pointer = r24
.DEF counter = r25

.CSEG
reset:
	rjmp main

main:
	ldi ZH, HIGH(source * 2)
	ldi ZL, LOW(source * 2)
	ldi YH, HIGH(array)
	ldi YL, LOW(array)
	ldi counter, ARRAY_SIZE * BYTE_LONG / 2

copy:
	lpm r16, Z+
	st Y+, r16
	dec counter
	brne copy
	ldi YH, HIGH(array)
	ldi YL, LOW(array)

enter_for: // ������ �����

for: // ����

enter_while: // �������� ���������� ����� ����������
	ld r16, Y+ // r16-19 ����� �����
	ld r17, Y+
	ld r18, Y+
	ld r19, Y+
	ld r20, Y+ // r20-23 ������ ����� r23 - ����� ������� ������, r20 - ����� �������
	ld r21, Y+
	ld r22, Y+
	ld r23, Y
	mov r1, r20 // ���� - ����� r20-r23
	mov r2, r21
	mov r3, r22
	mov r4, r23

while: // ������ �����

cp_r1: // ��������� �������� ������� �����
	cp r19, r23 // ���������� ����� ������� �������, ��� ����� ������� ����
	breq cp_r2
	brlt left_lesser_right // ������ ������ �������
	rjmp left_bigger_right // ������ ������ �������

cp_r2:
	cp r18, r22 // ������ ���� ������
	breq cp_r3
	brlt left_lesser_right // ������ ������ �������
	rjmp left_bigger_right // ������ ������ �������

cp_r3:
	cp r17, r21 // ������ ���� ������
	breq cp_r4
	brlt left_lesser_right // ������ ������ �������
	rjmp left_bigger_right // ������ ������ �������

cp_r4: // ��������� ������ �������� ������� �����
	cp r16, r20 // ����� ������� �����
	breq loop
	brlt left_lesser_right // ������ ������ �������
	rjmp left_bigger_right // ������ ������ �������

left_lesser_right:
	ldi r26, 1

left_bigger_right:
	mov r1, r16
	mov r2, r17
	mov r3, r18

print_rest_of_array:

loop:
	rjmp loop

source: .DD 0x86A53468, 0x42110001, 0xFF00FF00, 0x00000001, 0x0A521143, 0x6778899A