set(YACL_TEST token)

add_executable(${PROJECT_NAME}-${YACL_TEST} token.c)

target_include_directories(${PROJECT_NAME}-${YACL_TEST} PRIVATE ${CMAKE_SOURCE_DIR}/yacl)
target_link_libraries(${PROJECT_NAME}-${YACL_TEST} YACL)

add_test(NAME "TOK11" COMMAND "${PROJECT_NAME}-${YACL_TEST}" "11" "cmd arg1 arg2 arg3 arg4 arg5 arg6 arg7 arg8 arg9 arg10")
add_test(NAME "TOK12" COMMAND "${PROJECT_NAME}-${YACL_TEST}" "12" "cmd arg1 arg2 arg3 arg4 arg5 arg6 arg7 arg8 arg9 arg10 arg11")
add_test(NAME "TOK13" COMMAND "${PROJECT_NAME}-${YACL_TEST}" "13" "cmd arg1 arg2 arg3 arg4 arg5 arg6 arg7 arg8 arg9 arg10 arg11 arg12")

target_compile_options(${PROJECT_NAME}-${YACL_TEST} PRIVATE -Wall)