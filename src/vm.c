#include "vm.h"
#include <stdio.h>

word32 memory_tape[MEMORY_TAPE_SIZE / sizeof(word32)];
word32 instr_tape[INSTRUCTION_TAPE_SIZE / sizeof(word32)];

word32 registers[NR_REGISTERS];
int zero_flag;

int PC_jump_to;

static int exit_status = 0;

void vm_dump(int dump_depth){
    printf("vm dump at PC %x:\n", registers[PC]);
    printf("memory dump:\n");
    for(int i = 0; i < dump_depth; i++){
        printf("|%x|", memory_tape[i]);
    }
    printf("\n");
    printf("register dump:\n");
    printf("MEMPTR: %x, PC: %x, GP0: %x, GP1: %x, GP2: %x, RET: %x\n\n",
    registers[MEM_PTR], registers[PC], registers[GP0], registers[GP1],
    registers[GP2], registers[RET]);
}

int load_instruction_tape(){
    FILE *file = fopen("instruction_tape.bin", "rb");

    size_t items_read = fread(instr_tape, sizeof(word32), INSTRUCTION_TAPE_SIZE / sizeof(word32), file);
    if (items_read != INSTRUCTION_TAPE_SIZE / sizeof(word32)) {
        printf("Error reading instruction tape: expected %zu, read %zu elements.\n", 
               INSTRUCTION_TAPE_SIZE / sizeof(word32), items_read);
    }

    fclose(file);
    return items_read == INSTRUCTION_TAPE_SIZE / sizeof(word32) ? 0 : 1;
}

void set_zeroflag(){
    zero_flag = 1;
}

void zero_zeroflag(){
    zero_flag = 0;
}

void checkresult_register(int reg){
    if (registers[reg] == 0x0){
        set_zeroflag();
    }
    else {
        zero_zeroflag();
    }
}

int validate_PC (){
    return registers[PC] >= 0 && registers[PC] <= INSTRUCTION_TAPE_SIZE/ sizeof(word32); 
}

int get_opcode(word32 instr){
    return instr & (0x1f);
}

int get_reg1_arg(word32 instr){
    return (instr>>5) & (0b111);
}

int get_reg2_arg(word32 instr){
    return (instr>>8) & (0b111);
}

int get_reg3_arg(word32 instr){
    return (instr>>11) & 0b111;
}

word32 get_21bit_arg(word32 instr){
    return instr>>11;
}

int instruction_engine(word32 instr){
    int opcode = get_opcode(instr);
    int reg1 = get_reg1_arg(instr);
    int reg2 = get_reg2_arg(instr);
    int reg3 = get_reg3_arg(instr);
    if (reg1 == ZERO){
        exit_status = 1;
        printf("Error attempted to write to zero register\n");
        return 1;
    }
    word32 value;
    switch (opcode){
        case EXIT:
            exit_status = 1;
            break;
        case LW:
            char* byte_ptr = (char*) memory_tape;
            byte_ptr += registers[reg2];
            registers[reg1] = *((word32*)byte_ptr);
            break;
        case SW:
            char* byte_pt = (char*) memory_tape;
            byte_pt += registers[reg2];
            word32* wptr = (word32*) byte_pt; 
            *wptr = registers[reg1];
            break;
        case ADD:
            registers[reg1] = registers[reg2] + registers[reg3];
            break;
        case SUB:
            registers[reg1] = registers[reg2] - registers[reg3];
            break;
        case MUL:
            registers[reg1] = registers[reg2]*registers[reg3];
            break;
        case DIV:
            registers[reg1] = registers[reg2]/registers[reg3];
            break;
        case ADDI:
            registers[reg1] = registers[reg2] + get_21bit_arg(instr);
            break;
        case AND:
            registers[reg1] = registers[reg2]&registers[reg3];
            break;
        case NOT:
            registers[reg1] = ~registers[reg1];
            break;
        case PRNT:
            printf("%x\n", registers[reg1]);
            break;
        case J: 
            registers[PC] = registers[reg1];
            zero_zeroflag();
            return 0;
        case JZ:
            if (zero_flag){
                registers[PC] = registers[reg1];
                zero_zeroflag();
                return 0;
            }
            break;
        default:
            break;
    }

    if (opcode >= ADD && opcode <= NOT){
        checkresult_register(reg1);
    } else {
        zero_zeroflag();
    }
    registers[PC]++;
    return 0;
}

int main(){
    registers[ZERO] = 0;
    load_instruction_tape();


    while(!exit_status){
        if(!validate_PC()) {
            printf("invalid PC %d\n", registers[PC]);
            return 1;
        };
        //printf("PC: %x\n", registers[PC]);
        //vm_dump(10);
        instruction_engine(instr_tape[registers[PC]]);
    }

    printf("\nGraceful shutdown\n");
    return 0;
}