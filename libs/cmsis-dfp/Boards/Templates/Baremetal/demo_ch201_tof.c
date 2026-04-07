/* Copyright (C) 2025 Alif Semiconductor - All Rights Reserved.
 * Use, distribution and modification of this code is permitted under the
 * terms stated in the Alif Semiconductor Software License Agreement
 *
 * You should have received a copy of the Alif Semiconductor Software
 * License Agreement with this file. If not, please write to:
 * contact@alifsemi.com, or visit: https://alifsemi.com/license
 *
 */

/*******************************************************************************
 * @file     : demo_ch201_tof.c
 * @author   : Shreehari H K
 * @email    : shreehari.hk@alifsemi.com
 * @version  : V1.0.0
 * @date     : 21-Nov-2025
 * @brief    : Baremetal testapp to verify CH201 Time of Flight sensor data
 *
 *           : Hardware setup
 *              TestApp will communicate with CH201 sensor
 *               which is on-board connected with the I2C1_C.
 *               (so no any external hardware connection are required).
 *                Pins used:
 *                 P7_2 (SDA)
 *                 P7_3 (SCL)
 *                 GND
 *
 * @bug      : None.
 * @Note     : None.
 ******************************************************************************/

/* System Includes */
#include <stdio.h>
#include <inttypes.h>

#include "app_utils.h"
#include "sensor_utils.h"

/* Driver include */
#include "Driver_Range_Sensor.h"

#include "board_config.h"
#include "RTE_Device.h"
#include "RTE_Components.h"
#if defined(RTE_CMSIS_Compiler_STDOUT)
#include "retarget_init.h"
#include "retarget_stdout.h"
#endif /* RTE_CMSIS_Compiler_STDOUT */

extern ARM_DRIVER_RANGE_SENSOR Driver_CH201;
ARM_DRIVER_RANGE_SENSOR *CH201_drv = &Driver_CH201;

/* Maximum detection range for the sensor in mm */
#define CH201_DETECTION_RANGE_MM    (2000)

#define MEASUREMENT_INTERVAL_MS     (1000)

/* Target detection thresholds */
/* threshold 0 - starts at zero as per datasheet*/
#define CH201_THRESH_0_START    (0)
#define CH201_THRESH_0_LEVEL    (5000)

#define CH201_THRESH_1_START    (26)
#define CH201_THRESH_1_LEVEL    (2000)

#define CH201_THRESH_2_START    (39)
#define CH201_THRESH_2_LEVEL    (800)

#define CH201_THRESH_3_START    (56)
#define CH201_THRESH_3_LEVEL    (400)

#define CH201_THRESH_4_START    (79)
#define CH201_THRESH_4_LEVEL    (250)

#define CH201_THRESH_5_START    (89)
#define CH201_THRESH_5_LEVEL    (175)

#define CH201_MAX_SUPPORTED_THRESHOLDS   (6)
/* CH201 Sensor supports 6 thresholds
 * If all thresholds not required then
 * set the corresponding entry to Zero
 */
ARM_RANGE_SENSOR_THRESHOLD ch201_detect_thresholds[CH201_MAX_SUPPORTED_THRESHOLDS] = {
        {CH201_THRESH_0_START, CH201_THRESH_0_LEVEL},      /* threshold 0 */
        {CH201_THRESH_1_START, CH201_THRESH_1_LEVEL},      /* threshold 1 */
        {CH201_THRESH_2_START, CH201_THRESH_2_LEVEL},      /* threshold 2 */
        {CH201_THRESH_3_START, CH201_THRESH_3_LEVEL},      /* threshold 3 */
        {CH201_THRESH_4_START, CH201_THRESH_4_LEVEL},      /* threshold 4 */
        {CH201_THRESH_5_START, CH201_THRESH_5_LEVEL},      /* threshold 5 */
};

/**
 * @brief Ch201 time of flight demo.
 *
 * This function initializes the CH201 sensor and runs
 * the Time-of-Flight demonstration routine.
 *
 * @param[in]  None
 *
 * @return None
 */
