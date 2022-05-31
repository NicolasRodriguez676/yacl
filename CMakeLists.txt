cmake_minimum_required(VERSION 3.22)

option(BUILD-EMBEDDED "build library using stm32 gcc toolchain" ON)

if (${BUILD-EMBEDDED})
    set(CMAKE_TOOLCHAIN_FILE $ENV{CMAKE_STM32_SCRIPTS}/stm32_gcc.cmake)
    set(CMAKE_INSTALL_PREFIX $ENV{EMBEDDED_LIBS})
endif()

project(YACL C ASM)

add_library(${PROJECT_NAME} STATIC)
target_sources(${PROJECT_NAME} PRIVATE yacl/yacl.c yacl/yacl.h)

if (${BUILD-EMBEDDED})
    target_compile_options(${PROJECT_NAME}
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
                ${PROJECT_NAME}
            EXPORT
                ${PROJECT_NAME}
            DESTINATION
                lib/${PROJECT_NAME}/${CMAKE_BUILD_TYPE}
    )

    install(
            EXPORT
                ${PROJECT_NAME}
            NAMESPACE
                ${PROJECT_NAME}::
            DESTINATION
                lib/${PROJECT_NAME}/${CMAKE_BUILD_TYPE}
    )

    install(
            FILES
                yacl/yacl.h
            DESTINATION
                include/${PROJECT_NAME}
    )

    install(
            FILES
                cmake/YACLConfig.cmake
            DESTINATION
                lib/${PROJECT_NAME}
    )

    target_include_directories(${PROJECT_NAME} PUBLIC $<INSTALL_INTERFACE:include/${PROJECT_NAME}>)

else()
    target_compile_options(${PROJECT_NAME}
            PUBLIC
                -Wall
                -std=c11
            )

    add_subdirectory(tests)
endif()
