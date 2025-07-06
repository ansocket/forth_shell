#include "forth.h"
#include "stddef.h"
#include "vm.h"
#include "stdint.h"


extern const size_t false_program_arr[];

static void invert_handler(vm_t* vm)
{
    if(vm->sp > (vm->stack_top - 1))
    {
        vm->exceptions_flags |= VM_EXCEPTION_STACK_UNDERFLOW;
        return;
    }
    *vm->sp = ~*vm->sp;
}
const size_t invert_program_arr[] = 
{
    (FORTH_DICT_FLAG_TEXT)
    | ((size_t)'I' << 8)
    | ((size_t)'N' << 16)
    | ((size_t)'V' << 24)
#if __SIZEOF_SIZE_T__ == 8
    | ((size_t)'E' << 32)
    | ((size_t)'R' << 40)
    | ((size_t)'T' << 48)
    | ((size_t)'\0' << 56),
#else
     ,((size_t)'E' << 0)
    | ((size_t)'R' << 8)
    | ((size_t)'T' << 16)
     | ((size_t)'\0' << 24),
#endif
    (size_t)false_program_arr,
    VM_OP(VM_PUSH),
    (size_t)invert_handler,
    VM_OP(VM_C_EXEC),
    VM_OP(VM_RET)
};

static void negate_handler(vm_t* vm)
{
    if(vm->sp > (vm->stack_top - 1))
    {
        vm->exceptions_flags |= VM_EXCEPTION_STACK_UNDERFLOW;
        return;
    }
    *vm->sp = -*vm->sp;
}
const size_t negate_program_arr[] = 
{
    (FORTH_DICT_FLAG_TEXT)
    | ((size_t)'N' << 8)
    | ((size_t)'E' << 16)
    | ((size_t)'G' << 24)
#if __SIZEOF_SIZE_T__ == 8
    | ((size_t)'A' << 32)
    | ((size_t)'T' << 40)
    | ((size_t)'E' << 48)
    | ((size_t)'\0' << 56),
#else
     ,((size_t)'A' << 0)
    | ((size_t)'T' << 8)
    | ((size_t)'E' << 16)
     | ((size_t)'\0' << 24),
#endif
    (size_t)invert_program_arr,
    VM_OP(VM_PUSH),
    (size_t)negate_handler,
    VM_OP(VM_C_EXEC),
    VM_OP(VM_RET)
};

static void dup_if_no_zero_handler(vm_t* vm)
{
    if(vm->sp > (vm->stack_top - 1))
    {
        vm->exceptions_flags |= VM_EXCEPTION_STACK_UNDERFLOW;
        return;
    }
    if(*vm->sp != 0)
    {
        *(vm->sp - 1) = *vm->sp;
        vm->sp--;
    }
}
const size_t dup_if_no_zero_program_arr[] = 
{
    (FORTH_DICT_FLAG_TEXT)
    | ((size_t)'?' << 8)
    | ((size_t)'D' << 16)
    | ((size_t)'U' << 24)
#if __SIZEOF_SIZE_T__ == 8
    | ((size_t)'P' << 32)
    | ((size_t)'\0' << 40),
#else
     , ((size_t)'P')
     | ((size_t)'\0' << 8),
#endif
    (size_t)negate_program_arr,
    VM_OP(VM_PUSH),
    (size_t)dup_if_no_zero_handler,
    VM_OP(VM_C_EXEC),
    VM_OP(VM_RET)
};

const size_t xor_program_arr[] = 
{
    (FORTH_DICT_FLAG_TEXT)
    | ((size_t)'X' << 8)
    | ((size_t)'O' << 16)
    | ((size_t)'R' << 24)
#if __SIZEOF_SIZE_T__ == 8
    | ((size_t)'\0' << 32),
#else
     , ((size_t)'\0'),
#endif
    (size_t)dup_if_no_zero_program_arr,
    VM_OP(VM_XOR),
    VM_OP(VM_RET)
};

static void abs_handler(vm_t* vm)
{
    if(vm->sp > (vm->stack_top - 1))
    {
        vm->exceptions_flags |= VM_EXCEPTION_STACK_UNDERFLOW;
        return;
    }
    if((int)*vm->sp < 0) *vm->sp = -(*(int*)vm->sp);
}

