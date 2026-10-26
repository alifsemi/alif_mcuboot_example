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
 * @file     Driver_CH201.c
 * @author   Shreehari H K
 * @email    shreehari.hk@alifsemi.com
 * @version  V1.0.0
 * @date     30-Nov-2025
 * @brief    Alif Driver for CH201 Chirp sensor.
 * @bug      None.
 * @Note     None
 ******************************************************************************/

#include "invn/soniclib/sensor_fw/ch201/ch201_gprmt.h"
#include "invn/soniclib/soniclib.h"

/* Project Includes */
#include "CH201_Private.h"

#include "sensor_utils.h"

#include "RTE_Device.h"
#include "RTE_Components.h"

#if defined(RTE_Drivers_CH201)

#define ARM_CH201_DRV_VERSION       ARM_DRIVER_VERSION_MAJOR_MINOR(1, 1)

/* Driver version*/
static const ARM_DRIVER_VERSION DriverVersion = {
             ARM_RANGE_SENSOR_API_VERSION, ARM_CH201_DRV_VERSION};

/* Driver Capabilities */
static const ARM_RANGE_SENSOR_CAPABILITIES DriverCapabilities = {
    1, /* Supports Range finding */
    0, /* Doesn't support static target rejection */
    6, /* Supports 6 internal detection thresholds */
    0  /* reserved (must be zero)*/
};

extern CH201_DRV_INFO ch201_drv_info;

/* CH201 GPRMT - multi threshold */
#define CHIRP_SENSOR_FW_INIT_FUNC   ch201_gprmt_init

/* Static target rejection range */
#define CHIRP_STATIC_REJECT_RANGE   (0)

/* Receive sensor pre-triggering */
#define CHIRP_RX_PRETRIGGER_ENABLE  (0)

/* Acceleration value in ug */
#define CH201_RANGE_VAL(x)          ((x * MILLIS_PER_UNIT) / 32.0)

/**
 * @brief       Periodic timer trigger.
 * @param[in]   None
 * @return      None
 */
static void ARM_CH201_Periodic_Timer_Trigger(void)
{
    ch_group_trigger(&ch201_drv_info.ch201_group);
}

/**
 * @brief       Data ready Interrupt callback.
 * @param[in]   grp_ptr  Ch201 group pointer
 * @param[in]   dev_num  Ch201 device number
 * @param[in]   int_type Interrupt type
 * @return      None
 */
static void ARM_CH201_Int_Callback(ch_group_t *grp_ptr,
                                   uint8_t dev_num,
                                   ch_interrupt_type_t int_type)
{
    ARG_UNUSED(grp_ptr);
    ARG_UNUSED(dev_num);

    if (int_type == CH_INTERRUPT_TYPE_DATA_RDY) {
        /* Set data-ready flag */
        ch201_drv_info.data_ready = true;
    }
}

/**
 * @brief       Read the sampled data.
 * @param[in]   dev_ptr Ch201 device pointer
 * @param[in]   data    Sensor data
 * @return      None
 */
static void ARM_CH201_Handle_Data(ch_dev_t *dev_ptr,
                                  ARM_RANGE_SENSOR_DATA *data)
{
    uint64_t loc_range;

    loc_range = ch_get_range(dev_ptr, CH_RANGE_ECHO_ONE_WAY);

    if (loc_range != CH_NO_TARGET) {
        /* Target object was successfully detected (range available)
         * Get the new amplitude value - it's only updated if range
         * was successfully measured.
         */
        data->amplitude = ch_get_amplitude(dev_ptr);
        loc_range = CH201_RANGE_VAL(loc_range);
        SENSOR_EXTRACT_INT_FRACT_PART(data->range, loc_range);

    } else {
        /* No target object was detected.
         * no updated amplitude and range value
         */
        data->amplitude = 0;
    }
}

/**
 * @brief       Gets CH201 driver version.
 * @return      \ref ARM_DRIVER_VERSION
 */
static ARM_DRIVER_VERSION ARM_CH201_GetVersion(void)
{
    return DriverVersion;
}

