#include <cstddef>
#include <gtest/gtest.h>

#include <cstdio>
#include "vm.h"

#include "forth.h"
#include "forth_dict.h"
#include "forth_inter.h"
#include "vm.h"
#include <cstring>

extern "C" void vm_trace(vm_t* vm)
{
    size_t* temp = vm->sp;
    while(temp < vm->stack_top)
    {
        printf("%ld ",*(size_t*)temp);
        temp++;
    }
  printf("\n");

}

TEST(forth_token_test, forth_compile_group) {
    char* origin = (char*)malloc(255*sizeof(char));
    memset(origin,0,255);
    char* buffer = origin;
    strncpy(buffer,"50 10 +  BEBE 10 TETE",255);
    char* token = forth_inter_token(&buffer);
    ASSERT_STREQ(token, "50");
    token = forth_inter_token(&buffer);
    ASSERT_STREQ(token, "10");
    token = forth_inter_token(&buffer);
    ASSERT_STREQ(token, "+");
    token = forth_inter_token(&buffer);
    ASSERT_STREQ(token, "BEBE");
    token = forth_inter_token(&buffer);
    ASSERT_STREQ(token, "10");
    token = forth_inter_token(&buffer);
    ASSERT_STREQ(token, "TETE");
    token = forth_inter_token(&buffer);
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
    strcpy((char*)vm->ram + FORTH_STRBUF_OFFSET," BEBE 50 OK + 4");
    char* token = forth_get_token(vm);
    ASSERT_STREQ(token, "BEBE");
    token = forth_get_token(vm);
    ASSERT_STREQ(token, "50");
    token = forth_get_token(vm);
    ASSERT_STREQ(token, "OK");
    token = forth_get_token(vm);
    ASSERT_STREQ(token, "+");
    token = forth_get_token(vm);
    ASSERT_STREQ(token, "4");
    token = forth_get_token(vm);
    ASSERT_EQ(token, nullptr);
    free(vm);
    free(ram);
    free(stack);
    free(rstack);
}

TEST(forth_compile_test, forth_compile_group) {
    vm_t* vm = (vm_t*)malloc(sizeof(vm_t));
    uint8_t* ram = (uint8_t*)malloc(8192);
    size_t* stack = (size_t*)malloc(256);
    size_t* rstack = (size_t*)malloc(256);
    vm_init(vm,ram,8192,stack,256/sizeof(size_t),rstack,256/sizeof(size_t));
    forth_init(vm);
    strcpy((char*)vm->ram + FORTH_STRBUF_OFFSET," 50 40 +");

    char* token = forth_get_token(vm);
    ASSERT_STREQ(token, "50");
    forth_error_t err = forth_compile(vm, token);
    ASSERT_EQ(err, FORTH_ERR_OK);

    token = forth_get_token(vm);
    ASSERT_STREQ(token, "40");
    err = forth_compile(vm, token);
    ASSERT_EQ(err, FORTH_ERR_OK);

    token = forth_get_token(vm);
    ASSERT_STREQ(token, "+");
    err = forth_compile(vm, token);
    ASSERT_EQ(err, FORTH_ERR_OK);

    token = forth_get_token(vm);
    ASSERT_EQ(token, nullptr);
    
    size_t* sandbox = forth_search(vm, "SANDBOX");
    ASSERT_NE(sandbox, nullptr);
    size_t** copy_ptr = (size_t**)forth_get_variable_data_ptr(vm, sandbox);
    *(*copy_ptr)++ = VM_OP(VM_RET);

    vm_set_trace_cb(vm, vm_trace);
    vm_start(vm, (size_t*)(vm->ram + FORTH_SANDBOX_OFFSET));
    memset(vm->ram + FORTH_STRBUF_OFFSET,'\0',FORTH_STRBUF_SIZE);
    
    vm->exceptions_flags = 0;
    memset((vm->ram + FORTH_SANDBOX_OFFSET),0,FORTH_STRBUF_SIZE);
    *copy_ptr = (size_t*)(vm->ram + FORTH_SANDBOX_OFFSET);
    size_t* in_ptr = forth_search(vm, ">IN");
    in_ptr = forth_get_variable_data_ptr(vm, in_ptr);
    *in_ptr = 0;


    strcpy((char*)vm->ram + FORTH_STRBUF_OFFSET," 50 40 + HERE TRUE");

    token = forth_get_token(vm);
    ASSERT_STREQ(token, "50");
    err = forth_compile(vm, token);
    ASSERT_EQ(err, FORTH_ERR_OK);

    token = forth_get_token(vm);
    ASSERT_STREQ(token, "40");
    err = forth_compile(vm, token);
    ASSERT_EQ(err, FORTH_ERR_OK);

    token = forth_get_token(vm);
    ASSERT_STREQ(token, "+");
    err = forth_compile(vm, token);
    ASSERT_EQ(err, FORTH_ERR_OK);

        token = forth_get_token(vm);
    ASSERT_STREQ(token, "HERE");
    err = forth_compile(vm, token);
    ASSERT_EQ(err, FORTH_ERR_OK);

    token = forth_get_token(vm);
    ASSERT_STREQ(token, "TRUE");
    err = forth_compile(vm, token);
    ASSERT_EQ(err, FORTH_ERR_OK);

    token = forth_get_token(vm);
    ASSERT_EQ(token, nullptr);
    
    sandbox = forth_search(vm, "SANDBOX");
    ASSERT_NE(sandbox, nullptr);
    copy_ptr = (size_t**)forth_get_variable_data_ptr(vm, sandbox);
    *(*copy_ptr)++ = VM_OP(VM_RET);

    vm_set_trace_cb(vm, vm_trace);
    vm_start(vm, (size_t*)(vm->ram + FORTH_SANDBOX_OFFSET));
    memset(vm->ram + FORTH_STRBUF_OFFSET,'\0',FORTH_STRBUF_SIZE);
    free(vm);
    free(ram);
    free(stack);
    free(rstack);
}

TEST(forth_colon_test, forth_compile_group) {
    vm_t* vm = (vm_t*)malloc(sizeof(vm_t));
    uint8_t* ram = (uint8_t*)malloc(8192);
    size_t* stack = (size_t*)malloc(256);
    size_t* rstack = (size_t*)malloc(256);
    vm_init(vm,ram,8192,stack,256/sizeof(size_t),rstack,256/sizeof(size_t));
    vm_set_trace_cb(vm, vm_trace);
    forth_init(vm);
    strcpy((char*)vm->ram + FORTH_STRBUF_OFFSET," 50 : BEBE 10 + ; BEBE -10 +");

    char* token = forth_get_token(vm);
    ASSERT_STREQ(token, "50");
    forth_error_t err = forth_compile(vm, token);
    ASSERT_EQ(err, FORTH_ERR_OK);

    token = forth_get_token(vm);
    ASSERT_STREQ(token, ":");
    err = forth_compile(vm, token);
    ASSERT_EQ(err, FORTH_ERR_OK);

    token = forth_get_token(vm);
    ASSERT_STREQ(token, "10");
    err = forth_compile(vm, token);
    ASSERT_EQ(err, FORTH_ERR_OK);

    token = forth_get_token(vm);
    ASSERT_STREQ(token, "+");
    err = forth_compile(vm, token);
    ASSERT_EQ(err, FORTH_ERR_OK);

    token = forth_get_token(vm);
    ASSERT_STREQ(token, ";");
    err = forth_compile(vm, token);
    ASSERT_EQ(err, FORTH_ERR_OK);
    
    token = forth_get_token(vm);
    ASSERT_STREQ(token, "BEBE");
    err = forth_compile(vm, token);
    ASSERT_EQ(err, FORTH_ERR_OK);

        token = forth_get_token(vm);
    ASSERT_STREQ(token, "-10");
    err = forth_compile(vm, token);
    ASSERT_EQ(err, FORTH_ERR_OK);

    token = forth_get_token(vm);
    ASSERT_STREQ(token, "+");
    err = forth_compile(vm, token);
    ASSERT_EQ(err, FORTH_ERR_OK);

    token = forth_get_token(vm);
    ASSERT_EQ(token, nullptr);
    size_t* sandbox = forth_search(vm, "SANDBOX");
    ASSERT_NE(sandbox, nullptr);
    size_t** copy_ptr = (size_t**)forth_get_variable_data_ptr(vm, sandbox);
    *(*copy_ptr)++ = VM_OP(VM_RET);
    vm_start(vm, (size_t*)(vm->ram + FORTH_SANDBOX_OFFSET));
    free(vm);
    free(ram);
    free(stack);
    free(rstack);
}

TEST(forth_interp_test, forth_compile_group) {
    vm_t* vm = (vm_t*)malloc(sizeof(vm_t));
    uint8_t* ram = (uint8_t*)malloc(8192);
    size_t* stack = (size_t*)malloc(256);
    size_t* rstack = (size_t*)malloc(256);
    vm_init(vm,ram,8192,stack,256/sizeof(size_t),rstack,256/sizeof(size_t));
    vm_set_trace_cb(vm, vm_trace);
    forth_init(vm);
    strcpy((char*)vm->ram + FORTH_STRBUF_OFFSET," 50 40 +");
    forth_error_t err = forth_start_compiling(vm);
    ASSERT_EQ(err, FORTH_ERR_OK);
    vm_start(vm, (size_t*)(vm->ram + FORTH_SANDBOX_OFFSET));
    forth_vm_reload(vm);

    strcpy((char*)vm->ram + FORTH_STRBUF_OFFSET," 80 +");
    err = forth_start_compiling(vm);
    ASSERT_EQ(err, FORTH_ERR_OK);
    vm_start(vm, (size_t*)(vm->ram + FORTH_SANDBOX_OFFSET));
    forth_vm_reload(vm);
    ASSERT_EQ(*vm->sp++,170);
    ASSERT_EQ(vm->sp,vm->stack_top);
    free(vm);
    free(ram);
    free(stack);
    free(rstack);
}

TEST(forth_loop_test, forth_compile_group) {
    vm_t* vm = (vm_t*)malloc(sizeof(vm_t));
    uint8_t* ram = (uint8_t*)malloc(8192);
    size_t* stack = (size_t*)malloc(1024);
    size_t* rstack = (size_t*)malloc(1024);
    vm_init(vm,ram,8192,stack,1024/sizeof(size_t),rstack,1024/sizeof(size_t));
    vm_set_trace_cb(vm, vm_trace);
    forth_init(vm);
    strcpy((char*)vm->ram + FORTH_STRBUF_OFFSET,": BEBE 10 0 DO 3 0 DO J LOOP LOOP ; BEBE");
    forth_error_t err = forth_start_compiling(vm);
    ASSERT_EQ(err, FORTH_ERR_OK);
    vm_start(vm, (size_t*)(vm->ram + FORTH_SANDBOX_OFFSET));
    forth_vm_reload(vm);
    free(vm);
    free(ram);
    free(stack);
    free(rstack);
}