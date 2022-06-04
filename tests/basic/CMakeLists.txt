set(YACL_TEST basic)

add_executable(${PROJECT_NAME}-${YACL_TEST} basic.c)

target_include_directories(${PROJECT_NAME}-${YACL_TEST} PRIVATE ${CMAKE_SOURCE_DIR}/yacl)
target_link_libraries(${PROJECT_NAME}-${YACL_TEST} YACL)

add_test(NAME "HELP0" COMMAND  "${PROJECT_NAME}-${YACL_TEST}" "help")
add_test(NAME "HELP1" COMMAND  "${PROJECT_NAME}-${YACL_TEST}" "help ")
add_test(NAME "HELP2" COMMAND  "${PROJECT_NAME}-${YACL_TEST}" "help arg1 arg2")
add_test(NAME "READ" COMMAND   "${PROJECT_NAME}-${YACL_TEST}" "read hello there")
add_test(NAME "WRITE" COMMAND  "${PROJECT_NAME}-${YACL_TEST}" "write hello there")

target_compile_options(${PROJECT_NAME}-${YACL_TEST} PRIVATE -Wall -std=c11)