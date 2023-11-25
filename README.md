# CavemanVM Doc

The VM features 16 32-bit registers, each serving specific roles or acting as general-purpose registers.
Register List

   * ZERO: Read-only register, always holds zero.
   * PC: Program Counter - Points to the current instruction.
   * STACK_PTR: Stack Pointer - Can also be used generally.
   * FRAME_PTR: Frame Pointer - Can also be used generally.
   * GLOB_PTR: Global Pointer - Can also be used generally.
   * GPS0, GPS1, GPS2: Saved Global Pointers or General-Purpose Registers.
   * FUN: Function Address - Can also be used generally.
   * ARG0, ARG1, ARG2: Argument Registers or General-Purpose Registers.
   * GPT0, GPT1, GPT2: Temporary or General-Purpose Registers.
   * RET: Return Address Holder - Can also be used as a General-Purpose Register.

## Instruction Set

The VM operates with a variety of instructions, each identified by an opcode and having specific functionalities.
Opcode Definitions. Use basic assembler to write code. 

   * EXIT: Exits the program.
   * LW: Load Word - LW $reg1 $reg2.
   * SW: Store Word - SW $reg1 $reg2.
   * LWOPSW: Load-Operate-Store Word - LWOPSW $reg1 $reg2 $reg3.
   * ADD: Add - ADD $reg3 $reg1 $reg2.
   * SUB: Subtract - SUB $reg3 $reg1 $reg2.
   * MUL: Multiply - MUL $reg3 $reg1 $reg2.
   * DIV: Divide - DIV $reg3 $reg1 $reg2.
   * ADDI: Add Immediate - ADDI $reg3 $reg2 VALUE.
   * NAND: Bitwise NAND - NAND $reg3 $reg1 $reg2.
   * SLT: Set Less Than - SLT $reg3 $reg1 $reg2.
   * BEQ: Branch if Equal - BEQ $reg1 $reg2 VALUE.
   * JZ: Jump if Zero Flag Set - JZ $reg1.
   * SHIFT: Shift - SHIFT $reg1 $reg2 DIRECTION.
   * PUTC: Print Character - PUTC $reg3 OFFSET.
   * GETC: Get Character - GETC $reg3 OFFSET.

## Hello world
example of hello world:
```
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
```

## work in progress

## first iteration of idea: 

A virtual memory tape, where each adress/index on the tape contains a byte.
The computers also has access to a instruction tape where each instruction is 32-bits.
An extremely basic CPU with 5 registers, 2 32-bit input registers and 32 bit output register
, as well as a PC register which is a instruction tape pointer, and a  register for Memory tape pointer.
the CPU supports only 5 operations, addition, subtraction, bitwise AND, And an operation which add/subss an offset to the PC pointer,
Also another opearation which adds/subs an offset to mem pointer.
We also have a zero-flag register that is 1 if the previous result was 0.