/**
 * @brief       Gets CH201 driver capabilities.
 * @return      \ref ARM_CH201_CAPABILITIES
 */
static ARM_RANGE_SENSOR_CAPABILITIES ARM_CH201_GetCapabilities(void)
{
    return DriverCapabilities;
}

/**
 * @brief       Gets CH201 driver status.
 * @return      \ref ARM_CH201_STATUS
 */
ARM_RANGE_SENSOR_STATUS ARM_CH201_GetStatus(void)
{
    ARM_RANGE_SENSOR_STATUS status;

    if (ch201_drv_info.data_ready) {
        status.data_ready = true;
    } else {
        status.data_ready = false;
    }

    return status;
}

/**
 * @brief       Control CH201 Slave.
 * @param[in]   control  Operation
 * @param[in]   arg      Argument of operation
 * @return      \ref execution_status
 */
static int32_t ARM_CH201_Control(uint32_t control, uint32_t arg)
{
    int32_t ret;
    uint8_t iter;
    ARM_RANGE_SENSOR_THRESHOLD *thresh_ptr = (ARM_RANGE_SENSOR_THRESHOLD *)(arg);
    ch_dev_t *dev_ptr = ch_get_dev_ptr(&ch201_drv_info.ch201_group,
                                       ch201_drv_info.dev_num);

    if ((ch201_drv_info.state & CH201_DRIVER_READY)
        != CH201_DRIVER_READY) {
        return ARM_DRIVER_ERROR;
    }

    if (!ch_sensor_is_connected(dev_ptr)) {
        return ARM_DRIVER_ERROR;
    }

    switch (control) {
    case ARM_RANGE_SENSOR_SET_MODE:

        if (arg == ARM_RANGE_SENSOR_MODE_TRIGGERED_TX_RX) {
            ret = chbsp_periodic_timer_init();
            if (ret != ARM_DRIVER_OK) {
                return ret;
            }
            /* Acquired periodic timer trigger */
            ch201_drv_info.periodic_timer_acq = true;
            ch201_drv_info.dev_config.mode = CH_MODE_TRIGGERED_TX_RX;
        } else if (arg == ARM_RANGE_SENSOR_MODE_FREERUN) {
            if (ch201_drv_info.periodic_timer_acq) {
                ret = chbsp_periodic_timer_deinit();
                if (ret != ARM_DRIVER_OK) {
                    return ret;
                }
                ch201_drv_info.periodic_timer_acq = false;
            }
            ch201_drv_info.dev_config.mode = CH_MODE_FREERUN;
        } else {
            if (ch201_drv_info.periodic_timer_acq) {
                ret = chbsp_periodic_timer_deinit();
                if (ret != ARM_DRIVER_OK) {
                    return ret;
                }
                ch201_drv_info.periodic_timer_acq = false;
            }
            ch201_drv_info.dev_config.mode = CH_MODE_IDLE;
        }
        break;

    case ARM_RANGE_SENSOR_SET_RANGE:

        if (arg > CH201_MAX_DETECTION_RANGE) {
            return ARM_DRIVER_ERROR_UNSUPPORTED;
        }
        ch201_drv_info.dev_config.max_range = arg;
        break;

    case ARM_RANGE_SENSOR_SET_THRESHOLD:

        if (!arg) {
            return ARM_DRIVER_ERROR_PARAMETER;
        }

        for (iter = 0; iter < CH_NUM_THRESHOLDS; iter++) {
            ch201_drv_info.dev_threshold.threshold[iter].start_sample =
            thresh_ptr->start_sample;
            ch201_drv_info.dev_threshold.threshold[iter].level = thresh_ptr->level;
            thresh_ptr++;
        }
        ch201_drv_info.dev_algo_config.thresh_ptr =
        (ch_thresholds_t *) &ch201_drv_info.dev_threshold;
        break;

    case ARM_RANGE_SENSOR_MEAS_INTERVAL:

        ch201_drv_info.meas_interval_ms = arg;
        break;

    case ARM_RANGE_SENSOR_MEAS_START:

        ch201_drv_info.dev_algo_config.static_range = CHIRP_STATIC_REJECT_RANGE;
        /* Set threshold */
        ch_rangefinder_set_algo_config(dev_ptr, &ch201_drv_info.dev_algo_config);

        /* Disable receive sensor pre-triggering */
        ch_set_rx_pretrigger(&ch201_drv_info.ch201_group, CHIRP_RX_PRETRIGGER_ENABLE);

        if (ch201_drv_info.dev_config.mode == CH_MODE_TRIGGERED_TX_RX) {
            ch201_drv_info.dev_config.sample_interval = 0;
            if (ch201_drv_info.periodic_timer_acq) {
                /* Start the timer only when it's acquired */
                ret = chbsp_periodic_timer_start(ch201_drv_info.meas_interval_ms,
                                                 ARM_CH201_Periodic_Timer_Trigger);
                if (ret != ARM_DRIVER_OK) {
                    return ret;
                }
            } else {
                return ARM_DRIVER_ERROR;
            }
        } else {
            /* Configure sample interval */
            ch201_drv_info.dev_config.sample_interval = ch201_drv_info.meas_interval_ms;
            ch_set_sample_interval(&ch201_drv_info.ch201_dev,
                                   ch201_drv_info.dev_config.sample_interval);

            /* Sensor is in freerun mode, so configure INT line input to receive
             * interrupt from sensor
             */
            chdrv_int_group_set_dir_in(&ch201_drv_info.ch201_group);
            chdrv_int_group_interrupt_enable(&ch201_drv_info.ch201_group);
        }

        if (ch_set_config(&ch201_drv_info.ch201_dev,
                          &ch201_drv_info.dev_config) != 0) {
            return ARM_DRIVER_ERROR;
        }
        break;

    case ARM_RANGE_SENSOR_MEAS_STOP:

        if (ch201_drv_info.dev_config.mode == CH_MODE_FREERUN) {
            /* Disable INT interrupt */
            chdrv_int_group_interrupt_disable(&ch201_drv_info.ch201_group);
        } else {
            if (ch201_drv_info.periodic_timer_acq) {
                /* Stop the timer only when it's acquired */
                ret = chbsp_periodic_timer_stop();
                if (ret != ARM_DRIVER_OK) {
                    return ret;
                }
            }
        }
        break;

    case ARM_RANGE_SENSOR_GET_DATA:

        if (!arg) {
            return ARM_DRIVER_ERROR_PARAMETER;
        }

        if (ch_sensor_is_connected(&ch201_drv_info.ch201_dev)) {

            /* Handle CH201 sampled data */
            ARM_CH201_Handle_Data(&ch201_drv_info.ch201_dev,
                                  (ARM_RANGE_SENSOR_DATA *)arg);
            ch201_drv_info.data_ready = false;

            if (ch201_drv_info.dev_config.mode == CH_MODE_FREERUN) {
                /* Sensor is in freerun mode, so configure INT line
                 * input to receive interrupt from sensor
                 */
                chdrv_int_group_set_dir_in(&ch201_drv_info.ch201_group);
                chdrv_int_group_interrupt_enable(&ch201_drv_info.ch201_group);
            }
        } else {
            return ARM_DRIVER_ERROR;
        }
        break;

    default:
        break;
    }

    return ARM_DRIVER_OK;
}

