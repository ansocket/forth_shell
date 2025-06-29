#include "forth.h"
#include "string.h"
#include "forth_dict.h"


static char* align_for(char* data, uint8_t base)
{
    char* res = data;
    if((size_t)data % base > 0)
    {
        res += (base - (size_t)data % base);
    }
    return res;
}
size_t* forth_dict_add_header(size_t* here, uint8_t flags, const char* name, size_t* link_addr)
{
    char* temp = (char*)here;
    if((here == NULL) || (name == NULL)) return NULL;
    *temp++ = flags;
    strncpy(temp, name, 31);
    temp += strlen(name) + 1;
    temp = align_for(temp, sizeof(size_t));
    *(size_t*)temp = (size_t)link_addr;
    here = (size_t*)temp;
    return ++here;
}

forth_error_t forth_dict_init(size_t* dict_addr, size_t* last_rom_dict_addr)
{
    if(dict_addr == NULL) return FORTH_ERR_ERR;
    size_t* here = dict_addr;
    here = forth_dict_add_header(here,FORTH_DICT_FLAG_VARIABLE,"FORTH",last_rom_dict_addr);
    *here = (size_t)(here + 1);
    here++;
    here = forth_dict_add_header(here,FORTH_DICT_FLAG_VARIABLE,"HERE",dict_addr);
    *here = (size_t)(here + 1);
    here++;
    return FORTH_ERR_OK;
}
char* forth_dict_get_name(size_t* addr)
{
    if(addr == NULL) return NULL;
    char* res = (char*)addr;
    return ++res;
}
size_t* forth_dict_get_link_addr(size_t* addr)
{
    char* temp = (char*)addr;
    temp++;
    temp += strlen(temp) + 1;
    temp = align_for(temp, sizeof(size_t));
    return (size_t*)temp;
}
size_t* forth_dict_get_text_ptr(size_t* addr)
{
    addr = forth_dict_get_link_addr(addr);
    addr++;
    return addr;
}
uint8_t forth_dict_get_flags(size_t* addr)
{
    return *(uint8_t*)addr;
}
size_t* forth_dict_search(size_t* dict_addr, const char* name)
{if(dict_addr == NULL) return NULL;
    
    char* temp = NULL;
    do {
        temp = (char*)dict_addr;
        temp++;
        if(strncmp(name, temp, 31) == 0)
        {
            break;
        }
        else 
        {
            temp += strlen(temp);
            temp = align_for(temp, sizeof(size_t));
            dict_addr = *(size_t**)temp;
        }
    }while (dict_addr != NULL);
    return dict_addr;
}

