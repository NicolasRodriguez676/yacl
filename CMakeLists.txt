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
            0.3)

add_subdirectory(yacl)

if (${BUILD-EMBEDDED})
    include(cmake/InstallLibs.cmake)

    if (${CMAKE_BUILD_TYPE} MATCHES "^[Rr]elease")
        install_lib(${PROJECT_NAME})
    endif()
else()
    add_subdirectory(tests)
endif()
