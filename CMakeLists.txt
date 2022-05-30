cmake_minimum_required(VERSION 3.22)

option(BUILD-EMBEDDED "build library using stm32 gcc toolchain" ON)

if (${BUILD-EMBEDDED})
    set(CMAKE_TOOLCHAIN_FILE $ENV{CMAKE_STM32_SCRIPTS}/stm32_gcc.cmake)
    set(CMAKE_INSTALL_PREFIX $ENV{EMBEDDED_LIBS})
endif()

project(YACL C ASM)

add_library(yacl STATIC)
target_sources(yacl PRIVATE yacl/yacl.c yacl/yacl.h)

if (${BUILD-EMBEDDED})
    target_compile_options(yacl
            PUBLIC
                -mcpu=cortex-m4
                -mfpu=fpv4-sp-d16
                -mfloat-abi=hard
                -Wall
            PRIVATE
                -std=c11
            )

    install(
            TARGETS
                yacl
            DESTINATION
                ${PROJECT_NAME}/${CMAKE_BUILD_TYPE}/lib
    )

    install(
            FILES
                yacl/yacl.h
            DESTINATION
                ${PROJECT_NAME}/${CMAKE_BUILD_TYPE}/include
    )

else()
    target_compile_options(yacl
            PUBLIC
                -Wall
                -std=c11
            )

    add_subdirectory(tests)
endif()
