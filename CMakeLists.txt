cmake_minimum_required(VERSION 3.22)

option(BUILD-EMBEDDED "build library using stm32 gcc toolchain" ON)

if (${BUILD-EMBEDDED})
    set(CMAKE_TOOLCHAIN_FILE $ENV{CMAKE_STM32_SCRIPTS}/stm32_gcc.cmake)
    set(CMAKE_INSTALL_PREFIX $ENV{EMBEDDED_LIBS})
endif()

project(YACL
        LANGUAGES
            C
            ASM
        VERSION
            0.1)

add_library(${PROJECT_NAME} STATIC)
target_sources(${PROJECT_NAME} PRIVATE yacl/yacl.c yacl/yacl.h)
set_target_properties(${PROJECT_NAME} PROPERTIES PUBLIC_HEADER yacl/yacl.h)

target_compile_options(${PROJECT_NAME}
        PRIVATE
            -O2
            -std=c11
            -Wall
            -Wextra
            -Werror=implicit-function-declaration
            -Wno-unused-parameter
        )

if (${BUILD-EMBEDDED})
    target_compile_options(${PROJECT_NAME}
            PUBLIC
                -mcpu=cortex-m4
                -mfpu=fpv4-sp-d16
                -mfloat-abi=hard
            )

    if (${CMAKE_BUILD_TYPE} MATCHES "^[Rr]elease")
        install(
                TARGETS
                    ${PROJECT_NAME}
                EXPORT
                    ${PROJECT_NAME}
                DESTINATION
                    lib/${PROJECT_NAME}-${PROJECT_VERSION}/${CMAKE_BUILD_TYPE}
                PUBLIC_HEADER DESTINATION
                    include/${PROJECT_NAME}-${PROJECT_VERSION}
        )

        install(
                EXPORT
                    ${PROJECT_NAME}
                NAMESPACE
                    ${PROJECT_NAME}::
                DESTINATION
                    lib/${PROJECT_NAME}-${PROJECT_VERSION}/${CMAKE_BUILD_TYPE}
        )

        configure_file(cmake/YACLConfig-version.cmake.in ${CMAKE_CURRENT_BINARY_DIR}/YACLConfig-version.cmake @ONLY)

        install(
                FILES
                    cmake/YACLConfig.cmake
                    ${CMAKE_CURRENT_BINARY_DIR}/YACLConfig-version.cmake
                DESTINATION
                    lib/${PROJECT_NAME}-${PROJECT_VERSION}
        )

        target_include_directories(${PROJECT_NAME} PUBLIC $<INSTALL_INTERFACE:include/${PROJECT_NAME}-${PROJECT_VERSION}>)

    endif()

else()
    add_subdirectory(tests)
endif()
