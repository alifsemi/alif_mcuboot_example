# Copyright (c) 2024 Alif Semiconductor. All rights reserved.

cmake_minimum_required(VERSION 3.19.0 FATAL_ERROR)

set(ENSEMBLE_CORE "M55_HE" CACHE STRING "Ensemble core selection")
set_property(CACHE ENSEMBLE_CORE PROPERTY STRINGS "M55_HE" "M55_HP")

if (${ENSEMBLE_CORE} STREQUAL "M55_HP")
    add_compile_definitions(
        _RTE_
        M55_HP
    )
elseif (${ENSEMBLE_CORE} STREQUAL "M55_HE")
    add_compile_definitions(
        _RTE_
        M55_HE
    )
else()
    message(FATAL_ERROR "Unsupported ENSEMBLE_CORE value!")
endif()

set(CMAKE_C_COMPILER                arm-none-eabi-gcc)
set(CMAKE_CXX_COMPILER              arm-none-eabi-g++)
SET(CMAKE_LINKER                    arm-none-eabi-ld)
set(CMAKE_CROSSCOMPILING            true)
set(CMAKE_SYSTEM_NAME               Generic)

set(CMAKE_SYSTEM_PROCESSOR          "cortex-m55")
set(CMAKE_C_FLAGS                   "-mcpu=cortex-m55")
set(CMAKE_CXX_FLAGS                 "-mcpu=cortex-m55")
set(CMAKE_ASM_CPU_FLAG              cortex-m55)
set(CPU_COMPILE_DEF                 CPU_CORTEX_M55)

set(CPU_NAME                        ${CMAKE_SYSTEM_PROCESSOR})
set(CMAKE_C_COMPILER_WORKS          1)
set(CMAKE_CXX_COMPILER_WORKS        1)

set(CMAKE_C_FLAGS_DEBUG            "-Og -g"          CACHE STRING "Flags used by the C compiler during DEBUG builds.")
set(CMAKE_C_FLAGS_MINSIZEREL       "-Os -g -DNDEBUG" CACHE STRING "Flags used by the C compiler during MINSIZEREL builds.")
set(CMAKE_C_FLAGS_RELEASE          "-O2 -g -DNDEBUG" CACHE STRING "Flags used by the C compiler during RELEASE builds.")

set(CMAKE_CXX_FLAGS_DEBUG          "-Og -g"          CACHE STRING "Flags used by the CXX compiler during DEBUG builds.")
set(CMAKE_CXX_FLAGS_MINSIZEREL     "-Os -g -DNDEBUG" CACHE STRING "Flags used by the CXX compiler during MINSIZEREL builds.")
set(CMAKE_CXX_FLAGS_RELEASE        "-O2 -g -DNDEBUG" CACHE STRING "Flags used by the CXX compiler during RELEASE builds.")

add_compile_options(
    -mfloat-abi=hard
    -Wall
    -Wno-unused-function
    -Wextra
    -Wvla
    -Wno-error=cpp
    -c
    -Werror
    -fdata-sections
    -ffunction-sections
    -fshort-enums
    -funsigned-char
    $<$<COMPILE_LANGUAGE:C>:-std=c99>
    $<$<COMPILE_LANGUAGE:CXX>:-std=c++11>
    -MD
)

add_link_options(
    -mthumb
    -mfloat-abi=hard
    -mlittle-endian
    -specs=nosys.specs
    -specs=nano.specs
    -Wl,--print-memory-usage
    -Wl,--gc-sections
    --entry=Reset_Handler
)

set(LIB_BOOTUTIL bootutil)
set(LIBRARIES_DIR ${CMAKE_CURRENT_SOURCE_DIR}/libs)
set(MBEDTLS_DIR ${LIBRARIES_DIR}/mbedtls)
set(MCUBOOT_DIR ${LIBRARIES_DIR}/mcuboot)

# Set up the project
project(alif-mcuboot-example-${ENSEMBLE_CORE}
    VERSION 1.0.0
    LANGUAGES C CXX ASM
)

add_library(rte_interface INTERFACE)
target_include_directories(rte_interface INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/RTE
)

include(${LIBRARIES_DIR}/cmsis.cmake)

set(CMSIS_SRC_PATH ${LIBRARIES_DIR}/CMSIS)
set(CMSIS_DSP_SRC_PATH ${LIBRARIES_DIR}/CMSIS-dsp)
include(${LIBRARIES_DIR}/cmsis-dfp.cmake)
include(${LIBRARIES_DIR}/common_app_utils.cmake)

add_subdirectory(${MBEDTLS_DIR} EXCLUDE_FROM_ALL)
target_compile_definitions(mbedcrypto PRIVATE
    MBEDTLS_CONFIG_FILE="alif_mbedtls_config.h"
)
target_include_directories(mbedcrypto PRIVATE
    mbedtls_config
)

add_subdirectory(${MCUBOOT_DIR}/boot/bootutil EXCLUDE_FROM_ALL)
target_include_directories(bootutil PUBLIC
    ${MCUBOOT_DIR}/boot/alif/include
)

target_link_libraries(${LIB_BOOTUTIL}
    PRIVATE
        mbedcrypto
)

add_subdirectory(bootloader_app)
add_subdirectory(example_app)

