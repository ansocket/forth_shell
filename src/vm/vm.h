#ifndef VM_H
#define VM_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stddef.h"
#include "stdint.h"

#define VM_EXCEPTION_ZERO_DIVISION          (1 << 0)
#define VM_EXCEPTION_MEMFAULT               (1 << 1)
#define VM_EXCEPTION_STACK_UNDERFLOW        (1 << 2)
#define VM_EXCEPTION_RSTACK_UNDERFLOW       (1 << 3)
#define VM_EXCEPTION_BYE                    (1 << 4)
typedef struct vm vm_t;
typedef void (*vm_ops_t)(vm_t* vm);

typedef struct vm
{
    uint8_t* ram;
    size_t* sp;
    size_t* rsp;
    size_t* pc;

    uint8_t* ram_start;
    size_t* stack_ptr;
    size_t* rstack_ptr;

    uint32_t ram_size;
    size_t* stack_top;
    size_t* rstack_top;
    uint32_t exceptions_flags;
    vm_ops_t trace_cb;
}vm_t;

#define VM_TRUE -1
#define VM_FALSE 0


int vm_init(vm_t* vm, uint8_t* ram, uint32_t ram_size, size_t* stack, uint32_t stack_size, size_t* rstack, uint32_t rstack_size);
int vm_start(vm_t* vm, size_t* addr);
void vm_set_trace_cb(vm_t* vm, vm_ops_t trace_cb);
#define VM_OP(op) op
enum
{
    VM_NONE,
    VM_PUSH,
    VM_C_EXEC,
    VM_RET,
    VM_CALL,
    VM_LOAD,
    VM_STR,
    VM_JMP,
    VM_RPUSH,
    VM_DROP,
    VM_RPOP,
    VM_RDROP,
    VM_PC,
    VM_SP,
    VM_RSP,
    VM_ADD,
    VM_SUB,
    VM_MUL,
    VM_DIV,
    VM_MOD,
    VM_AND,
    VM_OR,
    VM_XOR,
    VM_LESS,
    VM_GREATER,
    VM_IT,
    VM_SWAP,
    VM_LOADBYTE,
    VM_QUIT,
    VM_STRBYTE,
    VM_BYE
};

#ifdef __cplusplus
}
#endif
#endif