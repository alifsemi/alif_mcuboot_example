if (TOOLCHAIN STREQUAL "gcc")
  set(TOOLCHAIN_COMMON_FLAGS
    -mthumb
    -mcpu=cortex-m55
    -mfloat-abi=hard
    -mfpu=fpv5-sp-d16
    )

  # Adjust this if you’re targeting secure/non-secure or RTOS differences
  set(FREERTOS_PORT GCC_ARM_CM55_NTZ_NONSECURE CACHE INTERNAL "")

elseif (TOOLCHAIN STREQUAL "clang")
  set(TOOLCHAIN_COMMON_FLAGS
    --target=arm-none-eabi
    -mcpu=cortex-m55
    -mfpu=fpv5-sp-d16
    )

  set(FREERTOS_PORT GCC_ARM_CM55_NTZ_NONSECURE CACHE INTERNAL "")

elseif (TOOLCHAIN STREQUAL "iar")
  set(TOOLCHAIN_COMMON_FLAGS
    --cpu cortex-m55
    --fpu VFPv5-SP
    )

  set(FREERTOS_PORT IAR_ARM_CM55_NTZ_NONSECURE CACHE INTERNAL "")

endif ()