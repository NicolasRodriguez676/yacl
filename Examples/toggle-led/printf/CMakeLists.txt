target_sources(toggle-led
        PRIVATE
            printf.c
            printf.h
)

target_compile_options(toggle-led
        PRIVATE
        -DPRINTF_DISABLE_SUPPORT_EXPONENTIAL
        -DPRINTF_DISABLE_SUPPORT_LONG_LONG
)