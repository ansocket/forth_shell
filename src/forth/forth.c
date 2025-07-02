#include "forth.h"
#include "forth_dict.h"
#include "forth_inter.h"
#include "forth_rodata.h"
#include "string.h"
#include "stdlib.h"
#include "vm.h"
static int to_numeric(char* str,int32_t* value)
{
    if(str == NULL) return -1;
    unsigned long len = strlen(str);
    unsigned long i = 0;
    if(str[0] == '-') 
    {
        i++;
    }
    for(; i < len; i++)
    {
        if((str[i] < '0') || (str[i] > '9'))
        {
            return -1;
        }
    }
    *value = atoi(str);
    return 0;
}

forth_error_t forth_init(vm_t* vm)
{
    forth_error_t res = forth_dict_init((size_t*)(vm->ram + FORTH_DICT_OFFSET),(size_t*)FORTH_LAST_ROM_ADDRESS);
    forth_add_variable(vm,">IN",0);
    memset(vm->ram + FORTH_STRBUF_OFFSET,'\0',FORTH_STRBUF_SIZE);
    forth_add_variable(vm,"STATE",FORTH_STATE_INTERPRET);
    forth_add_variable(vm,"SANDBOX",(size_t)(vm->ram + FORTH_SANDBOX_OFFSET));
    forth_add_constant(vm, "I", -1);
    forth_add_constant(vm, "J", -1);
    return res; 
}

size_t* forth_search(vm_t* vm, const char* name)
{
    size_t* forth_addr = (size_t*)(vm->ram + FORTH_DICT_OFFSET);
    forth_addr = forth_dict_get_text_ptr(forth_addr);
    return forth_dict_search((size_t*)*(size_t*)forth_addr,name);
}


size_t* forth_add_variable(vm_t* vm, const char* name, size_t value)
{
    size_t** here_addr = (size_t**)forth_dict_get_text_ptr(forth_search(vm, "HERE"));
    size_t** forth_addr = (size_t**)forth_dict_get_text_ptr( forth_search(vm, "FORTH"));
    if((here_addr == NULL) || (forth_addr == NULL)) return NULL;
    size_t* link = *forth_addr;
    *forth_addr = *here_addr;
    *here_addr = forth_dict_add_header(*here_addr,FORTH_DICT_FLAG_VARIABLE,name,link);
    **here_addr = value;
    *here_addr += 1;
    return *forth_addr;
}

size_t* forth_add_constant(vm_t* vm, const char* name, size_t value)
{
    size_t** here_addr = (size_t**)forth_dict_get_text_ptr(forth_search(vm, "HERE"));
    size_t** forth_addr = (size_t**)forth_dict_get_text_ptr( forth_search(vm, "FORTH"));
    if((here_addr == NULL) || (forth_addr == NULL)) return NULL;
    size_t* link = *forth_addr;
    *forth_addr = *here_addr;
    *here_addr = forth_dict_add_header(*here_addr,FORTH_DICT_FLAG_CONSTANT,name,link);
    **here_addr = value;
    *here_addr += 1;
    return *forth_addr;
}
size_t* forth_add_definition(vm_t* vm, const char* name)
{
    size_t** here_addr = (size_t**)forth_dict_get_text_ptr(forth_search(vm, "HERE"));
    size_t** forth_addr = (size_t**)forth_dict_get_text_ptr( forth_search(vm, "FORTH"));
    if((here_addr == NULL) || (forth_addr == NULL)) return NULL;
    size_t* link = *forth_addr;
    *forth_addr = *here_addr;
    *here_addr = forth_dict_add_header(*here_addr,FORTH_DICT_FLAG_TEXT,name,link);
    return *forth_addr;
}
size_t* forth_get_variable_data_ptr(vm_t* vm,size_t* addr)
{
    uint8_t flags = forth_dict_get_flags(addr);
    if((flags & FORTH_DICT_FLAG_VARIABLE) == FORTH_DICT_FLAG_VARIABLE)
    {
        return forth_dict_get_text_ptr(addr);
    }
    return NULL;
}

size_t forth_get_constant_data(vm_t* vm,size_t* addr)
{
    uint8_t flags = forth_dict_get_flags(addr);
    if((flags & FORTH_DICT_FLAG_CONSTANT) == FORTH_DICT_FLAG_CONSTANT)
    {
        return *forth_dict_get_text_ptr(addr);
    }
    return 0;
}

char* forth_get_token(vm_t* vm)
{
    if(vm == NULL) return NULL;
    size_t *in_var_addr = forth_search(vm, ">IN");
    if(in_var_addr == NULL) return NULL;
    size_t* in_data = forth_get_variable_data_ptr(vm, in_var_addr);
    char* buf_pos = (char*)(vm->ram + FORTH_STRBUF_OFFSET) + *in_data;
    
    char* res = forth_inter_token(&buf_pos);
    *in_data = buf_pos - (char*)(vm->ram + FORTH_STRBUF_OFFSET);
    return res;
}

