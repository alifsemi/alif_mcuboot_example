# Copyright (c) 2024 Alif Semiconductor. All rights reserved.

cmake_minimum_required(VERSION 3.19.0 FATAL_ERROR)

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

set(ALIF_BOARD "DevKit-e7" CACHE STRING "Set ALIF_BOARD to: DevKit-e1c, DevKit-e7, DevKit-e8, EngKit-e8-CSP")


if(NOT CMAKE_BUILD_TYPE)
    message(STATUS "Setting build type to Release.")
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Build type (Release or Debug)" FORCE)
else()
    message(STATUS "Build type was set to ${CMAKE_BUILD_TYPE}.")
endif()

set(MCUBOOT_MODE "SWAP" CACHE STRING "MCUboot mode selection. Either SWAP or RAMLOAD.")
set_property(CACHE MCUBOOT_MODE PROPERTY STRINGS "SWAP" "RAMLOAD")

add_compile_definitions(
    MCUBOOT_MODE_${MCUBOOT_MODE}
)

set(HE_UPDATES_BOTH FALSE CACHE BOOL "Set single updater mode. In single updater mode HE core updates both binaries.")

if(HE_UPDATES_BOTH)
    add_compile_definitions(
        HE_UPDATES_BOTH=1
    )
endif()

set(HE_UPDATES_SERAM FALSE CACHE BOOL "Enable SERAM update from HE bootloader.")

if (HE_UPDATES_SERAM AND NOT HE_UPDATES_BOTH)
    message(FATAL_ERROR "SERAM update is only supported when HE bootloader handles all cores (HE_UPDATES_BOTH=1).")
endif()


if(HE_UPDATES_SERAM)
    add_compile_definitions(
        HE_UPDATES_SERAM=1
    )
endif()

set(LIBRARIES_DIR ${CMAKE_CURRENT_SOURCE_DIR}/libs)

# Set up the project
project(alif-mcuboot-example
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
include(${LIBRARIES_DIR}/mbedtls.cmake)
include(${LIBRARIES_DIR}/mcuboot.cmake)
include(${LIBRARIES_DIR}/tinycbor.cmake)
include(${LIBRARIES_DIR}/mcumgr.cmake)
include(${LIBRARIES_DIR}/tinyusb.cmake)

add_subdirectory(bootloader_app)
add_subdirectory(example_app)
add_subdirectory(blinky_app)
add_subdirectory(seram_update)
