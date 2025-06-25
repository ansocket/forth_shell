
set(TEST_COMPILE_FLAGS -g)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.17.0.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

add_executable(vm_test vm_test.cpp)
target_link_libraries(vm_test PRIVATE GTest::gtest_main vm)
target_compile_options(vm_test PRIVATE ${TEST_COMPILE_FLAGS})
add_test(NAME vm_test COMMAND vm_test)

add_executable(forth_dict_test forth_dict_test.cpp)
target_link_libraries(forth_dict_test PRIVATE GTest::gtest_main forth)
target_compile_options(forth_dict_test PRIVATE ${TEST_COMPILE_FLAGS})
add_test(NAME forth_dict_test COMMAND forth_dict_test)