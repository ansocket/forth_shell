#include "forth.h"
#include "string.h"

static char* align_for(char* data, uint8_t base)
{
    char* res = data;
    if((size_t)data % base > 0)
    {
        res += (base - (size_t)data % base);
    }
    return res;
}
size_t* forth_dict_add_header(vm_t* vm, size_t* here, forth_dict_flag_t* flags, const char* name, size_t* link_addr)
{
    char* temp = (char*)here;
    if((vm == NULL) || (here == NULL) || (name == NULL)) return NULL;
    if(flags != NULL)
    {
        *temp++ = flags->body;
    }
    else 
    {
        *temp++ = 0;
    }
    strncpy(temp, name, 31);
    temp += strlen(name);
    temp = align_for(temp, sizeof(size_t));
    *(size_t*)temp = (size_t)link_addr;
    here = (size_t*)temp;
    return ++here;
}

forth_error_t forth_dict_init(vm_t* vm, size_t* last_rom_dict_addr)
{
    if(vm == NULL) return FORTH_ERR_ERR;
    size_t* dict_start = (size_t*)(vm->ram + FORTH_DICT_OFFSET);
    forth_dict_flag_t flags;
    /* FORTH VARIABLE */
    flags.compile_only = 0;
    flags.type = FORTH_DICT_FLAG_VARIABLE;
    flags.immediate = 0;
    dict_start = forth_dict_add_header(vm,dict_start,&flags,"FORTH",last_rom_dict_addr);
    *dict_start++ = 0;
    dict_start = forth_dict_add_header(vm,dict_start,&flags,"HERE",(size_t*)(vm->ram + FORTH_DICT_OFFSET));
    *dict_start = (size_t)dict_start;
    dict_start++;
    return FORTH_ERR_OK;
}

