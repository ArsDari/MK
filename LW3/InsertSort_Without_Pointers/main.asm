.EQU ARRAY_SIZE = 6
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

.DEF array_size_low = r18 // младший байт длины массива в байтах
.DEF array_size_high = r19 // старший байт длины массива в байтах

.DEF iterator_outer_cycle = r20
.DEF iterator_inner_cycle = r21
.DEF iterator_to_previous_element = r22

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
	get_size_of_array array_size_low, array_size_high, temp1, temp2 // array_size_in_bytes_high:array_size_in_bytes_low <= ARRAY_SIZE * ELEMENT_SIZE
	ldi temp2, 1
	ldi temp3, 0

copy_array_to_ram:
	lpm temp1, Z+ // чтение из памяти программы байта значения 
	st Y+, temp1 // и его последующая запись
	sub_word array_size_low, array_size_high, temp2, temp3 // array_size_high:array_size_low <= array_size_high:array_size_low - 1
	brne copy_array_to_ram

enter_outer_cycle: // проверка на начало внешнего цикла (сортировки)
	inc iterator_outer_cycle // начало цикла с 1 элемента (отсчет в цикле идёт с нулевого элемента)
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
	cpi iterator_inner_cycle, 255 // проверка на переполнение
	breq end_inner_cycle

inner_cycle: // выполнение цикла
	prepare_pointer_to_obj X, array
	get_pointer_to_element X, iterator_inner_cycle, temp1
	ld_double_word r2, r3, r4, r5, X // r2 - самый старший разряд левого числа; r5 - самый младший разряд левого числа

cp_high_high_cell: // сравнение наибольшего разряда числа
	cp r9, r5 // ключ =V= левое значение
	breq cp_high_low_cell // переход на младший разряд
	brlt need_swap // ключ < левое значение => меняем
	rjmp end_inner_cycle // ключ > левое значение => пропускаем

cp_high_low_cell:
	cp r8, r4
	breq cp_low_high_cell
	brlo need_swap // беззнаковое сравнение
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
	ldi temp2, 1 // вынужденная мера
	ldi temp3, 0 // вынужденная мера

print:
	ld temp1, Z+
	out OCR0A, temp1
	sub_word array_size_low, array_size_high, temp2, temp3
	brne print

loop: // конец
	rjmp loop

source: .DD 0x876cfbd0, 0x1aa55cc9, 0x0001dccc, 0x46d8a34a, 0x290f831b, 0xfad04a1a // 6 элементов