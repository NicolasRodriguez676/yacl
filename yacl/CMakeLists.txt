add_library(yacl STATIC)
target_sources(yacl PRIVATE yacl.c yacl.h)

if (DEFINED ENV{WINDOWS-BUILD})
    target_compile_options(yacl PRIVATE -Wall)

endif ()

if (DEFINED ENV{EMBEDDED-BUILD})
    target_compile_options(yacl PRIVATE -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard -Wall)
    target_link_options(yacl PRIVATE -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard)

endif ()
