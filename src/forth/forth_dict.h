#ifndef FORTH_DICT_H
#define FORTH_DICT_H

#ifdef __cplusplus
extern "C" {
#endif

#include "forth.h"

forth_error_t forth_dict_init(size_t* dict_addr, size_t* last_rom_dict_addr);
size_t* forth_dict_add_header(size_t* here, uint8_t flags, const char* name, size_t* link_addr);
size_t* forth_dict_get_text_ptr(size_t* addr);
uint8_t forth_dict_get_flags(size_t* addr);
char* forth_dict_get_name(size_t* addr);
size_t* forth_dict_search(size_t* dict_addr, const char* name);
#ifdef __cplusplus
}
#endif
#endif