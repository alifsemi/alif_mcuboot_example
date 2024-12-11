# Copyright (c) 2024 Alif Semiconductor. All rights reserved.

set(LIB_BOOTUTIL_HE bootutil-he)
set(LIB_BOOTUTIL_HP bootutil-hp)
set(MCUBOOT_DIR ${CMAKE_CURRENT_LIST_DIR}/mcuboot)

macro(add_bootutil_library bootutil_lib_name)

add_library(${bootutil_lib_name} STATIC EXCLUDE_FROM_ALL)

target_include_directories(${bootutil_lib_name}
    PUBLIC
        ${MCUBOOT_DIR}/boot/bootutil/include
    PRIVATE
        ${MCUBOOT_DIR}/boot/bootutil/src
)

target_sources(${bootutil_lib_name}
    PRIVATE
        ${MCUBOOT_DIR}/boot/bootutil/src/boot_record.c
        ${MCUBOOT_DIR}/boot/bootutil/src/bootutil_misc.c
        ${MCUBOOT_DIR}/boot/bootutil/src/bootutil_public.c
        ${MCUBOOT_DIR}/boot/bootutil/src/caps.c
        ${MCUBOOT_DIR}/boot/bootutil/src/encrypted.c
        ${MCUBOOT_DIR}/boot/bootutil/src/fault_injection_hardening.c
        ${MCUBOOT_DIR}/boot/bootutil/src/fault_injection_hardening_delay_rng_mbedtls.c
        ${MCUBOOT_DIR}/boot/bootutil/src/image_ecdsa.c
        ${MCUBOOT_DIR}/boot/bootutil/src/image_ed25519.c
        ${MCUBOOT_DIR}/boot/bootutil/src/image_rsa.c
        ${MCUBOOT_DIR}/boot/bootutil/src/image_validate.c
        ${MCUBOOT_DIR}/boot/bootutil/src/loader.c
        ${MCUBOOT_DIR}/boot/bootutil/src/swap_misc.c
        ${MCUBOOT_DIR}/boot/bootutil/src/swap_move.c
        ${MCUBOOT_DIR}/boot/bootutil/src/swap_scratch.c
        ${MCUBOOT_DIR}/boot/bootutil/src/tlv.c
        ${CMAKE_CURRENT_SOURCE_DIR}/mcuboot_customize/flash_map_mram.c
)

target_link_libraries(${bootutil_lib_name}
    PRIVATE
        ${MBEDTLS_LIB}
)

target_compile_options(${bootutil_lib_name}
    PRIVATE
        -Wno-error=format
)

endmacro()

add_bootutil_library(${LIB_BOOTUTIL_HE})
add_bootutil_library(${LIB_BOOTUTIL_HP})

target_include_directories(${LIB_BOOTUTIL_HE} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/mcuboot_config/he
    ${CMAKE_CURRENT_SOURCE_DIR}/mcuboot_config
    ${MCUBOOT_DIR}/boot/alif/include
)

target_compile_definitions(${LIB_BOOTUTIL_HE} PUBLIC
    M55_HE
)

target_include_directories(${LIB_BOOTUTIL_HP} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/mcuboot_config/hp
    ${CMAKE_CURRENT_SOURCE_DIR}/mcuboot_config
    ${MCUBOOT_DIR}/boot/alif/include
)

target_compile_definitions(${LIB_BOOTUTIL_HP} PUBLIC
    M55_HP
)
