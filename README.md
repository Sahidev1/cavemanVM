# CavemanVM
This hobbyproject is work in progress. 
## Registers
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

    EXIT: Exits the program.
        Terminates the execution of the VM.

    LW (Load Word): LW $reg1 $reg2
        Loads a word (32-bit value) from the memory address specified in $reg2 into $reg1.

    SW (Store Word): SW $reg1 $reg2
        Stores the word in $reg1 into the memory address specified in $reg2.

    LWOPSW (Load-Operate-Store Word): LWOPSW $reg1 $reg2 $reg3 OPCODE
        Loads words from memory addresses in $reg2 and $reg3, performs an operation specified by OPCODE, and stores the result in the memory address specified in $reg1.Not Only few basic operations are supported like add, sub, mul, div, slt.

    ADD: ADD $reg3 $reg1 $reg2
        Adds the contents of $reg1 and $reg2 together, and stores the result in $reg3.

    SUB (Subtract): SUB $reg3 $reg1 $reg2
        Subtracts the content of $reg2 from $reg1 and stores the result in $reg3.

    MUL (Multiply): MUL $reg3 $reg1 $reg2
        Multiplies the contents of $reg1 and $reg2, and stores the result in $reg3.

    DIV (Divide): DIV $reg3 $reg1 $reg2
        Divides the content of $reg1 by $reg2 and stores the result in $reg3.

    ADDI (Add Immediate): ADDI $reg3 $reg2 VALUE
        Adds a constant value (VALUE) to the content of $reg2 and stores the result in $reg3.

    NAND (Bitwise NAND): NAND $reg3 $reg1 $reg2
        Performs a bitwise NAND operation on the contents of $reg1 and $reg2, and stores the result in $reg3.

    SLT (Set Less Than): SLT $reg3 $reg1 $reg2
        Compares the contents of $reg1 and $reg2. If $reg1 is less than $reg2, it sets $reg3 to 1, otherwise sets it to 0.

    BEQ (Branch if Equal): BEQ $reg1 $reg2 VALUE
        Compares the contents of $reg1 and $reg2. If they are equal, the program counter (PC) jumps to the address specified by VALUE.

    JZ (Jump if Zero Flag Set): JZ $reg1
        Checks the zero flag. If it is set, jumps to the address contained in $reg1.

    SHIFT: SHIFT $reg1 $reg2 DIRECTION
        Performs a bitwise shift on $reg1. The number of positions to shift is specified by $reg2. The DIRECTION determines if it's a left shift (0) or a right shift (non-zero).

    PUTC (Print Character): PUTC $reg3 OFFSET
        Prints a character to the standard output. The character is extracted from the register $reg3, based on the OFFSET (0 to 3), which specifies which byte to print.

    GETC (Get Character): GETC $reg3 OFFSET
        Reads a character from the standard input and stores it in the register $reg3. The OFFSET (0 to 3) specifies where to store the byte in the register.

## Hello world
Example hello world program:
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

## first iteration of idea: 

A virtual memory tape, where each adress/index on the tape contains a byte.
The computers also has access to a instruction tape where each instruction is 32-bits.
An extremely basic CPU with 5 registers, 2 32-bit input registers and 32 bit output register
, as well as a PC register which is a instruction tape pointer, and a  register for Memory tape pointer.
the CPU supports only 5 operations, addition, subtraction, bitwise AND, And an operation which add/subss an offset to the PC pointer,
Also another opearation which adds/subs an offset to mem pointer.
We also have a zero-flag register that is 1 if the previous result was 0.
