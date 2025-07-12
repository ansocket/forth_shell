cmake_minimum_required(VERSION 3.28)
project(forth)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_compile_options(-Wall -m32)
add_link_options(-m32)

include(CTest)
if(BUILD_TESTING)
     add_compile_options(-g)
     add_subdirectory(tests)
    add_subdirectory(examples)
endif()

add_subdirectory(src)