#ifndef FORTH_INTER_H
#define FORTH_INTER_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stddef.h"
#include "vm.h"
#include "forth.h"

char* forth_inter_token(char** buf);
forth_error_t forth_interpreter_init(vm_t* vm, vm_ops_t key, vm_ops_t emit);
forth_error_t forth_interpreter_process(vm_t* vm);
#ifdef __cplusplus
}
#endif
#endif