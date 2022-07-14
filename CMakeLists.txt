cmake_minimum_required(VERSION 3.22)

option(BUILD_FOR_STM32F4 "build library using stm32f4 compile options" ON)
option(INSTALL_YACL      "install YACL on host machine"                ON)

set(CMAKE_TOOLCHAIN_FILE $ENV{CMAKE_STM32_SCRIPTS}/stm32_gcc.cmake)

if (DEFINED ENV{EMBEDDED_LIBS})
    set(CMAKE_INSTALL_PREFIX $ENV{EMBEDDED_LIBS})
endif()

project(YACL
        LANGUAGES
            C
            ASM
        VERSION
            1.0
        )

add_subdirectory(yacl)

if (${INSTALL-YACL})
    include(cmake/InstallLibs.cmake)
    install_lib(YACL)

# tests do not work. anymore. maybe i will figure out how to make
# some more and know exactly what to test
# else()
#     add_subdirectory(tests)
endif()
