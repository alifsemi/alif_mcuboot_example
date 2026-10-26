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

#ifndef __MPI_ISP_GAMMA_OUT_H__
#define __MPI_ISP_GAMMA_OUT_H__

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif

/**
 * @cond GAMMA_OUT_V10
 *
 * @defgroup mpi_isp_gamma_out GAMMA_OUT V10 Definitions
 * @{
 *
 */


#define ISP_GAMMA_OUT_CURVE_SIZE 17 /**< \brief Gamma out v10 curve size. */

#define VSI_ISP_GAMMA_OUT_GAMMAY_MAX 4095          /**< \brief The maximum value of GAMMA OUT GammaY. */
#define VSI_ISP_GAMMA_OUT_GAMMAY_MIN 0             /**< \brief The minimum value of GAMMA OUT GammaY. */

/** \brief   GAMMA_OUT attributes. */
typedef struct vsiISP_GAMMA_OUT_ATTR_S {
    vsi_bool_t enable;                             /**< \brief Whether to enable DPF. \n 0: Disable. \n 1: Enable. */
    vsi_u16_t  gammaY[ISP_GAMMA_OUT_CURVE_SIZE]; /**< \brief Gamma curve */
} ISP_GAMMA_OUT_ATTR_S;

/*****************************************************************************/
/**
 * @brief   Gets GammaOut attributes.
 *
 * @param   IspPort             Port ID
 * @param   pGammaOutAttr     Pointer to the GammaOut attributes
 *
 * @retval  VSI_SUCCESS         Operation succeeded
 *
 *****************************************************************************/
int VSI_MPI_ISP_GetGammaOutAttr(ISP_PORT IspPort, ISP_GAMMA_OUT_ATTR_S *pGammaOutAttr);

/*****************************************************************************/
/**
 * @brief   Sets GammaOut attributes.
 *
 * @param   IspPort             Port ID
 * @param   pGammaOutAttr     Pointer to the GammaOut attributes
 *
 * @retval  VSI_SUCCESS         Operation succeeded
 *
 *****************************************************************************/
int VSI_MPI_ISP_SetGammaOutAttr(ISP_PORT IspPort, ISP_GAMMA_OUT_ATTR_S *pGammaOutAttr);

/* @} mpi_isp_gamma_out */
/* @endcond */

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif
