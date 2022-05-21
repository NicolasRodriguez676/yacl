set(YACL_TEST print)

add_executable(${YACL_TEST} print.c)

target_include_directories(${YACL_TEST} PRIVATE ${PROJECT_SOURCE_DIR}/yacl)
target_link_libraries(${YACL_TEST} yacl)

add_test(NAME "PRINT" COMMAND "${YACL_TEST}")

target_compile_options(${YACL_TEST} PRIVATE -Wall)
