add_library(I2C STATIC)

target_sources(I2C
        PRIVATE
            i2c.c
            i2c.h
)

set_target_properties(I2C
        PROPERTIES
            PUBLIC_HEADER
            ${CMAKE_SOURCE_DIR}/comm/i2c/i2c.h
)

target_compile_options(I2C
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

