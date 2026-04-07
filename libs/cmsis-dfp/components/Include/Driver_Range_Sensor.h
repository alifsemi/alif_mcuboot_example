/* Copyright (C) 2025 Alif Semiconductor - All Rights Reserved.
 * Use, distribution and modification of this code is permitted under the
 * terms stated in the Alif Semiconductor Software License Agreement
 *
 * You should have received a copy of the Alif Semiconductor Software
 * License Agreement with this file. If not, please write to:
 * contact@alifsemi.com, or visit: https://alifsemi.com/license
 */

/******************************************************************************
 * @file     Driver_Range_Sensor.h
 * @author   Shreehari H K
 * @email    shreehari.hk@alifsemi.com
 * @version  V1.0.0
 * @date     30-Nov-2025
 * @brief    Driver for Range Measurement Sensor
 * @bug      None.
 * @Note     None.
 ******************************************************************************/


#ifndef DRIVER_RANGE_SENSOR_H_
#define DRIVER_RANGE_SENSOR_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "Driver_Common.h"

/* API version */
#define ARM_RANGE_SENSOR_API_VERSION ARM_DRIVER_VERSION_MAJOR_MINOR(1, 0)

#define ARM_RANGE_SENSOR_SET_MODE                    \
    (1UL << 0)  ///< Set Operating mode; arg: variable of type ref: ARM_RANGE_SENSOR_MODE
#define ARM_RANGE_SENSOR_SET_RANGE                   \
    (1UL << 1)  ///< Set detection range in mm (Max is 5000mm); arg = range
#define ARM_RANGE_SENSOR_SET_THRESHOLD               \
    (1UL << 2)  ///< Set sensing threshold; arg = threshold
#define ARM_RANGE_SENSOR_MEAS_INTERVAL               \
    (1UL << 3)  ///< Measurement interval in millisec; arg = interval
#define ARM_RANGE_SENSOR_MEAS_START    (1UL << 4)  ///< Start sensing; arg = 0
#define ARM_RANGE_SENSOR_MEAS_STOP     (1UL << 5)  ///< Stop sensing; arg = 0
#define ARM_RANGE_SENSOR_GET_DATA                    \
    (1UL << 6)  ///< Get range sensor data;
                ///< arg: addr of variable (of type ARM_RANGE_SENSOR_DATA)

/**
 * brief Range Sensor Driver Capabilities.
 */
typedef struct _ARM_RANGE_SENSOR_CAPABILITIES {
    uint32_t range          : 1;                   ///< Object range finding
    uint32_t static_range   : 1;                   ///< Static target rejection
    uint32_t det_thresholds : 4;                   ///< Number of Detection thresholds
    uint32_t reserved       : 26;                  ///< Reserved (must be zero)
} ARM_RANGE_SENSOR_CAPABILITIES;

/**
 * brief Range Sensor operating modes
 */
typedef enum _ARM_RANGE_SENSOR_MODE {
    ARM_RANGE_SENSOR_MODE_IDLE            = 0x0,   ///< Idle mode - Idle mode
    ARM_RANGE_SENSOR_MODE_FREERUN         = 0x1,   ///< Free-running mode -sensor uses
                                                   ///< internal clk to wake and measure.
    ARM_RANGE_SENSOR_MODE_TRIGGERED_TX_RX = 0x2    ///< Triggered transmit/receive mode
} ARM_RANGE_SENSOR_MODE;

typedef struct _ARM_RANGE_SENSOR_THRESHOLD {
    uint16_t start_sample;                         ///< sample starting number
    uint16_t level;                                ///< Threshold level
} ARM_RANGE_SENSOR_THRESHOLD;

/**
 * @brief Representation of a sensor readout value.
 *
 * The value is represented as having an integer and a fractional part,
 * and can be obtained using the formula val1 + val2 * 10^(-6). Negative
 * values also adhere to the above formula, but may need special attention.
 * Here are some examples of the value representation:
 *
 *      0.5: val1 =  0, val2 =  500000
 *     -0.5: val1 =  0, val2 = -500000
 *     -1.0: val1 = -1, val2 =  0
 *     -1.5: val1 = -1, val2 = -500000
 */
