/* Copyright (C) 2025 Alif Semiconductor - All Rights Reserved.
 * Use, distribution and modification of this code is permitted under the
 * terms stated in the Alif Semiconductor Software License Agreement
 *
 * You should have received a copy of the Alif Semiconductor Software
 * License Agreement with this file. If not, please write to:
 * contact@alifsemi.com, or visit: https://alifsemi.com/license
 *
 */
/******************************************************************************
 * @file     sensor_utils.h
 * @author   Shreehari H K
 * @email    shreehari.hk@alifsemi.com
 * @brief    Common Utility header for all sensors
 * @version  V1.0.0
 * @date     18-Aug-2025
 * @bug      None
 * @Note     None
 ******************************************************************************/
#ifndef SENSOR_UTILS_H
#define SENSOR_UTILS_H

#include "sys_utils.h"

#ifdef __cplusplus
extern "C" {
#endif

/* micro-units per 1 unit */
#define MICROS_PER_UNIT (1000000LL)

/* milli-units per 1 unit */
#define MILLIS_PER_UNIT (1000LL)

/* macro to extract integer and fractional part of sensor data */
#define SENSOR_EXTRACT_INT_FRACT_PART(x, conv_val) {    \
        (x).int_val     = (conv_val / MICROS_PER_UNIT); \
        (x).fract_val   = (conv_val % MICROS_PER_UNIT); \
}

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
typedef struct _SENSOR_VALUE {
    /* Integer part of the value. */
    int32_t int_val;
    /* Fractional part of the value (in one-millionth parts). */
    int32_t fract_val;
} SENSOR_VALUE;


/**
 * @brief Helper function for converting struct sensor_value to double.
 *
 * @param val A pointer to a SENSOR_VALUE struct.
 * @return The converted value.
 */
static inline double sensor_value_to_double(const SENSOR_VALUE *val)
{
    return (double)val->int_val + (double)val->fract_val / 1000000;
}

#ifdef __cplusplus
}
#endif

#endif /* SENSOR_UTILS_H */
