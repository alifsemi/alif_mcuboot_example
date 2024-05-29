# Copyright (c) 2024 Alif Semiconductor. All rights reserved.

set(CMSIS_DIR ${CMAKE_CURRENT_LIST_DIR}/cmsis/CMSIS)
set(CMSIS_SRC_PATH ${CMAKE_CURRENT_LIST_DIR}/cmsis/)

add_library(armcmsis_interface INTERFACE)
target_include_directories(armcmsis_interface INTERFACE
    ${CMSIS_DIR}/Core/Include
)
