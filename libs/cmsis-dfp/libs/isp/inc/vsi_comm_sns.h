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

#ifndef __VSI_COMM_SNS_H__
#define __VSI_COMM_SNS_H__

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif

/**
 * @defgroup vsi_comm_sns Mpp Sensor Common Definitions
 * @{
 *
 *
 */

#define HDR_FRAME_MAX 4
#define ISP_MAX_SNS_REG 64
#define ISP_SNS_FPS_ACCU 100
#define ISP_SNS_GAIN_ACCU 1024
#define ISP_AE_ROUTE_MAX_NODES 16
#define ISP_ANTIFLCKET_ACCU 0x100

/** \brief   ISP sensor type. */
typedef enum vsiISP_SNS_TYPE_E {
    ISP_SNS_I2C_TYPE = 0,    /**< \brief I2C. */
    ISP_SNS_SSP_TYPE,        /**< \brief SSP. */
} ISP_SNS_TYPE_E;

/** \brief   ISP sensor Data. */
typedef struct vsiISP_I2C_DATA_S {
    vsi_u8_t   delayFrameNum;   /**< \brief The number of delay frames. */
    vsi_u32_t  regAddr;         /**< \brief The address of register. */
    vsi_u32_t  data;            /**< \brief The data of register. */
} ISP_SNS_DATA_S;

/** \brief   ISP sensor register information. */
typedef struct vsiISP_SNS_REGS_INFO_S {
    vsi_u8_t  snsDev;       /**< \brief The sensor device number. */
    vsi_u8_t  slaveAddr;    /**< \brief The sensor slave address. */
    vsi_u8_t  addrByteNum;  /**< \brief The byte number of address. */
    vsi_u8_t  dataByteNum;  /**< \brief The byte number of data. */
    vsi_u32_t regCnt;       /**< \brief The number of register. */
    vsi_u8_t  delayMax;     /**< \brief The maximum delay frames. */
    ISP_SNS_DATA_S snsData[ISP_MAX_SNS_REG];   /**< \brief The data of sensor. */
} ISP_SNS_REGS_INFO_S;

/** \brief   ISP sensor mode. */
typedef struct vsiISP_SNS_MODE_S {
    vsi_u32_t width;            /**< \brief The width of image. */
    vsi_u32_t height;           /**< \brief The height of image. */
    vsi_u32_t fps;              /**< \brief The FPS of sensor. */
    PIXEL_FORMAT_E pixelFormat; /**< \brief The pixel format. */
    ISP_HDR_MODE_E hdrMode;     /**< \brief The HDR mode. */
    ISP_STICH_MODE_E stichMode; /**< \brief The stitching mode. */
} ISP_SNS_MODE_S;

/** \brief   ISP exposure ratio. HDR is not currently supported */
typedef struct vsiISP_EXP_RATIO_S {
    ISP_OP_TYPE_E         opType;       /**< \brief The mode of operation. */
    vsi_u32_t ratio[HDR_FRAME_MAX - 1]; /**< \brief The exposure ratio of HDR. */
    vsi_u32_t minRatio;                 /**< \brief The minimum ratio of exposure. */
    vsi_u32_t maxRatio;                 /**< \brief The maximum ratio of exposure. */
} ISP_EXP_RATIO_S;

/** \brief   ISP AE range. */
typedef struct vsiISP_AE_RANGE_S {
    vsi_u32_t min;     /**< \brief The minimum. */
    vsi_u32_t max;     /**< \brief The maximum. */
} ISP_AE_RANGE_S;

/** \brief   ISP AE route node. */
typedef struct vsiISP_AE_ROTUE_NODE_S {
    vsi_u32_t intTime;       /**< \brief The exposure time, the range depend on sensor. */
    vsi_u32_t again;         /**< \brief The analog gain, the range depend on sensor. */
    vsi_u32_t dgain;         /**< \brief The digital gain, the range depend on sensor. */
} ISP_AE_ROTUE_NODE_S;

/** \brief   ISP AE route. */
typedef struct vsiISP_AE_ROUTE_S {
    vsi_u32_t totalNum;       /**< \brief The number of AE route node. */
    ISP_AE_ROTUE_NODE_S routeNode[ISP_AE_ROUTE_MAX_NODES];  /**< \brief AE route. */
} ISP_AE_ROUTE_S;

/** \brief   ISP AE mode. */
typedef enum vsiISP_AE_MODE_E {
    AE_MODE_FIX_FRAME_RATE = 0,  /**< \brief Fix frame rate. */
    AE_MODE_SLOW_SHUTTER = 1,    /**< \brief Slow shutter, not currently supported. */
} ISP_AE_MODE_E;

/** \brief   ISP AE antiflicker. */
typedef struct vsiISP_ANTIFLICKER_S {
    vsi_bool_t enable;      /**< \brief Whether to enable antiflicker.
                                  \n 0: Disable antiflicker. \n 1: Enable antiflicker. */
    vsi_u32_t flickerFreq;  /**< \brief The frequency of antiflicker. */
} ISP_ANTIFLICKER_S;

/**< \brief ISP AE delay, Not currently supported */
typedef struct vsiISP_AE_DELAY_S {
    vsi_u16_t blackDelayFrame;   /**< \brief The delay frames when scene darkens. */
    vsi_u16_t whiteDelayFrame;   /**< \brief The delay frames when scene gets brighter. */
} ISP_AE_DELAY_S;

