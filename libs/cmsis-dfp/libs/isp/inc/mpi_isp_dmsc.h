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

#ifndef __VSI_ISP_DMSC_H__
#define __VSI_ISP_DMSC_H__

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif

/**
 * @cond DMSC_V10
 *
 * @defgroup mpi_isp_dmsc DMSC V10 Definitions
 * @{
 *
 */

typedef struct vsiISP_CAC_ATTR_S {
    vsi_bool_t enable;     /**< \brief Whether to enable CAC. \n 0: Disable CAC. \n 1: Enable CAC. */
    vsi_u8_t  hClipMode;  /**< \brief Horizontal clip mode, range [0 1]
                                \n 0: Set horizontal vector clipping to +/-4 pixel displacement
                                \n 1: Set horizontal vector clipping to +/-4 or +/-5 pixel displacement
                                \n depending on pixel position inside the Bayer raster
                                \n (dynamic switching between +/-4 and +/-5) */
    vsi_u8_t  vClipMode; /**< \brief Vertical clip mode, range [0 2]
                                \n 0: Set vertical vector clipping to +/-2 pixel ; fix filter_enable (Default)
                                \n 1: Set vertical vector clipping to +/-3 pixel;
                                \n dynamic filter_enable for chroma low pass filter
                                \n 2: Set vertical vector clipping +/-3 or +/-4 pixel displacement
                                \n 	depending on pixel position inside the Bayer raster
                                \n 	(dynamic switching between +/-3 and +/-4 */
    vsi_u16_t hStart;    /**< \brief 12 bit h_count preload value of the horizontal CAC pixel counter, range [1 4095].
                                \n Before line start h_count has to be preloaded with (h_size/2 + h_center_offset),
                                \n with h_size the image width and h_center_offset the horizontal distance between
                                \n image center and optical center. */
    vsi_u16_t vStart;     /**< \brief 12 bit v_count preload value of the vertical CAC line counter, range [1 4095].
                                \n Before frame start v_count has to be preloaded with (v_size/2 + v_center_offset),
                                \n with v_size the image height and v_center_offset the vertical distance between
                                \n image center and optical center. */
    vsi_s16_t aBlue;     /**< \brief Parameter A_Blue for radial blue shift calculation, range [-256 255].
                                \n 4bit fractional part(-16 15.9375)*/
    vsi_s16_t aRed;      /**< \brief Parameter A_Red for radial red shift calculation, range [-256 255].
                                \n 4bit fractional part(-16 15.9375)*/
    vsi_s16_t bBlue;     /**< \brief Parameter B_Blue for radial blue shift calculation, range [-256 255].
                                \n 4bit fractional part(-16 15.9375)*/
    vsi_s16_t bRed;      /**< \brief Parameter B_Red for radial red shift calculation, range [-256 255].
                                \n 4bit fractional part(-16 15.9375)*/
    vsi_s16_t cBlue;     /**< \brief Parameter C_Blue for radial blue shift calculation, range [-256 255].
                                \n 4bit fractional part(-16 15.9375)*/
    vsi_s16_t cRed;      /**< \brief Parameter C_Red for radial red shift calculation, range [-256 255].
                                \n 4bit fractional part(-16 15.9375)*/
    vsi_u8_t xNormShift; /**< \brief Horizontal normalization shift parameter x_ns (3 bit unsigned integer) in equation
                                　\n　x_d[7:0] = (((h_count << 4) >> x_ns) * x_normfactor) >> 5, range [0 15]　*/
    vsi_u8_t xNormFactor;/**< \brief　Horizontal scaling factor x_normfactor (5 bit unsigned integer) range 0 .. 31 in equation
                                \n x_d[7:0] = (((h_count << 4) >> x_ns) * x_normfactor) >> 5, range [0 31]　*/
    vsi_u8_t yNormShift; /**< \brief　Vertical normalization shift parameter y_ns (3 bit unsigned integer) in equation
                                \n　y_d[7:0] = (((v_count << 4) >> y_ns) * y_normfactor) >> 5, range [0 15]　*/
    vsi_u8_t yNormFactor; /**< \brief Vertical scaling factor y_normfactor(5 bit unsigned integer) range 0 .. 31 in equation
				\n y_d[7:0] = (((v_count << 4) >> y_ns) * y_normfactor) >> 5, range [0 31] */
} ISP_CAC_ATTR_S;

/** \brief   Demosaic attribute. */
typedef struct vsiISP_DMSC_ATTR_S {
    vsi_bool_t enable;                /**< \brief Whether to enable dmsc. \n 0: Enable dmsc. \n 1: Disable dmsc. */
    vsi_u8_t threshold;               /**< \brief The threshold for Bayer demosaicing texture detection.
                                            \n This value shifted left 4bit is compared with the
                                            \n difference of the vertical and horizontal 12Bit wide texture indicators,
                                            \n to decide if the vertical or horizontal texture flag must be set.
                                            \n 0x00: maximum edge sensitivity; 0xFF: no texture detection.
                                            \n range [0 255]*/
    ISP_CAC_ATTR_S cacAttr;           /**< \brief cac attribute */
} ISP_DMSC_ATTR_S;


/*****************************************************************************/
/**
 * @brief   Gets DMSC attribute.
 *
 * @param   IspPort             Port ID
 * @param   pDmscAttr           Pointer to the Dmsc attribute
 *
 * @retval  VSI_SUCCESS         Operation succeeded
 *
 *****************************************************************************/
int VSI_MPI_ISP_GetDmscAttr(ISP_PORT IspPort, ISP_DMSC_ATTR_S *pDmscAttr);

/*****************************************************************************/
/**
 * @brief   Sets Dmsc attribute.
 *
 * @param   IspPort             Port ID
 * @param   pDmscAttr           Pointer to the Dmsc attribute

 *
 * @retval  VSI_SUCCESS         Operation succeeded
 *
 *****************************************************************************/
int VSI_MPI_ISP_SetDmscAttr(ISP_PORT IspPort, ISP_DMSC_ATTR_S *pDmscAttr);

/* @} mpi_isp_dmsc */
/* @endcond */

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif
