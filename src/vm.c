#include "vm.h"
#include <stdio.h>

static decoded_instruction decoded_instr;

static int load_instruction_tape(vm* vm, int run_mode){
    FILE *file = fopen("instruction_tape.bin", "rb");

    size_t items_read = fread(vm->instr_tape, sizeof(word32), INSTRUCTION_TAPE_SIZE / sizeof(word32), file);
    if (run_mode > RUN_MODE_NORMAL && items_read != INSTRUCTION_TAPE_SIZE / sizeof(word32)) {
        printf("Error reading instruction tape: expected %zu, read %zu elements.\n", 
               INSTRUCTION_TAPE_SIZE / sizeof(word32), items_read);
    }

    fclose(file);
    return items_read == INSTRUCTION_TAPE_SIZE / sizeof(word32) ? 0 : 1;
}

static void set_zeroflag(vm *vm){
    vm->zero_flag = 1;
}

static void zero_zeroflag(vm *vm){
    vm->zero_flag = 0;
}

static void checkresult_register(vm *vm, int reg){
    if (vm->registers[reg] == 0x0){
        set_zeroflag(vm);
    }
    else {
        zero_zeroflag(vm);
    }
}

int validate_PC (vm *vm){
    return vm->registers[PC] >= 0 && vm->registers[PC] <= INSTRUCTION_TAPE_SIZE/ sizeof(word32); 
}

static int get_opcode(word32 instr){
    return instr & (0xf);
}

static int get_reg1_arg(word32 instr){
    return (instr>>4) & (0b1111);
}

static int get_reg2_arg(word32 instr){
    return (instr>>8) & (0b1111);
}

static int get_reg3_arg(word32 instr){
    return (instr>>12) & 0b1111;
}

static word32 get_16bit_arg(word32 instr){
    return instr>>16;
}

static void decode_instruction(word32 tape_instruction,decoded_instruction* decode_struct){
    decode_struct->opcode = get_opcode(tape_instruction);
    decode_struct->reg1 = get_reg1_arg(tape_instruction);
    decode_struct->reg2 = get_reg2_arg(tape_instruction);
    decode_struct->reg3 = get_reg3_arg(tape_instruction);
    decode_struct->arg = get_16bit_arg(tape_instruction);
}

static word32 tripple_instruction_exe(word32 value0, word32 value1, int opcode){
    switch(opcode){
        case ADD:
            return value0 + value1;
        case SUB:
            return value0 - value1;
        case MUL:
            return value0 * value1;
        case DIV:
            return value0 / value1;
        case NAND:
            return ~(value0 & value1);
        case SLT:
            return value0 < value1 ? 1 : 0;
    }
}

static int instruction_engine(vm *vm){
    decoded_instruction* instr = &decoded_instr;
    decode_instruction(vm->instr_tape[vm->registers[PC]],instr);
    if (instr->opcode == EXIT){
        vm->exit_status = 1;
        return 0;
    }
    if (instr->reg1 == ZERO){
        printf("Error attempted to write to zero register\n");
        return 1;
    }
    switch(instr->opcode){
        case LW:
            char* byte_ptr = (char*) vm->memory_tape;
            byte_ptr += vm->registers[instr->reg2];
            vm->registers[instr->reg1] = *((word32*) byte_ptr);
            zero_zeroflag(vm);
            break;
        case SW:
            byte_ptr = (char*) vm->memory_tape;
            byte_ptr += vm->registers[instr->reg2];
            *((word32*) byte_ptr) = vm->registers[instr->reg1];
            zero_zeroflag(vm);
            break;
        case LWOPSW:
            if(instr->arg >= ADD && instr->arg <= SLT && instr->arg != ADDI){
                word32 mem_value1 = vm->memory_tape[vm->registers[instr->reg2]];
                word32 mem_value2 = vm->memory_tape[vm->registers[instr->reg3]];
                word32 result = tripple_instruction_exe(mem_value1,mem_value2,instr->arg);
                vm->memory_tape[vm->registers[instr->reg1]] = result;
                zero_zeroflag(vm);
            } else {
                printf("Error invalid instruction\n");
                return 1;
            }
            break;
        case ADD: case SUB: case MUL: case DIV: case NAND: case SLT:
            vm->registers[instr->reg1] = tripple_instruction_exe(vm->registers[instr->reg2],vm->registers[instr->reg3],instr->opcode);
            checkresult_register(vm,instr->reg1);
            break;
        case ADDI:
            vm->registers[instr->reg1] = vm->registers[instr->reg2] + instr->arg;
            checkresult_register(vm,instr->reg1);
            break;
        case BEQ:
            if (vm->registers[instr->reg1] == vm->registers[instr->reg2]){
                vm->registers[PC] = instr->arg;
                zero_zeroflag(vm);
                return 0;
            }
            break;
        case JZ:
            if (vm->zero_flag){
                vm->registers[PC] = vm->registers[instr->reg1];
                zero_zeroflag(vm);
                return 0;
            }
            break;
        case PUTC:
            char* cptr = (char*) &vm->registers[instr->reg1];
            int offset = instr->reg2;
            if (offset > 3){
                printf("Error invalid offset\n");
                return 1;
            }
            printf("%c",(char) *(cptr + offset));
            zero_zeroflag(vm);
            break;
        case GETC:
            char stdin_input;
            scanf("%c",&stdin_input);
            char* cptr2 = (char*) &vm->registers[instr->reg1];
            unsigned int offset2 = instr->reg2;
            if (offset2 > 3){
                printf("Error invalid offset\n");
                return 1;
            }
            *(cptr2 + offset2) = stdin_input;
            zero_zeroflag(vm);
            break;
        case PRNT:
            char* cptr3 = (char*) &vm->registers[instr->reg1];
            cptr += instr->reg2;
            if(instr->reg2 > 3){
                printf("Error invalid offset\n");
                return 1;
            }
            printf("%x\n",*((word32*) cptr3));
            zero_zeroflag(vm);
            break;
        default:
            printf("Error invalid instruction with opcode: %x \n", instr->opcode);
            return 1;
    }
    vm->registers[PC]++;
    return 0;
}

