# Copyright (c) 2024 Alif Semiconductor. All rights reserved.

set(MBEDTLS_DIR ${CMAKE_CURRENT_LIST_DIR}/mbedtls)
set(MBEDTLS_LIB mbedcrypto)

add_subdirectory(${MBEDTLS_DIR} EXCLUDE_FROM_ALL)
target_compile_definitions(${MBEDTLS_LIB} PRIVATE
    MBEDTLS_CONFIG_FILE="alif_mbedtls_config.h"
)
target_include_directories(${MBEDTLS_LIB} PRIVATE
    mbedtls_config
)
