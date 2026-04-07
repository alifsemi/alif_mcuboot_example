/* Copyright (C) 2025 Alif Semiconductor - All Rights Reserved.
 * @Use, distribution and modification of this code is permitted under the
 * @terms stated in the Alif Semiconductor Software License Agreement
 *
 * @You should have received a copy of the Alif Semiconductor Software
 * @License Agreement with this file. If not, please write to:
 * @contact@alifsemi.com, or visit: https://alifsemi.com/license
 *
 */

/*******************************************************************************
 * @@file     isp.h
 * @@author   Yogender Kumar Arya
 * @@email    yogender.kumar@alifsemi.com
 * @@version  V1.0.0
 * @@date     13-September-2025
 * @@brief    Low level driver Specific header file.
 ******************************************************************************/

#ifndef ISP_H_
#define ISP_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include <soc.h>
#include "soc_features.h"
#include "vsios_isp.h"
#include "vsi_comm_isp.h"
#include "vsios_type.h"

/* ISP Register (ISP_IMSC/ISP_RIS/ISP_MIS/ISP_ICR) bit Definition, Macros, Offsets and Masks
 * @these include Data-loss, size violation/Incorrect programming, AWB complete,
 * @Input frame complete, V_Sync, H_Sync and Exposure measurement complete.
 */
/* Data-loss IRQ bit[2] */
#define ISP_INTR_DATALOSS        (1U << 2)

/* Size-error IRQ bit[3] */
#define ISP_INTR_SIZE_ERR        (1U << 3)

/* AWB done IRQ bit[5] */
#define ISP_INTR_AWB_DONE        (1U << 4)

/* Frame Input complete IRQ bit[6] */
#define ISP_INTR_FRAME_IN        (1U << 5)

/* VSYNC Detected IRQ bit[16] */
#define ISP_INTR_VSYNC           (1U << 6)

/* HSYNC Detected IRQ bit[20] */
#define ISP_INTR_HSYNC           (1U << 7)

/* Exposure complete IRQ bit[20] */
#define ISP_INTR_EXPM_COMPLETE   (1U << 18)

/* ISP Register (MI_IMSC/MI_RIS/MI_MIS/MI_ICR) bit Definition, Macros, Offsets and Masks
 * @these include Frame End, Macro block line, Fill-level Wrap-around of Y, Cb & Cr,
 */
/* MP End of Frame IRQ bit[0] */
#define ISP_MI_INTR_MP_FRAME_END (1U << 0)

/* Macro block line IRQ bit[2] */
#define ISP_MI_INTR_MBLK_LINE    (1U << 2)

/* Fill level IRQ bit[3] */
#define ISP_MI_INTR_FILL_MP_Y    (1U << 3)

/* Main Picture Y-addr wrap IRQ bit[4] */
#define ISP_MI_INTR_WRAP_MP_Y    (1U << 4)

/* Main Picture Cb-addr wrap IRQ bit[5] */
#define ISP_MI_INTR_WRAP_MP_CB   (1U << 5)

/* Main Picture Cr-addr wrap IRQ bit[6] */
#define ISP_MI_INTR_WRAP_MP_CR   (1U << 6)

/* Masked interrupt status (r) */
#define ISP_MIS                  0x00000B08

/* Interrupt mask (rw) */
#define ISP_IMSC                 0x00000B00

/* Interrupt clear register (w) */
#define ISP_ICR                  0x00000B0C

/*masked interrupt status register (ro) */
#define MI_MIS                   0x00000F00

/* Interrupt Mask (1: interrupt active, 0: interrupt masked) (rw) */
#define MI_IMSC                  0x00000EF8

/* interrupt clear register (ro) */
#define MI_ICR                   0x00000F04

/**
 * @fn          uint32_t isp_get_interrupt_status(ISP_PORT IspPort)
 * @brief       Get the interrupt status from the ISP.
 * @param[in]   ISP_PORT port-ID to ISP type
 * @return      interrupt status from the ISP.
 */
static inline uint32_t isp_get_interrupt_status(ISP_PORT IspPort)
{
    uint32_t reg;

    vsios_isp_read_reg(IspPort.devId, ISP_MIS, (vsi_u32_t *) &reg);
    return reg;
}

