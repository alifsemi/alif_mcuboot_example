/****************************************************************************
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2014-2024 Vivante Corporation
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 *
 ****************************************************************************/

#ifndef __VSI_COMM_AE_H__
#define __VSI_COMM_AE_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/**
 * @cond AE_V10
 *
 * @defgroup mpi_isp_ae AE V10 Common Definitions
 * @{
 *
 */

#include "vsios_sys.h"
#include "vsi_comm_video.h"
#include "vsi_comm_isp.h"
#include "vsi_comm_sns.h"

#include "mpi_isp_expm.h"
#ifdef ISP_HIST256
#include "mpi_isp_hist256.h"
#endif

/* @brief   Auto exposure attribute. */
typedef struct vsiISP_AE_ATTR_S {
    ISP_AE_RANGE_S expTimeRange; /* @brief The max and min value of exposure time,depend on sensor*/
    ISP_AE_RANGE_S againRange;   /* @brief The max and min value of analog gain, depend on sensor */
    ISP_AE_RANGE_S dgainRange;   /* @brief The max and min value of digital gain, depend on sensor*/
    vsi_u8_t aeRunInterval;      /* @brief The number of interval frames to run AE, default 1. */
    vsi_u8_t aeTarget;           /* @brief The target luminance of AE range [0, 255] */
    vsi_u8_t dampOver;           /* @brief When current luminance over the target, the larger the
                                  * value, the slower the rate of approaching the target.
                                  * range [0, 255]
                                  **/
    vsi_u8_t dampUnder;          /* @brief When current luminance under the target, the larger the
                                  * value, the slower the rate of approaching the target.
                                  * range [0, 255]
                                  **/
    vsi_u8_t tolerance;          /* @brief The threshold range to reach target. range [0, 100] */
    ISP_ANTIFLICKER_S antiflicker; /* @brief The antiflicker configuration,not currently supported*/
    ISP_AE_MODE_E aeMode;          /* @brief The mode of AE, currently support fix frame rate,
                                    * correspond to value 0
                                    **/
    vsi_u32_t      gainThreshold;  /* @brief Not currently supported */
    ISP_AE_ROUTE_S aeRoute;        /* @brief The route of AE         */
    ISP_AE_DELAY_S aeDelayAttr;    /* @brief Not currently supported */
    vsi_u8_t       weight[5][5];   /* @brief Not currently supported */
    vsi_u8_t       reserver[128];  /* @brief Reserve                 */
} ISP_AE_ATTR_S;

/* @brief   Manual exposure attribute. */
typedef struct vsiISP_ME_ATTR_S {
    vsi_u32_t intTime; /* @brief The exposure time, the range depend on sensor. */
    vsi_u32_t again;   /* @brief The analog gain, the range depend on sensor.   */
    vsi_u32_t dgain;   /* @brief The digital gain, the range depend on sensor.  */
} ISP_ME_ATTR_S;

/* @brief   Exposure attribute. */
typedef struct vsiISP_EXPOSURE_ATTR_S {
    ISP_OP_TYPE_E opType;   /* @brief The exposure running mode. \n 0: Automatic. \n 1: Manual. */
    ISP_AE_ATTR_S autoAttr;   /* @brief Auto exposure attributes.   */
    ISP_ME_ATTR_S manualAttr; /* @brief Manual exposure attributes. */
} ISP_EXPOSURE_ATTR_S;

/* @brief   HDR exposure attribute, not currently supported. */
typedef struct vsiISP_HDR_EXPOSURE_ATTR_S {
    ISP_OP_TYPE_E opType; /* @brief The HDR exposure running mode. \n 0: Automatic. \n 1: Manual.
                           */
    vsi_u32_t ratio[HDR_FRAME_MAX - 1]; /* @brief The HDR ratio. */
    vsi_u32_t minRatio;                 /* @brief The min ratio. */
    vsi_u32_t maxRatio;                 /* @brief The max ratio. */
} ISP_HDR_EXPOSURE_ATTR_S;

/* @brief   Exposure information. */
typedef struct vsiISP_EXPOSURE_INFO_S {
    vsi_u32_t expTime[HDR_FRAME_MAX];   /* @brief The exposure time. */
    vsi_u32_t again[HDR_FRAME_MAX];     /* @brief The analog gain.   */
    vsi_u32_t dgain[HDR_FRAME_MAX];     /* @brief The digital gain.  */
    vsi_u32_t exposure[HDR_FRAME_MAX];  /* @brief The exposure.      */
    vsi_u32_t iso;                      /* @brief The ISO. the larger the value,
                                         *  the more sensitive to light.
                                         **/
    vsi_u32_t ratio[HDR_FRAME_MAX - 1]; /* @brief Not currently supported. */
} ISP_EXPOSURE_INFO_S;

/* @brief   AE parameters. */
typedef struct vsiISP_AE_PARAM_S {
    ISP_HDR_MODE_E      hdrMode;   /* @brief The hdr mode.       */
    ISP_STICH_MODE_E    stichMode; /* @brief The stitching mode. */
    AE_SNS_FUNC_S       aeSnsFunc; /* @brief The structure of functions that AE control sensor. */
    ISP_EXPOSURE_ATTR_S expAttr;   /* @brief The AE attribute.   */
} ISP_AE_PARAM_S;

/* @brief   Exposure statistics. */
typedef struct vsiISP_AE_STAT_INFO_S {
    ISP_EXPM_STATISTICS_S expmStat; /* @brief The statistic of mean luminance. */
#ifdef ISP_HIST256
    ISP_HIST256_STATISTICS_S histStat; /* @brief The statistic of histogram.   */
#endif
} ISP_AE_STAT_INFO_S;

/* @brief   AE result. */
typedef struct vsiISP_AE_RESULT_S {
    vsi_u32_t intLine; /* @brief The number of exposure line. */
    vsi_u32_t again;   /* @brief The analog gain.             */
    vsi_u32_t dgain;   /* @brief The digital gain.            */
} ISP_AE_RESULT_S;

/* @brief   AE commands. */
typedef enum vsiISP_AE_CMD_E {
    ISP_AE_CMD_SET_ATTR     = 0, /* @brief The index of setting AE attribute.  */
    ISP_AE_CMD_GET_ATTR     = 1, /* @brief The index of getting AE attribute.  */
    ISP_AE_CMD_SET_HDR_ATTR = 2, /* @brief The index of setting HDR attribute. */
    ISP_AE_CMD_GET_HDR_ATTR = 3, /* @brief The index of getting HDR attribute. */
    ISP_AE_CMD_QUERY_INFO   = 4, /* @brief The index of query AE information.  */
} ISP_AE_CMD_E;

/* @brief   AE functions. */
typedef struct vsiISP_AE_FUNC_S {
    /* @brief The function pointer of initializing AE. */
    int (*pfnAeInit)(ISP_PORT              IspPort, const ISP_AE_PARAM_S *pParam);

    /* @brief The function pointer of running AE. */
    int (*pfnAeRun)(ISP_PORT                  IspPort, const ISP_AE_STAT_INFO_S *pAeStatInfo);

    /* @brief The function pointer of controlling AE. */
    int (*pfnAeCtrl)(ISP_PORT IspPort, vsi_u32_t cmd, void *pValue);

    /* @brief The function pointer of exiting AE. */
    int (*pfnAeExit)(ISP_PORT IspPort);
} ISP_AE_FUNC_S;

/* @} mpi_isp_ae */
/* @endcond */

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif
