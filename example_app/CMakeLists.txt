# Copyright (c) 2024 Alif Semiconductor. All rights reserved.


cmake_minimum_required(VERSION 3.19.0 FATAL_ERROR)

set(EXAMPLE_APP_TARGET alif-example-app)
set(EXAMPLE_APP_UPDATE_TARGET alif-example-app-update)

# Set up the project
project(alif-example-${ENSEMBLE_CORE}
    VERSION 1.0.0
    LANGUAGES C CXX ASM
)


if(${CMAKE_HOST_SYSTEM_NAME} STREQUAL Linux)
set(JLINK_COMMAND_NAME JLinkExe)
else()
set(JLINK_COMMAND_NAME JLink.exe)
endif()
find_program(JLINKEXECUTABLE ${JLINK_COMMAND_NAME})

macro(create_executable_target target define)

add_executable(${target})

set_target_properties(${target}
        PROPERTIES
        SUFFIX ".elf"
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    )

target_sources(${target} PRIVATE
    src/main.c
)

target_link_libraries(${target} PRIVATE
    common_app_utils
)

target_compile_definitions(${target} PRIVATE
    ${define} 
)

target_link_options(${target} PRIVATE
    -T ${CMAKE_CURRENT_SOURCE_DIR}/linker_files/gcc_M55_HE.ld
    "SHELL:-Xlinker -Map=${CMAKE_BINARY_DIR}/bin/${target}.map"
)

add_custom_command(TARGET ${target} POST_BUILD
    COMMAND arm-none-eabi-objcopy -O binary ${target}.elf ${target}.bin
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)
endmacro()

create_executable_target(${EXAMPLE_APP_TARGET} EXAMPLE_APP_TARGET)
create_executable_target(${EXAMPLE_APP_UPDATE_TARGET} EXAMPLE_APP_UPDATE_TARGET)

add_custom_command(TARGET ${EXAMPLE_APP_TARGET} POST_BUILD
    COMMAND python ${MCUBOOT_DIR}/scripts/imgtool.py sign --key ${MCUBOOT_DIR}/root-rsa-2048.pem --header-size 0x800 --align 16 --pad-header --version 1.0.0 --slot-size 0x10000 ${EXAMPLE_APP_TARGET}.bin ${EXAMPLE_APP_TARGET}_signed.bin
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)
add_custom_command(TARGET ${EXAMPLE_APP_TARGET} POST_BUILD
    COMMAND python ${MCUBOOT_DIR}/scripts/imgtool.py sign --key ${MCUBOOT_DIR}/root-rsa-2048.pem --header-size 0x800 --align 16 --pad-header --pad --confirm --version 1.0.0 --slot-size 0x10000 ${EXAMPLE_APP_TARGET}.bin ${EXAMPLE_APP_TARGET}_as_update_signed.bin
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

add_custom_command(TARGET ${EXAMPLE_APP_UPDATE_TARGET} POST_BUILD
    COMMAND python ${MCUBOOT_DIR}/scripts/imgtool.py sign --key ${MCUBOOT_DIR}/root-rsa-2048.pem --header-size 0x800 --align 16 --pad-header --pad --confirm --version 2.0.0 --slot-size 0x10000 ${EXAMPLE_APP_UPDATE_TARGET}.bin ${EXAMPLE_APP_UPDATE_TARGET}_signed.bin
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

add_custom_target(update 
    COMMAND ${JLINKEXECUTABLE} -CommandFile ${CMAKE_CURRENT_SOURCE_DIR}/write-update.jlink
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)
