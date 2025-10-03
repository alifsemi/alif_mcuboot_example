# Copyright (c) 2024 Alif Semiconductor. All rights reserved.

set(CMSIS_DIR ${CMAKE_CURRENT_LIST_DIR}/cmsis-dfp)
set(ALIF_DRIVER_DIR             ${CMSIS_DIR}/Alif_CMSIS)
set(ALIF_DEVICE_DIR             ${CMSIS_DIR}/Device)
set(ALIF_COMPONENTS_DIR         ${CMSIS_DIR}/components)
set(ALIF_ENSEMBLE_DRIVERS_DIR   ${CMSIS_DIR}/drivers)
set(ALIF_SERVICES_DIR           ${CMSIS_DIR}/se_services)

macro(create_ensemble_libs name ALIF_CORE)

add_library(${name}_interface INTERFACE)

if (${ALIF_CORE} STREQUAL "M55_HE")
set(ALIF_CORE_STRING "rtss_he")
set(ALIF_CORE_DEFINE "RTSS_HE")
else()
set(ALIF_CORE_STRING "rtss_hp")
set(ALIF_CORE_DEFINE "RTSS_HP")
endif()

set(ALIF_DEVICE_SKU "AE722F80F55D5")


target_include_directories(${name}_interface INTERFACE
    ${ALIF_DRIVER_DIR}/Include
    ${ALIF_DRIVER_DIR}/Include/config
    ${ALIF_COMPONENTS_DIR}/Include
    ${ALIF_COMPONENTS_DIR}/Source
    ${ALIF_DEVICE_DIR}/system/include
    ${ALIF_DEVICE_DIR}/core/common/config
    ${ALIF_DEVICE_DIR}/core/common/include
    ${ALIF_DEVICE_DIR}/core/common/config
    ${ALIF_DEVICE_DIR}/core/${ALIF_CORE_STRING}/config
    ${ALIF_DEVICE_DIR}/soc/${ALIF_DEVICE_SKU}/config
    ${ALIF_DEVICE_DIR}/soc/${ALIF_DEVICE_SKU}/include
    ${ALIF_DEVICE_DIR}/soc/${ALIF_DEVICE_SKU}/include/${ALIF_CORE_STRING}
    ${ALIF_ENSEMBLE_DRIVERS_DIR}/include
    ${ALIF_SERVICES_DIR}/include
    ${ALIF_SERVICES_DIR}/port/include
)

target_compile_definitions(${name}_interface INTERFACE
    ${ALIF_CORE}
    ${ALIF_CORE_DEFINE}
    _RTE_
)

target_link_libraries(${name}_interface INTERFACE
    armcmsis_interface
)

add_library(${name})

target_sources(${name} PRIVATE
    ${ALIF_DEVICE_DIR}/core/common/source/cache.c
    ${ALIF_DEVICE_DIR}/core/common/source/mpu.c
    ${ALIF_DEVICE_DIR}/core/common/source/pm.c
    ${ALIF_DEVICE_DIR}/core/common/source/sau_tcm_ns_setup.c
    ${ALIF_DEVICE_DIR}/core/common/source/startup.c
    ${ALIF_DEVICE_DIR}/core/common/source/system.c
    ${ALIF_DEVICE_DIR}/core/common/source/tgu.c
    ${ALIF_DEVICE_DIR}/core/common/source/vectors.c
    ${ALIF_DEVICE_DIR}/system/source/sys_utils.c
    ${ALIF_DEVICE_DIR}/system/source/sys_clocks.c
    ${ALIF_DRIVER_DIR}/Source/Driver_USART.c
    ${ALIF_DRIVER_DIR}/Source/Driver_IO.c
    ${ALIF_DRIVER_DIR}/Source/Driver_CRC.c
    ${ALIF_DRIVER_DIR}/Source/Driver_LPTIMER.c
    ${ALIF_DRIVER_DIR}/Source/Driver_HWSEM.c
    ${ALIF_ENSEMBLE_DRIVERS_DIR}/source/uart.c
    ${ALIF_ENSEMBLE_DRIVERS_DIR}/source/crc.c
    ${ALIF_ENSEMBLE_DRIVERS_DIR}/source/pinconf.c
    ${ALIF_ENSEMBLE_DRIVERS_DIR}/source/mhu_driver.c
    ${ALIF_ENSEMBLE_DRIVERS_DIR}/source/mhu_receiver.c
    ${ALIF_ENSEMBLE_DRIVERS_DIR}/source/mhu_sender.c
    ${ALIF_SERVICES_DIR}/source/services_host_handler.c
    ${ALIF_SERVICES_DIR}/source/services_host_maintenance.c
    ${ALIF_SERVICES_DIR}/source/services_host_power.c
    ${ALIF_SERVICES_DIR}/source/services_host_boot.c
    ${ALIF_SERVICES_DIR}/source/services_host_update.c
    ${ALIF_SERVICES_DIR}/source/services_host_system.c
)

target_include_directories(${name} PRIVATE
    ${ALIF_DRIVER_DIR}/Include/config
    ${ALIF_DRIVER_DIR}/Source
)

target_link_libraries(${name}
    ${name}_interface
    rte_interface
)

target_compile_options(${name} PRIVATE
    -Wno-error=stringop-truncation
)

endmacro()

create_ensemble_libs(ensemblecmsis_he M55_HE)
create_ensemble_libs(ensemblecmsis_hp M55_HP)
