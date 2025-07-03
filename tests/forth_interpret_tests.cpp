#include <cstddef>
#include <gtest/gtest.h>

#include <cstdio>
#include "vm.h"

#include "forth.h"
#include "forth_dict.h"
#include "forth_inter.h"
#include "vm.h"
#include <cstring>

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

void forth_output_data(vm_t* vm)
{
    /* stack -> ADDR LENGTH */
    char* data = (char*)*vm->sp++;
    int len = *vm->sp++;
    for(int i = 0; i < len; i++)
    {
        putchar(data[i]);
    }
    *vm->rsp = (*vm->rsp + len);
    if((*vm->rsp % sizeof(size_t)) > 0)
    {
        *vm->rsp = (size_t)(*vm->rsp + (sizeof(size_t) - (*vm->rsp % sizeof(size_t))));
    } 

}
TEST(forth_output_test, forth_interpret_group) {
    vm_t* vm = (vm_t*)malloc(sizeof(vm_t));
    uint8_t* ram = (uint8_t*)malloc(8192);
    size_t* stack = (size_t*)malloc(256);
    size_t* rstack = (size_t*)malloc(256);
    vm_init(vm,ram,8192,stack,256/sizeof(size_t),rstack,256/sizeof(size_t));
   // vm_set_trace_cb(vm, vm_trace);
    forth_init(vm);
    forth_add_custom_function(vm, "OUTPUT", forth_output_data);
    strcpy((char*)vm->ram + FORTH_STRBUF_OFFSET," .\" Hello World \" 1 2 .\" bebe \" 2");
    forth_error_t err = forth_start_compiling(vm);
    ASSERT_EQ(err, FORTH_ERR_OK);
    vm_start(vm, (size_t*)(vm->ram + FORTH_SANDBOX_OFFSET));
    forth_vm_reload(vm);

    free(vm);
    free(ram);
    free(stack);
    free(rstack);
}