add_library(${PROJECT_NAME} STATIC)

target_sources(${PROJECT_NAME}
        PRIVATE
            yacl.c
            yacl.h
            vt100/vt100.c
            vt100/vt100.h
)

set_target_properties(${PROJECT_NAME}
        PROPERTIES
            PUBLIC_HEADER
            ${CMAKE_SOURCE_DIR}/yacl/yacl.h
)

target_compile_options(${PROJECT_NAME}
    PUBLIC
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