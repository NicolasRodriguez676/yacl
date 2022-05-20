set(YACL_TEST basic)

add_executable(${YACL_TEST} basic.c)

target_include_directories(${YACL_TEST} PRIVATE ${PROJECT_SOURCE_DIR}/yacl)
target_link_libraries(${YACL_TEST} yacl)

add_test(NAME "YaclTest-HELP0" COMMAND "${YACL_TEST}" "help")
add_test(NAME "YaclTest-HELP1" COMMAND "${YACL_TEST}" "help ")
add_test(NAME "YaclTest-HELP2" COMMAND "${YACL_TEST}" "help arg1 arg2")
add_test(NAME "YaclTest-READ" COMMAND "${YACL_TEST}" "read hello there")
add_test(NAME "YaclTest-WRITE" COMMAND "${YACL_TEST}" "write hello there")
