.EQU ARRAY_SIZE = 6
.EQU ELEMENT_SIZE = 4

.MACRO prepare_pointer_to_obj // ��������� ������ ����������� ���� X, Y, Z
	ldi @0H, HIGH(@1)
	ldi @0L, LOW(@1)
.ENDMACRO

.MACRO ld_double_word
	ld @0, @4+
	ld @1, @4+
	ld @2, @4+
	ld @3, @4
.ENDMACRO

.MACRO st_double_word
	st @0+, @1
	st @0+, @2
	st @0+, @3
	st @0, @4
.ENDMACRO

.MACRO get_pointer_to_element // ��������� ������ ����������� ���� X, Y, Z
	ldi @2, ELEMENT_SIZE
	mul @1, @2
	add @0L, r0
	adc @0H, r1
.ENDMACRO

.MACRO get_size_of_array
	ldi @2, ARRAY_SIZE
	ldi @3, ELEMENT_SIZE
	mul @2, @3
	mov @0, r0
	mov @1, r1
.ENDMACRO

.MACRO sub_word
	sub @0, @2
	sbc @1, @3
.ENDMACRO

.MACRO add_word
	add @0, @2
	adc @1, @3
.ENDMACRO

.DSEG
array: .BYTE ARRAY_SIZE * ELEMENT_SIZE

.DEF array_size_low = r18 // ������� ���� ����� ������� � ������
.DEF array_size_high = r19 // ������� ���� ����� ������� � ������

.DEF iterator_outer_cycle = r20
.DEF iterator_inner_cycle = r21
.DEF iterator_to_previous_element = r22

.DEF temp1 = r23 // ������ ��������� ������� �� ��� ������ �����
.DEF temp2 = r24 // ������ ��������� ������� �� ��� ������ �����
.DEF temp3 = r25 // ������ ��������� ������� �� ��� ������ �����

.CSEG
reset:
	rjmp main

main: // ������ ���������
	ldi temp1, ARRAY_SIZE
	cpi temp1, 0 // �������� �� ������
	brne prepare // ���� � ���������
	jmp loop // ����� �� ���������

prepare:
	prepare_pointer_to_obj Z, (source * 2) // ���������� ������ ������� ��� ������ �� ������ ���������
	prepare_pointer_to_obj Y, array // ���������� ������ ��� ������ � ���
	get_size_of_array array_size_low, array_size_high, temp1, temp2 // array_size_in_bytes_high:array_size_in_bytes_low <= ARRAY_SIZE * ELEMENT_SIZE
	ldi temp2, 1
	ldi temp3, 0

copy_array_to_ram:
	lpm temp1, Z+ // ������ �� ������ ��������� ����� �������� 
	st Y+, temp1 // � ��� ����������� ������
	sub_word array_size_low, array_size_high, temp2, temp3 // array_size_high:array_size_low <= array_size_high:array_size_low - 1
	brne copy_array_to_ram

enter_outer_cycle: // �������� �� ������ �������� ����� (����������)
	inc iterator_outer_cycle // ������ ����� � 1 �������� (������ � ����� ��� � �������� ��������)
	cpi iterator_outer_cycle, ARRAY_SIZE // �������� �� ���������� ��������
	brne outer_cycle // ���������� �������� ��� ������������ - ���� � ����
	rjmp print_array // ���������� ��������� - ����� �� �����

outer_cycle: // ���������� �����
	prepare_pointer_to_obj Y, array
	get_pointer_to_element Y, iterator_outer_cycle, temp1
	ld_double_word r6, r7, r8, r9, Y // r6 - ����� ������� ������ �����; r9 - ����� ������� ������ �����
	mov iterator_inner_cycle, iterator_outer_cycle // ���������� ������� ����������� �����
	dec iterator_inner_cycle // inner_cycle_iteration = outer_cycle_iteration - 1

enter_inner_cycle: // �������� �� ������ ����������� �����
	cpi iterator_inner_cycle, 255 // �������� �� ������������
	breq end_inner_cycle

inner_cycle: // ���������� �����
	prepare_pointer_to_obj X, array
	get_pointer_to_element X, iterator_inner_cycle, temp1
	ld_double_word r2, r3, r4, r5, X // r2 - ����� ������� ������ ������ �����; r5 - ����� ������� ������ ������ �����

cp_high_high_cell: // ��������� ����������� ������� �����
	cp r9, r5 // ���� =V= ����� ��������
	breq cp_high_low_cell // ������� �� ������� ������
	brlt need_swap // ���� < ����� �������� => ������
	rjmp end_inner_cycle // ���� > ����� �������� => ����������

cp_high_low_cell:
	cp r8, r4
	breq cp_low_high_cell
	brlo need_swap // ����������� ���������
	rjmp end_inner_cycle

cp_low_high_cell:
	cp r7, r3
	breq cp_low_low_cell
	brlo need_swap
	rjmp end_inner_cycle

cp_low_low_cell:
	cp r6, r2
	breq enter_inner_cycle
	brlo need_swap
	rjmp end_inner_cycle

need_swap: // arr[inner_cycle_iteration - 1] > key
	inc iterator_inner_cycle
	prepare_pointer_to_obj X, array
	get_pointer_to_element X, iterator_inner_cycle, temp1
	st_double_word X, r2, r3, r4, r5
	dec iterator_inner_cycle
	dec iterator_inner_cycle
	rjmp enter_inner_cycle

end_inner_cycle:
	inc iterator_inner_cycle
	prepare_pointer_to_obj X, array
	get_pointer_to_element X, iterator_inner_cycle, temp1
	st_double_word X, r6, r7, r8, r9
	rjmp enter_outer_cycle

print_array:
	prepare_pointer_to_obj Z, array
	get_size_of_array array_size_low, array_size_high, temp1, temp2 // array_size_in_bytes_low:array_in_bytes_high <= ARRAY_SIZE * ELEMENT_SIZE
	ldi temp2, 1 // ����������� ����
	ldi temp3, 0 // ����������� ����

print:
	ld temp1, Z+
	out OCR0A, temp1
	sub_word array_size_low, array_size_high, temp2, temp3
	brne print

loop: // �����
	rjmp loop

source: .DD 0x876cfbd0, 0x1aa55cc9, 0x0001dccc, 0x46d8a34a, 0x290f831b, 0xfad04a1a // 6 ���������