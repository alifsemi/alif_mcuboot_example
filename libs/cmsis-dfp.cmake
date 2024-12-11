# Copyright (c) 2024 Alif Semiconductor. All rights reserved.

set(CMSIS_DIR ${CMAKE_CURRENT_LIST_DIR}/cmsis-dfp)
set(ALIF_DRIVER_DIR             ${CMSIS_DIR}/Alif_CMSIS)
set(ALIF_DEVICE_DIR             ${CMSIS_DIR}/Device)
set(ALIF_COMPONENTS_DIR         ${CMSIS_DIR}/components)
set(ALIF_ENSEMBLE_DRIVERS_DIR   ${CMSIS_DIR}/drivers)
set(ALIF_SERVICES_DIR           ${CMSIS_DIR}/se_services)

macro(create_ensemble_libs name ENSEMBLE_CORE)

add_library(${name}_interface INTERFACE)

target_include_directories(${name}_interface INTERFACE
    ${ALIF_DRIVER_DIR}/Include
    ${ALIF_DRIVER_DIR}/Include/config
    ${ALIF_COMPONENTS_DIR}/Include
    ${ALIF_COMPONENTS_DIR}/Source
    ${ALIF_PACK_DIR}/drivers/include
    ${ALIF_DEVICE_DIR}/common/include
    ${ALIF_DEVICE_DIR}/common/config
    ${ALIF_DEVICE_DIR}/core/${ENSEMBLE_CORE}/include
    ${ALIF_DEVICE_DIR}/core/${ENSEMBLE_CORE}/config
    ${ALIF_ENSEMBLE_DRIVERS_DIR}/include
    ${CMSIS_DIR}/se_services/port/include
    ${CMSIS_DIR}/se_services/include
)

target_compile_definitions(${name}_interface INTERFACE
    ${ENSEMBLE_CORE}
    _RTE_
)

target_link_libraries(${name}_interface INTERFACE
    armcmsis_interface
)

add_library(${name})

target_sources(${name} PRIVATE
    ${ALIF_DEVICE_DIR}/common/source/mpu_M55.c
    ${ALIF_DEVICE_DIR}/core/${ENSEMBLE_CORE}/source/startup_${ENSEMBLE_CORE}.c
    ${ALIF_DEVICE_DIR}/common/source/system_M55.c
    ${ALIF_DEVICE_DIR}/common/source/tgu_M55.c
    ${ALIF_DEVICE_DIR}/common/source/pm.c
    ${ALIF_DEVICE_DIR}/common/source/system_utils.c
    ${ALIF_DEVICE_DIR}/common/source/clk.c
    ${ALIF_DRIVER_DIR}/Source/Driver_USART.c
    ${ALIF_DRIVER_DIR}/Source/Driver_GPIO.c
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
