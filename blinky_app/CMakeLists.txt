# Copyright (c) 2024 Alif Semiconductor. All rights reserved.


cmake_minimum_required(VERSION 3.19.0 FATAL_ERROR)

set(BLINKY_APP_TARGET alif-blinky-app)
set(BLINKY_APP_UPDATE_TARGET alif-blinky-app-update)

# Set up the project
project(alif-blinky-${ENSEMBLE_CORE}
    VERSION 1.0.0
    LANGUAGES C CXX ASM
)

if(MCUBOOT_MODE STREQUAL "RAMLOAD")
set(RAM_LOAD_ADDITION "--load-addr=0x800")
set(LINKER_FILE "gcc_M55_HP_TCM.ld")
else()
set(RAM_LOAD_ADDITION "")
set(LINKER_FILE "gcc_M55_HP_MRAM.ld")
endif()

macro(create_executable_target target define)

add_executable(${target})

set_target_properties(${target}
        PROPERTIES
        SUFFIX ".elf"
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    )

target_sources(${target} PRIVATE
    src/main.c
    src/mpu_M55.c
)

target_link_libraries(${target} PRIVATE
    rte_interface
    common_app_utils_hp
    ensemblecmsis_hp
    ${LIB_BOOTUTIL_HP}
)

target_compile_definitions(${target} PRIVATE
    ${define} 
)

target_link_options(${target} PRIVATE
    -T ${CMAKE_CURRENT_SOURCE_DIR}/linker_files/${LINKER_FILE}
    "SHELL:-Xlinker -Map=${CMAKE_BINARY_DIR}/bin/${target}.map"
)

add_custom_command(TARGET ${target} POST_BUILD
    COMMAND arm-none-eabi-objcopy -O binary ${target}.elf ${target}.bin
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)
endmacro()

create_executable_target(${BLINKY_APP_TARGET} BLINKY_APP_TARGET)
create_executable_target(${BLINKY_APP_UPDATE_TARGET} BLINKY_APP_UPDATE_TARGET)

add_custom_command(TARGET ${BLINKY_APP_TARGET} POST_BUILD
    COMMAND python ${MCUBOOT_DIR}/scripts/imgtool.py sign --key ${MCUBOOT_DIR}/root-rsa-2048.pem --header-size 0x800 --align 16 --pad-header --version 1.0.0 --slot-size 0x10000 ${RAM_LOAD_ADDITION} ${BLINKY_APP_TARGET}.bin ${BLINKY_APP_TARGET}_signed.bin
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)
add_custom_command(TARGET ${BLINKY_APP_TARGET} POST_BUILD
    COMMAND python ${MCUBOOT_DIR}/scripts/imgtool.py sign --key ${MCUBOOT_DIR}/root-rsa-2048.pem --header-size 0x800 --align 16 --pad-header --version 1.0.0 --slot-size 0x10000 ${RAM_LOAD_ADDITION} ${BLINKY_APP_TARGET}.bin ${BLINKY_APP_TARGET}_as_update_signed.bin
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

add_custom_command(TARGET ${BLINKY_APP_UPDATE_TARGET} POST_BUILD
    COMMAND python ${MCUBOOT_DIR}/scripts/imgtool.py sign --key ${MCUBOOT_DIR}/root-rsa-2048.pem --header-size 0x800 --align 16 --pad-header --version 2.0.0 --slot-size 0x10000 ${RAM_LOAD_ADDITION} ${BLINKY_APP_UPDATE_TARGET}.bin ${BLINKY_APP_UPDATE_TARGET}_signed.bin
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)
