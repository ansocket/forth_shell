#ifndef FORTH_RODATA_H
#define FORTH_RODATA_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stddef.h"
#include "forth.h"
#include "vm.h"


static const size_t false_program_arr[] = 
{
    (FORTH_DICT_FLAG_CONST << 2) | 
    ((size_t)'F' << 8) |
    ((size_t)'A' << 16) |
    ((size_t)'L' << 24) |
    ((size_t)'S' << 32) |
    ((size_t)'E' << 40) | 
    ((size_t)'\0' << 48),
    (size_t)NULL,
    0,
};

static const size_t true_program_arr[] = 
{
    (FORTH_DICT_FLAG_CONST << 2) | 
    ((size_t)'T' << 8) |
    ((size_t)'R' << 16) |
    ((size_t)'U' << 24) |
    ((size_t)'E' << 32) |
    ((size_t)'\0' << 40),
    (size_t)false_program_arr,
    -1,
};

#define FORTH_LAST_ROM_ADDRESS true_program_arr

#ifdef __cplusplus
}
#endif
#endif