/* Copyright (C) 2023 Alif Semiconductor - All Rights Reserved.
 * Use, distribution and modification of this code is permitted under the
 * terms stated in the Alif Semiconductor Software License Agreement
 *
 * You should have received a copy of the Alif Semiconductor Software
 * License Agreement with this file. If not, please write to:
 * contact@alifsemi.com, or visit: https://alifsemi.com/license
 *
 */

/******************************************************************************
 * @file     sys_ctrl_aes.h
 * @author   Silesh C V, Manoj A Murudi
 * @email    silesh@alifsemi.com, manoj.murudi@alifsemi.com
 * @version  V1.0.0
 * @date     30-May-2023
 * @brief    AES control.
 * @bug      None.
 * @Note     None
 ******************************************************************************/

#ifndef SYS_CTRL_AES_H
#define SYS_CTRL_AES_H

#include <stdbool.h>
#include "soc.h"

#ifdef __cplusplus
extern "C" {
#endif

#define AES_CONTROL_DECRYPT_EN  (1U << 0)
#define AES_CONTROL_RESET_LOGIC (1U << 1)
#define AES_CONTROL_XIP_EN      (1U << 4)
#define AES_CONTROL_LD_KEY      (1U << 7)

static inline void aes_enable_xip(AES_Type *aes)
{
    aes->AES_CONTROL |= AES_CONTROL_XIP_EN;
}

static inline void aes_disable_xip(AES_Type *aes)
{
    aes->AES_CONTROL &= ~AES_CONTROL_XIP_EN;
}

#if SOC_FEAT_AES_BAUD2_DELAY_VAL
#define AES_INTERRUPT_MASK_BAUD2_DELAY (1U << 30)

static inline void aes_set_baud2_delay(AES_Type *aes)
{
    aes->AES_INTERRUPT_MASK |= AES_INTERRUPT_MASK_BAUD2_DELAY;
}
#endif

#if SOC_FEAT_AES_OSPI_SIGNALS_DELAY
#define AES_SIGNAL_0_DELAY_POS  (0)
#define AES_SIGNAL_1_DELAY_POS  (8)
#define AES_SIGNAL_2_DELAY_POS  (16)
#define AES_SIGNAL_3_DELAY_POS  (24)
#define AES_SIGNAL_4_DELAY_POS  (0)
#define AES_SIGNAL_5_DELAY_POS  (8)
#define AES_SIGNAL_6_DELAY_POS  (16)
#define AES_SIGNAL_7_DELAY_POS  (24)
#define AES_SIGNAL_8_DELAY_POS  (0)
#define AES_SIGNAL_9_DELAY_POS  (8)
#define AES_SIGNAL_10_DELAY_POS (16)
#define AES_SIGNAL_11_DELAY_POS (24)
#define AES_SIGNAL_12_DELAY_POS (0)
#define AES_SIGNAL_13_DELAY_POS (8)
#define AES_SIGNAL_14_DELAY_POS (16)
#define AES_SIGNAL_15_DELAY_POS (24)
#define AES_TXD_DM_0_DELAY_POS  (0)
#define AES_TXD_DM_1_DELAY_POS  (8)
#define AES_DM_OE_N_DELAY_0_POS (16)
#define AES_DM_OE_N_DELAY_1_POS (24)
#define AES_SCLK_DELAY_POS      (0)
#define AES_SCLK_N_DELAY_POS    (8)

static inline void aes_set_rxds_delay(AES_Type *aes, uint8_t delay)
{
    aes->AES_RXDS_DELAY = ((delay << AES_SIGNAL_0_DELAY_POS) | (delay << AES_SIGNAL_1_DELAY_POS));
}

static inline void aes_set_signal_delay(AES_Type *aes, uint8_t delay)
{
    aes->AES_RXD_DELAY_0 = ((delay << AES_SIGNAL_0_DELAY_POS) | (delay << AES_SIGNAL_1_DELAY_POS) |
                            (delay << AES_SIGNAL_2_DELAY_POS) | (delay << AES_SIGNAL_3_DELAY_POS));
    aes->AES_RXD_DELAY_1 = ((delay << AES_SIGNAL_4_DELAY_POS) | (delay << AES_SIGNAL_5_DELAY_POS) |
                            (delay << AES_SIGNAL_6_DELAY_POS) | (delay << AES_SIGNAL_7_DELAY_POS));
    aes->AES_RXD_DELAY_2 =
        ((delay << AES_SIGNAL_8_DELAY_POS) | (delay << AES_SIGNAL_9_DELAY_POS) |
         (delay << AES_SIGNAL_10_DELAY_POS) | (delay << AES_SIGNAL_11_DELAY_POS));
    aes->AES_RXD_DELAY_3 =
        ((delay << AES_SIGNAL_12_DELAY_POS) | (delay << AES_SIGNAL_13_DELAY_POS) |
         (delay << AES_SIGNAL_14_DELAY_POS) | (delay << AES_SIGNAL_15_DELAY_POS));

    aes->AES_TXD_DELAY_0 = ((delay << AES_SIGNAL_0_DELAY_POS) | (delay << AES_SIGNAL_1_DELAY_POS) |
                            (delay << AES_SIGNAL_2_DELAY_POS) | (delay << AES_SIGNAL_3_DELAY_POS));
    aes->AES_TXD_DELAY_1 = ((delay << AES_SIGNAL_4_DELAY_POS) | (delay << AES_SIGNAL_5_DELAY_POS) |
                            (delay << AES_SIGNAL_6_DELAY_POS) | (delay << AES_SIGNAL_7_DELAY_POS));
    aes->AES_TXD_DELAY_2 =
        ((delay << AES_SIGNAL_8_DELAY_POS) | (delay << AES_SIGNAL_9_DELAY_POS) |
         (delay << AES_SIGNAL_10_DELAY_POS) | (delay << AES_SIGNAL_11_DELAY_POS));
    aes->AES_TXD_DELAY_3 =
        ((delay << AES_SIGNAL_12_DELAY_POS) | (delay << AES_SIGNAL_13_DELAY_POS) |
         (delay << AES_SIGNAL_14_DELAY_POS) | (delay << AES_SIGNAL_15_DELAY_POS));

    aes->AES_SSI_OE_N_DELAY_0 =
        ((delay << AES_SIGNAL_0_DELAY_POS) | (delay << AES_SIGNAL_1_DELAY_POS) |
         (delay << AES_SIGNAL_2_DELAY_POS) | (delay << AES_SIGNAL_3_DELAY_POS));
    aes->AES_SSI_OE_N_DELAY_1 =
        ((delay << AES_SIGNAL_4_DELAY_POS) | (delay << AES_SIGNAL_5_DELAY_POS) |
         (delay << AES_SIGNAL_6_DELAY_POS) | (delay << AES_SIGNAL_7_DELAY_POS));
    aes->AES_SSI_OE_N_DELAY_2 =
        ((delay << AES_SIGNAL_8_DELAY_POS) | (delay << AES_SIGNAL_9_DELAY_POS) |
         (delay << AES_SIGNAL_10_DELAY_POS) | (delay << AES_SIGNAL_11_DELAY_POS));
    aes->AES_SSI_OE_N_DELAY_3 =
        ((delay << AES_SIGNAL_12_DELAY_POS) | (delay << AES_SIGNAL_13_DELAY_POS) |
         (delay << AES_SIGNAL_14_DELAY_POS) | (delay << AES_SIGNAL_15_DELAY_POS));

    aes->AES_SS_N_DELAY = ((delay << AES_SIGNAL_0_DELAY_POS) | (delay << AES_SIGNAL_1_DELAY_POS));

    aes->AES_TXD_DM_DELAY =
        ((delay << AES_TXD_DM_0_DELAY_POS) | (delay << AES_TXD_DM_1_DELAY_POS) |
         (delay << AES_DM_OE_N_DELAY_0_POS) | (delay << AES_DM_OE_N_DELAY_1_POS));

    aes->AES_SCLK_DELAY = ((delay << AES_SCLK_DELAY_POS) | (delay << AES_SCLK_N_DELAY_POS));
}
#else

static inline void aes_set_rxds_delay(AES_Type *aes, uint8_t rxds_delay)
{
    aes->AES_RXDS_DELAY = rxds_delay;
}
#endif

#if SOC_FEAT_AES_HAS_ADDR_CTRL_SHIM
typedef struct _aes_addr_ctrl {
    /* Number of bits in the lower portion of the address */
    uint8_t addr_lower_bits;
    /* Bit position of the lowest bit in the upper portion of the address */
    uint8_t addr_upper_shift;
    /* Enable array mode for SS0 line (lower 256MB) */
    bool ss0_array_mode_en;
    /* Enable array mode for SS1 line (upper 256MB) */
    bool ss1_array_mode_en;
    /* Mask bits in the lower portion of the address to drive low */
    uint16_t addr_mask;
} aes_addr_ctrl;

#define AES_ADDR_CTRL_ARRAY_MASK_POS        (0)
#define AES_ADDR_CTRL_SS0_ARRAY_MODE_POS    (18)
#define AES_ADDR_CTRL_SS1_ARRAY_MODE_POS    (19)
#define AES_ADDR_CTRL_ARRAY_MODE_SHIFT_POS  (22)
#define AES_ADDR_CTRL_ARRAY_MODE_SPLIT_POS  (28)

static inline void aes_control_address(AES_Type *aes, aes_addr_ctrl *addr_cfg)
{
    aes->AES_ADDR_CONTROL = ((addr_cfg->addr_mask << AES_ADDR_CTRL_ARRAY_MASK_POS) |
                             (addr_cfg->ss0_array_mode_en << AES_ADDR_CTRL_SS0_ARRAY_MODE_POS) |
                             (addr_cfg->ss1_array_mode_en << AES_ADDR_CTRL_SS1_ARRAY_MODE_POS) |
                             (addr_cfg->addr_upper_shift << AES_ADDR_CTRL_ARRAY_MODE_SHIFT_POS) |
                             (addr_cfg->addr_lower_bits << AES_ADDR_CTRL_ARRAY_MODE_SPLIT_POS));
}
#endif

#ifdef __cplusplus
}
#endif
#endif /* SYS_CTRL_AES_H */
