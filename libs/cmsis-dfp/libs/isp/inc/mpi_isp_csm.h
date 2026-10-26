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

#ifndef __MPI_ISP_CSM_H__
#define __MPI_ISP_CSM_H__

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif

/**
 * @cond CSM_V10
 *
 * @defgroup mpi_isp_csm CSM V10 Definitions
 * @{
 *
 */

/** \brief   CSM type. */
typedef enum vsiISP_CSM_TYPE_E {
    ISP_CSM_TYPE_601 = 0,   /**< \brief BT601 standard */
    ISP_CSM_TYPE_709,       /**< \brief BT709 standard */
    ISP_CSM_TYPE_USER,
} ISP_CSM_TYPE_E;

/** \brief   CSM quantization. */
typedef enum vsiISP_CSM_QUANTIZATION_E {
    ISP_CSM_LIM_RANGE  = 0, /**< \brief Limit range */
    ISP_CSM_FULL_RANGE = 1, /**< \brief full range */
} ISP_CSM_QUANTIZATION_E;

/** \brief   CSM attribute. */
typedef struct vsiISP_CSM_ATTR_S {
    ISP_CSM_TYPE_E type;  /**< \brief Standard type */
    ISP_CSM_QUANTIZATION_E quantization;  /**< \brief Range type */
    vsi_s16_t coef[9];   /**< \brief Csm coefficient, range [-256 255]
                                \n 8bit fractional part(-1 0.996) */
} ISP_CSM_ATTR_S;

/*****************************************************************************/
/**
 * @brief   Gets CSM attribute.
 *
 * @param   IspPort             Port ID
 * @param   pCsmAttr          Pointer to the CSM attribute
 *
 * @retval  VSI_SUCCESS         Operation succeeded
 *
 *****************************************************************************/
int VSI_MPI_ISP_GetCsmAttr(ISP_PORT IspPort, ISP_CSM_ATTR_S *pCsmAttr);

/*****************************************************************************/
/**
 * @brief   Sets CSM attribute.
 *
 * @param   IspPort             Port ID
 * @param   pCsmAttr          Pointer to the CSM attribute

 *
 * @retval  VSI_SUCCESS         Operation succeeded
 *
 *****************************************************************************/
int VSI_MPI_ISP_SetCsmAttr(ISP_PORT IspPort, ISP_CSM_ATTR_S *pCsmAttr);

/* @} mpi_isp_csm */
/* @endcond */

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif