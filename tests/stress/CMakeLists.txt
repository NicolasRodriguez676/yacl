set(YACL_TEST stress)

add_executable(${YACL_TEST} stress.c)

target_include_directories(${YACL_TEST} PRIVATE ${PROJECT_SOURCE_DIR}/yacl)
target_link_libraries(${YACL_TEST} yacl)

add_test(NAME "STRESS-100K" COMMAND "${YACL_TEST}")

target_compile_options(${YACL_TEST} PRIVATE -Wall)
