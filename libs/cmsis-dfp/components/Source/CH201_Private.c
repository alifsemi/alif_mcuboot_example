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
 * @file     : CH201_Private.c
 * @author   : Shreehari H K
 * @email    : shreehari.hk@alifsemi.com
 * @version  : V1.0.0
 * @date     : 21-Nov-2025
 * @brief    : Alif's Board Support file for CH201 Time of Flight sensor
 *
 * @bug      : None.
 * @Note     : None.
 ******************************************************************************/

/* Project Includes */
#include "CH201_Private.h"
#include "Driver_I2C_EX.h"

/* IO Driver */
#include "Driver_IO.h"

/* RTC Driver */
#include "Driver_RTC.h"
#include "Driver_LPTIMER.h"

#include "RTE_Device.h"

#include "sys_utils.h"

CH201_DRV_INFO ch201_drv_info;

/* I2C Driver instance 1 */
extern ARM_DRIVER_I2C  Driver_I2C1;
static ARM_DRIVER_I2C *I2C_MstDrv = &Driver_I2C1;

/* RTC Driver instance 0 */
extern ARM_DRIVER_RTC  Driver_RTC0;
ARM_DRIVER_RTC *RTCdrv = &Driver_RTC0;

/* LPTIMER0 Driver instance */
extern ARM_DRIVER_LPTIMER Driver_LPTIMER0;
ARM_DRIVER_LPTIMER       *LPTimerDrv = &Driver_LPTIMER0;

/* Ch201 Program pin control IO port */
extern ARM_DRIVER_GPIO  ARM_Driver_GPIO_(BOARD_CH201_SENSOR_PROG_IO_PORT);
static ARM_DRIVER_GPIO *IO_Driver_PROG = &ARM_Driver_GPIO_(BOARD_CH201_SENSOR_PROG_IO_PORT);

/* Ch201 Interrupt pin control IO port */
extern ARM_DRIVER_GPIO  ARM_Driver_GPIO_(BOARD_CH201_SENSOR_INT_IO_PORT);
static ARM_DRIVER_GPIO *IO_Driver_INT = &ARM_Driver_GPIO_(BOARD_CH201_SENSOR_INT_IO_PORT);

/* Ch201 RESET pin control IO port */
extern ARM_DRIVER_GPIO  ARM_Driver_GPIO_(BOARD_CH201_SENSOR_RESET_IO_PORT);
static ARM_DRIVER_GPIO *IO_Driver_RESET = &ARM_Driver_GPIO_(BOARD_CH201_SENSOR_RESET_IO_PORT);

/* No restart */
#define I2C_COMM_STOP                  (0x00)

/* Timeout in Microsec - 1 millisec*/
#define CH201_I2C_TIMEOUT_US           100000
#define DELAY_1US                      1

/* RTC prescaler for 1Hz */
#define CH201_RTC_PRESCALER            0x8000

/* LPTIMER_TICK_PERIOD_US clock tick period in microsec */
#define LPTIMER_TICK_PERIOD_US        (1000000 / 32768)

/* RTC counter to millisec conversion */
#define CONVERT_RTC_COUNTER_TO_MS(x)   CONVERT_S_TO_MS(x)

/* I2C tranfer status */
#define CH201_I2C_XFER_DONE           (1 << 0)
#define CH201_I2C_XFER_ERR            (1 << 1)

/**
 * @brief       CH201 RTC event callback.
 * @param[in]   event RTC Event
 * @return      None
 */
static void chbsp_rtc_callback(uint32_t event)
{
    ARG_UNUSED(event);
}

/**
 * @brief       CH201 I2C event callback.
 * @param[in]   event I2C Event
 * @return      None
 */
static void chbsp_i2c_callback(uint32_t event)
{
    /* callback event occurred */
    if (event & (ARM_I2C_EVENT_TRANSFER_INCOMPLETE | ARM_I2C_EVENT_ADDRESS_NACK |
                 ARM_I2C_EVENT_BUS_ERROR | ARM_I2C_EVENT_ARBITRATION_LOST)) {
        /* Transfer Error. */
	ch201_drv_info.ch201_i2c_event = CH201_I2C_XFER_ERR;
    } else if (event & ARM_I2C_EVENT_TRANSFER_DONE) {
        /* Transfer Done. */
	ch201_drv_info.ch201_i2c_event = CH201_I2C_XFER_DONE;
    }
}

/**
 * @brief       CH201 INT gpio callback event.
 * @param[in]   event GPIO Event
 * @return      None
 */
void chbsp_int_gpio_callback(uint32_t event)
{
    ARG_UNUSED(event);

    if (ch201_drv_info.int1_wait_mode) {
        /* set status if in waiting mode */
        ch201_drv_info.gpio_int_sts = true;
    } else {
        /* callback with device number */
        ch_interrupt(ch201_drv_info.grp_ptr, ch201_drv_info.dev_num);
    }
}

/**
 * @brief       LPTIMER callback event.
 * @param[in]   event LPTIMER Event
 * @return      None
 */
void chbsp_lptimer_callback(uint8_t event)
{
    if (event == ARM_LPTIMER_EVENT_UNDERFLOW) {
        if (ch201_drv_info.periodic_timeout_cb) {
            ch201_drv_info.periodic_timeout_cb();
        }
    }
}

/**
 * @brief       CH201 INT line IO control.
 * @param[in]   io_driver IO driver instance
 * @param[in]   pin       Pin number
 * @param[in]   enable    True or false
 * @return      \ref execution_status
 */
static int32_t chbsp_intr_enable(ARM_DRIVER_GPIO *io_driver,
                                 uint8_t pin, bool enable)
{
    uint32_t arg = ARM_GPIO_IRQ_POLARITY_HIGH | ARM_GPIO_IRQ_SENSITIVE_EDGE;
    int32_t  ret;

    if (enable) {
        /* Enable interrupt */
        ret = io_driver->Control(pin, ARM_GPIO_ENABLE_INTERRUPT, &arg);
    } else {
        /* Disable interrupt */
        ret = io_driver->Control(pin, ARM_GPIO_DISABLE_INTERRUPT, NULL);
    }

    return ret;
}

/**
 * @brief       LPTIMER setup function.
 * @param[in]   None
 * @return      \ref execution_status
 */
static int32_t chbsp_lptimer_init(void)
{
    int32_t ret;

    ret = LPTimerDrv->Initialize(RTE_CH201_LPTIMER_CHANNEL, chbsp_lptimer_callback);
    if (ret != ARM_DRIVER_OK) {
        return ret;
    }

    ret = LPTimerDrv->PowerControl(RTE_CH201_LPTIMER_CHANNEL, ARM_POWER_FULL);
    if (ret != ARM_DRIVER_OK) {
        return ret;
    }

    ch201_drv_info.state |= CH201_LPTIMER_DRIVER_READY;

    return ARM_DRIVER_OK;
}

/**
 * @brief       LPTIMER De-Initialize function.
 * @param[in]   None
 * @return      \ref execution_status
 */
static int32_t chbsp_lptimer_deinit(void)
{
    int32_t ret;

    ret = LPTimerDrv->Stop(RTE_CH201_LPTIMER_CHANNEL);
    if (ret != ARM_DRIVER_OK) {
        return ret;
    }

    ret = LPTimerDrv->PowerControl(RTE_CH201_LPTIMER_CHANNEL, ARM_POWER_OFF);
    if (ret != ARM_DRIVER_OK) {
        return ret;
    }

    ret = LPTimerDrv->Uninitialize(RTE_CH201_LPTIMER_CHANNEL);
    if (ret != ARM_DRIVER_OK) {
        return ret;
    }

    ch201_drv_info.state &= ~CH201_LPTIMER_DRIVER_READY;

    return ARM_DRIVER_OK;
}

/**
 * @brief       RTC setup function.
 * @param[in]   None
 * @return      \ref execution_status
 */
static int32_t chbsp_rtc_init(void)
{
    int32_t ret;

    /* Initialize RTC driver */
    ret = RTCdrv->Initialize(chbsp_rtc_callback);
    if (ret != ARM_DRIVER_OK) {
        return ret;
    }

    /* Enable the power for RTC */
    ret = RTCdrv->PowerControl(ARM_POWER_FULL);
    if (ret != ARM_DRIVER_OK) {
        return ret;
    }

    /* Enable the power for RTC */
    ret = RTCdrv->Control(ARM_RTC_SET_PRESCALER, CH201_RTC_PRESCALER);
    if (ret != ARM_DRIVER_OK) {
        return ret;
    }

    ch201_drv_info.state |= CH201_RTC_DRIVER_READY;

    return ARM_DRIVER_OK;
}

/**
 * @brief       RTC De-Initialize function.
 * @param[in]   None
 * @return      \ref execution_status
 */
static int32_t chbsp_rtc_deinit(void)
{
    int32_t ret;

    /* Disable the power for RTC */
    ret = RTCdrv->PowerControl(ARM_POWER_OFF);
    if (ret != ARM_DRIVER_OK) {
        return ret;
    }

    /* De-Initialize RTC driver */
    ret = RTCdrv->Uninitialize();
    if (ret != ARM_DRIVER_OK) {
        return ret;
    }

    ch201_drv_info.state &= ~CH201_RTC_DRIVER_READY;

    return ARM_DRIVER_OK;
}

/**
 * @brief       GPIO setup function.
 * @param[in]   None
 * @return      \ref execution_status
 */
static int32_t chbsp_gpios_init(void)
{
    int32_t ret;

    /* Initialize IO driver for CH201 PROG */
    ret = IO_Driver_PROG->Initialize(BOARD_CH201_SENSOR_PROG_PIN_NO, NULL);
    if (ret != ARM_DRIVER_OK) {
        return ret;
    }

    /* Power-up IO driver for CH201 PROG  */
    ret = IO_Driver_PROG->PowerControl(BOARD_CH201_SENSOR_PROG_PIN_NO, ARM_POWER_FULL);
    if (ret != ARM_DRIVER_OK) {
        return ret;
    }

    ret = IO_Driver_PROG->SetDirection(BOARD_CH201_SENSOR_PROG_PIN_NO,
                                       GPIO_PIN_DIRECTION_OUTPUT);
    if (ret != ARM_DRIVER_OK) {
        return ret;
    }

    /* Initialize IO driver for CH201 INT */
    ret = IO_Driver_INT->Initialize(BOARD_CH201_SENSOR_INT_PIN_NO,
                                    &chbsp_int_gpio_callback);
    if (ret != ARM_DRIVER_OK) {
        return ret;
    }

    /* Power-up IO driver for CH201 INT  */
    ret = IO_Driver_INT->PowerControl(BOARD_CH201_SENSOR_INT_PIN_NO, ARM_POWER_FULL);
    if (ret != ARM_DRIVER_OK) {
        return ret;
    }

    /* Initialize IO driver for CH201 RESET */
    ret = IO_Driver_RESET->Initialize(BOARD_CH201_SENSOR_RESET_PIN_NO, NULL);
    if (ret != ARM_DRIVER_OK) {
        return ret;
    }

    /* Power-up IO driver for CH201 RESET  */
    ret = IO_Driver_RESET->PowerControl(BOARD_CH201_SENSOR_RESET_PIN_NO, ARM_POWER_FULL);
    if (ret != ARM_DRIVER_OK) {
        return ret;
    }

    ret = IO_Driver_RESET->SetDirection(BOARD_CH201_SENSOR_RESET_PIN_NO,
                                        GPIO_PIN_DIRECTION_OUTPUT);
    if (ret != ARM_DRIVER_OK) {
        return ret;
    }

    ch201_drv_info.state |= CH201_GPIO_DRIVER_READY;

    return ARM_DRIVER_OK;
}

/**
 * @brief       GPIO De-Initialize function.
 * @param[in]   None
 * @return      \ref execution_status
 */
static int32_t chbsp_gpios_deinit(void)
{
    int32_t ret;

    /* Power-down IO driver for CH201 PROG  */
    ret = IO_Driver_PROG->PowerControl(BOARD_CH201_SENSOR_PROG_PIN_NO, ARM_POWER_OFF);
    if (ret != ARM_DRIVER_OK) {
        return ret;
    }

    /* Uninitialize IO driver for CH201 PROG */
    ret = IO_Driver_PROG->Uninitialize(BOARD_CH201_SENSOR_PROG_PIN_NO);
    if (ret != ARM_DRIVER_OK) {
        return ret;
    }

    /* Power-down IO driver for CH201 INT  */
    ret = IO_Driver_INT->PowerControl(BOARD_CH201_SENSOR_INT_PIN_NO, ARM_POWER_OFF);
    if (ret != ARM_DRIVER_OK) {
        return ret;
    }

    /* Uninitialize IO driver for CH201 INT */
    ret = IO_Driver_INT->Uninitialize(BOARD_CH201_SENSOR_INT_PIN_NO);
    if (ret != ARM_DRIVER_OK) {
        return ret;
    }

    /* Power-down IO driver for CH201 RESET  */
    ret = IO_Driver_RESET->PowerControl(BOARD_CH201_SENSOR_RESET_PIN_NO, ARM_POWER_OFF);
    if (ret != ARM_DRIVER_OK) {
        return ret;
    }

    /* Uninitialize IO driver for CH201 RESET */
    ret = IO_Driver_RESET->Uninitialize(BOARD_CH201_SENSOR_RESET_PIN_NO);
    if (ret != ARM_DRIVER_OK) {
        return ret;
    }

    ch201_drv_info.state &= ~CH201_GPIO_DRIVER_READY;

    return ARM_DRIVER_OK;
}

/**
 * @brief       Writes data to CH201 registers.
 * @param[in]   tar_addr CH201 I2C slave address
 * @param[in]   reg_addr Register address
 * @param[in]   reg_data Pointer to source data buf
 * @param[in]   len      Number of bytes to write
 * @return      0 on success, 1 on failure
 */
static int32_t chbsp_write(uint8_t tar_addr, uint16_t reg_addr,
                           uint8_t *reg_data, uint8_t len)

{
    int32_t  ret     = 0U;
    uint8_t  iter    = 0U;
    uint32_t counter = 0U;

    __ALIGNED(4) uint8_t tx_buf[len + CH201_REG_ADDR_SIZE];

    /* Store register's address in 0th index */
    tx_buf[0] = reg_addr;

    if (len) {
        for (iter = 0U; iter < len; iter++) {
            tx_buf[iter + CH201_REG_ADDR_SIZE] = reg_data[iter];
        }
    }

    ch201_drv_info.ch201_i2c_event = 0;

    /* Send msg to Ch201 sensor */
    ret = I2C_MstDrv->MasterTransmit(tar_addr, tx_buf,
                                     (len + CH201_REG_ADDR_SIZE), I2C_COMM_STOP);
    if (ret != ARM_DRIVER_OK) {
        return 1;
    }

    /* wait for callback event. */
    while (ch201_drv_info.ch201_i2c_event == 0) {
        if (counter++ < CH201_I2C_TIMEOUT_US) {
            sys_busy_loop_us(DELAY_1US);
        } else {
            return 1;
        }
    }
    if (!(ch201_drv_info.ch201_i2c_event & CH201_I2C_XFER_DONE)) {
        return 1;
    }

    return 0;
}

/**
 * @brief       Reads data from CH201 registers.
 * @param[in]   tar_addr I2C slave address
 * @param[in]   reg_addr Register address
 * @param[in]   reg_data Pointer to destination buf
 * @param[in]   len      Number of bytes to read
 * @return      0 on success, 1 on failure
 */
static int32_t chbsp_read(uint8_t tar_addr, uint16_t reg_addr,
                         uint8_t *reg_data, uint8_t len)
{
    int32_t  ret;
    uint32_t counter = 0U;
    uint32_t control_arg = (ARM_I2C_WRITE_READ_MODE_EN | CH201_REG_ADDR_SIZE);

    ch201_drv_info.ch201_i2c_event = 0;

    /* Setup Write-Read */
    ret = I2C_MstDrv->Control(ARM_I2C_MODE_WRITE_READ, control_arg);
    if (ret != ARM_DRIVER_OK) {
        return 1;
    }

    *reg_data = reg_addr;
    /* Receive data from slave */
    ret = I2C_MstDrv->MasterReceive(tar_addr, reg_data, len, I2C_COMM_STOP);
    if (ret != ARM_DRIVER_OK) {
        return 1;
    }

    /* wait for callback event. */
    while (ch201_drv_info.ch201_i2c_event == 0) {
        if (counter++ < CH201_I2C_TIMEOUT_US) {
            sys_busy_loop_us(DELAY_1US);
        } else {
            return 1;
        }
    }
    if (!(ch201_drv_info.ch201_i2c_event & CH201_I2C_XFER_DONE)) {
        return 1;
    }
    return 0;
}

/**
 * @brief       Toggle a debug indicator pin.
 * @param[in]   dbg_pin_num Index of debug pin to toggle
 * @return      None
 */
void chbsp_debug_toggle(uint8_t dbg_pin_num)
{
    ARG_UNUSED(dbg_pin_num);
}

/**
 * @brief       Turn on a debug indicator pin.
 * @param[in]   dbg_pin_num Index of debug pin to turn on
 * @return      None
 */
void chbsp_debug_on(uint8_t dbg_pin_num)
{
    ARG_UNUSED(dbg_pin_num);
}

/**
 * @brief       Turn off a debug indicator pin.
 * @param[in]   dbg_pin_num Index of debug pin to turn off
 * @return      None
 */
void chbsp_debug_off(uint8_t dbg_pin_num)
{
    ARG_UNUSED(dbg_pin_num);
}

/**
 * @brief       Assert reset for all sensors (drive RESET_N low).
 * @param[in]   None
 * @return      None
 */
void chbsp_reset_assert(void)
{
    /* Drive the RESET pin to low */
    IO_Driver_RESET->SetValue(BOARD_CH201_SENSOR_RESET_PIN_NO,
                              GPIO_PIN_OUTPUT_STATE_LOW);
}

/**
 * @brief       Deassert reset for all sensors (drive RESET_N high).
 * @param[in]   None
 * @return      None
 */
void chbsp_reset_release(void)
{
    /* Drive the RESET pin to High */
    IO_Driver_RESET->SetValue(BOARD_CH201_SENSOR_RESET_PIN_NO,
                              GPIO_PIN_OUTPUT_STATE_HIGH);
}

/**
 * @brief       Assert PROG pin for specified device.
 * @param[in]   dev_ptr    Pointer to device descriptor
 * @return      None
 */
void chbsp_program_enable(ch_dev_t *dev_ptr)
{
    uint8_t dev_num = ch_get_dev_num(dev_ptr);

    if (dev_num == ch201_drv_info.dev_num) {
        /* Drive the PROG pin to high */
        IO_Driver_PROG->SetValue(BOARD_CH201_SENSOR_PROG_PIN_NO,
                                 GPIO_PIN_OUTPUT_STATE_HIGH);
    }
}

/**
 * @brief       Deassert PROG pin for specified device.
 * @param[in]   dev_ptr    Pointer to device descriptor
 * @return      None
 */
void chbsp_program_disable(ch_dev_t *dev_ptr)
{
    uint8_t dev_num = ch_get_dev_num(dev_ptr);

    if (dev_num == ch201_drv_info.dev_num) {
        /* Drive the PROG pin to low */
        IO_Driver_PROG->SetValue(BOARD_CH201_SENSOR_PROG_PIN_NO,
                                 GPIO_PIN_OUTPUT_STATE_LOW);
    }
}

/**
 * @brief       Configure INT1 as output for a group of sensors.
 * @param[in]   grp_ptr    Pointer to group descriptor
 * @return      None
 */
void chbsp_group_set_int1_dir_out(ch_group_t *grp_ptr)
{
    ch_dev_t *dev_ptr;

    ch201_drv_info.grp_ptr = grp_ptr;

    if (ch_get_num_ports(grp_ptr) == ch201_drv_info.dev_num + 1) {
        dev_ptr = ch_get_dev_ptr(grp_ptr, ch201_drv_info.dev_num);
        if (ch_sensor_is_connected(dev_ptr)) {
            chbsp_set_int1_dir_out(dev_ptr);
        }
    }
}

/**
 * @brief       Configure INT1 as output for one sensor.
 * @param[in]   dev_ptr    Pointer to device descriptor
 * @return      None
 */
void chbsp_set_int1_dir_out(ch_dev_t *dev_ptr)
{
    uint8_t dev_num = ch_get_dev_num(dev_ptr);

    if (dev_num == ch201_drv_info.dev_num) {
        /* Set the INT1 pin direction to Output */
        (void)IO_Driver_INT->SetDirection(BOARD_CH201_SENSOR_INT_PIN_NO,
                                          GPIO_PIN_DIRECTION_OUTPUT);
    }
}

/**
 * @brief       Configure INT1 as input for a group of sensors.
 * @param[in]   grp_ptr    Pointer to group descriptor
 * @return      None
 */
void chbsp_group_set_int1_dir_in(ch_group_t *grp_ptr)
{
    ch_dev_t *dev_ptr;

    ch201_drv_info.grp_ptr = grp_ptr;

    if (ch_get_num_ports(grp_ptr) == ch201_drv_info.dev_num + 1) {
        dev_ptr = ch_get_dev_ptr(grp_ptr, ch201_drv_info.dev_num);
        if (ch_sensor_is_connected(dev_ptr)) {
            chbsp_set_int1_dir_in(dev_ptr);
        }
    }
}

/**
 * @brief       Configure INT1 as input for one sensor.
 * @param[in]   dev_ptr    Pointer to device descriptor
 * @return      None
 */
void chbsp_set_int1_dir_in(ch_dev_t *dev_ptr)
{
    uint8_t dev_num = ch_get_dev_num(dev_ptr);

    if (dev_num == ch201_drv_info.dev_num) {
        /* Set the INT1 pin direction to Input */
        (void)IO_Driver_INT->SetDirection(BOARD_CH201_SENSOR_INT_PIN_NO,
                                          GPIO_PIN_DIRECTION_INPUT);
    }
}

/**
 * @brief       Drive INT1 low for a group of sensors.
 * @param[in]   grp_ptr    Pointer to group descriptor
 * @return      None
 */
void chbsp_group_int1_clear(ch_group_t *grp_ptr)
{
    ch_dev_t *dev_ptr;

    ch201_drv_info.grp_ptr = grp_ptr;

    if (ch_get_num_ports(grp_ptr) == ch201_drv_info.dev_num + 1) {
        dev_ptr = ch_get_dev_ptr(grp_ptr, ch201_drv_info.dev_num);
        if (ch_sensor_is_connected(dev_ptr)) {
            chbsp_int1_clear(dev_ptr);
        }
    }
}

/**
 * @brief       Drive INT1 low for one sensor.
 * @param[in]   dev_ptr    Pointer to device descriptor
 * @return      None
 */
void chbsp_int1_clear(ch_dev_t *dev_ptr)
{
    uint8_t dev_num = ch_get_dev_num(dev_ptr);

    if (dev_num == ch201_drv_info.dev_num) {
        /* Drive the INT1 pin to low */
        IO_Driver_INT->SetValue(BOARD_CH201_SENSOR_INT_PIN_NO,
                                GPIO_PIN_OUTPUT_STATE_LOW);
    }
}

/**
 * @brief       Drive INT1 high for a group of sensors.
 * @param[in]   grp_ptr    Pointer to group descriptor
 */
void chbsp_group_int1_set(ch_group_t *grp_ptr)
{
    ch_dev_t *dev_ptr;

    ch201_drv_info.grp_ptr = grp_ptr;

    if (ch_get_num_ports(grp_ptr) == ch201_drv_info.dev_num + 1) {
        dev_ptr = ch_get_dev_ptr(grp_ptr, ch201_drv_info.dev_num);
        if (ch_sensor_is_connected(dev_ptr)) {
            chbsp_int1_set(dev_ptr);
        }
    }
}

/**
 * @brief       Drive INT1 high for one sensor.
 * @param[in]   dev_ptr    Pointer to device descriptor
 * @return      None
 */
void chbsp_int1_set(ch_dev_t *dev_ptr)
{
    uint8_t dev_num = ch_get_dev_num(dev_ptr);

    if (dev_num == ch201_drv_info.dev_num) {
        /* Drive the INT1 pin to high */
        IO_Driver_INT->SetValue(BOARD_CH201_SENSOR_INT_PIN_NO,
                                GPIO_PIN_OUTPUT_STATE_HIGH);
    }
}

/**
 * @brief       Enable INT1 interrupt for a group of sensors.
 * @param[in]   grp_ptr    Pointer to group descriptor
 * @return      None
 */
void chbsp_group_int1_interrupt_enable(ch_group_t *grp_ptr)
{
    ch_dev_t *dev_ptr;

    if (ch_get_num_ports(grp_ptr) == ch201_drv_info.dev_num + 1) {
        dev_ptr = ch_get_dev_ptr(grp_ptr, ch201_drv_info.dev_num);
        chbsp_int1_interrupt_enable(dev_ptr);
    }
}

/**
 * @brief       Enable INT1 interrupt for one sensor.
 * @param[in]   dev_ptr    Pointer to device descriptor
 * @return      None
 */
void chbsp_int1_interrupt_enable(ch_dev_t *dev_ptr)
{
    if (ch_sensor_is_connected(dev_ptr)) {
        chbsp_intr_enable(IO_Driver_INT, BOARD_CH201_SENSOR_INT_PIN_NO, true);
    }
}

/**
 * @brief       Disable INT1 interrupt for a group of sensors.
 * @param[in]   grp_ptr    Pointer to group descriptor
 * @return      None
 */
void chbsp_group_int1_interrupt_disable(ch_group_t *grp_ptr)
{
    ch_dev_t *dev_ptr;

    if (ch_get_num_ports(grp_ptr) == ch201_drv_info.dev_num + 1) {
        dev_ptr = ch_get_dev_ptr(grp_ptr, ch201_drv_info.dev_num);
        chbsp_int1_interrupt_disable(dev_ptr);
    }
}

/**
 * @brief       Disable INT1 interrupt for one sensor.
 * @param[in]   dev_ptr    Pointer to device descriptor
 * @return      None
 */
void chbsp_int1_interrupt_disable(ch_dev_t *dev_ptr)
{
    if (ch_sensor_is_connected(dev_ptr)) {
        chbsp_intr_enable(IO_Driver_INT, BOARD_CH201_SENSOR_INT_PIN_NO, false);
    }
}

/**
 * @brief       Configure INT2 as output for a group of sensors (ICU).
 * @param[in]   grp_ptr    Pointer to group descriptor
 * @return      None
 */
void chbsp_group_set_int2_dir_out(ch_group_t *grp_ptr)
{
    ARG_UNUSED(grp_ptr);
}

/**
 * @brief       Configure INT2 as output for one sensor (ICU).
 * @param[in]   dev_ptr    Pointer to device descriptor
 * @return      None
 */
void chbsp_set_int2_dir_out(ch_dev_t *dev_ptr)
{
    ARG_UNUSED(dev_ptr);
}

/**
 * @brief       Configure INT2 as input for a group of sensors (ICU).
 * @param[in]   grp_ptr    Pointer to group descriptor
 * @return      None
 */
void chbsp_group_set_int2_dir_in(ch_group_t *grp_ptr)
{
    ARG_UNUSED(grp_ptr);
}

/**
 * @brief       Configure INT2 as input for one sensor (ICU).
 * @param[in]   dev_ptr    Pointer to device descriptor
 * @return      None
 */
void chbsp_set_int2_dir_in(ch_dev_t *dev_ptr)
{
    ARG_UNUSED(dev_ptr);
}

/**
 * @brief       Drive INT2 low for a group of sensors (ICU).
 * @param[in]   grp_ptr    Pointer to group descriptor
 * @return      None
 */
void chbsp_group_int2_clear(ch_group_t *grp_ptr)
{
    ARG_UNUSED(grp_ptr);
}

/**
 * @brief       Drive INT2 low for one sensor (ICU).
 * @param[in]   dev_ptr    Pointer to device descriptor
 * @return      None
 */
void chbsp_int2_clear(ch_dev_t *dev_ptr)
{
    ARG_UNUSED(dev_ptr);
}

/**
 * @brief       Drive INT2 high for a group of sensors (ICU).
 * @param[in]   grp_ptr    Pointer to group descriptor
 * @return      None
 */
void chbsp_group_int2_set(ch_group_t *grp_ptr)
{
    ARG_UNUSED(grp_ptr);
}

/**
 * @brief       Drive INT2 high for one sensor (ICU).
 * @param[in]   dev_ptr    Pointer to device descriptor
 * @return      None
 */
void chbsp_int2_set(ch_dev_t *dev_ptr)
{
    ARG_UNUSED(dev_ptr);
}

/**
 * @brief       Enable INT2 interrupt for a group of sensors (ICU).
 * @param[in]   grp_ptr    Pointer to group descriptor
 * @return      None
 */
void chbsp_group_int2_interrupt_enable(ch_group_t *grp_ptr)
{
    ARG_UNUSED(grp_ptr);
}

/**
 * @brief       Enable INT2 interrupt for one sensor (ICU).
 * @param[in]   dev_ptr    Pointer to device descriptor
 * @return      None
 */
void chbsp_int2_interrupt_enable(ch_dev_t *dev_ptr)
{
    ARG_UNUSED(dev_ptr);
}

/**
 * @brief       Disable INT2 interrupt for a group of sensors (ICU).
 * @param[in]   grp_ptr    Pointer to group descriptor
 * @return      None
 */
void chbsp_group_int2_interrupt_disable(ch_group_t *grp_ptr)
{
    ARG_UNUSED(grp_ptr);
}

/**
 * @brief       Disable INT2 interrupt for one sensor (ICU).
 * @param[in]   dev_ptr    Pointer to device descriptor
 * @return      None
 */
void chbsp_int2_interrupt_disable(ch_dev_t *dev_ptr)
{
    ARG_UNUSED(dev_ptr);
}

/**
 * @brief       Delay for specified microseconds.
 * @param[in]   us         Microseconds to delay
 * @return      None
 */
void chbsp_delay_us(uint32_t us)
{
    sys_busy_loop_us(us);
}

/**
 * @brief       Delay for specified milliseconds.
 * @param[in]   ms         Milliseconds to delay
 * @return      None
 */
void chbsp_delay_ms(uint32_t ms)
{
    sys_busy_loop_us(CONVERT_MS_TO_US(ms));
}

/**
 * @brief       Return a free-running counter value in milliseconds.
 * @param[in]   None
 * @return      Milliseconds tick count
 */
