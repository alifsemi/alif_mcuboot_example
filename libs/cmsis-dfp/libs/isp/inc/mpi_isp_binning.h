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
#ifndef __MPI_ISP_BINNING_H__
#define __MPI_ISP_BINNING_H__

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif

/**
 * @cond BINNING_V10
 *
 * @defgroup mpi_isp_binning BINNING V10 Definitions
 * @{
 *
 */


/** \brief   BINNING attribute. */
typedef struct vsiISP_BINNING_ATTR_S {
    vsi_bool_t     enable;         /**< \brief Whether to enable BINNING. \n 0: Disable BINNING. \n 1: Enable BINNING. */
    vsi_u8_t       binHStep;      /**< \brief Binning horizontal step size. */
    vsi_u8_t       binVStep;      /**< \brief Binning vertical step size. */
} ISP_BINNING_ATTR_S;


/*****************************************************************************/
/**
 * @brief   Gets BINNING attribute.
 *
 * \param   IspPort             Port ID
 * \param   pBinningAttr      Pointer to the BINNING attribute
 *
 * \retval  VSI_SUCCESS         Operation succeeded
 *
 *****************************************************************************/
int VSI_MPI_ISP_GetBinningAttr(ISP_PORT IspPort, ISP_BINNING_ATTR_S *pBinningAttr);

/*****************************************************************************/
/**
 * @brief   Sets BINNING attribute.
 *
 * @param   IspPort             Port ID
 * @param   pBinningAttr      Pointer to the BINNING attribute

 *
 * @retval  VSI_SUCCESS              Operation succeeded
 * @retval  VSI_ERR_ILLEGAL_PARAM    Invalid parameter
 *
 *****************************************************************************/
int VSI_MPI_ISP_SetBinningAttr(ISP_PORT IspPort, ISP_BINNING_ATTR_S *pBinningAttr);



/* @} mpi_isp_binning */
/* @endcond */


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif
