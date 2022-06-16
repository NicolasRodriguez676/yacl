cmake_minimum_required(VERSION 3.22)

if (NOT DEFINED ENV{CMAKE_STM32_SCRIPTS})
    message(FATAL_ERROR "Could not find STM32 CMake scripts path")
endif()

set(CMAKE_MODULE_PATH  $ENV{CMAKE_STM32_SCRIPTS})
set(CMAKE_TOOLCHAIN_FILE $ENV{CMAKE_STM32_SCRIPTS}/stm32_gcc.cmake)

project(App C ASM)

find_package(CMSIS COMPONENTS STM32F4 REQUIRED)
find_package(HAL COMPONENTS "STM32F4 LL_RCC LL_GPIO LL_USART CORTEX" REQUIRED)

add_compile_options(
        -mcpu=cortex-m4
        -mfpu=fpv4-sp-d16
        -mfloat-abi=hard
        -fno-builtin-printf
        -DUSE_FULL_LL_DRIVER
        -O2
        -std=c11
        -Wall
        -Wextra
        -Werror=implicit-function-declaration
        -Wno-unused-parameter
)

add_subdirectory(vendor/printf)
add_subdirectory(vendor/yacl/yacl)

add_executable(App App/App.c App/App.h)
target_include_directories(App PUBLIC App/ vendor/printf vendor/yacl/yacl)

target_link_libraries(App
        PUBLIC
        HAL::STM32::F4::LL_RCC
        HAL::STM32::F4::LL_GPIO
        HAL::STM32::F4::LL_USART
        HAL::STM32::F4::CORTEX
        CMSIS::STM32::F446RE
        STM32::NoSys
        PRINTF
        YACL
)

stm32_print_size_of_target(App)
stm32_generate_binary_file(App)