uint32_t chbsp_timestamp_ms(void)
{
    int32_t ret;
    uint32_t ts;

    if (!(ch201_drv_info.state & CH201_RTC_DRIVER_READY)) {
        return 0;
    }

    /* Read current counter from RTC */
    ret = RTCdrv->ReadCounter(&ts);
    if (ret != ARM_DRIVER_OK) {
        return 0;
    }

    return CONVERT_RTC_COUNTER_TO_MS(ts);
}

/**
 * @brief       Wait for any interrupt event or timeout.
 * @param[in]   time_out_ms Timeout in milliseconds
 * @param[in]   event_mask  Event bitmask to wait for
 * @return      0 if event received, 1 if timeout
 */
uint8_t chbsp_event_wait(uint16_t time_out_ms, uint32_t event_mask)
{
    uint32_t time_out = CONVERT_MS_TO_US(time_out_ms);

    if (event_mask == (1 << CH201_I2C_DEV_NUM)) {
        do {
            sys_busy_loop_us(DELAY_1US);
        } while ((--time_out) && (!ch201_drv_info.gpio_int_sts));

        /* Disable waiting mode */
        ch201_drv_info.int1_wait_mode = false;

        if (!ch201_drv_info.gpio_int_sts) {
            return 1;
        }
        ch201_drv_info.gpio_int_sts = false;

        return 0;
    } else {
        return 1;
    }
}

/**
 * @brief       Notify an interrupt event (ISR context).
 * @param[in]   event_mask Event bitmask corresponding to device(s)
 * @return      None
 */
void chbsp_event_notify(uint32_t event_mask)
{
    if (event_mask == (1 << CH201_I2C_DEV_NUM)) {
        ch201_drv_info.grp_ptr->io_int_callback(ch201_drv_info.grp_ptr,
                                                ch201_drv_info.dev_num,
                                                CH_INTERRUPT_TYPE_DATA_RDY);
    }
}

/**
 * @brief       Prepare mechanism to wait for an event.
 * @param[in]   event_mask Event bitmask to configure
 * @return      None
 */
void chbsp_event_wait_setup(uint32_t event_mask)
{
    if (event_mask == (1 << CH201_I2C_DEV_NUM)) {
        /* Enable waiting mode */
        ch201_drv_info.int1_wait_mode = true;
    }
}

/**
 * @brief       Initialize host I2C hardware and software structures.
 * @param[in]   None
 * @return      0 on success, 1 on failure
 */
int chbsp_i2c_init(void)
{
    int32_t ret = 0;

    /* Initialize Master I2C1 driver */
    ret = I2C_MstDrv->Initialize(chbsp_i2c_callback);
    if (ret != ARM_DRIVER_OK) {
        return 1;
    }

    /* I2C Master Power control  */
    ret = I2C_MstDrv->PowerControl(ARM_POWER_FULL);
    if (ret != ARM_DRIVER_OK) {
        return 1;
    }

    /* I2C Master Control */
    ret = I2C_MstDrv->Control(ARM_I2C_BUS_SPEED, ARM_I2C_BUS_SPEED_STANDARD);
    if (ret != ARM_DRIVER_OK) {
        return 1;
    }

    ch201_drv_info.state |= CH201_I2C_DRIVER_READY;

    /* Set device number to zero as only one CH201 sensor on board */
    ch201_drv_info.dev_num = CH201_I2C_DEV_NUM;

    return 0;
}

/**
 * @brief       De-Initialize host I2C hardware and software structures.
 * @param[in]   None
 * @return      \ref Execution status
 */
static int chbsp_i2c_deinit(void)
{

    int32_t ret = 0;

    /* I2C Master Power control  */
    ret = I2C_MstDrv->PowerControl(ARM_POWER_OFF);
    if (ret != ARM_DRIVER_OK) {
        return ret;
    }

    /* Initialize Master I2C1 driver */
    ret = I2C_MstDrv->Uninitialize();
    if (ret != ARM_DRIVER_OK) {
        return ret;
    }

    ch201_drv_info.state &= ~CH201_I2C_DRIVER_READY;

    return ARM_DRIVER_OK;
}

/**
 * @brief       Get I2C address/bus/flags for a device.
 * @param[in]   grp_ptr    Pointer to group descriptor
 * @param[in]   dev_num    Device number within group
 * @param[out]  info_ptr   Pointer to I2C info structure to fill
 * @return      0 on success, 1 on error
 */
uint8_t chbsp_i2c_get_info(ch_group_t *grp_ptr,
                           uint8_t dev_num,
                           ch_i2c_info_t *info_ptr)
{
    ARG_UNUSED(grp_ptr);

    if (dev_num != CH201_I2C_BUS_NUM) {
        return 1;
    }

    info_ptr->address = CH201_I2C_ADDRS;
    info_ptr->bus_num = CH201_I2C_BUS_NUM;
    info_ptr->drv_flags = 0;

    return 0;
}

/**
 * @brief       Write bytes to an I2C slave.
 * @param[in]   dev_ptr    Pointer to device descriptor
 * @param[in]   data       Pointer to data buffer
 * @param[in]   num_bytes  Number of bytes to write
 * @return      0 on success, 1 on error or NACK
 */
int chbsp_i2c_write(ch_dev_t *dev_ptr, const uint8_t *data, uint16_t num_bytes)
{
    int32_t  ret;
    uint32_t counter  = 0U;
    uint8_t  i2c_addr = ch_get_i2c_address(dev_ptr);

    if ((i2c_addr != CH201_I2C_ADDRS) &&
        (i2c_addr != CH_I2C_ADDR_PROG)) {
        return 1;
    }

    if (!(ch201_drv_info.state & CH201_I2C_DRIVER_READY)) {
        return 1;
    }

    ch201_drv_info.ch201_i2c_event = 0;
    ret = I2C_MstDrv->MasterTransmit(dev_ptr->i2c_address, data,
                                     num_bytes, I2C_COMM_STOP);
    if (ret != ARM_DRIVER_OK) {
        return 1;
    }
    /* wait for callback event. */
    while (ch201_drv_info.ch201_i2c_event == 0) {
        if (counter++ < CH201_I2C_TIMEOUT_US) {
            sys_busy_loop_us(DELAY_1US);
        } else {
            return 1;
        }
    }
    if (!(ch201_drv_info.ch201_i2c_event & CH201_I2C_XFER_DONE)) {
        return 1;
    }

    return 0;
}

