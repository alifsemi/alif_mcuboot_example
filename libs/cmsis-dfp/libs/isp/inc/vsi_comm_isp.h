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

#ifndef __VSI_COMM_ISP_H__
#define __VSI_COMM_ISP_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#include "vsios_type.h"
#include "vsi_comm_video.h"

/**
 * @defgroup mpi_isp Mpp Isp Common Definitions
 * @{
 *
 *
 */

#define ISP_AUTO_STRENGTH_NUN 16

typedef vsi_u32_t ISP_DEV;

/** \brief   ISP port. */
typedef struct {
    vsi_u32_t devId;  /**< \brief The ID of device. */
    vsi_u32_t portId; /**< \brief The ID of port. */
} ISP_PORT;

/** \brief   ISP channel. */
typedef struct {
    vsi_u32_t devId;  /**< \brief The ID of device. */
    vsi_u32_t portId; /**< \brief The ID of port. */
    vsi_u32_t chnId;  /**< \brief The ID of channel. */
} ISP_CHN;

/** \brief   ISP work mode. */
typedef enum vsiISP_WORK_MODE_E {
    WORK_MODE_NORMAL = 0, /**< \brief Normal */
    WORK_MODE_MCM    = 1, /**< \brief MCM */
    WORK_MODE_BUTT        /**< \brief Upper border (only for an internal evaluation). */
} ISP_WORK_MODE_E;

/** \brief   ISP device attribute. */
typedef struct vsiISP_DEV_ATTR_S {
    ISP_WORK_MODE_E ispWorkMode; /**< \brief ISP work mode */
} ISP_DEV_ATTR_S;

/** \brief   ISP input type. */
typedef enum vsiISP_INPUT_TYPE_E {
    INPUT_TYPE_SENSOR = 0, /**< \brief Data from sensor port to isp */
    INPUT_TYPE_TPG    = 1, /**< \brief Data from ISP tpg to isp */
    INPUT_TYPE_DMA    = 2, /**< \brief Data from DMA to isp */
    INPUT_TYPE_BUTT        /**< \brief Upper border (only for an internal evaluation). */
} ISP_INPUT_TYPE_E;

/** \brief   ISP mode. */
typedef enum vsiISP_MODE_E {
    ISP_MODE_BT656 = 1, /**< \brief ITU-R BT.656 (YUV with embedded sync). */
    ISP_MODE_BT601 = 2, /**< \brief ITU-R BT.601 (YUV input with H and Vsync signals). */
    ISP_MODE_RAW   = 3, /**< \brief RAW picture with BT.601 sync (ISP bypass). */
    ISP_MODE_BUTT,      /**< \brief Upper border (only for an internal evaluation). */
} ISP_MODE_E;

/** \brief   ISP HDR mode. */
typedef enum vsiISP_HDR_MODE_E {
    HDR_MODE_LINEAR       = 0, /**< \brief Linear */
    HDR_MODE_ISP_STICH    = 1, /**< \brief ISP stitching */
    HDR_MODE_SENSOR_STICH = 2, /**< \brief Sensor stitching */
    HDR_MODE_BUTT,             /**< \brief Upper border (only for an internal evaluation). */
} ISP_HDR_MODE_E;

/** \brief   ISP stitching mode. */
typedef enum vsiISP_STICH_MODE_E {
    STICH_MODE_3DOL_DUAL_DCG       = 0, /**< \brief 3DOL dual DCG. */
    STICH_MODE_3DOL                = 1, /**< \brief 3DOL. */
    STICH_MODE_3DOL_LINEBYLINE     = 2, /**< \brief 3DOL line by line. */
    STICH_MODE_3DOL_16BIT_COMPRESS = 3, /**< \brief 3DOL 16bit compress . */
    STICH_MODE_2DOL_DUAL_DCG       = 4, /**< \brief 2DOL dual DCG. */
    STICH_MODE_2DOL                = 5, /**< \brief 2DOL. */
    STICH_MODE_2DOL_L_AND_S        = 6, /**< \brief 2DOL low and small frequency. */
    STICH_MODE_BUTT, /**< \brief Upper border (only for an internal evaluation). */
} ISP_STICH_MODE_E;

