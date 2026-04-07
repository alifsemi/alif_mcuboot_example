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
 * @file     sys_ctrl_usb.h
 * @author   Mahesh Avula
 * @email    mahesh.avula@alifsemi.com
 * @version  V1.0.0
 * @date     25-Apr-2023
 * @brief    System Control Device information for USB
 * @bug      None.
 * @Note     None
 ******************************************************************************/
#ifndef SYS_CTRL_USB_H

#define SYS_CTRL_USB_H

/* Includes ------------------------------------------------------------------*/
#include "soc.h"
#include "soc_features.h"
#ifdef SOC_FEAT_USB_NEED_EXTRA_CLK
#include "sys_ctrl_sd.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#define USB_CTRL2_PHY_POR (1U << 8)
/* Enable clock supply for USB */
#define PERIPH_CLK_ENA_USB_CKEN (1U << 20)

/**
 *\fn     static inline void usb_phy_por_set(void)
 *\brief  Set the USB phy power on reset bit in USB_CTRL2 register.
 *\param  none.
 *\return none.
 */
static inline void usb_phy_por_set(void)
{
    CLKCTL_PER_MST->USB_CTRL2 |= USB_CTRL2_PHY_POR;
}

/**
 *\fn     static inline void usb_phy_por_clear(void)
 *\brief  Clear the USB phy power on reset bit in USB_CTRL2 register.
 *\param  none.
 *\return none.
 */
static inline void usb_phy_por_clear(void)
{
    CLKCTL_PER_MST->USB_CTRL2 &= ~USB_CTRL2_PHY_POR;
}

/**
 *\fn     static inline void enable_usb_periph_clk(void)
 *\brief  Enable the USB peripheral clock in PERIPH_CLK_ENA register.
 *\param  none.
 *\return none.
 */
static inline void enable_usb_periph_clk(void)
{
    uint32_t clock_enable = PERIPH_CLK_ENA_USB_CKEN;

#ifdef SOC_FEAT_USB_NEED_EXTRA_CLK
    clock_enable |= PERIPH_CLK_ENA_SDC_CKEN;
#endif
    CLKCTL_PER_MST->PERIPH_CLK_ENA |= clock_enable;
}

/**
 *\fn     static inline void disable_usb_periph_clk(void)
 *\brief  Disable the USB peripheral clock in PERIPH_CLK_ENA register.
 *\param  none.
 *\return none.
 */
static inline void disable_usb_periph_clk(void)
{
    uint32_t clock_disable = PERIPH_CLK_ENA_USB_CKEN;

#ifdef SOC_FEAT_USB_NEED_EXTRA_CLK
    clock_disable |= PERIPH_CLK_ENA_SDC_CKEN;
#endif
    CLKCTL_PER_MST->PERIPH_CLK_ENA &= ~clock_disable;
}

#ifdef __cplusplus
}
#endif
#endif /* SYS_CTRL_USB_H */
