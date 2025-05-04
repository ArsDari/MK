import json

with open('flash.txt') as file:
    original_machine_code = []
    for machine_code in file.readlines():
        pointer = machine_code[9:-3]
        for pos in range(3, len(pointer), 4):
            original_machine_code.append(pointer[pos - 1:pos + 1] + pointer[pos - 3:pos - 1]) # поменять байты местами

for instruction in original_machine_code:
    binary_instruction = bin(int(instruction, 16))[2:].rjust(16, '0')
    print(instruction, binary_instruction)
