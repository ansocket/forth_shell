add_library(forth forth_dict.c forth_core.c forth_inter.c forth.c)
target_include_directories(forth PUBLIC .)
target_link_libraries(forth PUBLIC vm)