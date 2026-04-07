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

#ifndef __MPI_ISP_CALIB_H__
#define __MPI_ISP_CALIB_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#include "vsi_comm_calib.h"
#include "isp_interface.h"

static inline int VSI_MPI_ISP_SetCalib(ISP_PORT IspPort, ISP_CALIB_DATA_S *pCalibData)
{
    VSI_MPI_ISP_SetAutoRoute(IspPort, &pCalibData->modules.autoRoute);

#if (RTE_ISP_BLS_MODULE)
    VSI_MPI_ISP_SetBlsAttr(IspPort, &pCalibData->modules.bls);
#endif

#if (RTE_ISP_EXPM_MODULE)
    VSI_MPI_ISP_SetExpmAttr(IspPort, &pCalibData->modules.aem);
#endif  // (RTE_ISP_EXPM_MODULE)

#if (ISP_AE_V10)
    VSI_MPI_ISP_SetExposureAttr(IspPort, &pCalibData->modules.ae);
#endif  // (ISP_AE_V10)

#if (RTE_ISP_WBM_MODULE)
    VSI_MPI_ISP_SetWbmAttr(IspPort, &pCalibData->modules.wbm);
#endif  // (RTE_ISP_WBM_MODULE)

#if (ISP_WB_V10_1)
    VSI_MPI_ISP_SetWbAttr(IspPort, &pCalibData->modules.wb);
#endif  // (ISP_WB_V10_1)

#if (RTE_ISP_DMSC_MODULE)
    VSI_MPI_ISP_SetDmscAttr(IspPort, &pCalibData->modules.dmsc);
#endif  // (RTE_ISP_DMSC_MODULE)

#if (RTE_ISP_FLT_MODULE)
    VSI_MPI_ISP_SetFltAttr(IspPort, &pCalibData->modules.flt);
#endif  // (RTE_ISP_FLT_MODULE)

#if (RTE_ISP_CCM_MODULE)
    VSI_MPI_ISP_SetCcmAttr(IspPort, &pCalibData->modules.ccm);
#endif  // (RTE_ISP_CCM_MODULE)

#if (RTE_ISP_GAMMAOUT_MODULE)
    VSI_MPI_ISP_SetGammaOutAttr(IspPort, &pCalibData->modules.gammaOut);
#endif  // (RTE_ISP_GAMMAOUT_MODULE)

#if (RTE_ISP_CSM_MODULE)
    VSI_MPI_ISP_SetCsmAttr(IspPort, &pCalibData->modules.csm);
#endif  // (RTE_ISP_CSM_MODULE)

    return VSI_SUCCESS;
}

static inline int VSI_MPI_ISP_GetCalib(ISP_PORT IspPort, ISP_CALIB_DATA_S *pCalibData)
{
    VSI_MPI_ISP_GetAutoRoute(IspPort, &pCalibData->modules.autoRoute);

#if (RTE_ISP_BLS_MODULE)
    VSI_MPI_ISP_GetBlsAttr(IspPort, &pCalibData->modules.bls);
#endif

#if (RTE_ISP_EXPM_MODULE)
    VSI_MPI_ISP_GetExpmAttr(IspPort, &pCalibData->modules.aem);
#endif  // (RTE_ISP_EXPM_MODULE)

#if (ISP_AE_V10)
    VSI_MPI_ISP_GetExposureAttr(IspPort, &pCalibData->modules.ae);
#endif  // (ISP_AE_V10)

#if (RTE_ISP_WBM_MODULE)
    VSI_MPI_ISP_GetWbmAttr(IspPort, &pCalibData->modules.wbm);
#endif  // (RTE_ISP_WBM_MODULE)

#if (ISP_WB_V10_1)
    VSI_MPI_ISP_GetWbAttr(IspPort, &pCalibData->modules.wb);
#endif  // (ISP_WB_V10_1)

#if (RTE_ISP_DMSC_MODULE)
    VSI_MPI_ISP_GetDmscAttr(IspPort, &pCalibData->modules.dmsc);
#endif  // (RTE_ISP_DMSC_MODULE)

#if (RTE_ISP_FLT_MODULE)
    VSI_MPI_ISP_GetFltAttr(IspPort, &pCalibData->modules.flt);
#endif  // (RTE_ISP_FLT_MODULE)

#if (RTE_ISP_CCM_MODULE)
    VSI_MPI_ISP_GetCcmAttr(IspPort, &pCalibData->modules.ccm);
#endif  // (RTE_ISP_CCM_MODULE)

#if (RTE_ISP_GAMMAOUT_MODULE)
    VSI_MPI_ISP_GetGammaOutAttr(IspPort, &pCalibData->modules.gammaOut);
#endif  // (RTE_ISP_GAMMAOUT_MODULE)

#if (RTE_ISP_CSM_MODULE)
    VSI_MPI_ISP_GetCsmAttr(IspPort, &pCalibData->modules.csm);
#endif  // (RTE_ISP_CSM_MODULE)

    return VSI_SUCCESS;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif
