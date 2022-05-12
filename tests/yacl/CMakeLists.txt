add_executable(yacl_test yacl_test.c)

target_include_directories(yacl_test PRIVATE ${PROJECT_SOURCE_DIR}/yacl)
target_link_libraries(yacl_test yacl)

add_test(NAME "YaclTest-HELP" COMMAND "yacl_test" "help hello there\n testing!")
add_test(NAME "YaclTest-READ" COMMAND "yacl_test" "read hello there\n testing!")
add_test(NAME "YaclTest-WRITE" COMMAND "yacl_test" "write hello there\n testing!")