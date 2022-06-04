set(YACL_TEST stress)

add_executable(${PROJECT_NAME}-${YACL_TEST} stress.c)

target_include_directories(${PROJECT_NAME}-${YACL_TEST} PRIVATE ${CMAKE_SOURCE_DIR}/yacl)
target_link_libraries(${PROJECT_NAME}-${YACL_TEST} YACL)

add_test(NAME "STRESS-100K" COMMAND "${PROJECT_NAME}-${YACL_TEST}")

target_compile_options(${PROJECT_NAME}-${YACL_TEST} PRIVATE -Wall -std=c11)
