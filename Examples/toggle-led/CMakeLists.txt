cmake_minimum_required(VERSION 3.21)

set(CMAKE_MODULE_PATH  $ENV{CMAKE_STM32_SCRIPTS})
set(CMAKE_TOOLCHAIN_FILE $ENV{CMAKE_STM32_SCRIPTS}/stm32_gcc.cmake)
set(CMAKE_INCLUDE_CURRENT_DIR TRUE)

list(APPEND CMAKE_PREFIX_PATH "$ENV{EMBEDDED_LIBS}")

project(toggle-led C ASM)

find_package(CMSIS COMPONENTS STM32F4 REQUIRED)
find_package(HAL COMPONENTS "STM32F4 LL_RCC LL_GPIO LL_USART CORTEX" REQUIRED)
find_package(YACL 0.4 REQUIRED)

add_executable(${PROJECT_NAME} toggle-led.c stm32f4xx_setup.h stm32f4xx_hal_conf.h)

add_subdirectory(printf)

target_include_directories(${PROJECT_NAME} PRIVATE printf)

target_link_libraries(${PROJECT_NAME}
        PUBLIC
            HAL::STM32::F4::LL_RCC
            HAL::STM32::F4::LL_GPIO
            HAL::STM32::F4::LL_USART
            HAL::STM32::F4::CORTEX
            CMSIS::STM32::F446RE
            STM32::NoSys
            YACL::YACL
        )

target_compile_options(${PROJECT_NAME}
        PUBLIC
            -fno-builtin-printf
            -O2
            -DUSE_FULL_LL_DRIVER
            -Wall
            -std=c11
        )

stm32_print_size_of_target(${PROJECT_NAME})
stm32_generate_binary_file(${PROJECT_NAME})

#add_custom_target(flash
#        COMMAND
#            st-flash write ${CMAKE_BINARY_DIR}/${PROJECT_NAME}.bin 0x8000000
#        COMMENT
#            "flash binary file to MCU"
#        )
#
#add_dependencies(flash ${PROJECT_NAME})
