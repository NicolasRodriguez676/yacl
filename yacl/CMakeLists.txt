add_library(yacl STATIC)
target_sources(yacl PRIVATE yacl.c yacl.h)

target_compile_options(yacl PRIVATE -Wall)
