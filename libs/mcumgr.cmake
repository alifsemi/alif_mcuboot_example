# Copyright (c) 2024 Alif Semiconductor. All rights reserved.

set(MCUMGR_DIR ${CMAKE_CURRENT_LIST_DIR}/mcumgr)
set(MCUMGR_LIB mcumgr)
set(MCUMGR_PATCHING_CMD mcumgr_apply_patch)
set(MCUMGR_PATCH_FILE ${CMAKE_CURRENT_LIST_DIR}/mcumgr.patch)

# Custom command for patching mcumgr config.
# Attempts to apply the patch file but if it fails, attempts reverse-patching. If it goes through.
# assume that the patch has already been applied.
add_custom_command(
    OUTPUT ${CMAKE_BINARY_DIR}/patch_timestamp
    COMMAND git apply ${MCUMGR_PATCH_FILE} || git apply ${MCUMGR_PATCH_FILE} -R --check && touch ${CMAKE_BINARY_DIR}/patch_timestamp
    WORKING_DIRECTORY ${MCUMGR_DIR}
    DEPENDS ${MCUMGR_DIR}/cmd/img_mgmt/include/img_mgmt/img_mgmt_config.h
    COMMENT "Patching mcumgr"
)

# Custom target for mcumgr to depend on to run the custom command to patch the mcumgr.
add_custom_target(${MCUMGR_PATCHING_CMD} DEPENDS ${CMAKE_BINARY_DIR}/patch_timestamp)

add_library(${MCUMGR_LIB})

target_include_directories(${MCUMGR_LIB} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/mcumgr_config
    ${MCUMGR_DIR}/cmd/img_mgmt/include
    ${MCUMGR_DIR}/mgmt/include
    ${MCUMGR_DIR}/cmd/os_mgmt/include
    ${MCUMGR_DIR}/smp/include
)

target_sources(${MCUMGR_LIB} PRIVATE
    ${MCUMGR_DIR}/mgmt/src/mgmt.c
    ${MCUMGR_DIR}/cborattr/src/cborattr.c
    ${MCUMGR_DIR}/util/src/mcumgr_util.c
    ${MCUMGR_DIR}/smp/src/smp.c

    # Image manager (updates)
    ${MCUMGR_DIR}/cmd/img_mgmt/src/img_mgmt.c
    ${MCUMGR_DIR}/cmd/img_mgmt/src/img_mgmt_state.c
    ${MCUMGR_DIR}/cmd/img_mgmt/src/img_mgmt_util.c
    ${CMAKE_CURRENT_SOURCE_DIR}/mcumgr_port/mcumgr_img_port.c

    # OS manager (reset and echo)
    ${MCUMGR_DIR}/cmd/os_mgmt/src/os_mgmt.c
    ${CMAKE_CURRENT_SOURCE_DIR}/mcumgr_port/mcumgr_os_port.c
)

target_include_directories(${MCUMGR_LIB} PRIVATE
    ${MCUMGR_DIR}/cborattr/include
    ${MCUMGR_DIR}/util/include
)

# Not needed in release
#add_dependencies(${MCUMGR_LIB}
#    ${MCUMGR_PATCHING_CMD}
#)

target_link_libraries(${MCUMGR_LIB} PUBLIC
    ${TINYCBOR_LIB}
    ${LIB_BOOTUTIL}
    ensemblecmsis_interface
    rte_interface
)

target_compile_options(${MCUMGR_LIB} PRIVATE
    -Wno-unused-parameter
)

target_compile_definitions(${MCUMGR_LIB} PRIVATE
    CBORATTR_MAX_SIZE=512
    OS_MGMT_RESET_MS=250
    OS_MGMT_ECHO=1
)
