#include "stdio.h"
#include "stdlib.h"
#include "forth.h"
#include "string.h"
#include "vm.h"

void forth_output_data(vm_t* vm)
{
    /* stack -> ADDR LENGTH */
    char* data = (char*)*vm->sp++;
    int len = *vm->sp++;
    for(int i = 0; i < len; i++)
    {
        putchar(data[i]);
    }
}
int get_string(char* data, int max_len)
{
    int i = 0;
    while (i < FORTH_SANDBOX_SIZE)
    {
        *(data + i) = getchar();
        if((*(data + i) == 8) && (i > 0))
        {
            i--;
        }
        else {
            if(*(data + i) == '\n')
            {
                *(data + i) = '\0';
                break;
            }
            i++;
        }
    }
    
    return i;
}
void custom_function(vm_t* vm)
{
    printf("Я вообще не связанная ни с чем функция\n");
}
void custom_function_arg(vm_t* vm)
{
    size_t arg = *vm->sp++;
    printf("Я вообще не связанная ни с чем функция, но умею получать аргументы. Аргумент: %ld\n", arg);
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
    forth_add_custom_function(vm, "CUSTOM", custom_function);
    forth_add_custom_function(vm, "CUSTOM_ARG", custom_function_arg);
    //forth_add_custom_function(vm, "INPUT", forth_input_data);
    strcpy((char*)vm->ram + FORTH_STRBUF_OFFSET,".\" Forth_shell by ansocket.\n \"");
    forth_error_t err = forth_start_compiling(vm);
    if(err != FORTH_ERR_OK) return -1;
    vm_start(vm, (size_t*)(vm->ram + FORTH_SANDBOX_OFFSET));
    forth_vm_reload(vm);

    while(1)
    {
        strcpy((char*)vm->ram + FORTH_STRBUF_OFFSET,".\" \n> \"");
        err = forth_start_compiling(vm);
        if(err == FORTH_ERR_OK)
        {
            vm_start(vm, (size_t*)(vm->ram + FORTH_SANDBOX_OFFSET));
            if((vm->exceptions_flags & VM_EXCEPTION_STACK_UNDERFLOW) == VM_EXCEPTION_STACK_UNDERFLOW)
            {
                forth_vm_reload(vm);
                strcpy((char*)vm->ram + FORTH_STRBUF_OFFSET,".\" STACK UNDERFLOW \"");
                err = forth_start_compiling(vm);
                vm_start(vm, (size_t*)(vm->ram + FORTH_SANDBOX_OFFSET));
            }
        }
        forth_vm_reload(vm);
        get_string((char*)vm->ram + FORTH_STRBUF_OFFSET,255);
        err = forth_start_compiling(vm);
        if(err == FORTH_ERR_OK)
        {
            vm_start(vm, (size_t*)(vm->ram + FORTH_SANDBOX_OFFSET));
            if((vm->exceptions_flags & VM_EXCEPTION_STACK_UNDERFLOW) == VM_EXCEPTION_STACK_UNDERFLOW)
            {
                forth_vm_reload(vm);
                strcpy((char*)vm->ram + FORTH_STRBUF_OFFSET,".\" STACK UNDERFLOW \"");
                err = forth_start_compiling(vm);
                vm_start(vm, (size_t*)(vm->ram + FORTH_SANDBOX_OFFSET));
            }
            else if((vm->exceptions_flags & VM_EXCEPTION_ZERO_DIVISION) == VM_EXCEPTION_ZERO_DIVISION)
            {
                forth_vm_reload(vm);
                strcpy((char*)vm->ram + FORTH_STRBUF_OFFSET,".\" ZERO DIVISION \"");
                err = forth_start_compiling(vm);
                vm_start(vm, (size_t*)(vm->ram + FORTH_SANDBOX_OFFSET));
            }
            else {
                forth_vm_reload(vm);
                strcpy((char*)vm->ram + FORTH_STRBUF_OFFSET,".\"  OK \"");
                err = forth_start_compiling(vm);
                vm_start(vm, (size_t*)(vm->ram + FORTH_SANDBOX_OFFSET));
                }
        }
        else if(err == FORTH_WORD_NOT_FOUND){
            forth_vm_reload(vm);
            strcpy((char*)vm->ram + FORTH_STRBUF_OFFSET,".\" WORD NOT FOUND \"");
            err = forth_start_compiling(vm);
            vm_start(vm, (size_t*)(vm->ram + FORTH_SANDBOX_OFFSET));
        }
        else if(err == FORTH_COMPILE_ONLY_ERROR)
        {
            forth_vm_reload(vm);
            strcpy((char*)vm->ram + FORTH_STRBUF_OFFSET,".\" THIS WORD IS COMPILE-ONLY \"");
            err = forth_start_compiling(vm);
            vm_start(vm, (size_t*)(vm->ram + FORTH_SANDBOX_OFFSET));
        }
        forth_vm_reload(vm);
    }
    free(vm);
    free(ram);
    free(stack);
    free(rstack);
}