/**
 * @fn           void isp_enable_interrupt(uint32_t irq_bitmask)
 * @brief        Enable ISP interrupt.
 * @param[in]    ISP_PORT port-ID to ISP type
 * @param[in]    irq_bitmask Possible ISP events (refer ISP_INTR_* macros Bitmask).
 * @return       none
 */
static inline void isp_enable_interrupt(ISP_PORT IspPort, uint32_t irq_bitmask)
{
    vsios_isp_write_reg(IspPort.devId, ISP_IMSC, irq_bitmask);
}

/**
 * @fn           void isp_disable_interrupt(ISP_PORT IspPort, uint32_t irq_bitmask)
 * @brief        Disable ISP interrupt.
 * @param[in]    ISP_PORT port-ID to ISP type
 * @param[in]    irq_bitmask Possible ISP events (refer ISP_INTR_* macros Bitmask).
 * @return       none
 */
static inline void isp_disable_interrupt(ISP_PORT IspPort, uint32_t irq_bitmask)
{
    vsios_isp_write_reg(IspPort.devId, ISP_IMSC, ~irq_bitmask);
}

/**
 * @fn           void isp_irq_handler_clear_intr_status(ISP_PORT IspPort, uint32_t irq_bitmask)
 * @brief        Clear ISP interrupt.
 * @param[in]    ISP_PORT port-ID to ISP type
 * @param[in]    irq_bitmask ISP interrupt status (refer ISP_INTR_* macros Bitmask)
 * @return       none.
 */
static inline void isp_irq_handler_clear_intr_status(ISP_PORT IspPort, uint32_t irq_bitmask)
{
    vsios_isp_write_reg(IspPort.devId, ISP_ICR, irq_bitmask);
}

/**
 * @fn          uint32_t isp_mi_get_interrupt_status(ISP_PORT IspPort)
 * @brief       Get the interrupt status from the ISP.
 * @param[in]   ISP_PORT port-ID to ISP type
 * @return      interrupt status from the ISP.
 */
static inline uint32_t isp_mi_get_interrupt_status(ISP_PORT IspPort)
{
    uint32_t reg;

    vsios_isp_read_reg(IspPort.devId, MI_MIS, (vsi_u32_t *) &reg);
    return reg;
}

/**
 * @fn           void isp_mi_enable_interrupt(ISP_PORT IspPort, uint32_t irq_bitmask)
 * @brief        Enable ISP interrupt.
 * @param[in]    ISP_PORT port-ID to ISP type
 * @param[in]    irq_bitmask Possible ISP events (refer ISP_MI_INTR_* macros Bitmask).
 * @return       none
 */
static inline void isp_mi_enable_interrupt(ISP_PORT IspPort, uint32_t irq_bitmask)
{
    vsios_isp_write_reg(IspPort.devId, MI_IMSC, irq_bitmask);
}

/**
 * @fn           void isp_mi_disable_interrupt(ISP_PORT IspPort, uint32_t irq_bitmask)
 * @brief        Disable ISP interrupt.
 * @param[in]    ISP_PORT port-ID to ISP type
 * @param[in]    irq_bitmask Possible ISP events (refer ISP_MI_INTR_* macros Bitmask).
 * @return       none
 */
static inline void isp_mi_disable_interrupt(ISP_PORT IspPort, uint32_t irq_bitmask)
{
    vsios_isp_write_reg(IspPort.devId, MI_IMSC, ~irq_bitmask);
}

/**
 * @fn           void isp_mi_irq_handler_clear_intr_status(ISP_PORT IspPort, uint32_t irq_bitmask)
 * @brief        Clear ISP interrupt.
 * @param[in]    ISP_PORT port-ID to ISP type
 * @param[in]    irq_bitmask ISP interrupt status (refer ISP_MI_INTR_* macros Bitmask)
 * @return       none.
 */
static inline void isp_mi_irq_handler_clear_intr_status(ISP_PORT IspPort, uint32_t irq_bitmask)
{
    vsios_isp_write_reg(IspPort.devId, MI_ICR, irq_bitmask);
}

#ifdef __cplusplus
}
#endif

#endif /* ISP_H_ */