/**
 * @brief       Initializes CH201 Interface.
 * @param[in]   None
 * @return      \ref execution_status
 */
static int32_t ARM_CH201_Initialize(void)
{
    int32_t ret;

    if ((ch201_drv_info.state & CH201_DRIVER_READY)
        == CH201_DRIVER_READY) {
        return ARM_DRIVER_OK;
    }

    ret = chbsp_board_init();
    if (ret != ARM_DRIVER_OK) {
        return ret;
    }

    ch_group_init(&ch201_drv_info.ch201_group, CHIRP_MAX_NUM_SENSORS,
                  CHIRP_NUM_BUSES, CHIRP_RTC_CAL_PULSE_MS);

    /* check if the number of devices are same as configured */
    if (ch_get_num_ports(&ch201_drv_info.ch201_group) != CHIRP_MAX_NUM_SENSORS) {
        return ARM_DRIVER_ERROR;
    }

    /* Initializes driver state and data received status */
    ch201_drv_info.grp_ptr             = NULL;
    ch201_drv_info.int1_wait_mode      = false;
    ch201_drv_info.dev_num             = 0;

    ch201_drv_info.data_ready          = false;
    ch201_drv_info.periodic_timeout_cb = NULL;
    ch201_drv_info.periodic_timer_acq  = false;

    return ARM_DRIVER_OK;
}

