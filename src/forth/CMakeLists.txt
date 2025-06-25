add_library(forth forth_dict.c)
target_include_directories(forth PUBLIC .)
target_link_libraries(forth PUBLIC vm)