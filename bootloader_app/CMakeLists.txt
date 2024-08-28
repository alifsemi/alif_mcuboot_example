# Copyright (c) 2024 Alif Semiconductor. All rights reserved.

# MCUBoot port for Alif baremetal

cmake_minimum_required(VERSION 3.19.0 FATAL_ERROR)

set(BL_APP_TARGET alif-mcuboot)

# Set up the project
project(alif-mcuboot-${ENSEMBLE_CORE}
    VERSION 1.0.0
    LANGUAGES C CXX ASM
)

add_executable(${BL_APP_TARGET})

set_target_properties(${BL_APP_TARGET}
        PROPERTIES
        SUFFIX ".elf"
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    )

target_sources(${BL_APP_TARGET} PRIVATE
    src/main.c
    ${MCUBOOT_DIR}/boot/alif/keys.c
    ${MCUBOOT_DIR}/boot/alif/flash/flash_map_mram.c
)

target_link_libraries(${BL_APP_TARGET} PRIVATE
    ${LIB_BOOTUTIL}
    common_app_utils
)

target_link_options(${BL_APP_TARGET} PRIVATE
    -T ${CMAKE_CURRENT_SOURCE_DIR}/linker_files/gcc_M55_HE.ld
    "SHELL:-Xlinker -Map=${CMAKE_BINARY_DIR}/bin/${BL_APP_TARGET}.map"
)

add_custom_command(TARGET ${BL_APP_TARGET} POST_BUILD
    COMMAND arm-none-eabi-objcopy -O binary ${BL_APP_TARGET}.elf ${BL_APP_TARGET}.bin
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)