const size_t abs_program_arr[] = 
{
    (FORTH_DICT_FLAG_TEXT)
    | ((size_t)'A' << 8)
    | ((size_t)'B' << 16)
    | ((size_t)'S' << 24)
#if __SIZEOF_SIZE_T__ == 8
    | ((size_t)'\0' << 32),
#else
     , ((size_t)'\0'),
#endif
    (size_t)xor_program_arr,
    VM_OP(VM_PUSH),
    (size_t)abs_handler,
    VM_OP(VM_C_EXEC),
    VM_OP(VM_RET)
};

const size_t mod_program_arr[] = 
{
    (FORTH_DICT_FLAG_TEXT)
    | ((size_t)'M' << 8)
    | ((size_t)'O' << 16)
    | ((size_t)'D' << 24)
#if __SIZEOF_SIZE_T__ == 8
    | ((size_t)'\0' << 32),
#else
     , ((size_t)'\0'),
#endif
    (size_t)abs_program_arr,
    VM_OP(VM_MOD),
    VM_OP(VM_RET)
};

static void rshift_handler(vm_t* vm)
{
    if(vm->sp > (vm->stack_top - 2))
    {
        vm->exceptions_flags |= VM_EXCEPTION_STACK_UNDERFLOW;
        return;
    }
    *(vm->sp + 1) = *(vm->sp + 1) >> *(vm->sp);
    vm->sp++;
}
const size_t rshift_program_arr[] = 
{
    (FORTH_DICT_FLAG_TEXT)
    | ((size_t)'R' << 8)
    | ((size_t)'S' << 16)
    | ((size_t)'H' << 24)
#if __SIZEOF_SIZE_T__ == 8
    | ((size_t)'I' << 32)
    | ((size_t)'F' << 40) 
    | ((size_t)'T' << 48)
    | ((size_t)'\0' << 56),
#else
     , ((size_t)'I')
    | ((size_t)'F' << 8) 
    | ((size_t)'T' << 16)
    | ((size_t)'\0' << 24),
#endif
    (size_t)mod_program_arr,
    VM_OP(VM_PUSH),
    (size_t)rshift_handler,
    VM_OP(VM_C_EXEC),
    VM_OP(VM_RET)
};

static void lshift_handler(vm_t* vm)
{
    if(vm->sp > (vm->stack_top - 2))
    {
        vm->exceptions_flags |= VM_EXCEPTION_STACK_UNDERFLOW;
        return;
    }
    *(vm->sp + 1) = *(vm->sp + 1) << *(vm->sp);
    vm->sp++;
}
const size_t lshift_program_arr[] = 
{
    (FORTH_DICT_FLAG_TEXT)
    | ((size_t)'L' << 8)
    | ((size_t)'S' << 16)
    | ((size_t)'H' << 24)
#if __SIZEOF_SIZE_T__ == 8
    | ((size_t)'I' << 32)
    | ((size_t)'F' << 40) 
    | ((size_t)'T' << 48)
    | ((size_t)'\0' << 56),
#elif __SIZEOF_SIZE_T__ == 4
     , ((size_t)'I')
    | ((size_t)'F' << 8) 
    | ((size_t)'T' << 16)
    | ((size_t)'\0' << 24),
#endif
    (size_t)rshift_program_arr,
    VM_OP(VM_PUSH),
    (size_t)lshift_handler,
    VM_OP(VM_C_EXEC),
    VM_OP(VM_RET)
};

const size_t div_program_arr[] = 
{
    (FORTH_DICT_FLAG_TEXT) | 
    ((size_t)'/' << 8) |
    ((size_t)'\0' << 16),
    (size_t)lshift_program_arr,
    VM_OP(VM_DIV),
    VM_OP(VM_RET)
};

const size_t mul_program_arr[] = 
{
    (FORTH_DICT_FLAG_TEXT) | 
    ((size_t)'*' << 8) |
    ((size_t)'\0' << 16),
    (size_t)div_program_arr,
    VM_OP(VM_MUL),
    VM_OP(VM_RET)
};

const size_t minus_program_arr[] = 
{
    (FORTH_DICT_FLAG_TEXT) | 
    ((size_t)'-' << 8) |
    ((size_t)'\0' << 16),
    (size_t)mul_program_arr,
    VM_OP(VM_SUB),
    VM_OP(VM_RET)
};

const size_t plus_program_arr[] = 
{
    (FORTH_DICT_FLAG_TEXT) | 
    ((size_t)'+' << 8) |
    ((size_t)'\0' << 16),
    (size_t)minus_program_arr,
    VM_OP(VM_ADD),
    VM_OP(VM_RET)
};
