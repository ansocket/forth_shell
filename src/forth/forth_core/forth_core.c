#include "forth_dict.h"
#include "forth_rodata.h"
#include "forth.h"
#include "stddef.h"
#include "vm.h"
#include "stdint.h"
#include "string.h"
#include "stdio.h" /* for sprintf */

extern const size_t plus_program_arr[];

static void words_handler(vm_t* vm)
{
    size_t* def_ptr = (size_t*)*forth_get_variable_data_ptr(vm, forth_search(vm, "FORTH"));
    size_t* runner = (size_t*)*(forth_dict_get_text_ptr(forth_search(vm, "OUTPUT")) + 1);
    char* temp = NULL;
    do {
        char buffer[256];
        temp = forth_dict_get_name(def_ptr);
        sprintf(buffer, "%s ",temp);
        *(--vm->sp) = strlen(buffer);
        *(--vm->sp) = (size_t)buffer;
        ((vm_ops_t)runner)(vm);
        def_ptr = (size_t*)*forth_dict_get_link_addr(def_ptr);
    }while (def_ptr != NULL);
}
static const size_t words_program_arr[] = 
{
    (FORTH_DICT_FLAG_TEXT)
    | ((size_t)'W' << 8)
    | ((size_t)'O' << 16)
    | ((size_t)'R' << 24)
#if __SIZEOF_SIZE_T__ == 8
    | ((size_t)'D' << 32)
    | ((size_t)'S' << 40)
    | ((size_t)'\0' << 48),
#else
     , ((size_t)'D')
     | ((size_t)'S' << 8)
     | ((size_t)'\0' << 16)
     ,
#endif
(size_t)NULL,
    VM_OP(VM_PUSH),
    (size_t)words_handler,
    VM_OP(VM_C_EXEC),
    VM_OP(VM_RET),
};

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
(size_t)words_program_arr,
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

const size_t false_program_arr[] = 
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
    if(vm->sp > (vm->stack_top - 2))
    {
        vm->exceptions_flags |= VM_EXCEPTION_STACK_UNDERFLOW;
        return;
    }
    size_t* jmp_addr = (size_t*)*(vm->rsp++);
    size_t* i_var = forth_dict_get_text_ptr(forth_search(vm, "I"));
    *i_var = *vm->sp;
    if((int)*(vm->sp + 1) <= (int)*vm->sp)
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

/* if START OF DEFINITION */
static void forth_if_immediate_handler(vm_t* vm)
{
    size_t* here = forth_search(vm, "HERE");
    size_t** copy_ptr = (size_t**)forth_get_variable_data_ptr(vm, here);
    *(*copy_ptr)++ = VM_OP(VM_IT);
    *(*copy_ptr)++ = VM_OP(VM_PUSH),
    *(--vm->sp) = (size_t)(*copy_ptr)++;
    *(*copy_ptr)++ = VM_OP(VM_JMP),
    *(*copy_ptr)++ = VM_OP(VM_NONE);
    *(*copy_ptr)++ = VM_OP(VM_NONE);
    *(*copy_ptr)++ = VM_OP(VM_NONE);
}
const size_t if_program_arr[] = 
{
    FORTH_DICT_FLAG_TEXT | FORTH_DICT_FLAG_IMMEDIATE | FORTH_DICT_FLAG_COMPILE_ONLY
    | ((size_t)'I' << 8)
    | ((size_t)'F' << 16)
    | ((size_t)'\0' << 24),
    (size_t)loop_program_arr,
    VM_OP(VM_PUSH),
    (size_t)forth_if_immediate_handler,
    VM_OP(VM_C_EXEC),
    VM_OP(VM_RET)
};
/* if END OF DEFINITION */

/* ELSE START OF DEFINITION */
static void forth_else_immediate_handler(vm_t* vm)
{
    size_t* here = forth_search(vm, "HERE");
    size_t** copy_ptr = (size_t**)forth_get_variable_data_ptr(vm, here);
    size_t* jmp_addr = (size_t*)*vm->sp++;
    *(*copy_ptr)++ = VM_OP(VM_PUSH);
    *(--vm->sp) = (size_t)(*copy_ptr)++;
    *(*copy_ptr)++ = VM_OP(VM_JMP);
    *jmp_addr = (size_t)(*copy_ptr);
}
const size_t else_program_arr[] = 
{
    FORTH_DICT_FLAG_TEXT | FORTH_DICT_FLAG_IMMEDIATE | FORTH_DICT_FLAG_COMPILE_ONLY
    | ((size_t)'E' << 8)
    | ((size_t)'L' << 16)
    | ((size_t)'S' << 24)
#if __SIZEOF_SIZE_T__ == 8
    | ((size_t)'E' << 32)
    | ((size_t)'\0' << 40),
#else
    , ((size_t)'E' << 0)
    | ((size_t)'\0' << 8),
#endif
    (size_t)if_program_arr,
    VM_OP(VM_PUSH),
    (size_t)forth_else_immediate_handler,
    VM_OP(VM_C_EXEC),
    VM_OP(VM_RET)
};


