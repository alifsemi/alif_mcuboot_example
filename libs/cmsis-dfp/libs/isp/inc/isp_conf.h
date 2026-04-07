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
 * @file     isp_conf.h
 * @author   Yogender Kumar Arya
 * @email    yogender.kumar@alifsemi.com
 * @version  V1.0.0
 * @date     07-Jun-2025
 * @brief    Uses macro from isp_interface.h to enable Verisilion feature.
 * @bug      None.
 * @Note     None
 ******************************************************************************/

#ifndef LIB_ISP_INC_ISP_CONF_H_
#define LIB_ISP_INC_ISP_CONF_H_

#include "isp_interface.h"

#define ISP_DEV_CNT  1
#define ISP_PORT_CNT 1
#define ISP_CHN_CNT  1

#ifdef ISP_ARY_CROP_V10
#undef ISP_ARY_CROP_V10
#endif

#define ISPPICO_V2101 1

#if (RTE_ISP_SCALAR_MODULE)
#define ISP_SCALE_V10 1
// #define ISP_SCALE_V20 1
#define ISP_MP_SCALE  1
#endif

#ifdef ISP_ACQ_NR_NUMBERS
#undef ISP_ACQ_NR_NUMBERS
#endif

#if (RTE_ISP_AE_MODULE == 1)
#define ISP_AE_V10      1
#define ISP_AE_ALGO_V10 1
#endif

#if (RTE_ISP_WB_MODULE == 1)
#define ISP_WB_V10_1     1
#define ISP_AWB_ALGO_V10 1
#endif

#if (RTE_ISP_BINNING_MODULE == 1)
#define ISP_BINNING_V10 1
#endif

#if (RTE_ISP_BLS_MODULE)
#define ISP_BLS_V10 1
#endif

#if (RTE_ISP_DMSC_MODULE)
#define ISP_DMSC_V10 1
#endif

#if (RTE_ISP_FLT_MODULE)
#define ISP_FLT_V10 1
#endif

#if (RTE_ISP_CCM_MODULE)
#define ISP_CCM_V10 1
#endif

#if (RTE_ISP_GAMMAOUT_MODULE)
#define ISP_GAMMA_OUT_V10 1
#endif

#if (RTE_ISP_EXPM_MODULE)
#define ISP_EXPM_V10 1
#endif

#if (RTE_ISP_CSM_MODULE)
#define ISP_CSM_V10 1
#endif

#if (RTE_ISP_WBM_MODULE)
#define ISP_WBM_V10 1
#endif

#endif /* LIB_ISP_INC_ISP_CONF_H_ */
