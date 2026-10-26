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

#ifndef __VSI_ISP_FLT_H__
#define __VSI_ISP_FLT_H__

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif

/**
 * @cond FLT_V10
 *
 * @defgroup mpi_isp_flt FLT V10 Definitions
 * @{
 *
 */

#define VSI_ISP_FLT_LEVEL_MIN    0    /**< \brief The minimum value of De-nosing level and Sharpen level. */
#define VSI_ISP_FLT_LEVEL_MAX    10   /**< \brief The maximum value of De-nosing level and Sharpen level. */



/** \brief   Filter manual attribute. */
typedef struct vsiISP_FLT_MANUAL_ATTR_S {
    vsi_u8_t denoiseLevel;    /**< \brief De-nosing level configurations.
                                    \n range [0 10]*/
    vsi_u8_t sharpenLevel;    /**< \brief Sharpen level configurations.
                                    \n range [0 10]*/
} ISP_FLT_MANUAL_ATTR_S;

/** \brief   Filter auto  attribute. */
typedef struct vsiISP_FLT_AUTO_ATTR_S {
    vsi_u8_t denoiseLevel[ISP_AUTO_STRENGTH_NUN];    /**< \brief De-nosing level configurations.
                                                            \n range [0 10]*/
    vsi_u8_t sharpenLevel[ISP_AUTO_STRENGTH_NUN];    /**< \brief Sharpen level configurations.
                                                            \n range [0 10]*/
} ISP_FLT_AUTO_ATTR_S;

/** \brief   Filter attribute. */
typedef struct vsiISP_FLT_ATTR_S {
    vsi_bool_t            enable;         /**< \brief Whether to enable filter. \n 0: Disable filter. \n 1: Enable filter. */
    ISP_OP_TYPE_E         opType;         /**< \brief FLT mode attribute */
    ISP_FLT_MANUAL_ATTR_S manualAttr;     /**< \brief FLT manual attribute */
    ISP_FLT_AUTO_ATTR_S   autoAttr;       /**< \brief FLT auto attribute */
} ISP_FLT_ATTR_S;


/*****************************************************************************/
/**
 * @brief   Gets FLT attribute.
 *
 * @param   IspPort             Port ID
 * @param   pFltAttr            Pointer to the Flt attribute
 *
 * @retval  VSI_SUCCESS         Operation succeeded
 *
 *****************************************************************************/
int VSI_MPI_ISP_GetFltAttr(ISP_PORT IspPort, ISP_FLT_ATTR_S *pFltAttr);

/*****************************************************************************/
/**
 * @brief   Sets Flt attribute.
 *
 * @param   IspPort             Port ID
 * @param   pFltAttr            Pointer to the Flt attribute

 *
 * @retval  VSI_SUCCESS         Operation succeeded
 *
 *****************************************************************************/
int VSI_MPI_ISP_SetFltAttr(ISP_PORT IspPort, ISP_FLT_ATTR_S *pFltAttr);

/* @} mpi_isp_flt */
/* @endcond */

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif
