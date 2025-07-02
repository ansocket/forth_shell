#ifndef FORTH_RODATA_H
#define FORTH_RODATA_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stddef.h"
#include "forth.h"
#include "vm.h"


extern const size_t loop_program_arr[];

#define FORTH_LAST_ROM_ADDRESS loop_program_arr

#ifdef __cplusplus
}
#endif
#endif