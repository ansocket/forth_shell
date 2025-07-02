#include "forth_dict.h"
#include "forth_rodata.h"
#include "forth.h"
#include "stddef.h"
#include "vm.h"
#include "stdint.h"

static const size_t dup_program_arr[] = 
{
    (FORTH_DICT_FLAG_TEXT)
    | ((size_t)'D' << 8)
    | ((size_t)'U' << 16)
    | ((size_t)'P' << 24)
#if __SIZEOF_SIZE_T__ == 8
    | ((size_t)'\0' << 32),
#else
     , ((size_t)'\0'),
#endif
(size_t)NULL,
    VM_OP(VM_SP),
    VM_OP(VM_LOAD),
    VM_OP(VM_PUSH),
    1*sizeof(size_t),
    VM_OP(VM_ADD),
    VM_OP(VM_LOAD),
    VM_OP(VM_RET),
};

void forth_swap_handler(vm_t* vm)
{
    *vm->sp ^= *(vm->sp + 1);
    *(vm->sp + 1) ^= *vm->sp;
    *vm->sp ^= *(vm->sp + 1);
}
static const size_t swap_program_arr[] = 
{
    (FORTH_DICT_FLAG_TEXT)
    | ((size_t)'S' << 8)
    | ((size_t)'W' << 16)
    | ((size_t)'A' << 24)
#if __SIZEOF_SIZE_T__ == 8
    | ((size_t)'P' << 32)
    | ((size_t)'\0' << 40),
#else
     , ((size_t)'P')
     | ((size_t)'\0' << 8),
#endif
(size_t)dup_program_arr,
    VM_OP(VM_PUSH),
    (size_t)forth_swap_handler,
    VM_OP(VM_C_EXEC),
    VM_OP(VM_RET),
};

static const size_t false_program_arr[] = 
{
    (FORTH_DICT_FLAG_CONSTANT)
    | ((size_t)'F' << 8)
    | ((size_t)'A' << 16)
    | ((size_t)'L' << 24)
#if __SIZEOF_SIZE_T__ == 8
    | ((size_t)'S' << 32)
    | ((size_t)'E' << 40) 
    | ((size_t)'\0' << 48),
#else
     , ((size_t)'S')
    | ((size_t)'E' << 8) 
    | ((size_t)'\0' << 16),
#endif
    
    (size_t)swap_program_arr,
    0,
};

static const size_t plus_program_arr[] = 
{
    (FORTH_DICT_FLAG_TEXT) | 
    ((size_t)'+' << 8) |
    ((size_t)'\0' << 16),
    (size_t)false_program_arr,
    VM_OP(VM_ADD),
    VM_OP(VM_RET)
};
const size_t true_program_arr[] = 
{
    (FORTH_DICT_FLAG_CONSTANT) 
    | ((size_t)'T' << 8)
    | ((size_t)'R' << 16)
    | ((size_t)'U' << 24)
#if __SIZEOF_SIZE_T__ == 8
    | ((size_t)'E' << 32) 
    | ((size_t)'\0' << 40),
#else
    ,((size_t)'E' << 0)
    | ((size_t)'\0' << 8),
#endif
    (size_t)plus_program_arr,
    (size_t)-1,
};
/* : START OF DEFINITION */
static void forth_colon_handler(vm_t* vm)
{
    char* new_token = forth_get_token(vm);
    if(new_token == NULL)
    {
        vm->exceptions_flags |= VM_EXCEPTION_MEMFAULT;
        return;
    }
    size_t* state = forth_search(vm, "STATE");
    if(state == NULL)
    {
        vm->exceptions_flags |= VM_EXCEPTION_MEMFAULT;
        return;
    }
    state = forth_get_variable_data_ptr(vm, state);
    *state = FORTH_STATE_COMPILE;
    forth_add_definition(vm, new_token);
}
const size_t colon_program_arr[] = 
{
    FORTH_DICT_FLAG_TEXT | FORTH_DICT_FLAG_IMMEDIATE |
    ((size_t)':' << 8) |
    ((size_t)'\0' << 16),
    (size_t)true_program_arr,
    VM_OP(VM_PUSH),
    (size_t)forth_colon_handler,
    VM_OP(VM_C_EXEC),
    VM_OP(VM_RET)
};
/* : END OF DEFINITION */

/* ; START OF DEFINITION */
static void forth_semicolon_handler(vm_t* vm)
{
    size_t* state = forth_search(vm, "STATE");
    if(state == NULL)
    {
        vm->exceptions_flags |= VM_EXCEPTION_MEMFAULT;
        return;
    }
    state = forth_get_variable_data_ptr(vm, state);
    *state = FORTH_STATE_INTERPRET;
    size_t* here = forth_search(vm, "HERE");
    size_t** copy_ptr = (size_t**)forth_get_variable_data_ptr(vm, here);
    *(*copy_ptr)++ = VM_OP(VM_RET);
}

