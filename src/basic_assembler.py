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
        'SHIFT': 0xd,
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
        if(opcode == opcode_mapping['ADDI'] or opcode == opcode_mapping['SHIFT'] or opcode == opcode_mapping['BEQ']): 
            value = int(parts[3])
            instr = instr | (value<<16)
        else:
            arg3 = register_mapping[parts[3]]
            instr = instr | (arg3<<12)
    else: 
        print('invalid nr of args\n')
    return instr

def write_to_binary_file(hex_instructions, file_name):
    with open(file_name, 'wb') as file:
        for hex_instr in hex_instructions:
            print(f'{hex_instr: x}')
            packed_word = struct.pack('<I', hex_instr)
            file.write(packed_word)


# Example usage

assembly_code0 = [
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

#hello world program
assembly_code = [
    'ADDI $sp $z 0',
    'ADDI $gp $z 16',
    'ADDI $a0 $z 27756',
    'ADDI $a1 $z 25960', 
    'ADDI $ra $z 6', #PC 4

    'BEQ $z $z 17', 
    'SW $fa $sp',
    'ADDI $sp $sp 4', #PC 7
    'ADDI $a0 $z 29295',
    'ADDI $a1 $z 30575',
    
    'ADDI $ra $z 12',
    'BEQ $z $z 17', #PC 10
    'SW $fa $sp',
    'ADDI $sp $sp 4', #PC  12
    'ADDI $a0 $z 25708',
    
    'SW $a0 $sp',
    'BEQ $z $z  23', #PC 15
    'SHIFT $a0 $gp 0', # FUNCTION that writes ors 16 bits into 32 bits
    'NAND $a0 $a0 $a0',
    'NAND $a1 $a1 $a1', #PC 19
    
    'NAND $fa $a0 $a1',
    'ADDI $t0 $z 0',
    'JZ $ra', #PC 21
    'ADDI $sp $sp 4',
    'ADDI $s0 $z 0',
    
    'SW $s0 $sp',
    'ADDI $sp $z 0',
    'BEQ $z $z 28', #PC 26
    'LW $s0 $sp',
    'BEQ $s0 $z 36', #PC 29
    
    'PUTC $s0 0',
    'PUTC $s0 1',
    'PUTC $s0 2', #PC 31
    'PUTC $s0 3',
    'ADDI $sp $sp 4',
    
    'BEQ $z $z 28', #PC 34
    'EXIT' 
]

hex_instructions = map(assemble_instruction, assembly_code)

#for hex_instr in hex_instructions:
 #   print(f'{hex_instr: x}')


write_to_binary_file(hex_instructions, 'instruction_tape.bin')