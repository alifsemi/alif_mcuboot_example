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
 * @file     : CH201_Private.h
 * @author   : Shreehari H K
 * @email    : shreehari.hk@alifsemi.com
 * @version  : V1.0.0
 * @date     : 21-Nov-2025
 * @brief    : Alif's Board Support file header for CH201 Time of Flight sensor
 *
 * @bug      : None.
 * @Note     : None.
 ******************************************************************************/

#ifndef CH201_PRIVATE_H_
#define CH201_PRIVATE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include "chirp_board_config.h"
#include "invn/soniclib/chirp_bsp.h"
#include "invn/soniclib/ch_rangefinder.h"

#include "Driver_Range_Sensor.h"

/* CH201 Driver status */
#define CH201_RTC_DRIVER_READY        (1 << 0U)
#define CH201_GPIO_DRIVER_READY       (1 << 1U)
#define CH201_I2C_DRIVER_READY        (1 << 2U)
#define CH201_DRIVER_READY            (7 << 0U)

/* Supported Max detection range is 5000 mm */
#define CH201_MAX_DETECTION_RANGE   5000

/* I2C Address assignments for each possible device */
#define CH201_I2C_ADDRS       0x29
#define CH201_I2C_BUS_NUM     0
#define CH201_I2C_DEV_NUM     0
/* Reg address size in bytes */
#define CH201_REG_ADDR_SIZE   1

/* CH201 BSP driver Info variable */
typedef struct _CH201_DRV_INFO {
    /* Configuration for group of sensors    */
    ch_group_t                   ch201_group;
    /* CH201 sensor group pointer            */
    ch_group_t                   *grp_ptr;
    /* CH201 sensor device                   */
    ch_dev_t                     ch201_dev;
    /* CH201 I2C device number               */
    uint8_t                      dev_num;
    /* CH201 Device configuration            */
    ch_config_t                  dev_config;
    /* CH201 Device algo config              */
    ch_rangefinder_algo_config_t dev_algo_config;
    /* Device Multiple detection threshold   */
    ch_thresholds_t              dev_threshold;
    /* Timeout value                         */
    volatile uint32_t            rtc_timeout_val;
    /* Timeout callback                      */
    ch_timer_callback_t          rtc_timeout_cb;
    /* I2C Event status                      */
    volatile uint32_t            ch201_i2c_event;
    /* Measurement interval in millisec      */
    uint16_t                     meas_interval_ms;
    /* Driver state                          */
    uint8_t                      state;
    /* INT gpio interrupt status             */
    volatile bool                gpio_int_sts;
    /* Wait mode on GPIO INT1                */
    volatile bool                int1_wait_mode;
    /* Data ready flag                       */
    volatile bool                data_ready;
    /* Flag to Timeout continuously from RTC */
    volatile bool                continue_rtc_timeout;
} CH201_DRV_INFO;

/**
 * @brief       Setup board dependencies.
 * @param[in]   None
 * @return      Execution status
 */
int chbsp_board_init(void);

/**
 * @brief       De initialize board dependencies.
 * @param[in]   None
 * @return      Execution status
 */
int chbsp_board_deinit(void);

/**
 * @brief       Initialize the periodic timer.
 * @param[in]   interval_ms       Interval in millisec
 * @param[in]   callback_func_ptr Callback function
 * @return      Execution status
 */
int chbsp_periodic_timer_init(uint16_t interval_ms,
                              ch_timer_callback_t callback_func_ptr);

#ifdef __cplusplus
}
#endif
#endif /* CH201_PRIVATE_H_ */