typedef struct _ARM_RANGE_SENSOR_RANGE_VALUE {
    /* Integer part of the value. */
    int32_t int_val;
    /* Fractional part of the value (in one-millionth parts). */
    int32_t fract_val;
} ARM_RANGE_SENSOR_RANGE_VALUE;

/* Range Sensor Data - To hold measurement data for one sensor */
typedef struct _ARM_RANGE_SENSOR_DATA {
    ARM_RANGE_SENSOR_RANGE_VALUE range;      ///< Object range
    uint16_t                     amplitude;  ///< Signal amplitude
} ARM_RANGE_SENSOR_DATA;

/**
 * brief RANGE_SENSOR Status
 */
typedef struct _ARM_RANGE_SENSOR_STATUS {
    uint32_t data_ready : 1;   ///< Data ready status
    uint32_t reserved   : 31;  ///< Reserved (must be zero)
} ARM_RANGE_SENSOR_STATUS;

// Function documentation
/**
 * @fn          ARM_DRIVER_VERSION GetVersion (void)
 * @brief       Get RANGE_SENSOR driver version.
 * @return      \ref ARM_DRIVER_VERSION
 */

/**
 * @fn          ARM_RANGE_SENSOR_CAPABILITIES GetCapabilities (void)
 * @brief       Get RANGE_SENSOR driver capabilities.
 * @return      \ref ARM_RANGE_SENSOR_CAPABILITIES
 */

/**
 * @fn          ARM_RANGE_SENSOR_STATUS GetStatus (void)
 * @brief       Get RANGE_SENSOR driver status.
 * @return      \ref ARM_RANGE_SENSOR_STATUS
 */

/**
 * @fn          int32_t Initialize (void)
 * @brief       Initialize RANGE_SENSOR Interface.
 * @param[in]   None
 * @return      \ref execution_status
 */

/**
 * @fn          int32_t Uninitialize (void)
 * @brief       Uninitialize RANGE_SENSOR Interface.
 * @return      \ref execution_status
 */

/**
 * @fn          int32_t PowerControl (ARM_POWER_STATE state)
 * @brief       Control RANGE_SENSOR Interface Power.
 * @param[in]   state  Power state
 * @return      \ref execution_status
 */

/**
 * @fn          int32_t Control (uint32_t control, uint32_t arg)
 * @brief       Control RANGE_SENSOR Interface.
 * @param[in]   control   Operation
 * @param[in]   arg       Argument of operation
 * @return      \ref execution_status
 */

/**
 * brief RANGE_SENSOR Operations.
 */
typedef struct _ARM_DRIVER_RANGE_SENSOR {
    /* Pointer to \ref ARM_RANGE_SENSOR_GetVersion : Get driver version                         */
    ARM_DRIVER_VERSION                   (*GetVersion)     (void);
    /* Pointer to \ref ARM_RANGE_SENSOR_GetCapabilities : Get driver capabilities               */
    ARM_RANGE_SENSOR_CAPABILITIES        (*GetCapabilities)(void);
    /* Pointer to \ref ARM_RANGE_SENSOR_GetStatus : Get driver status                           */
    ARM_RANGE_SENSOR_STATUS              (*GetStatus)      (void);
    /* Pointer to \ref ARM_RANGE_SENSOR_Initialize : Initialize RANGE_SENSOR Iface              */
    int32_t                              (*Initialize)     (void);
    /* Pointer to \ref ARM_RANGE_SENSOR_Uninitialize: Un-initialize RANGE_SENSOR Iface          */
    int32_t                              (*Uninitialize)   (void);
    /* Pointer to \ref ARM_RANGE_SENSOR_PowerControl: Control RANGE_SENSOR Iface Power          */
    int32_t                              (*PowerControl)   (ARM_POWER_STATE state);
    /* Pointer to \ref ARM_RANGE_SENSOR_Control : Control RANGE_SENSOR Iface                    */
    int32_t                              (*Control)        (uint32_t control, uint32_t arg);
} const ARM_DRIVER_RANGE_SENSOR;

#ifdef __cplusplus
}
#endif

#endif /* DRIVER_RANGE_SENSOR_H_ */
