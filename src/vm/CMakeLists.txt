add_library(vm vm.c)
target_include_directories(vm PUBLIC .)
target_link_options(vm PRIVATE -nostdlib)