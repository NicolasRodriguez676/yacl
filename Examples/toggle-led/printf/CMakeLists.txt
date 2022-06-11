target_sources(${PROJECT_NAME}
        PRIVATE
            printf.c
            printf.h
)

target_compile_options(${PROJECT_NAME}
        PRIVATE
        -DPRINTF_DISABLE_SUPPORT_EXPONENTIAL
        -DPRINTF_DISABLE_SUPPORT_LONG_LONG
)