/**
 * @brief       Uninitializes CH201 Interface.
 * @param[in]   None
 * @return      \ref execution_status
 */
static int32_t ARM_CH201_Uninitialize(void)
{
    int32_t ret;

    /* Stop and de-init the timer if acquired */
    if (ch201_drv_info.periodic_timer_acq == true) {
        ret = chbsp_periodic_timer_stop();
        if (ret != ARM_DRIVER_OK) {
            return ret;
        }
        ret = chbsp_periodic_timer_deinit();
        if (ret != ARM_DRIVER_OK) {
            return ret;
        }
        ch201_drv_info.periodic_timer_acq  = false;
    }

    /* Uninitialize board drivers */
    ret = chbsp_board_deinit();
    if (ret != ARM_DRIVER_OK) {
        return ret;
    }

    /* Resets driver state and data received status */
    ch201_drv_info.state               = 0U;
    ch201_drv_info.data_ready          = false;

    return ARM_DRIVER_OK;
}

/**
 * @brief       Controls power for Range Sensor Interface.
 * @param[in]   state  Power state
 * @return      \ref execution_status
 */
static int32_t ARM_CH201_PowerControl(ARM_POWER_STATE state)
{
    int32_t ret;

    switch (state) {
    case ARM_POWER_OFF:

        /* Stop sensor */
        ret = ARM_CH201_Control(ARM_RANGE_SENSOR_MEAS_STOP, 0);
        if (ret != ARM_DRIVER_OK) {
            return ret;
        }

        /* Set sensor to Idle mode s*/
        ret = ARM_CH201_Control(ARM_RANGE_SENSOR_SET_MODE, ARM_RANGE_SENSOR_MODE_IDLE);
        if (ret != ARM_DRIVER_OK) {
            return ret;
        }
        break;

    case ARM_POWER_FULL:
        ret = ch_init(&ch201_drv_info.ch201_dev,
                      &ch201_drv_info.ch201_group,
                      ch201_drv_info.dev_num,
                      CHIRP_SENSOR_FW_INIT_FUNC);
        if (ret != 0) {
            return ARM_DRIVER_ERROR;
        }

        ret = ch_group_start(&ch201_drv_info.ch201_group);
        if (ret != 0) {
            return ARM_DRIVER_ERROR;
        }

        /* Register callback function to be called when CH201 sensor interrupts */
        ch_io_int_callback_set(&ch201_drv_info.ch201_group, ARM_CH201_Int_Callback);
        break;

    case ARM_POWER_LOW:
    default:
        return ARM_DRIVER_ERROR_UNSUPPORTED;
    }

    return ARM_DRIVER_OK;
}

#if RTE_CH201 //RTE_CH201

extern ARM_DRIVER_RANGE_SENSOR Driver_CH201;
ARM_DRIVER_RANGE_SENSOR        Driver_CH201 = {
    ARM_CH201_GetVersion,
    ARM_CH201_GetCapabilities,
    ARM_CH201_GetStatus,
    ARM_CH201_Initialize,
    ARM_CH201_Uninitialize,
    ARM_CH201_PowerControl,
    ARM_CH201_Control,
};

#endif //RTE_CH201
#endif /* defined(RTE_Drivers_CH201) */
