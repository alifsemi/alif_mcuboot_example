# Copyright (c) 2024 Alif Semiconductor. All rights reserved.

set(COMMONAPPUTILS_DIR ${CMAKE_CURRENT_LIST_DIR}/common_app_utils)

macro(create_lib core)

add_library(common_app_utils_${core})

target_include_directories(common_app_utils_${core} PUBLIC
    ${COMMONAPPUTILS_DIR}/logging
    ${COMMONAPPUTILS_DIR}/fault_handler
    ${COMMONAPPUTILS_DIR}/profiling
)

target_sources(common_app_utils_${core} PRIVATE
    ${COMMONAPPUTILS_DIR}/logging/retarget.c
    ${COMMONAPPUTILS_DIR}/logging/uart_tracelib.c
    ${COMMONAPPUTILS_DIR}/fault_handler/fault_handler.c
)

target_link_libraries(common_app_utils_${core} PRIVATE
    ensemblecmsis_${core}
)
endmacro()

create_lib(he)
create_lib(hp)
