set(YACL_TEST overrun)

add_executable(${PROJECT_NAME}-${YACL_TEST} overrun.c)

target_include_directories(${PROJECT_NAME}-${YACL_TEST} PRIVATE ${CMAKE_SOURCE_DIR}/yacl)
target_link_libraries(${PROJECT_NAME}-${YACL_TEST} YACL)

add_test(NAME "OVERRUN" COMMAND "${PROJECT_NAME}-${YACL_TEST}")

target_compile_options(${PROJECT_NAME}-${YACL_TEST} PRIVATE -Wall -std=c11)