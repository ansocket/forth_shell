#include <cstddef>
#include <gtest/gtest.h>

#include "stdio.h"
#include "vm.h"

extern "C" void vm_trace(vm_t* vm)
{
    size_t* temp = vm->sp;
    while(temp < vm->stack_top)
    {
        printf("%ld ",*(size_t*)temp);
        temp++;
    }
  printf("\n");

}
/* BASIC TEST START */

static size_t basic_test_program[] = 
{
    VM_OP(VM_PUSH),
    10,
    VM_OP(VM_RET),
};
TEST(basic, vm_test_group) {
    vm_t* vm = (vm_t*)malloc(sizeof(vm_t));
    uint8_t* ram = (uint8_t*)malloc(8192);
    size_t* stack = (size_t*)malloc(256);
    size_t* rstack = (size_t*)malloc(256);
    vm_init(vm,ram,8192,stack,256/sizeof(size_t),rstack,256/sizeof(size_t));
    //vm_set_trace_cb(vm, vm_trace);
    vm_start(vm, basic_test_program);
    ASSERT_EQ(*vm->sp++,10);
    ASSERT_EQ(vm->sp,vm->stack_top);
    free(vm);
    free(ram);
    free(stack);
    free(rstack);
}
/* BASIC TEST END */

static void test_func(vm_t* vm)
{
    *(--vm->sp) = 10;
}
static size_t c_test_program[] = 
{
    VM_OP(VM_PUSH),
    (size_t)test_func,
    VM_OP(VM_C_EXEC),
    VM_OP(VM_RET),
};


TEST(c_exec, vm_test_group) {
    vm_t* vm = (vm_t*)malloc(sizeof(vm_t));
    uint8_t* ram = (uint8_t*)malloc(8192);
    size_t* stack = (size_t*)malloc(256);
    size_t* rstack = (size_t*)malloc(256);
    vm_init(vm,ram,8192,stack,256/sizeof(size_t),rstack,256/sizeof(size_t));
    //vm_set_trace_cb(vm, vm_trace);
    vm_start(vm, c_test_program);
    ASSERT_EQ(*vm->sp++,10);
    ASSERT_EQ(vm->sp,vm->stack_top);
    free(vm);
    free(ram);
    free(stack);
    free(rstack);
}

static size_t call_test_2[] = 
{
    VM_OP(VM_PUSH),
    3,
    VM_OP(VM_PUSH),
    4,
    VM_OP(VM_RET),
};
static size_t call_test_1[] = 
{
    VM_OP(VM_PUSH),
    1,
    VM_OP(VM_PUSH),
    2,
    VM_OP(VM_PUSH),
    (size_t)call_test_2,
    VM_OP(VM_CALL),
    VM_OP(VM_PUSH),
    5,
    VM_OP(VM_PUSH),
    6,
    VM_OP(VM_RET),
};
TEST(call_func, vm_test_group) {
    vm_t* vm = (vm_t*)malloc(sizeof(vm_t));
    uint8_t* ram = (uint8_t*)malloc(8192);
    size_t* stack = (size_t*)malloc(256*sizeof(size_t));
    size_t* rstack = (size_t*)malloc(256);
    vm_init(vm,ram,8192,stack,256/sizeof(size_t),rstack,256/sizeof(size_t));
    //vm_set_trace_cb(vm, vm_trace);
    vm_start(vm, call_test_1);
    for(int i = 6; i > 0; i--)
    {
        ASSERT_EQ(*vm->sp++,i);
    }
    ASSERT_EQ(vm->sp,vm->stack_top);
    free(vm);
    free(ram);
    free(stack);
    free(rstack);
}

static size_t add_test_program[] = 
{
    VM_OP(VM_PUSH),
    1,
    VM_OP(VM_PUSH),
    (size_t)-2,
    VM_OP(VM_ADD),
    VM_OP(VM_RET),
};
TEST(add_test, vm_test_group) {
    vm_t* vm = (vm_t*)malloc(sizeof(vm_t));
    uint8_t* ram = (uint8_t*)malloc(8192);
    size_t* stack = (size_t*)malloc(256*sizeof(size_t));
    size_t* rstack = (size_t*)malloc(256);
    vm_init(vm,ram,8192,stack,256/sizeof(size_t),rstack,256/sizeof(size_t));
    //vm_set_trace_cb(vm, vm_trace);
    vm_start(vm, add_test_program);
    ASSERT_EQ(*vm->sp++,-1);
    ASSERT_EQ(vm->sp,vm->stack_top);
    free(vm);
    free(ram);
    free(stack);
    free(rstack);
}

