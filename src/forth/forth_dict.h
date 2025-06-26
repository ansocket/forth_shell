#ifndef FORTH_DICT_H
#define FORTH_DICT_H

#ifdef __cplusplus
extern "C" {
#endif

#include "forth.h"
#include "vm.h"

forth_error_t forth_dict_init(vm_t* vm, size_t* last_rom_dict_addr);

#ifdef __cplusplus
}
#endif
#endif