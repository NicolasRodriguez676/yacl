cmake_minimum_required(VERSION 3.22)
project(yacl_r03 C)

set(CMAKE_C_STANDARD 11)

#add_executable(yacl_r03 main.c)

#target_include_directories(yacl_r03 PRIVATE yacl)

add_subdirectory(yacl)
add_subdirectory(tests)

#target_link_libraries(yacl_r03 yacl)
