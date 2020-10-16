# Copyright (c) 2020 ARM Limited. All rights reserved.
# SPDX-License-Identifier: Apache-2.0

if(${MBED_TOOLCHAIN} STREQUAL "GCC_ARM")
    target_sources(mbed-rtos
        PRIVATE
            TOOLCHAIN_GCC_ARM/mbed_boot_gcc_arm.c
    )
elseif(${MBED_TOOLCHAIN} STREQUAL "ARM")
    target_sources(mbed-rtos
        PRIVATE
            TOOLCHAIN_ARM_STD/mbed_boot_arm_std.c
    )
elseif(${MBED_TOOLCHAIN} STREQUAL "IAR")
    target_sources(mbed-rtos
        PRIVATE
            TOOLCHAIN_IAR/mbed_boot_iar.c
    )
endif()

target_include_directories(mbed-rtos
    PUBLIC
        include
)

target_sources(mbed-rtos PRIVATE
    source/mbed_boot.c
    source/mbed_rtos_rtx.c
    source/mbed_rtx_handlers.c
    source/mbed_rtx_idle.cpp
)

target_compile_definitions(mbed-rtos
    PUBLIC
        MBED_CONF_RTOS_PRESENT=1
)

#if(NOT "full" IN_LIST MBED_TARGET_SUPPORTED_APPLICATION_PROFILES)
#    message(FATAL_ERROR
#        "Full profile is not supported for this target")
#endif()

# if(${MBED_C_LIB} STREQUAL "small")
#    message(FATAL_ERROR
#        "Small C libraries are not RTOS thread safe."
#    )
# endif()
