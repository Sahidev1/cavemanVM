#include "vm.h"
#include <stdio.h>

vm myVM;


int main(int argc, char **argv){
    if (argc < 2){
        run_vm(&myVM, RUN_MODE_NORMAL);
    }
    else if (argc == 2){
        if (argv[1][0] == 'd'){
            run_vm(&myVM, RUN_MODE_DEBUG);
        }
        else if (argv[1][0] == 's'){
            run_vm(&myVM, RUN_MODE_STEP);
        }
        else{
            printf("Invalid run mode\n");
            return 1;
        }
    }

    return 0;
}