/* THEN START OF DEFINITION */
static void forth_then_immediate_handler(vm_t* vm)
{
    size_t* here = forth_search(vm, "HERE");
    size_t** copy_ptr = (size_t**)forth_get_variable_data_ptr(vm, here);
    *(size_t*)(*vm->sp++) = (size_t)(*copy_ptr);
}
const size_t then_program_arr[] = 
{
    FORTH_DICT_FLAG_TEXT | FORTH_DICT_FLAG_IMMEDIATE | FORTH_DICT_FLAG_COMPILE_ONLY
    | ((size_t)'T' << 8)
    | ((size_t)'H' << 16)
    | ((size_t)'E' << 24)
#if __SIZEOF_SIZE_T__ == 8
    | ((size_t)'N' << 32)
    | ((size_t)'\0' << 40),
#else
    , ((size_t)'N' << 0)
    | ((size_t)'\0' << 8),
#endif
    (size_t)else_program_arr,
    VM_OP(VM_PUSH),
    (size_t)forth_then_immediate_handler,
    VM_OP(VM_C_EXEC),
    VM_OP(VM_RET)
};
/* THEN END OF DEFINITION */

/* THEN START OF DEFINITION */
static void forth_dot_quote_post_handler(vm_t* vm)
{
    int len = *vm->sp++;
    if((len % sizeof(size_t)) > 0)
    {
        len += sizeof(size_t) - (len % sizeof(size_t));
    }
    vm->pc += len/sizeof(size_t);
}
static void forth_dot_quote_immediate_handler(vm_t* vm)
{
    size_t* in = forth_get_variable_data_ptr(vm, forth_search(vm, ">IN"));
    char* str = (char*)(vm->ram + FORTH_STRBUF_OFFSET + *in + 1);
    size_t* state = forth_get_variable_data_ptr(vm, forth_search(vm, "STATE"));
    size_t** copy_ptr = NULL;
    int len = 0;
    if(*state == FORTH_STATE_INTERPRET)
    {
        copy_ptr = (size_t**)forth_get_variable_data_ptr(vm, forth_search(vm, "SANDBOX"));
    }
    else {
        copy_ptr = (size_t**)forth_get_variable_data_ptr(vm, forth_search(vm, "HERE"));
    }
    while(*(str + len) !=  '"')
    {
        len++;
    }
    *(str + len + 1) = '\0';
    *in += len + 2;
    *(*copy_ptr)++ = VM_OP(VM_PUSH);
    *(*copy_ptr)++ = len;
    *(*copy_ptr)++ = VM_OP(VM_PUSH);
    *(*copy_ptr)++ = len;
    *(*copy_ptr)++ = VM_OP(VM_PC);
    *(*copy_ptr)++ = VM_OP(VM_LOAD);
    *(*copy_ptr)++ = VM_OP(VM_PUSH);
    *(*copy_ptr)++ = 10*sizeof(size_t);
    *(*copy_ptr)++ = VM_OP(VM_ADD);
    *(*copy_ptr)++ = VM_OP(VM_PUSH);
    *(*copy_ptr)++ = (size_t)forth_dict_get_text_ptr(forth_search(vm, "OUTPUT"));
    *(*copy_ptr)++ = VM_OP(VM_CALL);
    *(*copy_ptr)++ = VM_OP(VM_PUSH);
    *(*copy_ptr)++ = (size_t)forth_dot_quote_post_handler;
    *(*copy_ptr)++ = VM_OP(VM_C_EXEC);

    memcpy((char*)(*copy_ptr),str,len);
    *(char**)copy_ptr += len;
    if(((size_t)(*copy_ptr) % sizeof(size_t)) > 0)
    {
        *(char**)copy_ptr += sizeof(size_t) - ((size_t)(*copy_ptr) % sizeof(size_t));
    } 
    
}
const size_t dot_quote_program_arr[] = 
{
    FORTH_DICT_FLAG_TEXT | FORTH_DICT_FLAG_IMMEDIATE 
    | ((size_t)'.' << 8)
    | ((size_t)'"' << 16)
    | ((size_t)'\0' << 24),
    (size_t)then_program_arr,
    VM_OP(VM_PUSH),
    (size_t)forth_dot_quote_immediate_handler,
    VM_OP(VM_C_EXEC),
    VM_OP(VM_RET)
};

static void forth_dot_handler(vm_t* vm)
{
    if(vm->sp > (vm->stack_top - 1))
    {
        vm->exceptions_flags |= VM_EXCEPTION_STACK_UNDERFLOW;
        return;
    }
    size_t value = *vm->sp++;
    char buffer[32];
    sprintf(buffer,"%ld ",value);
    /* TODO: BASE */
    *(--vm->sp) = strlen(buffer);
    *(--vm->sp) = (size_t)buffer;
    size_t* runner = (size_t*)*(forth_dict_get_text_ptr(forth_search(vm, "OUTPUT")) + 1);
    ((vm_ops_t)runner)(vm);
}
const size_t dot_program_arr[] = 
{
    FORTH_DICT_FLAG_TEXT
    | ((size_t)'.' << 8)
    | ((size_t)'\0' << 16),
    (size_t)dot_quote_program_arr,
    VM_OP(VM_PUSH),
    (size_t)forth_dot_handler,
    VM_OP(VM_C_EXEC),
    VM_OP(VM_RET)
};