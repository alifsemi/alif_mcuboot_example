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

#ifndef __MPI_ISP_H__
#define __MPI_ISP_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/**
 * @defgroup mpi_isp Mpp Isp Definitions
 * @{
 *
 *
 */

/*****************************************************************************/
/**
 * @brief   Creates and initializes a CamDevice instance, and initializes the pipeline.
 *
 * @param   IspDev              ISP device ID.
 *
 * @retval  VSI_SUCCESS         Operation succeeded.
 *
 *****************************************************************************/
int VSI_MPI_ISP_Init(ISP_DEV IspDev);

/*****************************************************************************/
/**
 * @brief   Destroys a CamDevice instance.
 *
 * @param   IspDev              ISP device ID.
 *
 * @retval  VSI_SUCCESS         Operation succeeded
 *
 *****************************************************************************/
int VSI_MPI_ISP_Exit(ISP_DEV IspDev);

/*****************************************************************************/
/**
 * @brief   Register sensor call back function.
 *
 * @param   IspPort             Port ID.
 * @param   pSnsObj           Pointer to the sensor configuration.
 * @param   snsDev            Pointer to the sensor device.
 *
 * @retval  VSI_SUCCESS         Operation succeeded
 *
 *****************************************************************************/
int VSI_MPI_ISP_SnsRegCallBack(ISP_PORT IspPort, ISP_SNS_OBJ_S *pSnsObj, vsi_u8_t snsDev);

/*****************************************************************************/
/**
 * @brief   UnRegister sensor call back function.
 *
 * @param   IspPort             Port ID.
 *
 * @retval  VSI_SUCCESS         Operation succeeded
 *
 *****************************************************************************/
int VSI_MPI_ISP_SnsUnRegCallBack(ISP_PORT IspPort);

/*****************************************************************************/
/**
 * @brief   Sets the ISP device work mode.
 *
 * @param   IspDev              ISP device ID.
 * @param   pDevAttr            Pointer to the ISP device configuration.
 *
 * @retval  VSI_SUCCESS         Operation succeeded
 *
 *****************************************************************************/
int VSI_MPI_ISP_SetDevAttr(ISP_DEV IspDev, ISP_DEV_ATTR_S *pDevAttr);

/*****************************************************************************/
/**
 * @brief   Gets the ISP device work mode.
 *
 * @param   IspDev              ISP device ID.
 * @param   pDevAttr            Pointer to the ISP device configuration.
 *
 * @retval  VSI_SUCCESS         Operation succeeded
 *
 *****************************************************************************/
int VSI_MPI_ISP_GetDevAttr(ISP_DEV IspDev, ISP_DEV_ATTR_S *pDevAttr);

/*****************************************************************************/
/**
 * @brief   Enable the ISP device.
 *
 * @param   IspDev              ISP device ID.
 *
 * @retval  VSI_SUCCESS         Operation succeeded.
 *
 *****************************************************************************/
int VSI_MPI_ISP_EnableDev(ISP_DEV IspDev);

/*****************************************************************************/
/**
 * @brief   Disable the ISP device.
 *
 * @param   IspDev              ISP device ID.
 *
 * @retval  VSI_SUCCESS         Operation succeeded.
 *
 *****************************************************************************/
int VSI_MPI_ISP_DisableDev(ISP_DEV IspDev);

/*****************************************************************************/
/**
 * @brief   Sets the ISP device port.
 *
 * @param   IspPort             Port ID.
 * @param   pPortAttr         Pointer to the port configuration.
 *
 * @retval  VSI_SUCCESS         Operation succeeded
 *
 *****************************************************************************/
int VSI_MPI_ISP_SetPortAttr(ISP_PORT IspPort, ISP_PORT_ATTR_S *pPortAttr);

/*****************************************************************************/
/**
 * @brief   Gets the ISP device port.
 *
 * @param   IspPort             Port ID.
 * @param   pPortAttr         Pointer to the port configuration.
 *
 * @retval  VSI_SUCCESS         Operation succeeded
 *
 *****************************************************************************/
int VSI_MPI_ISP_GetPortAttr(ISP_PORT IspPort, ISP_PORT_ATTR_S *pPortAttr);

/*****************************************************************************/
/**
 * @brief   Enable the ISP device port.
 *
 * @param   IspPort             Port ID.
 *
 * @retval  VSI_SUCCESS         Operation succeeded
 *
 *****************************************************************************/
int VSI_MPI_ISP_EnablePort(ISP_PORT IspPort);

/*****************************************************************************/
/**
 * @brief   Disable the ISP device port.
 *
 * @param   IspPort             Port ID.
 *
 * @retval  VSI_SUCCESS         Operation succeeded
 *
 *****************************************************************************/
int VSI_MPI_ISP_DisablePort(ISP_PORT IspPort);

/*****************************************************************************/
/**
 * @brief   Writes the data to register.
 *
 * @param   IspPort             Port ID.
 * @param   reg              Data address
 * @param   val              Data value
 *
 * @retval  VSI_SUCCESS         Operation succeeded
 *
 *****************************************************************************/
int VSI_MPI_ISP_WriteReg(ISP_PORT IspPort, vsi_u32_t reg, vsi_u32_t val);

/*****************************************************************************/
/**
 * @brief   Reads data from register.
 *
 * @param   IspPort             Port ID.
 * @param   reg              Data address
 * @param   pVal             A pointer to the data value
 *
 * @retval  VSI_SUCCESS         Operation succeeded
 *
 *****************************************************************************/
int VSI_MPI_ISP_ReadReg(ISP_PORT IspPort, vsi_u32_t reg, vsi_u32_t *pVal);

/*****************************************************************************/
/**
 * @brief   Sets the output channel.
 *
 * @param   IspChn              Output channel ID.
 * @param   pChnAttr          Pointer to the channel configuration.
 *
 * @retval  VSI_SUCCESS         Operation succeeded
 *
 *****************************************************************************/
int VSI_MPI_ISP_SetChnAttr(ISP_CHN IspChn, ISP_CHN_ATTR_S *pChnAttr);

/*****************************************************************************/
/**
 * @brief   Gets the output channel.
 *
 * @param   IspChn              Output channel ID.
 * @param   pChnAttr          Pointer to the channel configuration.
 *
 * @retval  VSI_SUCCESS         Operation succeeded
 *
 *****************************************************************************/
int VSI_MPI_ISP_GetChnAttr(ISP_CHN IspChn, ISP_CHN_ATTR_S *pChnAttr);

/*****************************************************************************/
/**
 * @brief   Enable the output channel.
 *
 * @param   IspChn              Output channel ID.
 *
 * @retval  VSI_SUCCESS         Operation succeeded
 *
 *****************************************************************************/
int VSI_MPI_ISP_EnableChn(ISP_CHN IspChn);

/*****************************************************************************/
/**
 * @brief   Disable the output channel.
 *
 * @param   IspChn              Output channel ID.
 *
 * @retval  VSI_SUCCESS         Operation succeeded
 *
 *****************************************************************************/
int VSI_MPI_ISP_DisableChn(ISP_CHN IspChn);

/*****************************************************************************/
/**
 * @brief   Sets the ISP auto route.
 *
 * @param   IspPort             Port ID.
 * @param   pAutoRoute          Pointer to the auto route.
 *
 * @retval  VSI_SUCCESS         Operation succeeded
 *
 *****************************************************************************/
int VSI_MPI_ISP_SetAutoRoute(ISP_PORT IspPort, ISP_AUTO_ROUTE_S *pAutoRoute);

/*****************************************************************************/
/**
 * @brief   Gets the ISP auto route.
 *
 * @param   IspPort             Port ID.
 * @param   pAutoRoute          Pointer to the auto route.
 *
 * @retval  VSI_SUCCESS         Operation succeeded
 *
 *****************************************************************************/
int VSI_MPI_ISP_GetAutoRoute(ISP_PORT IspPort, ISP_AUTO_ROUTE_S *pAutoRoute);

/*****************************************************************************/
/**
 * @brief   Adds a buffer to an empty buffer queue.
 *
 * @param   IspChn              Output channel ID
 * @param   pBuf              Video buffer structure
 *
 * @retval  VSI_SUCCESS         Operation succeeded
 *
 *****************************************************************************/
int VSI_MPI_ISP_QBUF(ISP_CHN IspChn, VIDEO_BUF_S *pBuf);

/*****************************************************************************/
/**
 * @brief   Dequeues a buffer from a full buffer queue.
 *
 * @param   IspChn              Output channel ID
 * @param   pBuf              Video buffer structure
 * @param   timeMs           Dequeue buffer time
 *
 * @retval  VSI_SUCCESS         Operation succeeded
 *
 *****************************************************************************/
int VSI_MPI_ISP_DQBUF(ISP_CHN IspChn, VIDEO_BUF_S *pBuf, vsi_u32_t timeMs);

/* @} mpi_isp */

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif
