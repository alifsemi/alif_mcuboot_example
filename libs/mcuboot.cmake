# Copyright (c) 2024 Alif Semiconductor. All rights reserved.

set(LIB_BOOTUTIL bootutil)
set(MCUBOOT_DIR ${CMAKE_CURRENT_LIST_DIR}/mcuboot)

add_subdirectory(${MCUBOOT_DIR}/boot/bootutil EXCLUDE_FROM_ALL)
target_include_directories(${LIB_BOOTUTIL} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/mcuboot_config
    ${MCUBOOT_DIR}/boot/alif/include
)

target_link_libraries(${LIB_BOOTUTIL}
    PRIVATE
        ${MBEDTLS_LIB}
)
