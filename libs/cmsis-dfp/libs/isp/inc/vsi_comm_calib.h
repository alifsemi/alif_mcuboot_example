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

#ifndef __ISP_COMM_CALIB_H__
#define __ISP_COMM_CALIB_H__

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif

#include "mpi_isp.h"
#include "mpi_isp_bls.h"
#include "mpi_isp_expm.h"
#include "mpi_isp_ae.h"
#include "mpi_isp_wbm.h"
#include "mpi_isp_wb.h"
#include "mpi_isp_dmsc.h"
#include "mpi_isp_flt.h"
#include "mpi_isp_ccm.h"
#include "mpi_isp_gamma_out.h"
#include "mpi_isp_csm.h"

/** \brief   Calibration data head. */
typedef struct vsiISP_CALIB_HEAD_S {
    char name[64];    /**< \brief The file name of Calibration data */
    char date[64];    /**< \brief The generating date of Calibration data */
    char version[64]; /**< \brief The project version of Calibration data */
} ISP_CALIB_HEAD_S;

/** \brief   Calibration module. */
typedef struct vsiISP_CALIB_MODULE_S {
    ISP_AUTO_ROUTE_S autoRoute;   /**< \brief Auto route. */
    ISP_BLS_ATTR_S  bls;          /**< \brief BLS. */
    ISP_EXPM_ATTR_S aem;          /**< \brief AE measurement. */
    ISP_EXPOSURE_ATTR_S ae;       /**< \brief AE. */
    ISP_WBM_ATTR_S wbm;           /**< \brief WB measurement. */
    ISP_WB_ATTR_S wb;             /**< \brief WB. */
    ISP_DMSC_ATTR_S dmsc;         /**< \brief Demosaic. */
    ISP_FLT_ATTR_S flt;           /**< \brief Filter. */
    ISP_CCM_ATTR_S ccm;           /**< \brief CCM. */
    ISP_GAMMA_OUT_ATTR_S gammaOut;/**< \brief Gamma out. */
    ISP_CSM_ATTR_S csm;           /**< \brief CSM. */
} ISP_CALIB_MODULE_S;

/** \brief   Calibration data. */
typedef struct vsiISP_CALIB_DATA_S {
    ISP_CALIB_HEAD_S head;       /**< \brief Calibration data head. */
    ISP_CALIB_MODULE_S modules;  /**< \brief Calibration data module. */
} ISP_CALIB_DATA_S;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif