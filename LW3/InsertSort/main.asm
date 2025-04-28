.EQU ARRAY_SIZE = 255
.EQU ELEMENT_SIZE = 4

.MACRO prepare_pointer_to_obj // принимает только регистровые пары X, Y, Z
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

.MACRO get_pointer_to_element // принимает только регистровые пары X, Y, Z
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

.DEF array_size_in_bytes_low = r16 // r17:r16 - длина массива в байтах
.DEF array_size_in_bytes_high = r17 // старший байт длины массива в байтах

.DEF pointer_to_end_of_array_low = r18 // r19:r18 - указатель на верхную границу массива
.DEF pointer_to_end_of_array_high = r19 // указатель на верхную границу массива

.DEF iterator_outer_cycle = r20 // используется во внешнем цикле для условия выхода; по совместительству указатель на текущий элемент
.DEF iterator_inner_cycle = r21 // используется во внутреннем цикле для условия выхода
.DEF iterator_to_previous_element = r22 // итератор на предыдущий элемент

.DEF temp1 = r23 // первый временный регистр на все случаи жизни
.DEF temp2 = r24 // второй временный регистр на все случаи жизни
.DEF temp3 = r25 // третий временный регистр на все случаи жизни

.CSEG
reset:
	rjmp main

main: // начало программы
	ldi temp1, ARRAY_SIZE
	cpi temp1, 0 // проверка на массив
	brne prepare // вход в программу
	jmp loop // выход из программы

prepare:
	prepare_pointer_to_obj Z, (source * 2) // подготовка адреса массива для чтения из памяти программы
	prepare_pointer_to_obj Y, array // подготовка адреса для записи в ОЗУ
	get_size_of_array array_size_in_bytes_low, array_size_in_bytes_high, temp1, temp2 // r1:r0 <= ARRAY_SIZE * ELEMENT_SIZE
	ldi temp2, 1
	ldi temp3, 0

copy_array_to_ram:
	lpm temp1, Z+ // чтение из памяти программы байта значения 
	st Y+, temp1 // и его последующая запись
	sub_word array_size_in_bytes_low, array_size_in_bytes_high, temp2, temp3
	brne copy_array_to_ram

enter_program:
	get_size_of_array array_size_in_bytes_low, array_size_in_bytes_high, temp1, temp2 // array_size_in_bytes_low:array_size_in_bytes_high <= ARRAY_SIZE * ELEMENT_SIZE
	prepare_pointer_to_obj Z, array
	add_word array_size_in_bytes_low, array_size_in_bytes_high, ZL, ZH
	movw pointer_to_end_of_array_low, array_size_in_bytes_low

enter_outer_cycle: // проверка на начало внешнего цикла (сортировки)
	inc iterator_outer_cycle // начало цикла с 1 элемента (отсчет с нулевого элемента)
	cpi iterator_outer_cycle, ARRAY_SIZE // проверка на количество итерации
	brne outer_cycle // сортировка началась или продолжается - вход в цикл
	rjmp print_array // сортировка завершена - выход из цикла

outer_cycle: // выполнение цикла
	prepare_pointer_to_obj Y, array
	get_pointer_to_element Y, iterator_outer_cycle, temp1
	ld_double_word r6, r7, r8, r9, Y // r6 - самый старший разряд ключа; r9 - самый младший разряд ключа
	mov iterator_inner_cycle, iterator_outer_cycle // подготовка отсчёта внутренного цикла
	dec iterator_inner_cycle // inner_cycle_iteration = outer_cycle_iteration - 1

enter_inner_cycle: // проверка на начало внутреннего цикла
	prepare_pointer_to_obj Y, array
	get_pointer_to_element Y, iterator_inner_cycle, temp1
	//cpi iterator_inner_cycle, 255
	add_word temp2, temp3, YL, YH
	cp temp2, pointer_to_end_of_array_low
	cpc temp3, pointer_to_end_of_array_high
	brlt inner_cycle
	rjmp end_inner_cycle

inner_cycle: // выполнение цикла
	ld_double_word r2, r3, r4, r5, X // r2 - самый старший разряд левого числа; r5 - самый младший разряд левого числа

cp_r3: // сравнение старшего разряда числа
	cp r9, r5
	breq cp_r2
	brlt end_inner_cycle // левое значение меньше ключа -> пропускаем
	rjmp need_swap // левое значение больше ключа -> меняем его

cp_r2:
	cp r8, r4
	breq cp_r1
	brsh end_inner_cycle
	rjmp need_swap

cp_r1:
	cp r7, r3
	breq cp_r0
	brsh end_inner_cycle
	rjmp need_swap

cp_r0: // сравнение самого младшего разряда числа
	cp r6, r2
	breq enter_inner_cycle
	brsh end_inner_cycle
	rjmp need_swap

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
	get_size_of_array array_size_in_bytes_low, array_size_in_bytes_high, temp1, temp2 // array_size_in_bytes_low:array_in_bytes_high <= ARRAY_SIZE * ELEMENT_SIZE
	ldi temp2, 1 // вынужденная мера
	ldi temp3, 0 // вынужденная мера

