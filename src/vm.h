#ifndef VM_H
#define VM_H
#include <stdint.h>

#define MEMORY_TAPE_SIZE (1<<16) //memory tape size in bytes
#define INSTRUCTION_TAPE_SIZE (1<<16)
#define NR_REGISTERS 16

typedef int32_t word32;


typedef struct{
    word32 memory_tape[MEMORY_TAPE_SIZE / sizeof(word32)];
    word32 instr_tape[INSTRUCTION_TAPE_SIZE / sizeof(word32)];
    word32 registers[NR_REGISTERS];
    int zero_flag;
    int exit_status;
} vm;

typedef struct{
    int opcode;
    int reg1;
    int reg2;
    int reg3;
    word32 arg;
} decoded_instruction;

int run_vm(vm *vm, int RUN_MODE);
int validate_PC(vm *vm);

enum run_modes{
    RUN_MODE_NORMAL,
    RUN_MODE_DEBUG,
    RUN_MODE_STEP,
};

//registers are 4 bit addressed
enum registers{
    ZERO, //ALWAYS ZERO cant be WRITTEN TO
    PC, // PROGRAM COUNTER aka instruction tape pointer
    STACK_PTR, // STACK POINTER OR GENERAL PURPOSE
    FRAME_PTR, // FRAME POINTER OR GENERAL PURPOSE

    GLOB_PTR, // GLOBAL POINTER OR GENERAL PURPOSE
    GPS0, // SAVED GP OR GENERAL PURPOSE
    GPS1, //SAVED GP OR GENERAL PURPOSE
    GPS2, //SAVED GP OR GENERAL PURPOSE

    FUN, // FUNCTION ADDRESS OR GENERAL PURPOSE
    ARG0, // ARGUMENTS OR GENERAL PURPOSE
    ARG1, // ARGUMENTS OR GENERAL PURPOSE
    ARG2, // ARGUMENTS OR GENERAL PURPOSE

    GPT0, // TEMPORARY OR GENERAL PURPOSE
    GPT1, // TEMPORARY OR GENERAL PURPOSE
    GPT2, // TEMPORARY OR GENERAL PURPOSE    
    RET, // RETURN ADDRESS HOLDER OR GENERAL PURPOSE REGISTER
};

//opcodes are 4 bit addressed
enum opcodes {
    EXIT,   // EXIT PROGRAM
    LW , //LOAD WORD: LW $1 $2, loads word from memery address in $2 to $1
    SW, //STORE WORD: SW $1 $2 stores word in $1 at momery address $2
    LWOPSW, // LWOPSW: $1 $2 $3 | (opcode =! LWOPSW)<<11, directly loads from mem addresses in $2 and $3, performs an operation, and puts it directly in mem adress in $1 
    ADD, // ADD $3 $1 $2 , add result of $1 + $2 in register $3

    SUB, // SUB $3 $1 $2 , subtract $2 from $1 and store result in $3   
    MUL, // MUL $3 $1 $2 , multiply $1 and $2 and store result in $3
    DIV, // DIV $3 $1 $2 , divide $1 by $2 and store result in $3
    ADDI, // ADD $3 $2 VALUE , add VALUE to $2 and store result in $3

    NAND, // NAND $3 $1 $2 , bitwise NAND of $1 and $2 in register $3
    SLT, // SET LESS THAN $3 $1 $2 , if $1 < $2 set $3 to 1, else set $3 to 0
    BEQ, // BRANCH IF EQUAL $1 $2 VALUE , if $1 == $2 jump to VALUE

    JZ, //JUMP to address if zero flag is set: JZ $1
    SHIFT, // SHIFT $1 $2 DIRECTION, shift $1 by $2 bits in DIRECTION(0 = left, >0 = right)
    PUTC, // PUTC $3 offset , prints out first a byte from a register, offset(0 to 3) specifices which byte
    GETC, // GETC $3 offset, reads a byte from stdin and stores it in register $3, offset(0 to 3) specifices which byte
};

#endif