static size_t jmp_test_program[] = 
{
    VM_OP(VM_PUSH),
    1,
    VM_OP(VM_PC),
    VM_OP(VM_LOAD),
    VM_OP(VM_PUSH),
    12*sizeof(size_t),
    VM_OP(VM_ADD),
    VM_OP(VM_JMP),
    3,
    4,
    55,
    6,
    7,
    8,
    9,
    VM_OP(VM_PUSH),
    2,
    VM_OP(VM_RET),
};
TEST(jmp_test, vm_test_group) {
    vm_t* vm = (vm_t*)malloc(sizeof(vm_t));
    uint8_t* ram = (uint8_t*)malloc(8192);
    size_t* stack = (size_t*)malloc(256*sizeof(size_t));
    size_t* rstack = (size_t*)malloc(256);
    vm_init(vm,ram,8192,stack,256/sizeof(size_t),rstack,256/sizeof(size_t));
    //vm_set_trace_cb(vm, vm_trace);
    vm_start(vm, jmp_test_program);
    ASSERT_EQ(*vm->sp++,2);
    ASSERT_EQ(*vm->sp++,1);
    ASSERT_EQ(vm->sp,vm->stack_top);
    free(vm);
    free(ram);
    free(stack);
    free(rstack);
}

static size_t str_test_program[] = 
{
    VM_OP(VM_PUSH),
    1,
    VM_OP(VM_PUSH),
    99,
    VM_OP(VM_SP),
    VM_OP(VM_LOAD),
    VM_OP(VM_PUSH),
    2*sizeof(size_t),
    VM_OP(VM_ADD),
    VM_OP(VM_STR),
    VM_OP(VM_RET),
};
TEST(str_test, vm_test_group) {
    vm_t* vm = (vm_t*)malloc(sizeof(vm_t));
    uint8_t* ram = (uint8_t*)malloc(8192);
    size_t* stack = (size_t*)malloc(256*sizeof(size_t));
    size_t* rstack = (size_t*)malloc(256);
    vm_init(vm,ram,8192,stack,256/sizeof(size_t),rstack,256/sizeof(size_t));
    //vm_set_trace_cb(vm, vm_trace);
    vm_start(vm, str_test_program);
    ASSERT_EQ(*vm->sp++,99);
    ASSERT_EQ(vm->sp,vm->stack_top);
    free(vm);
    free(ram);
    free(stack);
    free(rstack);
}

static size_t it_test_program[] = 
{
    VM_OP(VM_PUSH),
    10,
    VM_OP(VM_SP), /* DUP */
    VM_OP(VM_LOAD),
    VM_OP(VM_PUSH),
    1*sizeof(size_t),
    VM_OP(VM_ADD),
    VM_OP(VM_LOAD),
    VM_OP(VM_SP),
    VM_OP(VM_LOAD),
    VM_OP(VM_PUSH),
    1*sizeof(size_t),
    VM_OP(VM_ADD),
    VM_OP(VM_LOAD),
    VM_OP(VM_IT),
    VM_OP(VM_PC),
    VM_OP(VM_LOAD),
    VM_OP(VM_PUSH),
    14*sizeof(size_t),
    VM_OP(VM_ADD),
    VM_OP(VM_JMP),
    VM_OP(VM_PUSH),
    (size_t)-1,
    VM_OP(VM_ADD),
    VM_OP(VM_PC),
    VM_OP(VM_LOAD),
    VM_OP(VM_PUSH),
    (size_t)-23*sizeof(size_t),
    VM_OP(VM_ADD),
    VM_OP(VM_JMP),
    VM_OP(VM_DROP),
    VM_OP(VM_RET),
};
TEST(it_test, vm_test_group) {
    vm_t* vm = (vm_t*)malloc(sizeof(vm_t));
    uint8_t* ram = (uint8_t*)malloc(8192);
    size_t* stack = (size_t*)malloc(256*sizeof(size_t));
    size_t* rstack = (size_t*)malloc(256);
    vm_init(vm,ram,8192,stack,256/sizeof(size_t),rstack,256/sizeof(size_t));
    //vm_set_trace_cb(vm, vm_trace);
    vm_start(vm, it_test_program);
    for(uint8_t i = 0; i < 11; i++)
    {
        ASSERT_EQ(*vm->sp++,i);
    }
    ASSERT_EQ(vm->sp,vm->stack_top);
    free(vm);
    free(ram);
    free(stack);
    free(rstack);
}