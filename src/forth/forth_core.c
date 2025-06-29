#include "forth_rodata.h"
#include "forth.h"
#include "stddef.h"


static const size_t false_program_arr[] = 
{
    (FORTH_DICT_FLAG_CONSTANT << 2) | 
    ((size_t)'F' << 8) |
    ((size_t)'A' << 16) |
    ((size_t)'L' << 24) |
    ((size_t)'S' << 32) |
    ((size_t)'E' << 40) | 
    ((size_t)'\0' << 48),
    (size_t)NULL,
    0,
};

static const size_t plus_program_arr[] = 
{
    (FORTH_DICT_FLAG_TEXT << 2) | 
    ((size_t)'+' << 8) |
    ((size_t)'\0' << 16),
    (size_t)false_program_arr,
    VM_OP(VM_ADD),
    VM_OP(VM_RET)
};
const size_t true_program_arr[] = 
{
    (FORTH_DICT_FLAG_CONSTANT << 2) | 
    ((size_t)'T' << 8) |
    ((size_t)'R' << 16) |
    ((size_t)'U' << 24) |
    ((size_t)'E' << 32) |
    ((size_t)'\0' << 40),
    (size_t)plus_program_arr,
    (size_t)-1,
};
