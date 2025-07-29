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

static forth_error_t get_string(vm_t* vm, char* data, int max_len)
{
    forth_error_t err = FORTH_ERR_OK;
    vm_ops_t key = (vm_ops_t)*(forth_dict_get_text_ptr(forth_search(vm, "KEY")) + 1);
    vm_ops_t emit = (vm_ops_t)*(forth_dict_get_text_ptr(forth_search(vm, "EMIT")) + 1);
    int i = 0;
    while (i < (FORTH_SANDBOX_SIZE - 1))
    {
        key(vm);
        int ch = *vm->sp++;
        switch(ch)
        {
            case '\n':
            {
                int len = strlen(data);
                data[len] = '\0';
                data[len + 1] = '\0';
            }
                goto exit;
            break;
            case '\r':
                goto exit;
            break;
            case 3:
                err = FORTH_EXIT;
                goto exit;
            break;
            case 127: /* backspace */
                if(i > 0)
                {
                    *(--vm->sp) = '\b';
                    emit(vm);
                    
                    int len = strlen(data + i) + 1;
                    i--;
                    for(int del_pos = 0; del_pos < len; del_pos++)
                    {
                        *(data + i + del_pos) = *(data + i + del_pos + 1);
                        if(*(data + i + del_pos) != '\0')
                        {
                            *(--vm->sp) = *(data + i + del_pos);
                            emit(vm);
                        }
                    }
                    *(--vm->sp) = ' ';
                    emit(vm);
                    *(--vm->sp) = '\b';
                    emit(vm);
                    for(int del_pos = 0; del_pos < (len - 1); del_pos++)
                    {
                        *(--vm->sp) = '\b';
                        emit(vm);
                    }
                }
            break;
            case '\033': /* ESCAPE */
                key(vm);
                ch = *vm->sp++;
                if(ch != '[') err = FORTH_ERR_ERR;
                key(vm);
                ch = *vm->sp++;
                if(ch == 'D')
                {
                    if(i > 0) 
                    {
                        *(--vm->sp) = '\033';
                        emit(vm);
                        *(--vm->sp) = '[';
                        emit(vm);
                        i--;
                        *(--vm->sp) = 'D';
                        emit(vm);
                    }
                }
                else if(ch == 'C')
                {
                    if(*(data + i) != '\0') 
                    {
                        *(--vm->sp) = '\033';
                        emit(vm);
                        *(--vm->sp) = '[';
                        emit(vm);
                        i++;
                        *(--vm->sp) = 'C';
                        emit(vm);
                    }
                }
                else if(ch == 51) /* delete */
                {
                    int len = strlen(data + i);
                    for(int del_pos = 0; del_pos < len; del_pos++)
                    {
                        *(data + i + del_pos) = *(data + i + del_pos + 1);
                        if(*(data + i + del_pos) != '\0')
                        {
                            *(--vm->sp) = *(data + i + del_pos);
                            emit(vm);
                        }
                    }
                    *(--vm->sp) = ' ';
                    emit(vm);
                    *(--vm->sp) = '\b';
                    emit(vm);
                    for(int del_pos = 0; del_pos < (len - 1); del_pos++)
                    {
                        *(--vm->sp) = '\b';
                        emit(vm);
                    }
                    key(vm);
                    ch = *vm->sp++;
                    if(ch != 126) err = FORTH_ERR_ERR;
                }
            break;
            default:
            {
                int len = strlen(data);
                *(--vm->sp) = ch;
                emit(vm);
                for(int ins_pos = i; ins_pos < len; ins_pos++)
                {
                    *(--vm->sp) = *(data + ins_pos);
                    emit(vm);
                    *(data + ins_pos + 1) = *(data + ins_pos);
                }
                for(int back = 0; back < (len - i); back++)
                {
                    *(--vm->sp) = '\b';
                    emit(vm);
                }
                *(data + i) = ch;
                i++;
            }
            break;
        }

    }
exit:
    *(--vm->sp) = '\n';
    emit(vm);
    return err;
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

    forth_add_custom_function(vm, "OUTPUT", forth_output_data); /* ?? */

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
        
        err = get_string(vm,(char*)vm->ram + FORTH_STRBUF_OFFSET,255);
        if(err == FORTH_EXIT) return FORTH_EXIT;
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