/** \brief   ISP AE sensor default. */
typedef struct vsiAE_SNS_DEFAULT_S {
    vsi_u32_t fullLinesMax;     /**< \brief The maximum value of full lines. */
    vsi_u32_t fullLinesStd;     /**< \brief The standard maximum of full lines. */
    vsi_u32_t fullLines;        /**< \brief The full lines. */

    vsi_u32_t fps;              /**< \brief The frame rate. */
    vsi_u32_t linesPer500ms;    /**< \brief The number of exposure lines per 500ms. */

    vsi_u32_t maxIntLine;       /**< \brief The maximum exposure lines. */
    vsi_u32_t minIntLine;       /**< \brief The minimum exposure lines. */
    vsi_u32_t intLineStep;      /**< \brief The step of exposure lines. */
    vsi_u32_t maxAgain;         /**< \brief The maximum of analog gain. */
    vsi_u32_t minAgain;         /**< \brief The minimum of analog gain. */
    vsi_u32_t againStep;        /**< \brief The step of analog gain. */
    vsi_u32_t maxDgain;         /**< \brief The maximum of digital gain. */
    vsi_u32_t minDgain;         /**< \brief The minimum of digital gain. */
    vsi_u32_t dgainStep;        /**< \brief The step of digital gain. */

    vsi_u8_t  aeRunInterval;    /**< \brief The number of interval frames to run AE. */
    vsi_u8_t  aeTarget;         /**< \brief The target luminance of AE. */
    vsi_u8_t  dampOver;         /**< \brief When current luminance over the target, the larger the value,
                                     the slower the rate of approaching the target. */
    vsi_u8_t  dampUnder;        /**< \brief When current luminance under the target, the larger the value,
                                     the slower the rate of approaching the target. */
    vsi_u8_t  tolerance;        /**< \brief The threshold range to reach target. */
    vsi_u32_t initExposure;     /**< \brief initial exposure. */
    ISP_ANTIFLICKER_S antiflicker;   /**< \brief ISP antiflicker. */
    ISP_EXP_RATIO_S   expRatio;      /**< \brief ISP exposure ratio. */
    ISP_AE_ROUTE_S    aeRoute;       /**< \brief ISP AE route. */
    ISP_AE_MODE_E     aeMode;        /**< \brief ISP AE mode. */
    vsi_u32_t         gainThreshold; /**< \brief Not currently supported */
    ISP_AE_DELAY_S    aeDelayAttr;   /**< \brief Not currently supported */
} AE_SNS_DEFAULT_S;

/** \brief   ISP sensor functions. */
typedef struct vsiISP_SNS_FUNC_S {
    int (*pfnSensorInit)(ISP_PORT IspPort, vsi_u8_t snsDev);  /**< \brief The function pointer of initializing sensor. */
    int (*pfnSensorExit)(ISP_PORT IspPort);                   /**< \brief The function pointer of exiting sensor. */
    int (*pfnWriteReg)(ISP_PORT IspPort, vsi_u32_t addr, vsi_u32_t data);  /**< \brief The function pointer of writing sensor register. */
    int (*pfnReadReg)(ISP_PORT IspPort, vsi_u32_t addr, vsi_u32_t *pData); /**< \brief The function pointer of reading sensor register. */
    int (*pfnSetMode)(ISP_PORT IspPort, ISP_SNS_MODE_S *pSnsMode);  /**< \brief The function pointer of setting sensor mode. */
    int (*pfnSetStream)(ISP_PORT IspPort, vsi_bool_t stream);       /**< \brief The function pointer of setting stream. */
    int (*pfnSetIspDefault)(ISP_PORT IspPort);                      /**< \brief The function pointer of setting ISP default. */
} ISP_SNS_FUNC_S;

/** \brief   Function of AE control sensor. */
typedef struct vsiAE_SNS_FUNC_S {
    int (*pfnGetAeDefault)(ISP_PORT IspPort, AE_SNS_DEFAULT_S *pAeSnsDft); /**< \brief The function pointer of getting sensor default. */
    int (*pfnSetFps)(ISP_PORT IspPort, vsi_u32_t fps);                     /**< \brief The function pointer of setting sensor FPS. */
    int (*pfnSlowFrameRate)(ISP_PORT IspPort, vsi_u32_t fullLines);        /**< \brief Not currently supported */
    int (*pfnIntTimeUpdate)(ISP_PORT IspPort, vsi_u32_t *pIntLine);        /**< \brief The function pointer of updating exposure time. */
    int (*pfnGainUpdate)(ISP_PORT IspPort, vsi_u32_t *pAgain, vsi_u32_t *pDgain);  /**< \brief The function pointer of updating gain. */
    int (*pfnSetExpRatio) (ISP_PORT IspPort, ISP_EXP_RATIO_S *pExpRatio);          /**< \brief Not currently supported */
    int (*pfnGetSnsRegInfo)(ISP_PORT IspPort, ISP_SNS_REGS_INFO_S *pSnsRegsInfo);  /**< \brief The function pointer of getting sensor register information. */
    int (*pfnQueryExpInfo)(ISP_PORT IspPort, vsi_u32_t *pIntLine, vsi_u32_t *pAgain, vsi_u32_t *pDgain);  /**< \brief The function pointer of querying exposure information. */
} AE_SNS_FUNC_S;

/** \brief   ISP sensor function object. */
typedef struct vsiISP_SNS_OBJ_S {
    int (*pfnInitIspSnsFunc) (ISP_SNS_FUNC_S *pIspSnsFunc);  /**< \brief The function pointer of initializing sensor function. */
    int (*pfnInitAeSnsFunc)  (AE_SNS_FUNC_S *pAeSnsFunc);    /**< \brief The function pointer of initializing function of AE control sensor. */
} ISP_SNS_OBJ_S;

/* @} vsi_comm_sns */

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif
