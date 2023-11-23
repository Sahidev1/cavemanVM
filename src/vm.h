#ifndef VM_H
#define VM_H
#include <stdint.h>

#define MEMORY_TAPE_SIZE (1<<16) //memory tape size in bytes
#define INSTRUCTION_TAPE_SIZE (1<<16)
#define NR_REGISTERS 7

typedef int32_t word32;

enum registers{
    MEM_PTR,
    PC,
    GP0, //GENERAL PURPOSE
    GP1,
    GP2,
    RET, // RETURN AND GENERAL PURPOSE REGISTER
    ZERO // always zero register
};

enum opcodes {
    EXIT,   // EXIT
    LW , //LOAD WORD: LW $1 $2, loads word from memery address in $2 to $1
    SW, //STORE WORD: SW $1 $2 stores word in $1 at momery address $2
 
    ADD, // ADD $3 $1 $2 , add result of $1 + $2 in register $3
    SUB, // 
    MUL,
    DIV,
    ADDI, // ADD $3 $2 VALUE

    AND, // AND $3 $1 $2
    NOT, // NOT $3 , BITFLIPS register $3

    J, //JUMP to address in return address register: J $1
    JZ, //JUMP to address if zero flag is set

    PRNT // PRNT $3 , prints out contents of $3 as a word
};

#endif