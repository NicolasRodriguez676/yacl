set(YACL_TEST print)

add_executable(${PROJECT_NAME}-${YACL_TEST} print.c)

target_include_directories(${PROJECT_NAME}-${YACL_TEST} PRIVATE ${CMAKE_SOURCE_DIR}/yacl)
target_link_libraries(${PROJECT_NAME}-${YACL_TEST} YACL)

add_test(NAME "PRINT" COMMAND "${PROJECT_NAME}-${YACL_TEST}")

target_compile_options(${PROJECT_NAME}-${YACL_TEST} PRIVATE -Wall -std=c11)
