# Copyright (c) 2024 Alif Semiconductor. All rights reserved.

include(ExternalProject)

set(TINYCBOR_DIR ${CMAKE_CURRENT_LIST_DIR}/tinycbor)
set(TINYCBOR_LIB tinycbor)

add_library(${TINYCBOR_LIB})

target_include_directories(${TINYCBOR_LIB} PUBLIC
    ${TINYCBOR_DIR}/include
)

target_sources(${TINYCBOR_LIB} PRIVATE
    ${TINYCBOR_DIR}/src/cborparser.c
    ${TINYCBOR_DIR}/src/cborencoder.c
    ${TINYCBOR_DIR}/src/cbor_buf_reader.c
    ${TINYCBOR_DIR}/src/cbor_buf_writer.c
)

target_include_directories(${TINYCBOR_LIB} PRIVATE
    
)