/**
 * @brief       Write bytes to an I2C slave using memory/register addressing.
 * @param[in]   dev_ptr    Pointer to device descriptor
 * @param[in]   mem_addr   Internal memory/register address
 * @param[in]   data       Pointer to data buffer
 * @param[in]   num_bytes  Number of bytes to write
 * @return      0 on success, 1 on error or NACK
 */
int chbsp_i2c_mem_write(ch_dev_t *dev_ptr, uint16_t mem_addr,
                        uint8_t *data, uint16_t num_bytes)
{
    uint8_t i2c_addr = ch_get_i2c_address(dev_ptr);

    if (!(ch201_drv_info.state & CH201_I2C_DRIVER_READY)) {
        return 1;
    }

    if ((i2c_addr != CH201_I2C_ADDRS) &&
        (i2c_addr != CH_I2C_ADDR_PROG)) {
        return 1;
    }

    return chbsp_write(i2c_addr, mem_addr, data, num_bytes);
}

/**
 * @brief       Start non-blocking I2C write using memory/register addressing.
 * @param[in]   dev_ptr    Pointer to device descriptor
 * @param[in]   mem_addr   Internal memory/register address
 * @param[in]   data       Pointer to data buffer
 * @param[in]   num_bytes  Number of bytes to write
 * @return      0 on success, 1 on error or NACK
 */
int chbsp_i2c_mem_write_nb(ch_dev_t *dev_ptr, uint16_t mem_addr,
                           uint8_t *data, uint16_t num_bytes)
{
    ARG_UNUSED(dev_ptr);
    ARG_UNUSED(mem_addr);
    ARG_UNUSED(data);
    ARG_UNUSED(num_bytes);
    return 0;
}

/**
 * @brief       Read bytes from an I2C slave.
 * @param[in]   dev_ptr    Pointer to device descriptor
 * @param[out]  data       Pointer to receive buffer
 * @param[in]   num_bytes  Number of bytes to read
 * @return      0 on success, 1 on error or NACK
 */
int chbsp_i2c_read(ch_dev_t *dev_ptr, uint8_t *data, uint16_t num_bytes)
{
    int32_t  ret;
    uint32_t counter = 0U;
    uint8_t i2c_addr = ch_get_i2c_address(dev_ptr);

    if ((i2c_addr != CH201_I2C_ADDRS) &&
        (i2c_addr != CH_I2C_ADDR_PROG)) {
        return 1;
    }

    if (!(ch201_drv_info.state & CH201_I2C_DRIVER_READY)) {
        return 1;
    }

    ch201_drv_info.ch201_i2c_event = 0;
    ret = I2C_MstDrv->MasterReceive(dev_ptr->i2c_address, data,
                                    num_bytes, I2C_COMM_STOP);
    if (ret != ARM_DRIVER_OK) {
        return 1;
    }
    /* wait for callback event. */
    while (ch201_drv_info.ch201_i2c_event == 0) {
        if (counter++ < CH201_I2C_TIMEOUT_US) {
            sys_busy_loop_us(DELAY_1US);
        } else {
            return 1;
        }
    }
    if (!(ch201_drv_info.ch201_i2c_event & CH201_I2C_XFER_DONE)) {
        return 1;
    }

    return 0;
}

/**
 * @brief       Read bytes from an I2C slave using memory/register addressing.
 * @param[in]   dev_ptr    Pointer to device descriptor
 * @param[in]   mem_addr   Internal memory/register address
 * @param[out]  data       Pointer to receive buffer
 * @param[in]   num_bytes  Number of bytes to read
 * @return      0 on success, 1 on error or NACK
 */
int chbsp_i2c_mem_read(ch_dev_t *dev_ptr, uint16_t mem_addr,
                       uint8_t *data, uint16_t num_bytes)
{
    uint8_t i2c_addr = ch_get_i2c_address(dev_ptr);
    uint8_t bus_num  = ch_get_bus(dev_ptr);

    if (!(ch201_drv_info.state & CH201_I2C_DRIVER_READY)) {
        return 1;
    }

    if ((i2c_addr != CH201_I2C_ADDRS) &&
        (i2c_addr != CH_I2C_ADDR_PROG)) {
        return 1;
    }

    return chbsp_read(i2c_addr, mem_addr, data, num_bytes);
}

/**
 * @brief       Start non-blocking I2C read.
 * @param[in]   dev_ptr    Pointer to device descriptor
 * @param[out]  data       Pointer to receive buffer
 * @param[in]   num_bytes  Number of bytes to read
 * @return      0 on success, 1 on error or NACK
 */
int chbsp_i2c_read_nb(ch_dev_t *dev_ptr, uint8_t *data, uint16_t num_bytes)
{
    ARG_UNUSED(dev_ptr);
    ARG_UNUSED(data);
    ARG_UNUSED(num_bytes);
    return 0;
}

/**
 * @brief       Start non-blocking I2C read using memory/register addressing.
 * @param[in]   dev_ptr    Pointer to device descriptor
 * @param[in]   mem_addr   Internal memory/register address
 * @param[out]  data       Pointer to receive buffer
 * @param[in]   num_bytes  Number of bytes to read
 * @return      0 on success, 1 on error
 */
int chbsp_i2c_mem_read_nb(ch_dev_t *dev_ptr, uint16_t mem_addr,
                          uint8_t *data, uint16_t num_bytes)
{
    ARG_UNUSED(dev_ptr);
    ARG_UNUSED(mem_addr);
    ARG_UNUSED(data);
    ARG_UNUSED(num_bytes);
    return 0;
}

/**
 * @brief       IRQ callout for external devices sharing SPI/I2C bus.
 * @param[in]   trans      Pointer to transaction control structure
 * @return      None
 */
void chbsp_external_irq_handler(chdrv_transaction_t *trans)
{
    ARG_UNUSED(trans);
}

/**
 * @brief       Reset I2C bus associated with device.
 * @param[in]   dev_ptr    Pointer to device descriptor
 * @return      None
 */
