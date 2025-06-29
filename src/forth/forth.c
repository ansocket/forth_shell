#include "forth.h"
#include "forth_dict.h"
#include "forth_rodata.h"

forth_error_t forth_init(vm_t* vm)
{
    forth_error_t res = forth_dict_init((size_t*)(vm->ram + FORTH_DICT_OFFSET),(size_t*)FORTH_LAST_ROM_ADDRESS);
    forth_add_variable(vm,">IN",(size_t)(vm->ram + FORTH_STRBUF_OFFSET));
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