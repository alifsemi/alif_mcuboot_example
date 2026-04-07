/* Copyright (C) 2025 Alif Semiconductor - All Rights Reserved.
 * Use, distribution and modification of this code is permitted under the
 * terms stated in the Alif Semiconductor Software License Agreement
 *
 * You should have received a copy of the Alif Semiconductor Software
 * License Agreement with this file. If not, please write to:
 * contact@alifsemi.com, or visit: https://alifsemi.com/license
 *
 */

/***************************************************************************************
 * @file     : demo_psram_e4.c
 * @author   : Manoj A Murudi
 * @email    : manoj.murudi@alifsemi.com
 * @version  : V1.0.0
 * @date     : 18-Nov-2025
 * @brief    : Demo program for the PSRAM device using XIP write/read.
 ***************************************************************************************/

#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <inttypes.h>

#include "ospi_psram_xip.h"
#include "board_config.h"
#include "Driver_IO.h"
#include "RTE_Components.h"
#include CMSIS_device_header
#if defined(RTE_CMSIS_Compiler_STDOUT)
#include "retarget_init.h"
#include "retarget_stdout.h"
#endif /*RTE_CMSIS_Compiler_STDOUT */

#if BOARD_APS512XXN_PSRAM_PRESENT
#include "APS512XXN_PSRAM.h"
#endif

#include "app_utils.h"

#define OSPI_XIP_BASE       0xA0000000
#define RAM_SIZE_BYTES      (64 * 1024 * 1024)  /* 64MB */

static ospi_psram_xip_config ram_config = {
    .instance       = BOARD_PSRAM_OSPI_INSTANCE,
#if BOARD_ISSI_HYPERRAM_PRESENT
    .ram_init       = NULL,
    .ram_type       = RAM_TYPE_HYPERRAM
#elif BOARD_APS512XXN_PSRAM_PRESENT
    .ram_init       = aps512xxn_psram_init,
    .ram_type       = RAM_TYPE_PSRAM
#else
#error "No RAM present on the board"
#endif
};

#if BOARD_ISSI_HYPERRAM_PRESENT
extern ARM_DRIVER_GPIO ARM_Driver_GPIO_(BOARD_IS66_HYPERRAM_RESET_GPIO_PORT);
ARM_DRIVER_GPIO       *GPIODrv = &ARM_Driver_GPIO_(BOARD_IS66_HYPERRAM_RESET_GPIO_PORT);

static int32_t issi_reset(void)
{
    int32_t ret;

    ret = GPIODrv->Initialize(BOARD_IS66_HYPERRAM_RESET_GPIO_PIN, NULL);
    if (ret != ARM_DRIVER_OK) {
        return -1;
    }

    ret = GPIODrv->PowerControl(BOARD_IS66_HYPERRAM_RESET_GPIO_PIN, ARM_POWER_FULL);
    if (ret != ARM_DRIVER_OK) {
        return -1;
    }

    ret = GPIODrv->SetDirection(BOARD_IS66_HYPERRAM_RESET_GPIO_PIN, GPIO_PIN_DIRECTION_OUTPUT);
    if (ret != ARM_DRIVER_OK) {
        return -1;
    }

    ret = GPIODrv->SetValue(BOARD_IS66_HYPERRAM_RESET_GPIO_PIN, GPIO_PIN_OUTPUT_STATE_LOW);
    if (ret != ARM_DRIVER_OK) {
        return -1;
    }

    ret = GPIODrv->SetValue(BOARD_IS66_HYPERRAM_RESET_GPIO_PIN, GPIO_PIN_OUTPUT_STATE_HIGH);
    if (ret != ARM_DRIVER_OK) {
        return -1;
    }

    return 0;
}
#endif

int main(void)
{
    uint32_t *const ptr          = (uint32_t *) OSPI_XIP_BASE;
    uint32_t        total_errors = 0, random_val;
    int32_t         ret;

#if defined(RTE_CMSIS_Compiler_STDOUT_Custom)
    extern int stdout_init(void);
    ret = stdout_init();
    if (ret != ARM_DRIVER_OK) {
        WAIT_FOREVER_LOOP
    }
#endif

    printf("PSRAM XIP demo app started\n");

    /* pin mux and configuration for all device IOs requested from pins.h*/
    ret = board_pins_config();
    if (ret != 0) {
        printf("Error in pin-mux configuration: %" PRId32 "\n", ret);
        goto error_exit;
    }

#if BOARD_ISSI_HYPERRAM_PRESENT
    ret = issi_reset();
    if (ret != 0) {
        printf("Error in reset sequence: %" PRId32 "\n", ret);
        goto error_exit;
    }
#endif

    if (ospi_psram_xip_init(&ram_config) < 0) {
        printf("RAM XIP init failed\n");
        goto error_exit;
    }

    printf("Writing data to the XIP region:\n");

    srand(1);
    for (uint32_t i = 0; i < (RAM_SIZE_BYTES / sizeof(uint32_t)); i++) {
        ptr[i] = rand();
    }

    printf("Reading back:\n");

    srand(1);
    for (uint32_t i = 0; i < (RAM_SIZE_BYTES / sizeof(uint32_t)); i++) {
        random_val = rand();
        if (ptr[i] != random_val) {
            printf("Data error at addr %" PRIx32 ", got %" PRIx32 ", expected %" PRIx32 "\n",
                   (i * sizeof(uint32_t)),
                   ptr[i],
                   random_val);
            total_errors++;
        }
    }

    printf("Done, total errors = %" PRIu32 "\n", total_errors);

error_exit:

    WAIT_FOREVER_LOOP
}
