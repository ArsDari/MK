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

source: .DD 0x776cfbd0, 0x1aa55cc9, 0x0001dccc, 0x46d8a34a, 0x290f831b, 0x0ad04a1a, 0xbfbd8b49, 0x461b255f, 0x28917343, 0x7d8c1fed, 0x6eea095b, 0x3c5bc5eb, 0xf3fb5f3a, 0x0469b207, 0xfb593081, 0x9a2d7819, 0x54a8fb79, 0x37451b69, 0xeec0bde3, 0x96f6ae5e, 0xf826b430, 0x383680ab, 0xc69f6a58, 0x7d228cd7, 0x353b22ec, 0xcadcbeae, 0x932c888c, 0x5e1a0184, 0x7012320a, 0x4def6163, 0xacc8b5c0, 0xbe4a80aa, 0xf067d6ca, 0x613face8, 0xc874a6f7, 0xb24d427f, 0x6be5b3e4, 0x380b536f, 0xa0d30aa1, 0xe440cc70, 0x5b4843ae, 0x45566aa8, 0x2c45246b, 0x02a7d995, 0x00fba551, 0xaab7670c, 0x047868ae, 0x60d3cb40, 0xa16684be, 0x01825b98, 0x9712a52b, 0x7ae15d8f, 0xaa23c2f7, 0x535cfd50, 0x95ff2bbe, 0xfbfa4f8c, 0x69c27cc7, 0xdc3adcca, 0xf0fe7115, 0xa836ab09, 0xfe55e5a9, 0x17930bbc, 0xaaaf92ab, 0x1b87f26f, 0x0e8c1bad, 0x141a5ca0, 0x69834033, 0x9fdf5df2, 0x991ed696, 0x0f9e100f, 0x4b967e3d, 0xc570f43b, 0x4c8d74b4, 0xeb7b34f1, 0xb66ea3d7, 0x3d5fee6e, 0x9e551b22, 0x01e673f6, 0xc6f3c9d4, 0x3655ab01, 0xc032389f, 0xf79fd9c9, 0x8643ad94, 0x5a960d0e, 0x7ca45e9a, 0x6cd89976, 0xf5083f7f, 0x59ac590c, 0x2ac4d049, 0x922a6a49, 0x51cb6e97, 0xfd1a638e, 0x11adeee9, 0x86e66926, 0x41a10a74, 0x25c0bd07, 0x4a0e8939, 0xcc89d3ed, 0x4100a28c, 0x4e5f53f8, 0x1d049b64, 0xc7ceb620, 0x15afb241, 0x18b4d449, 0x2c1aa972, 0x90457766, 0xae5e02a7, 0xed881f3a, 0x321ecab9, 0xb45225ac, 0x4f2f0345, 0x2d5286a7, 0x8d03c98b, 0x59bfe6aa, 0x4627d8f2, 0xcb9d4396, 0x85790e0e, 0xd1196036, 0x4281949e, 0xa17ac061, 0x18c75141, 0xeeeee59d, 0xa0e03010, 0x68539eed, 0xca2860f2, 0x697dff38, 0xa5ff1262, 0x41c89e5c, 0x0f77c0e1, 0xde2c20b3, 0xe69b3a5d, 0xccbf88aa, 0xf55d226f, 0x04846e99, 0x5eed6398, 0x9fffdc71, 0xeb2cf3f5, 0xa708dd00, 0xb30c7cf4, 0xc74ed73b, 0x5c8bde8e, 0xd6447730, 0xf5e26f2e, 0x777a84fe, 0xdeb84446, 0xc463f67e, 0xb638290f, 0x219772cf, 0x9d766719, 0x9aa80359, 0x9cf98fc3, 0x6ba0b745, 0xce2df875, 0xfc0e0050, 0xa4a9fb1a, 0x37e46338, 0x2757bcc8, 0x4e1d5f65, 0x44517ab7, 0xec2d4bae, 0xd5edc61d, 0x8af43604, 0x384579f1, 0x9dbf1278, 0x6d8578a7, 0x02011bc3, 0xf8964f41, 0x9eb95d62, 0xfefc6ef7, 0x5775ce5e, 0x397d1f51, 0xfb33c8c6, 0x4f859539, 0x85f0e483, 0x88286916, 0x423f8fd4, 0x777ecd7e, 0x235ade04, 0x24f5a750, 0xf63501d9, 0xc43a240b, 0xe7aa6dba, 0xf93d932c, 0xdcb69455, 0x302b5a7c, 0xf1326d55, 0x486663cc, 0xb1cdf0bf, 0x010c23ef, 0xea2bfce8, 0x9b592366, 0xfb75eb46, 0xe09af91c, 0x0628b50d, 0x8a00963f, 0xa18947ba, 0x0fc6eb5b, 0xc51d36ab, 0x2698212d, 0xee4b7c35, 0x935bfbf6, 0xb6c26e82, 0xb3955f2b, 0x5d46859d, 0x3fee3f94, 0x54ab6f1e, 0x2c843349, 0x8cf8e67e, 0x59933ed8, 0x9ae08d9c, 0x051d8c52, 0x8acfb7d8, 0x26ad3808, 0x75b49f05, 0xf9f858d3, 0x1fd7a8ad, 0x5b3974b1, 0xe1ab80e2, 0xb7473478, 0x09f16811, 0xdb09f02a, 0x2e1d3a28, 0x75c2b54a, 0xc3bad7eb, 0x98e636cf, 0xf91faf44, 0x730aed36, 0xb801c846, 0xfba5a456, 0x244da0a2, 0x39c6b731, 0xa497da2a, 0xc907d10e, 0x5e866e02, 0x054b49ce, 0x744897b0, 0xd8f4e4cf, 0xe89ef9d3, 0xb436ec35, 0x473e9a40, 0x3b36cb93, 0x8fd92882, 0x00f1277d, 0x0070011e, 0x736c07d2, 0xa7f71954, 0x1388a994, 0xc9f22762, 0x196e0773, 0x4dc9927c, 0x303dba39, 0x7526cda7, 0x1e4bf86d, 0x57b386c2, 0xd588d0e2