add_library(${PROJECT_NAME} STATIC)

target_sources(${PROJECT_NAME}
        PRIVATE
            yacl.c
            yacl.h
)

add_subdirectory(vt100)

target_include_directories(${PROJECT_NAME} PRIVATE vt100)

set_target_properties(${PROJECT_NAME}
        PROPERTIES
            PUBLIC_HEADER ${CMAKE_SOURCE_DIR}/yacl/yacl.h
)

target_compile_options(${PROJECT_NAME}
    PUBLIC
        -fno-builtin-printf
        -mcpu=cortex-m4
        -mfpu=fpv4-sp-d16
        -mfloat-abi=hard
    PRIVATE
        -O2
        -std=c11
        -Wall
        -Wextra
        -Werror=implicit-function-declaration
        -Wno-unused-parameter
)