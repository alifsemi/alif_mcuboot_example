/* Copyright (C) 2026 Alif Semiconductor - All Rights Reserved.
 * Use, distribution and modification of this code is permitted under the
 * terms stated in the Alif Semiconductor Software License Agreement
 *
 * You should have received a copy of the Alif Semiconductor Software
 * License Agreement with this file. If not, please write to:
 * contact@alifsemi.com, or visit: https://alifsemi.com/license
 *
 */

/*******************************************************************************
 * @file     Driver_JPEG_Private.h
 * @author   Chandra Bhushan Singh
 * @email    chandrabhushan.singh@alifsemi.com
 * @version  V1.0.0
 * @date     14-April-2026
 * @brief    CMSIS Driver Private Header file.
 ******************************************************************************/

#ifndef DRIVER_JPEG_PRIVATE_H_

#define DRIVER_JPEG_PRIVATE_H_

#include "RTE_Components.h"
#include CMSIS_device_header

#include "Driver_JPEG.h"
#include "jpeg_hantro_vc9000e.h"

#ifdef __cplusplus
extern "C" {
#endif

/** \brief JPEG encoder configuration. */
typedef struct _JPEG_ENC_CONFIG {
    JPEG_SUBSAMPLING_MODE jpeg_mode;             /**< JPEG chroma subsampling mode              */
    JPEG_CODING_MODE      jpeg_coding_mode;      /**< JPEG coding mode                          */
    uint8_t               max_burst_length;      /**< AXI maximum burst length                  */
    uint8_t               axi_write_outstanding; /**< AXI write outstanding transactions        */
    uint8_t               axi_read_outstanding;  /**< AXI read outstanding transactions         */
} JPEG_ENC_CONFIG;

/** \brief JPEG Driver states. */
typedef volatile struct _JPEG_DRIVER_STATE {
    uint32_t initialized: 1;                  /**< Driver Initialized                         */
    uint32_t powered    : 1;                  /**< Driver powered                             */
    uint32_t reserved   : 30;                 /**< Reserved                                   */
} JPEG_DRIVER_STATE;

/** \brief Resources for a JPEG instance */
typedef struct _JPEG_RESOURCES {
    JPEG_Type              *regs;             /**< JPEG Register Base Address                      */
    ARM_JPEG_SignalEvent_t cb_event;          /**< Pointer to call back function                   */
    uint32_t               irq_priority;      /**< JPEG Interrupt priority                         */
    ARM_JPEG_FORMAT        format;            /**< User configured image format                    */
    jpeg_image_info_t      img_cfg;           /**< Hardware specific image configuration           */
    uint8_t                quality;           /**< JPEG quality factor (1-100)                     */
    ARM_JPEG_BUFFER        *active_output;    /**< Active output buffer during encoding            */
    JPEG_ENC_CONFIG        *enc_cfg;          /**< Static encoder configuration                    */
    jpeg_encode_info_t     enc_info;          /**< JPEG encode information                         */
    JPEG_DRIVER_STATE      state;             /**< JPEG driver status                              */
} JPEG_RESOURCES;

#ifdef __cplusplus
}
#endif

#endif /* DRIVER_JPEG_PRIVATE_H_ */
