#include "forth.h"
#include "forth_dict.h"
#include "forth_inter.h"
#include "string.h"
#include "vm.h"
static const char term[] = {' ','\t','\n','\r','\0'};

static int is_term(char ch)
{
    for(int i = 0; i < sizeof(term); i++)
    {
        if(ch == term[i]) return 1;
    }
    return 0;
}

char* forth_inter_token(char** buf)
{
    char* res = NULL;
    if(buf == NULL) return res;
    
    while(is_term(**buf))
    {
        (*buf)++;
        if(**buf == '\0')
            break;
    } 
    int len = strlen(*buf);
    if(len == 0) return res;
    res = *buf;
    while(len > 0)
    {
        if(is_term(**buf))
        {
            **buf = '\0';
            break;
        }
        len--;
        (*buf)++;
    }
    return res;
}

static int get_string(vm_t* vm, char* data, int max_len)
{
    vm_ops_t key = (vm_ops_t)*(forth_dict_get_text_ptr(forth_search(vm, "KEY")) + 1);
    vm_ops_t emit = (vm_ops_t)*(forth_dict_get_text_ptr(forth_search(vm, "EMIT")) + 1);
    int i = 0;
    while (i < (FORTH_SANDBOX_SIZE - 1))
    {
        key(vm);
        int ch = *vm->sp++;

        if(ch == '\n')
        {
            *(data + i) = '\0';
            *(data + i + 1) = '\0';
            break;
        }
        if(ch == 127)
        {
            if(i > 0)
            {
                *(--vm->sp) = '\b';
                emit(vm);
                *(--vm->sp) = ' ';
                emit(vm);
                *(--vm->sp) = '\b';
                emit(vm);
                i--;
            }
            continue;
        }
        *(--vm->sp) = ch;
        emit(vm);
        *(data + i) = ch;
        i++;
    }
    *(--vm->sp) = '\n';
    emit(vm);
    return i;
}

void forth_output_data(vm_t* vm)
{
    vm_ops_t emit = (vm_ops_t)*(forth_dict_get_text_ptr(forth_search(vm, "EMIT")) + 1);
    /* stack -> ADDR LENGTH */
    char* data = (char*)*vm->sp++;
    int len = *vm->sp++;
    for(int i = 0; i < len; i++)
    {
        *(--vm->sp) = data[i];
        emit(vm);
    }
}

forth_error_t forth_interpreter_init(vm_t* vm, vm_ops_t key, vm_ops_t emit)
{
    forth_init(vm);
    forth_add_custom_function(vm, "EMIT", emit);
    forth_add_custom_function(vm, "KEY", key);

    forth_add_custom_function(vm, "OUTPUT", forth_output_data);

    strcpy((char*)vm->ram + FORTH_STRBUF_OFFSET,".\" Forth_shell by ansocket.\n \"");
    forth_error_t err = forth_start_compiling(vm);
    if(err != FORTH_ERR_OK) return -1;
    vm_start(vm, (size_t*)(vm->ram + FORTH_SANDBOX_OFFSET));
    forth_vm_reload(vm);
    return err;
}

forth_error_t forth_interpreter_process(vm_t *vm)
{
    forth_error_t err = FORTH_ERR_OK;
        size_t state = *forth_get_variable_data_ptr(vm, forth_search(vm, "STATE"));
        if(state == FORTH_STATE_INTERPRET)
        {
            strcpy((char*)vm->ram + FORTH_STRBUF_OFFSET,".\" \n> \"");
            err = forth_start_compiling(vm);
            if(err == FORTH_ERR_OK)
            {
                vm_start(vm, (size_t*)(vm->ram + FORTH_SANDBOX_OFFSET));
                forth_vm_reload(vm);
            }
        }
        
        get_string(vm,(char*)vm->ram + FORTH_STRBUF_OFFSET,255);
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
                size_t state = *forth_get_variable_data_ptr(vm, forth_search(vm, "STATE"));
                if(state == FORTH_STATE_INTERPRET)
                {
                    forth_vm_reload(vm);
                    strcpy((char*)vm->ram + FORTH_STRBUF_OFFSET,".\"  OK \"");
                    err = forth_start_compiling(vm);
                    vm_start(vm, (size_t*)(vm->ram + FORTH_SANDBOX_OFFSET));
                }
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
    return err;
}