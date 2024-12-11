# Copyright (c) 2024 Alif Semiconductor. All rights reserved.

# MCUBoot port for Alif baremetal

cmake_minimum_required(VERSION 3.19.0 FATAL_ERROR)

set(BL_APP_TARGET_HE alif-mcuboot-he)
set(BL_APP_TARGET_HP alif-mcuboot-hp)

# Set up the project
project(alif-mcuboot-${ENSEMBLE_CORE}
    VERSION 1.0.0
    LANGUAGES C CXX ASM
)

add_executable(${BL_APP_TARGET_HE})

set_target_properties(${BL_APP_TARGET_HE}
        PROPERTIES
        SUFFIX ".elf"
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    )

target_sources(${BL_APP_TARGET_HE} PRIVATE
    src/main_he.c
    src/uart_empty_fix.c
    ${MCUBOOT_DIR}/boot/alif/keys.c
)

target_link_libraries(${BL_APP_TARGET_HE} PRIVATE
    ${LIB_BOOTUTIL_HE}
    common_app_utils_he
    ensemblecmsis_he
)

target_compile_definitions(${BL_APP_TARGET_HE} PRIVATE
    BL_HE
)

target_link_options(${BL_APP_TARGET_HE} PRIVATE
    -T ${CMAKE_CURRENT_SOURCE_DIR}/linker_files/gcc_M55_HE.ld
    "SHELL:-Xlinker -Map=${CMAKE_BINARY_DIR}/bin/${BL_APP_TARGET_HE}.map"
)

add_custom_command(TARGET ${BL_APP_TARGET_HE} POST_BUILD
    COMMAND arm-none-eabi-objcopy -O binary ${BL_APP_TARGET_HE}.elf ${BL_APP_TARGET_HE}.bin
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

add_executable(${BL_APP_TARGET_HP})

set_target_properties(${BL_APP_TARGET_HP}
        PROPERTIES
        SUFFIX ".elf"
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    )

target_sources(${BL_APP_TARGET_HP} PRIVATE
    src/main_hp.c
    src/uart_empty_fix.c
    ${MCUBOOT_DIR}/boot/alif/keys.c
)

target_link_libraries(${BL_APP_TARGET_HP} PRIVATE
    ${LIB_BOOTUTIL_HP}
    common_app_utils_hp
    ensemblecmsis_hp
)

target_compile_definitions(${BL_APP_TARGET_HP} PRIVATE
    BL_HP
)

target_link_options(${BL_APP_TARGET_HP} PRIVATE
    -T ${CMAKE_CURRENT_SOURCE_DIR}/linker_files/gcc_M55_HP.ld
    "SHELL:-Xlinker -Map=${CMAKE_BINARY_DIR}/bin/${BL_APP_TARGET_HP}.map"
)

add_custom_command(TARGET ${BL_APP_TARGET_HP} POST_BUILD
    COMMAND arm-none-eabi-objcopy -O binary ${BL_APP_TARGET_HP}.elf ${BL_APP_TARGET_HP}.bin
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)
