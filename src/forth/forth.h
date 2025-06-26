#ifndef FORTH_H
#define FORTH_H
#ifdef __cplusplus
extern "C" {
#endif

#include "vm.h"
#include "stdint.h"

#define FORTH_STRBUF_SIZE       256
#define FORTH_SANDBOX_SIZE        1024

#define FORTH_STRBUF_OFFSET     0
#define FORTH_SANDBOX_OFFSET    (FORTH_STRBUF_SIZE)
#define FORTH_DICT_OFFSET   (FORTH_SANDBOX_OFFSET + FORTH_SANDBOX_SIZE) 


typedef enum : uint8_t
{
    FORTH_DICT_FLAG_TEXT,
    FORTH_DICT_FLAG_VARIABLE,
    FORTH_DICT_FLAG_CONST
}forth_flag_type_t;

typedef union
{
    struct
    {
        uint8_t compile_only : 1;
        uint8_t immediate : 1;
        forth_flag_type_t type : 2;
    };
    uint8_t body;
}forth_dict_flag_t;

typedef enum
{
    FORTH_ERR_OK,
    FORTH_ERR_ERR,
}forth_error_t;


size_t* forth_search(vm_t* vm, const char* name);
forth_error_t forth_init(vm_t* vm);
#ifdef __cplusplus
}
#endif
#endif