static void demo_ch201_tof(void)
{
    int32_t ret;
    ARM_RANGE_SENSOR_STATUS sensor_status;
    ARM_RANGE_SENSOR_DATA sensor_data;
    ARM_DRIVER_VERSION version;

    /* pin mux and configuration for all device IOs requested from pins.h*/
    ret = board_pins_config();
    if (ret != 0) {
        printf("Error in pin-mux configuration: %" PRId32 "\n", ret);
    }

    printf("Initializing CH201 sensor(s)... \r\n");

    version = CH201_drv->GetVersion();
    printf("\r\n CH201 version api:0x%" PRIx16 " driver:0x%" PRIx16 "...\r\n",
           version.api, version.drv);

    ret = CH201_drv->Initialize();
    if (ret != ARM_DRIVER_OK) {
        printf("CH201 Initialization failed\r\n");
        goto error_uninitialize;
    }

    ret = CH201_drv->PowerControl(ARM_POWER_FULL);
    if (ret != ARM_DRIVER_OK) {
        printf("CH201 PowerUp failed\r\n");
        goto error_uninitialize;
    }

    ret = CH201_drv->Control(ARM_RANGE_SENSOR_SET_RANGE,
		             CH201_DETECTION_RANGE_MM);
    if (ret != ARM_DRIVER_OK) {
        printf("CH201 Range Set failed\r\n");
        goto error_poweroff;
    }

    /* Set measurement interval */
    ret = CH201_drv->Control(ARM_RANGE_SENSOR_MEAS_INTERVAL,
		             MEASUREMENT_INTERVAL_MS);
    if (ret != ARM_DRIVER_OK) {
        printf("CH201 Measuring interval Set failed\r\n");
        goto error_poweroff;
    }

    /* Set measurement amplitude threshold */
    ret = CH201_drv->Control(ARM_RANGE_SENSOR_SET_THRESHOLD,
                            (uint32_t) &ch201_detect_thresholds);
    if (ret != ARM_DRIVER_OK) {
        printf("CH201 Measuring interval Set failed\r\n");
        goto error_poweroff;
    }

    /* Set measurement mode */
    ret = CH201_drv->Control(ARM_RANGE_SENSOR_SET_MODE,
                             ARM_RANGE_SENSOR_MODE_TRIGGERED_TX_RX);
    if (ret != ARM_DRIVER_OK) {
        printf("CH201 Mode Set failed\r\n");
        goto error_poweroff;
    }

    printf("Configuration completed...\n");

    /* Start measurement */
    ret = CH201_drv->Control(ARM_RANGE_SENSOR_MEAS_START, 0);
    if (ret != ARM_DRIVER_OK) {
        printf("CH201 Start Measurement failed\r\n");
        goto error_poweroff;
    }

    while (true) {
        sensor_status = CH201_drv->GetStatus();

        /* Fetch data if ready */
        if (sensor_status.data_ready) {
            ret = CH201_drv->Control(ARM_RANGE_SENSOR_GET_DATA,
			             (uint32_t) &sensor_data);
            if (ret != ARM_DRIVER_OK) {
                printf("CH201 Data read failed\r\n");
                goto error_poweroff;
            }

            printf("\t\tObject Range --> %f m, Amplitude --> %"PRIu16"\r\n",
                  sensor_value_to_double((SENSOR_VALUE *) &sensor_data.range),
                  sensor_data.amplitude);
        }
    }

error_poweroff:

    /* Power off CH201 driver */
    ret = CH201_drv->PowerControl(ARM_POWER_OFF);
    if (ret != ARM_DRIVER_OK) {
        printf("\r\n Error: CH201 Power OFF failed.\r\n");
    }

error_uninitialize:

    /* Un-initialize CH201 driver */
    ret = CH201_drv->Uninitialize();
    if (ret != ARM_DRIVER_OK) {
        printf("\r\n Error: CH201 Uninitialize failed.\r\n");
    }

    printf("\r\n *** CH201 demo thread is exiting ***...\r\n");
}

/**
 * @brief Main entry point.
 *
 * Use this function to initialize the system
 * and start the application loop.
 *
 * @param[in]  None
 *
 * @return 0 on success, or an error code otherwise.
 */
int main(void)
{
#if defined(RTE_CMSIS_Compiler_STDOUT_Custom)
    extern int stdout_init(void);
    int32_t    ret;

    ret = stdout_init();
    if (ret != ARM_DRIVER_OK) {
        WAIT_FOREVER_LOOP
    }
#endif

    /* invoke demo_tof */
    demo_ch201_tof();

    return 0;
}
