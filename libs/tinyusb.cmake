# Copyright (c) 2024 Alif Semiconductor. All rights reserved.

set(TINYUSB_DIR ${CMAKE_CURRENT_LIST_DIR}/tinyusb)
set(TINYUSB_LIB tinyusb)

add_library(${TINYUSB_LIB} EXCLUDE_FROM_ALL)

target_sources(${TINYUSB_LIB} PRIVATE
    ${TINYUSB_DIR}/src/tusb.c
    ${TINYUSB_DIR}/src/common/tusb_fifo.c
    ${TINYUSB_DIR}/src/device/usbd.c
    ${TINYUSB_DIR}/src/device/usbd_control.c
    ${TINYUSB_DIR}/src/class/cdc/cdc_device.c
    ${CMAKE_CURRENT_SOURCE_DIR}/tinyusb_port/tusb_alif_dcd.c
)

target_include_directories(${TINYUSB_LIB} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/tinyusb_config    
    ${TINYUSB_DIR}/src
    ${TINYUSB_DIR}/hw
)

target_link_libraries(${TINYUSB_LIB} PUBLIC
    ensemblecmsis
)

target_compile_definitions(${TINYUSB_LIB} PRIVATE
    CFG_TUSB_RHPORT0_MODE=OPT_MODE_DEVICE
    TUD_OPT_RHPORT=0
)

target_compile_definitions(${TINYUSB_LIB} PUBLIC
    CFG_TUSB_CONFIG_FILE="alif_tinyusb_config.h"
    TUP_DCD_ENDPOINT_MAX=8
    CFG_TUSB_MCU=OPT_MCU_NONE
)
