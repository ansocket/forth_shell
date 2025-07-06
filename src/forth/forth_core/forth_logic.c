#include "forth_dict.h"
#include "forth_rodata.h"
#include "forth.h"
#include "stddef.h"
#include "vm.h"
#include "stdint.h"
#include "string.h"
#include "stdio.h" /* for sprintf */


extern const size_t plus_program_arr[];

static void max_handler(vm_t* vm)
{
    if(vm->sp > (vm->stack_top - 2))
    {
        vm->exceptions_flags |= VM_EXCEPTION_STACK_UNDERFLOW;
        return;
    }
    *(vm->sp + 1) = ((int)*(vm->sp + 1) > (int)*(vm->sp)) ? *(vm->sp + 1) : *(vm->sp);
    vm->sp++;
}
const size_t max_program_arr[] = 
{
    (FORTH_DICT_FLAG_TEXT) 
    | ((size_t)'M' << 8)
    | ((size_t)'A' << 16)
    | ((size_t)'X' << 24)
#if __SIZEOF_SIZE_T__ == 8
    | ((size_t)'\0' << 32),
#else
     , ((size_t)'\0'),
#endif
    (size_t)plus_program_arr,
    VM_OP(VM_PUSH),
    (size_t)max_handler,
    VM_OP(VM_C_EXEC),
    VM_OP(VM_RET)
};


static void min_handler(vm_t* vm)
{
    if(vm->sp > (vm->stack_top - 2))
    {
        vm->exceptions_flags |= VM_EXCEPTION_STACK_UNDERFLOW;
        return;
    }
    *(vm->sp + 1) = ((int)*(vm->sp + 1) < (int)*(vm->sp)) ? *(vm->sp + 1) : *(vm->sp);
    vm->sp++;
}
const size_t min_program_arr[] = 
{
    (FORTH_DICT_FLAG_TEXT) 
    | ((size_t)'M' << 8)
    | ((size_t)'I' << 16)
    | ((size_t)'N' << 24)
#if __SIZEOF_SIZE_T__ == 8
    | ((size_t)'\0' << 32),
#else
     , ((size_t)'\0'),
#endif
    (size_t)max_program_arr,
    VM_OP(VM_PUSH),
    (size_t)min_handler,
    VM_OP(VM_C_EXEC),
    VM_OP(VM_RET)
};

static void equal_handler(vm_t* vm)
{
    if(vm->sp > (vm->stack_top - 2))
    {
        vm->exceptions_flags |= VM_EXCEPTION_STACK_UNDERFLOW;
        return;
    }
    *(vm->sp + 1) = (*(vm->sp + 1) == *(vm->sp)) ? VM_TRUE : VM_FALSE;
    vm->sp++;
}
const size_t equal_program_arr[] = 
{
    (FORTH_DICT_FLAG_TEXT) 
    | ((size_t)'=' << 8)
    | ((size_t)'\0' << 16),
    (size_t)min_program_arr,
    VM_OP(VM_PUSH),
    (size_t)equal_handler,
    VM_OP(VM_C_EXEC),
    VM_OP(VM_RET)
};

const size_t or_program_arr[] = 
{
    (FORTH_DICT_FLAG_TEXT) 
    | ((size_t)'O' << 8)
    | ((size_t)'R' << 16)
    | ((size_t)'\0' << 24),
    (size_t)equal_program_arr,
    VM_OP(VM_OR),
    VM_OP(VM_RET)
};

const size_t and_program_arr[] = 
{
    (FORTH_DICT_FLAG_TEXT) 
    | ((size_t)'A' << 8)
    | ((size_t)'N' << 16)
    | ((size_t)'D' << 24)
#if __SIZEOF_SIZE_T__ == 8
    | ((size_t)'\0' << 32),
#else
     , ((size_t)'\0'),
#endif
    (size_t)or_program_arr,
    VM_OP(VM_AND),
    VM_OP(VM_RET)
};

const size_t greater_program_arr[] = 
{
    (FORTH_DICT_FLAG_TEXT) | 
    ((size_t)'>' << 8) |
    ((size_t)'\0' << 16),
    (size_t)and_program_arr,
    VM_OP(VM_GREATER),
    VM_OP(VM_RET)
};
const size_t less_program_arr[] = 
{
    (FORTH_DICT_FLAG_TEXT) | 
    ((size_t)'<' << 8) |
    ((size_t)'\0' << 16),
    (size_t)greater_program_arr,
    VM_OP(VM_LESS),
    VM_OP(VM_RET)
};

