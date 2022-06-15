cmake_minimum_required(VERSION 3.22)

option(BUILD-STM32 "build library using gcc toolchain for STM32" ON)
option(INSTALL-YACL "make an install target" ON)

if (${BUILD-STM32})
    set(CMAKE_TOOLCHAIN_FILE $ENV{CMAKE_STM32_SCRIPTS}/stm32_gcc.cmake)
endif()

if (DEFINED ENV{EMBEDDED_LIBS})
    set(CMAKE_INSTALL_PREFIX $ENV{EMBEDDED_LIBS})
endif()

project(YACL
        LANGUAGES
            C
            ASM
        VERSION
            0.4)


add_subdirectory(yacl)

if (${INSTALL-YACL})
    include(cmake/InstallLibs.cmake)
    install_lib(YACL)

# tests do not work. anymore. maybe i will figure out how to make
# some more and know exactly what to test
# else()
#     add_subdirectory(tests)
endif()
