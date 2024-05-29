# Copyright (c) 2024 Alif Semiconductor. All rights reserved.

set(COMMONAPPUTILS_DIR ${CMAKE_CURRENT_LIST_DIR}/common_app_utils)

add_library(common_app_utils)

target_include_directories(common_app_utils PUBLIC
    ${COMMONAPPUTILS_DIR}/logging
    ${COMMONAPPUTILS_DIR}/fault_handler
    ${COMMONAPPUTILS_DIR}/profiling
)

target_sources(common_app_utils PRIVATE
    ${COMMONAPPUTILS_DIR}/logging/retarget.c
    ${COMMONAPPUTILS_DIR}/logging/uart_tracelib.c
    ${COMMONAPPUTILS_DIR}/fault_handler/fault_handler.c
)

target_link_libraries(common_app_utils PUBLIC
    ensemblecmsis
    ensemblecmsis_interface
)