void chbsp_i2c_reset(ch_dev_t *dev_ptr)
{
    uint8_t i2c_addr = ch_get_i2c_address(dev_ptr);

    if ((i2c_addr == CH201_I2C_ADDRS) ||
        (i2c_addr == CH_I2C_ADDR_PROG)) {
        /* There is no direct bus reset feature available.
         * Call bus clear feature that internally
         * calls bus reset if cannot be recovered
         */
        I2C_MstDrv->Control(ARM_I2C_BUS_CLEAR, 0);
    }
}

/**
 * @brief       Assert SPI chip select.
 * @param[in]   dev_ptr    Pointer to device descriptor
 * @return      None
 */
void chbsp_spi_cs_on(ch_dev_t *dev_ptr)
{
    ARG_UNUSED(dev_ptr);
}

/**
 * @brief       Deassert SPI chip select.
 * @param[in]   dev_ptr    Pointer to device descriptor
 * @return      None
 */
void chbsp_spi_cs_off(ch_dev_t *dev_ptr)
{
    ARG_UNUSED(dev_ptr);
}

/**
 * @brief       Write bytes to an SPI slave.
 * @param[in]   dev_ptr   Pointer to device descriptor
 * @param[in]   data      Pointer to data buffer
 * @param[in]   num_bytes Number of bytes to write
 * @return      0 on success, 1 on error
 */
int chbsp_spi_write(ch_dev_t *dev_ptr, const uint8_t *data, uint16_t num_bytes)
{
    ARG_UNUSED(dev_ptr);
    ARG_UNUSED(data);
    ARG_UNUSED(num_bytes);
    return 0;
}

/**
 * @brief       Read bytes from an SPI slave.
 * @param[in]   dev_ptr    Pointer to device descriptor
 * @param[out]  data       Pointer to receive buffer
 * @param[in]   num_bytes  Number of bytes to read
 * @return      0 on success, 1 on error
 */
int chbsp_spi_read(ch_dev_t *dev_ptr, uint8_t *data, uint16_t num_bytes)
{
    ARG_UNUSED(dev_ptr);
    ARG_UNUSED(data);
    ARG_UNUSED(num_bytes);
    return 0;
}

/**
 * @brief       Start non-blocking SPI read using memory/register addressing.
 * @param[in]   dev_ptr    Pointer to device descriptor
 * @param[in]   mem_addr   Internal memory/register address
 * @param[out]  data       Pointer to receive buffer
 * @param[in]   num_bytes  Number of bytes to read
 * @return      0 on success, 1 on error
 */
int chbsp_spi_mem_read_nb(ch_dev_t *dev_ptr, uint16_t mem_addr,
                          uint8_t *data, uint16_t num_bytes)
{
    ARG_UNUSED(dev_ptr);
    ARG_UNUSED(mem_addr);
    ARG_UNUSED(data);
    ARG_UNUSED(num_bytes);
    return 0;
}

/**
 * @brief       Output a text string via serial interface for debugging.
 * @param[in]   str        NUL-terminated string to print
 * @return      None
 */
void chbsp_print_str(const char *str)
{
    printf("%s", str);
}

/**
 * @brief       Initialize the periodic timer.
 * @param       None
 * @return      Execution status
 */
int chbsp_periodic_timer_init(void)
{
    return chbsp_lptimer_init();
}

/**
 * @brief       De-Initialize the periodic timer.
 * @param       None
 * @return      Execution status
 */
int chbsp_periodic_timer_deinit(void)
{
    return chbsp_lptimer_deinit();
}

/**
 * @brief       Setup board dependencies.
 * @param[in]   None
 * @return      Execution status
 */
int chbsp_board_init(void)
{
    int32_t ret;

    ret = chbsp_gpios_init();
    if (ret != ARM_DRIVER_OK) {
        return ret;
    }
    ret = chbsp_rtc_init();
    if (ret != ARM_DRIVER_OK) {
        return ret;
    }

    return ARM_DRIVER_OK;
}

/**
 * @brief       De initialize board dependencies.
 * @param[in]   None
 * @return      Execution status
 */
int chbsp_board_deinit(void)
{
    int32_t ret;

    ret = chbsp_gpios_deinit();
    if (ret != ARM_DRIVER_OK) {
        return ret;
    }
    ret = chbsp_rtc_deinit();
    if (ret != ARM_DRIVER_OK) {
        return ret;
    }

    ret = chbsp_i2c_deinit();
    if (ret != ARM_DRIVER_OK) {
        return ret;
    }

    return ARM_DRIVER_OK;
}

/**
 * @brief       Start the periodic timer.
 * @param[in]   interval_ms       Interval in millisec
 * @param[in]   callback_func_ptr Callback function
 * @return      Execution status
 */
int chbsp_periodic_timer_start(uint16_t interval_ms,
                               ch_timer_callback_t callback_func_ptr)
{
    int32_t  ret;

    if (!(ch201_drv_info.state & CH201_LPTIMER_DRIVER_READY)) {
        return ARM_DRIVER_ERROR;
    }

    /* Save callback function */
    ch201_drv_info.periodic_timeout_cb  = callback_func_ptr;
    ch201_drv_info.periodic_timeout_val = (CONVERT_MS_TO_US(interval_ms) /
                                           LPTIMER_TICK_PERIOD_US);

    /* Set LPTIMER timeout trigger */
    ret = LPTimerDrv->Control(RTE_CH201_LPTIMER_CHANNEL,
                              ARM_LPTIMER_SET_COUNT1,
                              (void *) &ch201_drv_info.periodic_timeout_val);
    if (ret != ARM_DRIVER_OK) {
        return ret;
    }

    /* Start the timer */
    ret = LPTimerDrv->Start(RTE_CH201_LPTIMER_CHANNEL);
    if (ret != ARM_DRIVER_OK) {
        return ret;
    }

    return ARM_DRIVER_OK;
}

/**
 * @brief       Stop the periodic timer.
 * @param       None
 * @return      Execution status
 */
int chbsp_periodic_timer_stop(void)
{
    int32_t  ret;

    if (!(ch201_drv_info.state & CH201_LPTIMER_DRIVER_READY)) {
        return ARM_DRIVER_ERROR;
    }

    /* Stop the timer */
    ret = LPTimerDrv->Stop(RTE_CH201_LPTIMER_CHANNEL);
    if (ret != ARM_DRIVER_OK) {
        return ret;
    }

    ch201_drv_info.periodic_timeout_cb = NULL;

    return ARM_DRIVER_OK;
}
