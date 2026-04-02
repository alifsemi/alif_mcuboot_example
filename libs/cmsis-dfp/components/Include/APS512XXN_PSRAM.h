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
 * @file     APS512XXN_PSRAM.h
 * @author   Manoj A Murudi
 * @email    manoj.murudi@alifsemi.com
 * @version  V1.0.0
 * @date     18-Nov-2025
 * @brief    Header file for APS512XXN PSRAM init library.
 ******************************************************************************/

#ifndef APS512XXN_PSRAM_H_
#define APS512XXN_PSRAM_H_

#include "ospi.h"

#ifdef __cplusplus
extern "C" {
#endif

/* APS512XXN Device ID */
#define APS512XXN_ID                           0xDE

/* APS512XXN Commands */
#define APS512XXN_CMD_SYNC_READ                0x00
#define APS512XXN_CMD_SYNC_WRITE               0x80
#define APS512XXN_CMD_LINEAR_BURST_READ        0x20
#define APS512XXN_CMD_LINEAR_BURST_WRITE       0xA0
#define APS512XXN_CMD_MODE_REGISTER_READ       0x40
#define APS512XXN_CMD_MODE_REGISTER_WRITE      0xC0
#define APS512XXN_CMD_GLOBAL_RESET             0xFF

/* APS512XXN wait cycle macros
 * device considers wait cycles from last cycle of addr phase
 */
#define APS512XXN_INIT_REG_READ_WAIT_CYCLES    4
#define APS512XXN_REG_WRITE_WAIT_CYCLES        0
#define APS512XXN_RESET_WAIT_CYCLES            3
#define APS512XXN_INIT_READ_WRITE_WAIT_CYCLES  4

/* APS512XXN Register address */
#define APS512XXN_MODE_REG0_ADDR               0x0
#define APS512XXN_MODE_REG1_ADDR               0x1
#define APS512XXN_MODE_REG2_ADDR               0x2
#define APS512XXN_MODE_REG3_ADDR               0x3
#define APS512XXN_MODE_REG4_ADDR               0x4
#define APS512XXN_MODE_REG6_ADDR               0x6
#define APS512XXN_MODE_REG8_ADDR               0x8

/* APS512XXN Register bits */
#define APS512XXN_MODE_REG0_DRIVE_STR          0
#define APS512XXN_MODE_REG0_READ_LATENCY_CODE  2
#define APS512XXN_MODE_REG0_LATENCY_TYPE       5
#define APS512XXN_MODE_REG4_WRITE_LATENCY_CODE 5
#define APS512XXN_MODE_REG4_READ_RF_RATE       3
#define APS512XXN_MODE_REG4_PASR               0
#define APS512XXN_MODE_REG8_TRANSFER_MODE      6
#define APS512XXN_MODE_REG8_RBX_READ_EN        3
#define APS512XXN_MODE_REG8_BURST_TYPE         2
#define APS512XXN_MODE_REG8_BURST_LEN          0

/* APS512XXN DFS for registers access */
#define APS512XXN_OSPI_REG_DFS                 16

/**
 * @fn      int32_t aps512xxn_psram_init(OSPI_Type *ospi, AES_Type *aes)
 * @brief   APS512XXN PSRAM init function.
 * @param   ospi    : Pointer to the OSPI register map.
 * @param   aes     : Pointer to the AES register map.
 * @retval  status
 */
int32_t aps512xxn_psram_init(OSPI_Type *ospi, AES_Type *aes);

#ifdef __cplusplus
}
#endif

#endif /* APS512XXN_PSRAM_H_ */
