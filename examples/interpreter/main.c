#include "stdio.h"
#include "stdlib.h"
#include "forth.h"
#include "string.h"

void forth_output_data(vm_t* vm)
{
    /* stack -> ADDR LENGTH */
    char* data = (char*)*vm->sp++;
    int len = *vm->sp;
    for(int i = 0; i < len; i++)
    {
        putchar(data[i]);
    }
}

int main()
{
    vm_t* vm = (vm_t*)malloc(sizeof(vm_t));
    uint8_t* ram = (uint8_t*)malloc(8192);
    size_t* stack = (size_t*)malloc(256);
    size_t* rstack = (size_t*)malloc(256);
    vm_init(vm,ram,8192,stack,256/sizeof(size_t),rstack,256/sizeof(size_t));
   // vm_set_trace_cb(vm, vm_trace);
    forth_init(vm);
    forth_add_custom_function(vm, "OUTPUT", forth_output_data);
    strcpy((char*)vm->ram + FORTH_STRBUF_OFFSET," 1 2 + DUP . 8883 + . .\" \nSasha molodec\n\"");
    forth_error_t err = forth_start_compiling(vm);
    if(err != FORTH_ERR_OK) return -1;
    vm_start(vm, (size_t*)(vm->ram + FORTH_SANDBOX_OFFSET));
    forth_vm_reload(vm);

    free(vm);
    free(ram);
    free(stack);
    free(rstack);
}