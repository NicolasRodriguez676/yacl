add_library(YACL STATIC)

target_sources(YACL PRIVATE yacl.c yacl.h)

add_subdirectory(vt100)
add_subdirectory(parser)

target_include_directories(YACL PRIVATE vt100 parser)

set_target_properties(YACL
        PROPERTIES
            PUBLIC_HEADER ${CMAKE_SOURCE_DIR}/yacl/yacl.h
        )

target_compile_options(YACL
        PRIVATE
            -O2
            -std=c11
            -Wall
            -Wextra
            -Werror=implicit-function-declaration
            -Wno-unused-parameter
        )

if (${BUILD_FOR_STM32F4})
    target_compile_options(YACL
        PUBLIC
            -mcpu=cortex-m4
            -mfpu=fpv4-sp-d16
            -mfloat-abi=hard
        )
endif()