forth_error_t forth_compile(vm_t* vm, char* token)
{
    if((vm == NULL) || (token == NULL)) return FORTH_ERR_ERR;
    size_t* state = forth_search(vm, "STATE");
    if(state == NULL) return FORTH_ERR_ERR;
    state = forth_get_variable_data_ptr(vm, state);
    size_t** copy_ptr = NULL;
    if(*state == FORTH_STATE_COMPILE)
    {
        size_t* here = forth_search(vm, "HERE");
        if(here == NULL) return FORTH_ERR_ERR;
        copy_ptr = (size_t**)forth_get_variable_data_ptr(vm, here);
    }
    else if(*state == FORTH_STATE_INTERPRET)
    {
        size_t* sandbox = forth_search(vm, "SANDBOX");
        if(sandbox == NULL) return FORTH_ERR_ERR;
        copy_ptr = (size_t**)forth_get_variable_data_ptr(vm, sandbox);
    }
    else return FORTH_ERR_ERR;
    
    size_t* command = forth_search(vm, token);
    if(command != NULL)
    {
        uint8_t flags = forth_dict_get_flags(command);
        if(((flags & FORTH_DICT_FLAG_COMPILE_ONLY) == FORTH_DICT_FLAG_COMPILE_ONLY) && (*state != FORTH_STATE_COMPILE))
        {
            return FORTH_COMPILE_ONLY_ERROR;
        } 
        if((flags & FORTH_DICT_FLAG_IMMEDIATE) == FORTH_DICT_FLAG_IMMEDIATE)
        {
            if((flags & FORTH_DICT_FLAG_VARIABLE) == FORTH_DICT_FLAG_VARIABLE)
            {
                *(--vm->sp) = (size_t)forth_get_variable_data_ptr(vm, command);
            }
            else if((flags & FORTH_DICT_FLAG_CONSTANT) == FORTH_DICT_FLAG_CONSTANT)
            {
                *(--vm->sp) = forth_get_constant_data(vm, command);
            }
            else {
                vm_start(vm,forth_dict_get_text_ptr(command));
                vm->exceptions_flags &= ~VM_EXCEPTION_BYE;
            }
            return FORTH_ERR_OK;
        }
        else
        {
            if((flags & FORTH_DICT_FLAG_VARIABLE) == FORTH_DICT_FLAG_VARIABLE)
            {
                *(*copy_ptr)++ = VM_OP(VM_PUSH);
                *(*copy_ptr)++ = (size_t)forth_get_variable_data_ptr(vm, command);
            }
            else if((flags & FORTH_DICT_FLAG_CONSTANT) == FORTH_DICT_FLAG_CONSTANT)
            {
                *(*copy_ptr)++ = VM_OP(VM_PUSH);
                *(*copy_ptr)++ = (size_t)forth_dict_get_text_ptr(command);
                *(*copy_ptr)++ = VM_OP(VM_LOAD);
            }
            else {
                *(*copy_ptr)++ = VM_OP(VM_PUSH);
                *(*copy_ptr)++ = (size_t)forth_dict_get_text_ptr(command);
                *(*copy_ptr)++ = VM_OP(VM_CALL);
            }
            return FORTH_ERR_OK;
        }
    }
    else {
        int32_t value = 0;
        if(to_numeric(token,&value) == 0)
        {
            *(*copy_ptr)++ = VM_OP(VM_PUSH);
            *(*copy_ptr)++ = value;
            return FORTH_ERR_OK;
        }
    }
    return FORTH_ERR_ERR;
}

forth_error_t forth_start_compiling(vm_t* vm)
{
    char* token = NULL;
    forth_error_t err = FORTH_ERR_OK;
    do{
        token = forth_get_token(vm);
        if(token != NULL)
        {
            err = forth_compile(vm, token);
        }
    }
    while(token != NULL);
    size_t* sandbox = forth_search(vm, "SANDBOX");
    size_t** copy_ptr = (size_t**)forth_get_variable_data_ptr(vm, sandbox);
    *(*copy_ptr)++ = VM_OP(VM_RET);
    return err;
}

forth_error_t forth_vm_reload(vm_t* vm)
{
    memset(vm->ram + FORTH_STRBUF_OFFSET,'\0',FORTH_STRBUF_SIZE);
    vm->exceptions_flags = 0;
    size_t* sandbox = forth_search(vm, "SANDBOX");
    size_t** copy_ptr = (size_t**)forth_get_variable_data_ptr(vm, sandbox);
    *copy_ptr = (size_t*)(vm->ram + FORTH_SANDBOX_OFFSET);
    size_t* in_ptr = forth_search(vm, ">IN");
    in_ptr = forth_get_variable_data_ptr(vm, in_ptr);
    *in_ptr = 0;
    return FORTH_ERR_OK;
}