

add_library(forth forth_dict.c forth_core/forth_core.c forth_core/forth_ariphmetic.c forth_core/forth_logic.c forth_inter.c forth.c)
target_include_directories(forth PUBLIC . forth_core)
target_link_libraries(forth PUBLIC vm)
target_link_options(forth PRIVATE -nostdlib)