/** \brief   ISP bayer pattern. */
typedef enum vsiISP_BAYER_PAT_E {
    BAYER_PAT_RGGB = 0, /**< \brief 1st line: RGRG... , 2nd line GBGB... , etc. */
    BAYER_PAT_GRBG = 1, /**< \brief 1st line: GRGR... , 2nd line BGBG... , etc. */
    BAYER_PAT_GBRG = 2, /**< \brief 1st line: GBGB... , 2nd line RGRG... , etc. */
    BAYER_PAT_BGGR = 3, /**< \brief 2st line: BGBG... , 2nd line GRGR... , etc. */
} ISP_BAYER_PAT_E;

/** \brief   ISP sample edge. */
typedef enum vsiISP_SAMPLE_EDGE_E {
    SAMPLE_EDGE_FALLING = 0, /**< \brief Sample falling edga. */
    SAMPLE_EDGE_RISING  = 1, /**< \brief Sample rising edge. */
} ISP_SAMPLE_EDGE_E;

/** \brief   ISP synchronized polarity. */
typedef enum vsiISP_SYNC_POL_E {
    SYNC_POL_HIGH = 0, /**< \brief The index of Rising edge. */
    SYNC_POL_LOW  = 1, /**< \brief The index of falling edge. */
} ISP_SYNC_POL_E;

/** \brief   ISP subsampling mode. */
typedef enum vsiISP_CONV422_E {
    CONV422_COSITED = 0, /**< \brief Co-sited color subsampling Y0Cb0Cr0 - Y1. */
    CONV422_INTER =
        1, /**< \brief IInterleaved color subsampling Y0Cb0 - Y1Cr1 (not recommended). */
    CONV422_NOCOSITED = 2, /**< \brief Non-cosited color subsampling Y0Cb(0+1)/2 - Y1Cr(0+1)/2. */
} ISP_CONV422_E;

/** \brief   ISP CCIR sequence. */
typedef enum vsiISP_CCIR_SEQ_E {
    CCIR_YCBYCR = 0, /**< \brief YCBYCR. */
    CCIR_YCRYCB = 1, /**< \brief YCRYCB. */
    CCIR_CBYCRY = 2, /**< \brief CBYCRY. */
    CCIR_CRYCBY = 3, /**< \brief CRYCBY. */
} ISP_CCIR_SEQ_E;

/** \brief   ISP field sampling. */
typedef enum vsiISP_FILED_SEL_E {
    FIELDSEL_BOTH = 0, /**< \brief Sample all fields (don't care about fields). */
    FIELDSEL_EVEN = 1, /**< \brief Sample only even fields. */
    FIELDSEL_ODD  = 2, /**< \brief Sample only odd fields. */
} ISP_FILED_SEL_E;

/** \brief   Bus width of ISP input. */
typedef enum vsiISP_INPUT_SEL_E {
    INPUT_SEL_12BIT             = 0, /**< \brief 12 Bit input */
    INPUT_SEL_10BIT_APPEND_ZERO = 1, /**< \brief 10 Bit input with 2 zereos as LSB's */
    INPUT_SEL_10BIT_APPEND_MSB  = 2, /**< \brief 10 Bit input with 2 MSB's as LSB's */
    INPUT_SEL_8BIT_APPEND_ZERO  = 3, /**< \brief 8 Bit input with 4 zeroes as LSB's */
    INPUT_SEL_8BIT_APPEND_MSB   = 4, /**< \brief 8 Bit input with 4 MSB's as LSB's */
} ISP_INPUT_SEL_E;

/** \brief   ISP latency FIFO mode. */
typedef enum vsiISP_LATENCY_FIFO_E {
    LATENCY_FIFO_INPUT_FMT = 0, /**< \brief Use input formatter data for latency FIFO. */
    LATENCY_FIFO_DMA_READ  = 1, /**< \brief Use DMA RGB read data for latency FIFO. */
} ISP_LATENCY_FIFO_E;

/** \brief   ISP auto route. */
typedef struct vsiISP_AUTO_ROUTE_S {
    vsi_u32_t autoRoute[ISP_AUTO_STRENGTH_NUN]; /**< \brief ISP auto route. */
} ISP_AUTO_ROUTE_S;