static int run_normal(vm *vm){
    while (validate_PC(vm) && !vm->exit_status){
        if (instruction_engine(vm)){
            return 1;
        }
    }
    return 0;
}
static int run_debug(vm *vm, int mem_debug_depth){
    while(validate_PC(vm) && !vm->exit_status){
        if (instruction_engine(vm)){
            return 1;
        }
        printf("PC: %d\n",vm->registers[PC]);
        printf("Zero flag: %d\n",vm->zero_flag);
        printf("Exit status: %d\n",vm->exit_status);
        printf("Registers:\n");
        for (int i = 0; i < NR_REGISTERS; i++){
            printf("[R%d]: %x, ",i,vm->registers[i]);
        }
        printf("\n");
        printf("Memory tape:\n");
        for (int i = 0; i < mem_debug_depth; i++){
            printf("[M%d: %x]",i,vm->memory_tape[i]);
        }
        printf("\n");
        printf("Instruction tape:\n");
        for (int i = 0; i < mem_debug_depth; i++){
            printf("[I%d]: %x, ",i,vm->instr_tape[i]);
        }
        printf("\n");
    }
    return 0;
}

static int run_step(vm *vm, int mem_debug_depth){
    int step = 0;
    while(validate_PC(vm) && !vm->exit_status){
        if (instruction_engine(vm)){
            return 1;
        }
        printf("PC: %d\n",vm->registers[PC]);
        printf("Zero flag: %d\n",vm->zero_flag);
        printf("Exit status: %d\n",vm->exit_status);
        printf("Registers:\n");
        for (int i = 0; i < NR_REGISTERS; i++){
            printf("[R%d]: %x, ",i,vm->registers[i]);
        }
        printf("\n");
        printf("Memory tape:\n");
        for (int i = 0; i < mem_debug_depth; i++){
            printf("[M%d: %x]",i,vm->memory_tape[i]);
        }
        printf("\n");
        printf("Instruction tape:\n");
        for (int i = 0; i < mem_debug_depth; i++){
            printf("[I%d]: %x, ",i,vm->instr_tape[i]);
        }
        printf("\n");
        printf("Step count: %d, type enter to continue\n",step);
        getchar();
    }
    return 1;
}

int run_vm(vm *vm, int RUN_MODE){
    load_instruction_tape(vm, RUN_MODE);
    switch (RUN_MODE){
        case RUN_MODE_NORMAL:
            return run_normal(vm);
        case RUN_MODE_DEBUG:
            return run_debug(vm, 20);
            return 1;
        case RUN_MODE_STEP:
            run_step(vm, 20);
            return 1;
        default:
            printf("Error invalid run mode\n");
            return 1;
    }
}

