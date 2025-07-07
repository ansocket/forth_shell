#include "vm.h"

static const vm_ops_t ops[];

int vm_init(vm_t* vm, uint8_t* ram, uint32_t ram_size, size_t* stack, uint32_t stack_size, size_t* rstack, uint32_t rstack_size)
{
    vm->ram_start = ram;
    vm->ram = vm->ram_start;
    vm->rstack_ptr = rstack;
    vm->stack_ptr = stack;
    vm->ram_size = ram_size;
    vm->stack_top = vm->stack_ptr + stack_size;
    vm->rstack_top = vm->rstack_ptr + rstack_size;
    vm->sp = vm->stack_top;
    vm->rsp = vm->rstack_top;
    vm->exceptions_flags = 0;
    vm->trace_cb = NULL;
    return 0;
}

int vm_start(vm_t* vm, size_t* addr)
{
    vm->pc = addr;
    while(1)
    {
        if(vm->exceptions_flags != 0)
        {
            vm->rsp = vm->rstack_top;
            if((vm->exceptions_flags & VM_EXCEPTION_BYE) == VM_EXCEPTION_BYE)
            {
                return VM_EXCEPTION_BYE;
            }
            else if((vm->exceptions_flags & VM_EXCEPTION_STACK_UNDERFLOW) == VM_EXCEPTION_STACK_UNDERFLOW)
            {
                return VM_EXCEPTION_STACK_UNDERFLOW;
            }
            else if((vm->exceptions_flags & VM_EXCEPTION_RSTACK_UNDERFLOW) == VM_EXCEPTION_RSTACK_UNDERFLOW)
            {
                return VM_EXCEPTION_RSTACK_UNDERFLOW;
            }
            else if((vm->exceptions_flags & VM_EXCEPTION_ZERO_DIVISION) == VM_EXCEPTION_ZERO_DIVISION)
            {
                return VM_EXCEPTION_ZERO_DIVISION;
            }
            else if((vm->exceptions_flags & VM_EXCEPTION_MEMFAULT) == VM_EXCEPTION_MEMFAULT)
            {
                return VM_EXCEPTION_MEMFAULT;
            }
        }
        ops[*vm->pc](vm);
        if(vm->trace_cb != NULL)
        {
            vm->trace_cb(vm);
        }
        vm->pc++;
    }

}
void vm_set_trace_cb(vm_t* vm, vm_ops_t trace_cb)
{
    if(vm != NULL)
        vm->trace_cb = trace_cb;
}
void vm_none(vm_t* vm)
{

}
void vm_push(vm_t* vm)
{
    *(--vm->sp) = *(++vm->pc);
}
void vm_ret(vm_t* vm)
{
    if(vm->rsp >= (vm->rstack_top))
    {
        vm->exceptions_flags |= VM_EXCEPTION_BYE;
        return;
    }
    vm->pc = ((size_t*)*vm->rsp++);
}
void vm_call(vm_t* vm)
{
    if(vm->sp > (vm->stack_top - 1))
    {
        vm->exceptions_flags |= VM_EXCEPTION_STACK_UNDERFLOW;
        return;
    }
    size_t* program = (size_t*)*vm->sp++;
    *(--vm->rsp) = (size_t)vm->pc;
    vm->pc = program - 1;
}
void vm_c_exec(vm_t* vm)
{
    if(vm->sp > (vm->stack_top - 1))
    {
        vm->exceptions_flags |= VM_EXCEPTION_STACK_UNDERFLOW;
        return;
    }
    vm_ops_t op = (vm_ops_t)*(vm->sp++);
    if(op == NULL)
    {
        vm->exceptions_flags |= VM_EXCEPTION_MEMFAULT;
        return;
    }
    op(vm);
}
void vm_jmp(vm_t* vm)
{
    if(vm->sp > (vm->stack_top - 1))
    {
        vm->exceptions_flags |= VM_EXCEPTION_STACK_UNDERFLOW;
        return;
    }
    vm->pc = (size_t*)*(vm->sp++) - 1;
}
void vm_pc(vm_t* vm)
{
    *(--vm->sp) = (size_t)&vm->pc;
}
void vm_sp(vm_t* vm)
{
    vm->sp -= 1;
    *(vm->sp) = (size_t)&vm->sp;
}
void vm_add(vm_t* vm)
{
    if(vm->sp > (vm->stack_top - 2))
    {
        vm->exceptions_flags |= VM_EXCEPTION_STACK_UNDERFLOW;
        return;
    }
    *(vm->sp + 1) = *(vm->sp + 1) + *(vm->sp);
    vm->sp++;
}
void vm_sub(vm_t* vm)
{
    if(vm->sp > (vm->stack_top - 2))
    {
        vm->exceptions_flags |= VM_EXCEPTION_STACK_UNDERFLOW;
        return;
    }
    *(vm->sp + 1) = *(vm->sp + 1) - *(vm->sp);
    vm->sp++;
}
void vm_mul(vm_t* vm)
{
    if(vm->sp > (vm->stack_top - 2))
    {
        vm->exceptions_flags |= VM_EXCEPTION_STACK_UNDERFLOW;
        return;
    }
    *(vm->sp + 1) = *(vm->sp + 1) * *(vm->sp);
    vm->sp++;
}
void vm_div(vm_t* vm)
{
    if(vm->sp > (vm->stack_top - 2))
    {
        vm->exceptions_flags |= VM_EXCEPTION_STACK_UNDERFLOW;
        return;
    }
    if(*(vm->sp) == 0)
    {
        vm->exceptions_flags |= VM_EXCEPTION_ZERO_DIVISION;
        return;
    }
    *(vm->sp + 1) = *(vm->sp + 1) / *(vm->sp);
    vm->sp++;
}
void vm_mod(vm_t* vm)
{
    if(vm->sp > (vm->stack_top - 2))
    {
        vm->exceptions_flags |= VM_EXCEPTION_STACK_UNDERFLOW;
        return;
    }
    if(*(vm->sp) == 0)
    {
        vm->exceptions_flags |= VM_EXCEPTION_ZERO_DIVISION;
        return;
    }
    *(vm->sp + 1) = *(vm->sp + 1) % *(vm->sp);
    vm->sp++;
}
void vm_and(vm_t* vm)
{
    if(vm->sp > (vm->stack_top - 2))
    {
        vm->exceptions_flags |= VM_EXCEPTION_STACK_UNDERFLOW;
        return;
    }
    *(vm->sp + 1) = (*(vm->sp + 1) && *(vm->sp)) ? VM_TRUE : VM_FALSE;
    vm->sp++;
}
void vm_or(vm_t* vm)
{
    if(vm->sp > (vm->stack_top - 2))
    {
        vm->exceptions_flags |= VM_EXCEPTION_STACK_UNDERFLOW;
        return;
    }
    *(vm->sp + 1) = (*(vm->sp + 1) || *(vm->sp)) ? VM_TRUE : VM_FALSE;
    vm->sp++;
}
void vm_xor(vm_t* vm)
{
    if(vm->sp > (vm->stack_top - 2))
    {
        vm->exceptions_flags |= VM_EXCEPTION_STACK_UNDERFLOW;
        return;
    }
    *(vm->sp + 1) = *(vm->sp + 1) ^ *(vm->sp);
    vm->sp++;
}
void vm_load(vm_t* vm)
{
    if(vm->sp > (vm->stack_top - 1))
    {
        vm->exceptions_flags |= VM_EXCEPTION_STACK_UNDERFLOW;
        return;
    }
    *(vm->sp) = *(size_t*)*(vm->sp);
}
void vm_less(vm_t* vm)
{
    if(vm->sp > (vm->stack_top - 2))
    {
        vm->exceptions_flags |= VM_EXCEPTION_STACK_UNDERFLOW;
        return;
    }
    *(vm->sp + 1) = ((int)*(vm->sp + 1) < (int)*(vm->sp)) ? VM_TRUE : VM_FALSE;
    vm->sp++;
}
void vm_greater(vm_t* vm)
{
    if(vm->sp > (vm->stack_top - 2))
    {
        vm->exceptions_flags |= VM_EXCEPTION_STACK_UNDERFLOW;
        return;
    }
    *(vm->sp + 1) = ((int)*(vm->sp + 1) > (int)*(vm->sp)) ? VM_TRUE : VM_FALSE;
    vm->sp++;
}
void vm_str(vm_t* vm)
{
    if(vm->sp > (vm->stack_top - 2))
    {
        vm->exceptions_flags |= VM_EXCEPTION_STACK_UNDERFLOW;
        return;
    }
    *(size_t*)*(vm->sp) = *(vm->sp + 1);
    vm->sp += 2;
}
void vm_it(vm_t* vm)
{
    if(vm->sp > (vm->stack_top - 1))
    {
        vm->exceptions_flags |= VM_EXCEPTION_STACK_UNDERFLOW;
        return;
    }
    if(*(vm->sp++) != VM_FALSE)
    {
        vm->pc += 6;
    }
}
void vm_drop(vm_t* vm)
{
    if(vm->sp > (vm->stack_top - 1))
    {
        vm->exceptions_flags |= VM_EXCEPTION_STACK_UNDERFLOW;
        return;
    }
    vm->sp++;
}
void vm_rdrop(vm_t* vm)
{
    if(vm->rsp > (vm->rstack_top - 1))
    {
        vm->exceptions_flags |= VM_EXCEPTION_RSTACK_UNDERFLOW;
        return;
    }
    vm->rsp++;
}
void vm_swap(vm_t* vm)
{
    if(vm->sp > (vm->stack_top - 2))
    {
        vm->exceptions_flags |= VM_EXCEPTION_STACK_UNDERFLOW;
        return;
    }
    *vm->sp ^= *(vm->sp + 1);
    *(vm->sp + 1) ^= *vm->sp;
    *vm->sp ^= *(vm->sp + 1);
}
void vm_rpush(vm_t* vm)
{
    if(vm->sp > (vm->stack_top - 1))
    {
        vm->exceptions_flags |= VM_EXCEPTION_STACK_UNDERFLOW;
        return;
    }
    *(--vm->rsp) = *vm->sp++;
}
void vm_rpop(vm_t* vm)
{
    if(vm->rsp > (vm->rstack_top - 1))
    {
        vm->exceptions_flags |= VM_EXCEPTION_RSTACK_UNDERFLOW;
        return;
    }
    *(--vm->sp) = *vm->rsp++;
}
static const vm_ops_t ops[] = 
{
    [VM_NONE] = vm_none,
    [VM_PUSH] = vm_push,
    [VM_RET] = vm_ret,
    [VM_CALL] = vm_call,
    [VM_LOAD] = vm_load,
    [VM_STR] = vm_str,
    [VM_C_EXEC] = vm_c_exec,
    [VM_JMP] = vm_jmp,
    [VM_RPUSH] = vm_rpush,
    [VM_DROP] = vm_drop,
    [VM_RPOP] = vm_rpop,
    [VM_RDROP] = vm_rdrop,
    [VM_PC] = vm_pc,
    [VM_SP] = vm_sp,
    [VM_ADD] = vm_add,
    [VM_SUB] = vm_sub,
    [VM_MUL] = vm_mul,
    [VM_DIV] = vm_div,
    [VM_MOD] = vm_mod,
    [VM_AND] = vm_and,
    [VM_OR] = vm_or,
    [VM_XOR] = vm_xor,
    [VM_LESS] = vm_less,
    [VM_GREATER] = vm_greater,
    [VM_IT] = vm_it,
    [VM_SWAP] = vm_swap
};