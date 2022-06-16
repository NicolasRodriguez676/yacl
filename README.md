# About
***
Yet Another Command Line (Program)

This program lets you write simple commands in a VT100 terminal to execute reads and writes to various
external devices over their respective protocols.

## Requirements 
***
Built and tested only with STM32 & WIN10

* CMake >= 3.16*
* GCC (arm-none-eabi)
* STM32Cube Package as needed
* Configurable terminal that understands basic VT100 sequences
* [stm32-cmake by ObKo](https://github.com/ObKo/stm32-cmake)
* stm32 MCU

###### \* I used the default value CLion ships with. Minimum requirement comes from ObKo's cmake scripts

## Build System
***
YACL is its own static library that can be installed or compiled within your project as a CMake subdirectory.

### CMake Environment Variables

* STM32_CUBE_\<FAMILY>_PATH - path to STM32 files
* STM32_TOOLCHAIN_PATH - path to GCC toolchain

### System Environment Variables

* CMAKE_STM32_SCRIPTS - path to ObKo's cmake scripts 
* EMBEDDED_LIBS - custom install-directory*

###### \* I do not like the idea of installing in the default path, which requires admin permissions on Windows

## Usage
***
#### CMake Install
1. configure compile options to suit your target MCU 
2. run install target in YACL project
3. in your project `find_package(YACL 0.4 REQUIRED)`
4. in your project `target_link_libraries(<TARGET> <PUBLIC/PRIVATE> YACL::YACL)`

#### CMake Subdirectory
1. Add `yacl/` source folder to your project (maybe as a submodule)
2. `add_subdirectory(yacl)` where `yacl` contains the CMakeLists.txt with the `add_library` command for YACL
3. `target_include_directories(<TARGET> <PUBLIC/PRIVATE> yacl)`
4. `target_link_libraries(<TARGET> <PUBLIC/PRIVATE> YACL)`

#### Code

1. include the header file `#include "yacl.h"`
2. provide the corresponding protocol functions 

```C
void usr_<protocol>_read(yacl_inout_data_t* inout_data);
void usr_<protocol>_write(yacl_inout_data_t* inout_data);
```
3. make a callback struct, fill in members and initialize

```C
yacl_usr_callbacks_t callbacks;
yacl_set_cb_null(&callbacks);       // helper function for ensuring callback validity

callbacks.usr_print_funcs.usr_printf = printf;

callbacks.usr_<protocol>_read = usr_<protocol>_read;
callbacks.usr_<protocol>_write = usr_<protocol>_write;

yacl_init(&callbacks);
```

5. write to YACL's input buffer by calling `yacl_wr_buf` inside UART IRQ and passing YACL the received data
6. simply call `yacl_parse_cmd` periodically to run the terminal to accept commands

#### Terminal

1. command signature
```
<protocol>  <action>  <device address>  <register address>  <optional address end>  <data>
```

2. examples
```
>> i2c  write  0x70  0x1f45  255
>> i2c  write  0x70  0x1f45  0x0 255
>> i2c  read  0x70  0x1f45

0x1f45 ::  255
```

#### Customizing YACL
1. since all memory allocated in YACL is static, the read buffer size must be known at compile time. Passing CMake `YACL_USR_INOUT_BFR_LEN` a value will override the default of 16.
2. if using YACL as a subdirectory, defining `YACL_USR_INOUT_BFR_LEN` before including `yacl.h` will work as well

#### YACL Is Not
* A library that provides I2C, SPI or any other protocol support. It merely calls your implementations with associated data (i2c device address, for example)
* A command line interface with full keyboard support. Only backspace is supported properly.
* Properly tested (do not try them, they will not work)

### Future Goals
***
* I want to implement various forms of plotting. Eventually real time plotting. (A major reason I bothered with VT100 escape sequences, other than backspace)
* Redesign YACL with a proper statemachine
* More keyboard support for arrow keys
* Proper unit test suite
