#include <cstddef>
#include <gtest/gtest.h>

#include <cstdio>
#include "vm.h"

#include "forth.h"
#include "forth_dict.h"
#include "forth_inter.h"

#include <cstring>

TEST(forth_token_test, forth_compile_group) {
    char* origin = (char*)malloc(255*sizeof(char));
    char* buffer = origin;
    strncpy(buffer,"50 10 +  BEBE 10 TETE",255);
    int len = strlen(buffer);
    char* token = forth_inter_token(&buffer, &len);
    ASSERT_STREQ(token, "50");
    token = forth_inter_token(&buffer,&len);
    ASSERT_STREQ(token, "10");
    token = forth_inter_token(&buffer,&len);
    ASSERT_STREQ(token, "+");
    token = forth_inter_token(&buffer,&len);
    ASSERT_STREQ(token, "BEBE");
    token = forth_inter_token(&buffer,&len);
    ASSERT_STREQ(token, "10");
    token = forth_inter_token(&buffer,&len);
    ASSERT_STREQ(token, "TETE");
    token = forth_inter_token(&buffer,&len);
    ASSERT_EQ(token, nullptr);
    free(origin);
}

TEST(forth_get_token_test, forth_compile_group) {
    vm_t* vm = (vm_t*)malloc(sizeof(vm_t));
    uint8_t* ram = (uint8_t*)malloc(8192);
    size_t* stack = (size_t*)malloc(256);
    size_t* rstack = (size_t*)malloc(256);
    vm_init(vm,ram,8192,stack,256/sizeof(size_t),rstack,256/sizeof(size_t));
    forth_init(vm);

    free(vm);
    free(ram);
    free(stack);
    free(rstack);
}