print:
	ld temp1, Z+
	out OCR0A, temp1
	sub_word array_size_in_bytes_low, array_size_in_bytes_high, temp2, temp3
	brne print

loop: // конец
	rjmp loop

// source: .DD 0x86A53468, 0x42110001, 0xFF00FF00, 0x00000001, 0x0A521143, 0x6778899A, 0x80000000
source: .DD 0x000000fe, 0x000000fd, 0x00000000, 0x000000fb, 0x000000fa, 0x000000f9, 0x000000f8, 0x000000f7, 0x000000f6, 0x000000f5, 0x000000f4, 0x000000f3, 0x000000f2, 0x000000f1, 0x000000f0, 0x000000ef, 0x000000ee, 0x000000ed, 0x000000ec, 0x000000eb, 0x000000ea, 0x000000e9, 0x000000e8, 0x000000e7, 0x000000e6, 0x000000e5, 0x000000e4, 0x000000e3, 0x000000e2, 0x000000e1, 0x000000e0, 0x000000df, 0x000000de, 0x000000dd, 0x000000dc, 0x000000db, 0x000000da, 0x000000d9, 0x000000d8, 0x000000d7, 0x000000d6, 0x000000d5, 0x000000d4, 0x000000d3, 0x000000d2, 0x000000d1, 0x000000d0, 0x000000cf, 0x000000ce, 0x000000cd, 0x000000cc, 0x000000cb, 0x000000ca, 0x000000c9, 0x000000c8, 0x000000c7, 0x000000c6, 0x000000c5, 0x000000c4, 0x000000c3, 0x000000c2, 0x000000c1, 0x000000c0, 0x000000bf, 0x000000be, 0x000000bd, 0x000000bc, 0x000000bb, 0x000000ba, 0x000000b9, 0x000000b8, 0x000000b7, 0x000000b6, 0x000000b5, 0x000000b4, 0x000000b3, 0x000000b2, 0x000000b1, 0x000000b0, 0x000000af, 0x000000ae, 0x000000ad, 0x000000ac, 0x000000ab, 0x000000aa, 0x000000a9, 0x000000a8, 0x000000a7, 0x000000a6, 0x000000a5, 0x000000a4, 0x000000a3, 0x000000a2, 0x000000a1, 0x000000a0, 0x0000009f, 0x0000009e, 0x0000009d, 0x0000009c, 0x0000009b, 0x0000009a, 0x00000099, 0x00000098, 0x00000097, 0x00000096, 0x00000095, 0x00000094, 0x00000093, 0x00000092, 0x00000091, 0x00000090, 0x0000008f, 0x0000008e, 0x0000008d, 0x0000008c, 0x0000008b, 0x0000008a, 0x00000089, 0x00000088, 0x00000087, 0x00000086, 0x00000085, 0x00000084, 0x00000083, 0x00000082, 0x00000081, 0x00000080, 0x0000007f, 0x0000007e, 0x0000007d, 0x0000007c, 0x0000007b, 0x0000007a, 0x00000079, 0x00000078, 0x00000077, 0x00000076, 0x00000075, 0x00000074, 0x00000073, 0x00000072, 0x00000071, 0x00000070, 0x0000006f, 0x0000006e, 0x0000006d, 0x0000006c, 0x0000006b, 0x0000006a, 0x00000069, 0x00000068, 0x00000067, 0x00000066, 0x00000065, 0x00000064, 0x00000063, 0x00000062, 0x00000061, 0x00000060, 0x0000005f, 0x0000005e, 0x0000005d, 0x0000005c, 0x0000005b, 0x0000005a, 0x00000059, 0x00000058, 0x00000057, 0x00000056, 0x00000055, 0x00000054, 0x00000053, 0x00000052, 0x00000051, 0x00000050, 0x0000004f, 0x0000004e, 0x0000004d, 0x0000004c, 0x0000004b, 0x0000004a, 0x00000049, 0x00000048, 0x00000047, 0x00000046, 0x00000045, 0x00000044, 0x00000043, 0x00000042, 0x00000041, 0x00000040, 0x0000003f, 0x0000003e, 0x0000003d, 0x0000003c, 0x0000003b, 0x0000003a, 0x00000039, 0x00000038, 0x00000037, 0x00000036, 0x00000035, 0x00000034, 0x00000033, 0x00000032, 0x00000031, 0x00000030, 0x0000002f, 0x0000002e, 0x0000002d, 0x0000002c, 0x0000002b, 0x0000002a, 0x00000029, 0x00000028, 0x00000027, 0x00000026, 0x00000025, 0x00000024, 0x00000023, 0x00000022, 0x00000021, 0x00000020, 0x0000001f, 0x0000001e, 0x0000001d, 0x0000001c, 0x0000001b, 0x0000001a, 0x00000019, 0x00000018, 0x00000017, 0x00000016, 0x00000015, 0x00000014, 0x00000013, 0x00000012, 0x00000011, 0x00000010, 0x0000000f, 0x0000000e, 0x0000000d, 0x0000000c, 0x0000000b, 0x0000000a, 0x00000009, 0x00000008, 0x00000007, 0x00000006, 0x00000005, 0x00000004, 0x00000003, 0x00000002, 0x00000001, 0x00000000