#ifndef FORTH_H
#define FORTH_H
#ifdef __cplusplus
extern "C" {
#endif

#include "vm.h"
#include "stdint.h"

#define FORTH_STRBUF_SIZE       256
#define FORTH_SANDBOX_SIZE        1024

#define FORTH_STRBUF_OFFSET     0
#define FORTH_SANDBOX_OFFSET    (FORTH_STRBUF_SIZE)
#define FORTH_DICT_OFFSET   (FORTH_SANDBOX_OFFSET + FORTH_SANDBOX_SIZE) 


#define FORTH_DICT_FLAG_COMPILE_ONLY    (1 << 0)
#define FORTH_DICT_FLAG_IMMEDIATE       (1 << 1)
#define FORTH_DICT_FLAG_TEXT            (0 << 2)
#define FORTH_DICT_FLAG_VARIABLE        (1 << 2)
#define FORTH_DICT_FLAG_CONSTANT        (2 << 2)

#define FORTH_STATE_INTERPRET       0
#define FORTH_STATE_COMPILE         -1

typedef enum
{
    FORTH_ERR_OK,
    FORTH_ERR_ERR,
    FORTH_COMPILE_ONLY_ERROR,
}forth_error_t;


size_t* forth_search(vm_t* vm, const char* name);
forth_error_t forth_init(vm_t* vm);
size_t forth_get_constant_data(vm_t* vm,size_t* addr);
size_t* forth_get_variable_data_ptr(vm_t* vm,size_t* addr);
size_t* forth_add_variable(vm_t* vm, const char* name, size_t value);
size_t* forth_add_constant(vm_t* vm, const char* name, size_t value);
char* forth_get_token(vm_t* vm);
forth_error_t forth_compile(vm_t* vm, char* token);
size_t* forth_add_definition(vm_t* vm, const char* name);
forth_error_t forth_vm_reload(vm_t* vm);
forth_error_t forth_start_compiling(vm_t* vm);
#ifdef __cplusplus
}
#endif
#endif