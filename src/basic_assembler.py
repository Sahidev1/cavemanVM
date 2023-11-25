import struct

def to_32_bit(value):
    return value & 0xffffffff

def assemble_instruction(instruction):
    opcode_mapping = {
        'EXIT': 0x0,
        'LW': 0x1,
        'SW': 0x2,
        'LWOPSW': 0x3,

        'ADD': 0x4,
        'SUB': 0x5,
        'MUL': 0x6,
        'DIV': 0x7,
        
        'ADDI': 0x8,
        'NAND': 0x9,
        'SLT': 0xa,
        'BEQ': 0xb,

        'JZ': 0xc,
        'PRNT': 0xd,
        'PUTC': 0xe,
        'GETC': 0xf
    }

    register_mapping = {
        '$z': 0x0,
        '$pc': 0x1,

        '$sp': 0x2,
        '$fp': 0x3,
        '$gp': 0x4,

        '$s0': 0x5,
        '$s1': 0x6,
        '$s2': 0x7,

        '$fa': 0x8,

        '$a0': 0x9,
        '$a1': 0xa,
        '$a2': 0xb,

        '$t0': 0xc,
        '$t1': 0xd,
        '$t2': 0xe,

        '$ra': 0xf
    }

    parts = instruction.split()
    opcode = opcode_mapping[parts[0]]
    nr_args = len(parts) - 1

    instr = to_32_bit(0x0)
    instr = instr | opcode
    if (nr_args == 0 and opcode ==opcode_mapping['EXIT']):
        return instr
    elif(nr_args == 1):
        arg1 = register_mapping[parts[1]]
        instr = instr | (arg1<<4)
    elif(nr_args == 2):
        if opcode in [0xd, 0xe, 0xf]:
            arg1 = register_mapping[parts[1]]
            value = int(parts[2])
            instr = instr | (arg1<<4) | (value<<8)
        else:
            arg1 = register_mapping[parts[1]]
            arg2 = register_mapping[parts[2]]
            instr = (instr | (arg1<<4))|arg2<<8
    elif(nr_args == 3):
        arg1 = register_mapping[parts[1]]
        arg2 = register_mapping[parts[2]]
        instr = (instr | (arg1<<4))|arg2<<8
        if(opcode == opcode_mapping['ADDI']): 
            value = int(parts[3])
            instr = instr | (value<<16)
        else:
            arg3 = register_mapping[parts[3]]
            instr = instr | (arg3<<12)
    elif(nr_args == 4):
        arg1 = register_mapping[parts[1]]
        arg2 = register_mapping[parts[2]]
        arg3 = register_mapping[parts[3]]
        second_op = int(parts[4])
        instr = (instr | (arg1<<4))|arg2<<8 | arg3<<12 | second_op<<16
    else: 
        print('invalid nr of args\n')
    return instr

def write_to_binary_file(hex_instructions, file_name):
    with open(file_name, 'wb') as file:
        for hex_instr in hex_instructions:
            packed_word = struct.pack('<I', hex_instr)
            file.write(packed_word)


# Example usage
assembly_code0 = [
    'ADDI $m $z 0',
    'ADDI $s0 $z 10', # loop var
    'ADDI $s1 $z 1', #inc value
    'ADDI $ra $z 4',
    'SW $ra $m',

    'ADDI $ra $z 12',
    'PRNT $s0',
    'SUB $s0 $s0 $s1',
    'JZ $ra',

    'LW $ra $m',
    'J $ra',
    'EXIT'
]

assembly_code1 = [
    'ADDI $s0 $z 215',
    'ADDI $s1 $z 471',
    'MUL $s2 $s1 $s0',
    'PRNT $s2',
    'SW $s0 $m',
    'ADDI $m $m 4',
    'SW $s1 $m',
    'ADDI $m $m 4',
    'SW $s2 $m',
    'LW $ra $m',
    'EXIT'
]

assembly_code3 = [
    'ADDI $m $z 0',
    'ADDI $s0 $z 10', # loop var
    'ADDI $s1 $z 1', #inc value
    'ADDI $ra $z 5',
    
    'SW $ra $m',
    'PRNT $s0',
    'ADDI $m $z 4',
    'SW $s1 $m',
    'ADDI $s1 $s0 5',

    'ADDI $ra $z 20',
    'J $ra',
    'PRNT $s2',
    'LW $s1 $m',

    'ADDI $m $z 0',
    'ADDI $ra $z 19',
    'SUB $s0 $s0 $s1',
    'JZ $ra',

    'LW $ra $m',
    'J $ra',
    'EXIT',

    'MUL $s2 $s1 $s1', #square function
    'ADDI $ra $z 11',
    'J $ra'
]

assembly_code = [
    'ADDI $sp $z 0',
    'ADDI $s0 $z 1123',
    'SW $s0 $sp',
    'ADD $s0 $z $sp',
    
    'ADDI $s2 $sp 4',
    'LWOPSW $s2 $s0 $s0 4',
    'LW $t0 $s2',
    'PRNT $t0 0',
    'EXIT'
]

assembly_codes = [
    'LWOPSW $s2 $s0 $s0 4'
]

hex_instructions = map(assemble_instruction, assembly_code)

#for hex_instr in hex_instructions:
 #   print(f'{hex_instr: x}')


write_to_binary_file(hex_instructions, 'instruction_tape.bin')