const size_t semicolon_program_arr[] = 
{
    FORTH_DICT_FLAG_TEXT | FORTH_DICT_FLAG_IMMEDIATE | FORTH_DICT_FLAG_COMPILE_ONLY |
    ((size_t)';' << 8) |
    ((size_t)'\0' << 16),
    (size_t)colon_program_arr,
    VM_OP(VM_PUSH),
    (size_t)forth_semicolon_handler,
    VM_OP(VM_C_EXEC),
    VM_OP(VM_RET)
};
/* ; END OF DEFINITION */

/* do START OF DEFINITION */
static void forth_do_handler(vm_t* vm)
{
    size_t* jmp_addr = (size_t*)*(vm->rsp++);
    size_t* j_var = forth_dict_get_text_ptr(forth_search(vm, "J"));
    size_t* i_var = forth_dict_get_text_ptr(forth_search(vm, "I"));
    *i_var = *vm->sp;
    if(*(vm->sp + 1) <= *vm->sp)
    {
        vm->sp += 2;
        vm->pc = jmp_addr;
        return;
    }
    *(--vm->rsp) = *vm->sp++;
    *(--vm->rsp) = *vm->sp++;
    *(--vm->rsp) = (size_t)(vm->pc - 3);
}
static void forth_do_immediate_handler(vm_t* vm)
{
    size_t* here = forth_search(vm, "HERE");
    size_t** copy_ptr = (size_t**)forth_get_variable_data_ptr(vm, here);
    *(*copy_ptr)++ = VM_OP(VM_PUSH);
    *(*copy_ptr)++ = 0;
    *(*copy_ptr)++ = VM_OP(VM_RPUSH);
    *(*copy_ptr)++ = VM_OP(VM_PUSH);
    *(*copy_ptr)++ = (size_t)forth_dict_get_text_ptr(forth_search(vm, "I"));
    *(*copy_ptr)++ = VM_OP(VM_LOAD);
    *(*copy_ptr)++ = VM_OP(VM_PUSH);
    *(*copy_ptr)++ = (size_t)forth_dict_get_text_ptr(forth_search(vm, "J"));
    *(*copy_ptr)++ = VM_OP(VM_STR);
    *(*copy_ptr)++ = VM_OP(VM_PUSH);
    *(*copy_ptr)++ = (size_t)forth_do_handler;
    *(*copy_ptr)++ = VM_OP(VM_C_EXEC);
}

const size_t do_program_arr[] = 
{
    FORTH_DICT_FLAG_TEXT | FORTH_DICT_FLAG_IMMEDIATE | FORTH_DICT_FLAG_COMPILE_ONLY |
    ((size_t)'D' << 8) |
    ((size_t)'O' << 16) |
    ((size_t)'\0' << 24),
    (size_t)semicolon_program_arr,
    VM_OP(VM_PUSH),
    (size_t)forth_do_immediate_handler,
    VM_OP(VM_C_EXEC),
    VM_OP(VM_RET)
};
/* do END OF DEFINITION */

/* do START OF DEFINITION */
static void forth_loop_handler(vm_t* vm)
{
    size_t* return_addr = (size_t*)*(vm->rsp++);
    size_t max = *(vm->rsp++);
    size_t i = *(vm->rsp++);
    i++;
    *(--vm->sp) = max;
    *(--vm->sp) = i;
    *(--vm->rsp) = (size_t)(vm->pc);
    vm->pc = return_addr;
}
static void forth_loop_immediate_handler(vm_t* vm)
{
    /* FOR EXAMPLE: "10 3 DO ... LOOP" */
    size_t* here = forth_search(vm, "HERE");
    size_t** copy_ptr = (size_t**)forth_get_variable_data_ptr(vm, here);
    *(*copy_ptr)++ = VM_OP(VM_PUSH);
    *(*copy_ptr)++ = (size_t)forth_loop_handler;
    *(*copy_ptr)++ = VM_OP(VM_C_EXEC);
}

const size_t loop_program_arr[] = 
{
    FORTH_DICT_FLAG_TEXT | FORTH_DICT_FLAG_IMMEDIATE | FORTH_DICT_FLAG_COMPILE_ONLY
    | ((size_t)'L' << 8)
    | ((size_t)'O' << 16)
    | ((size_t)'O' << 24)
#if __SIZEOF_SIZE_T__ == 8
    | ((size_t)'P' << 32)
    | ((size_t)'\0' << 40),
#else
    , ((size_t)'P' << 0)
    | ((size_t)'\0' << 8),
#endif
    (size_t)do_program_arr,
    VM_OP(VM_PUSH),
    (size_t)forth_loop_immediate_handler,
    VM_OP(VM_C_EXEC),
    VM_OP(VM_RET)
};
/* do END OF DEFINITION */