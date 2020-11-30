# Copyright (c) 2020 ARM Limited. All rights reserved.
# SPDX-License-Identifier: Apache-2.0

# Sets cpu core options
if(${MBED_TOOLCHAIN} STREQUAL "GCC_ARM")
    list(APPEND common_options
        "-mthumb"
        "-mfpu=fpv5-sp-d16"
        "-mfloat-abi=softfp"
        "-march=armv8-m.main"
        )
elseif(${MBED_TOOLCHAIN} STREQUAL "ARM")
    list(APPEND c_cxx_compile_options
        "-mcpu=cortex-m33+nodsp"
        "-mfpu=fpv5-sp-d16"
        "-mfloat-abi=hard"
        )
    list(APPEND asm_compile_options
        "-mcpu=Cortex-M33.no_dsp"
        )
    list(APPEND link_options
        "--cpu=Cortex-M33.no_dsp"
        )
endif()

function(mbed_set_cpu_core_definitions target)
    target_compile_definitions(${target}
        INTERFACE
            __CORTEX_M33
            ARM_MATH_ARMV8MML
            DOMAIN_NS=1
            __FPU_PRESENT=1U
            __CMSIS_RTOS
            __MBED_CMSIS_RTOS_CM
    )
endfunction()
