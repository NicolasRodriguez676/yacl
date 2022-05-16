set(YACL_TEST token)

add_executable(${YACL_TEST} token.c)

target_include_directories(${YACL_TEST} PRIVATE ${PROJECT_SOURCE_DIR}/yacl)
target_link_libraries(${YACL_TEST} yacl)

add_test(NAME "YaclTest-TOK11" COMMAND "${YACL_TEST}" "11" "cmd arg1 arg2 arg3 arg4 arg5 arg6 arg7 arg8 arg9 arg10")
add_test(NAME "YaclTest-TOK12" COMMAND "${YACL_TEST}" "12" "cmd arg1 arg2 arg3 arg4 arg5 arg6 arg7 arg8 arg9 arg10 arg11")
add_test(NAME "YaclTest-TOK13" COMMAND "${YACL_TEST}" "13" "cmd arg1 arg2 arg3 arg4 arg5 arg6 arg7 arg8 arg9 arg10 arg11 arg12")