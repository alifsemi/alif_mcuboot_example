# Copyright (c) 2024 Alif Semiconductor. All rights reserved.


cmake_minimum_required(VERSION 3.19.0 FATAL_ERROR)

add_custom_command(
    OUTPUT ${CMAKE_BINARY_DIR}/bin/seram_1.103.0.bin
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/bin/seram_1.103.0.bin ${CMAKE_BINARY_DIR}/bin/seram_1.103.0.bin
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/bin/seram_1.103.0.bin
)

add_custom_command(
    OUTPUT ${CMAKE_BINARY_DIR}/bin/seram_1.104.0.bin
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/bin/seram_1.104.0.bin ${CMAKE_BINARY_DIR}/bin/seram_1.104.0.bin
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/bin/seram_1.104.0.bin
)

add_custom_target(seram ALL
    DEPENDS ${CMAKE_BINARY_DIR}/bin/seram_1.103.0.bin
    DEPENDS ${CMAKE_BINARY_DIR}/bin/seram_1.104.0.bin
)

add_custom_command(TARGET seram POST_BUILD
    COMMAND python ${MCUBOOT_DIR}/scripts/imgtool.py sign --key ${MCUBOOT_DIR}/root-rsa-2048.pem --header-size 0x800 --align 16 --pad-header --version 1.103.0 --slot-size 0x50000 seram_1.103.0.bin seram_1.103.0_signed.bin
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

add_custom_command(TARGET seram POST_BUILD
    COMMAND python ${MCUBOOT_DIR}/scripts/imgtool.py sign --key ${MCUBOOT_DIR}/root-rsa-2048.pem --header-size 0x800 --align 16 --pad-header --version 1.104.0 --slot-size 0x50000 seram_1.104.0.bin seram_1.104.0_signed.bin
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

