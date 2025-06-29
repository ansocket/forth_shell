#include <gtest/gtest.h>

#include "stdio.h"
#include "vm.h"

#include "forth.h"
#include "forth_dict.h"

TEST(forth_dict_search_test, forth_dict_group) {
    vm_t* vm = (vm_t*)malloc(sizeof(vm_t));
    uint8_t* ram = (uint8_t*)malloc(8192);
    size_t* stack = (size_t*)malloc(256);
    size_t* rstack = (size_t*)malloc(256);
    vm_init(vm,ram,8192,stack,256/sizeof(size_t),rstack,256/sizeof(size_t));
    forth_init(vm);
    size_t* res = forth_search(vm,"FALSE");
    ASSERT_STREQ(((char*)res + 1),"FALSE");
    free(vm);
    free(ram);
    free(stack);
    free(rstack);
}

TEST(forth_dict_add_test, forth_dict_group) {
    vm_t* vm = (vm_t*)malloc(sizeof(vm_t));
    uint8_t* ram = (uint8_t*)malloc(8192);
    size_t* stack = (size_t*)malloc(256);
    size_t* rstack = (size_t*)malloc(256);
    vm_init(vm,ram,8192,stack,256/sizeof(size_t),rstack,256/sizeof(size_t));
    forth_init(vm);
    size_t* addr = forth_add_variable(vm,"kokok",5959);
    ASSERT_NE(addr,nullptr);
    size_t* res = forth_search(vm,"kokok");
    ASSERT_EQ(addr,res);
    size_t* data = forth_dict_get_text_ptr(res);
    *data = 10;
    addr = forth_add_variable(vm,"BEBE",32232);
    res = forth_search(vm,"BEBE");
    ASSERT_EQ(addr,res);
    res = forth_search(vm,"kokok");
    data = forth_dict_get_text_ptr(res);
    ASSERT_EQ(*data,10);
    free(vm);
    free(ram);
    free(stack);
    free(rstack);
}

TEST(forth_dict_constant, forth_dict_group) {
    vm_t* vm = (vm_t*)malloc(sizeof(vm_t));
    uint8_t* ram = (uint8_t*)malloc(8192);
    size_t* stack = (size_t*)malloc(256);
    size_t* rstack = (size_t*)malloc(256);
    vm_init(vm,ram,8192,stack,256/sizeof(size_t),rstack,256/sizeof(size_t));
    forth_init(vm);
    forth_add_constant(vm, "BEBE", 4455);
    forth_add_constant(vm, "BOBO", 3232325);
    size_t* res = forth_search(vm,"BEBE");
    size_t data = forth_get_constant_data(vm, res);
    ASSERT_EQ(data,4455);
    res = forth_search(vm,"BOBO");
    data = forth_get_constant_data(vm, res);
    ASSERT_EQ(data,3232325);
    free(vm);
    free(ram);
    free(stack);
    free(rstack);
}