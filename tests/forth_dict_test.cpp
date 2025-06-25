#include <gtest/gtest.h>

#include "stdio.h"
#include "vm.h"

#include "forth.h"
TEST(forth_dict_init_test, forth_dict_group) {
    vm_t* vm = (vm_t*)malloc(sizeof(vm_t));
    uint8_t* ram = (uint8_t*)malloc(8192);
    size_t* stack = (size_t*)malloc(256);
    size_t* rstack = (size_t*)malloc(256);
    vm_init(vm,ram,8192,stack,256/sizeof(size_t),rstack,256/sizeof(size_t));
    forth_dict_init(vm,NULL);
    free(vm);
    free(ram);
    free(stack);
    free(rstack);
}