/** \brief   ISP synchronization configuration. */
typedef struct vsiISP_SYNC_CONFIG_S {
    ISP_SAMPLE_EDGE_E  sampleEdge;     /**< \brief sampling edge. */
    ISP_SYNC_POL_E     hsyncPol;       /**< \brief Horizontal synchronization polarity. */
    ISP_SYNC_POL_E     vsyncPol;       /**< \brief Vertical synchronization polarity. */
    ISP_CONV422_E      con422;         /**< \brief ISP subsampling mode. */
    ISP_CCIR_SEQ_E     ccirSeq;        /**< \brief ISP CCIR sequence. */
    ISP_FILED_SEL_E    filedSelection; /**< \brief ISP field sampling mode. */
    ISP_LATENCY_FIFO_E lantencyFifo;   /**< \brief ISP latency FIFO mode. */
} ISP_SYNC_CONFIG_S;

/** \brief   ISP port attribute. */
typedef struct vsiISP_PORT_ATTR_S {
    ISP_INPUT_TYPE_E  ispInputType; /**< \brief ISP input type. */
    ISP_MODE_E        ispMode;      /**< \brief ISP mode. */
    ISP_HDR_MODE_E    hdrMode;      /**< \brief ISP HDR mode. */
    ISP_STICH_MODE_E  stichMode;    /**< \brief ISP stitching mode. */
    PIXEL_FORMAT_E    pixelFormat;  /**< \brief Pixel format. */
    ISP_SYNC_CONFIG_S syncCfg;      /**< \brief ISP synchronization configuration. */
    RECT_S            snsRect;      /**< \brief Sensor rectangle parameter. */
    vsi_u32_t         snsFps;       /**< \brief Sensor FPS. */
    RECT_S            inFormRect;   /**< \brief Inform rectangle parameter. */
    RECT_S            outFormRect;  /**< \brief Outform rectangle parameter. */
    RECT_S            iSRect;       /**< \brief Is rectangle parameter. */
} ISP_PORT_ATTR_S;

/** \brief   ISP transmission bus mode. */
typedef enum vsiISP_TRANS_BUS_E {
    TRANS_BUS_ONLINE = 0, /**< \brief Online. */
    TRANS_BUS_FLEXA  = 1, /**< \brief Flexa. */
    TRANS_BUS_DMA    = 2, /**< \brief DMA. */
} ISP_TRANS_BUS_E;

/** \brief   ISP channel attribute. */
typedef struct vsiISP_CHN_ATTR_S {
    ISP_TRANS_BUS_E transBus;  /**< \brief ISP transmission bus mode. */
    FORMAT_S        chnFormat; /**< \brief Channel format. */
} ISP_CHN_ATTR_S;

/** \brief   ISP channel ID. */
typedef enum vsiISP_CHN_ID_E {
    CHN_ID_MP  = 0, /**< \brief Main path. */
    CHN_ID_SP1 = 1, /**< \brief Self path. */
    CHN_ID_SP2 = 2, /**< \brief Self path two. */
    CHN_ID_RAW = 3, /**< \brief Raw path. */
} ISP_CHN_ID_E;

/** \brief   ISP status. */
typedef enum vsiISP_STATUS_E {
    ISP_STATE_STREAMOFF = 0, /**< \brief Stream off. */
    ISP_STATE_STREAMON  = 1, /**< \brief Stream on. */
} ISP_STATUS_E;

/** \brief   ISP operate type. */
typedef enum vsiISP_OP_TYPE_E {
    OP_TYPE_AUTO   = 0, /**< \brief Auto mode. */
    OP_TYPE_MANUAL = 1, /**< \brief Manual mode. */
} ISP_OP_TYPE_E;

/** \brief   ISP illuminant type. */
typedef enum vsiISP_ILLUMINANT_TYPE_E {
    ILLUMINANT_A = 0,    /**< \brief A. */
    ILLUMINANT_TL84,     /**< \brief TL84. */
    ILLUMINANT_CWF,      /**< \brief CWF. */
    ILLUMINANT_D50,      /**< \brief D50. */
    ILLUMINANT_D65,      /**< \brief D65. */
    ILLUMINANT_TYPE_CNT, /**< \brief The number of illuminant type. */
} ISP_ILLUMINANT_TYPE_E;

/* @} vsi_comm_isp */

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif
