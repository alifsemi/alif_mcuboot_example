/* Copyright (C) 2025 Alif Semiconductor - All Rights Reserved.
 * Use, distribution and modification of this code is permitted under the
 * terms stated in the Alif Semiconductor Software License Agreement
 *
 * You should have received a copy of the Alif Semiconductor Software
 * License Agreement with this file. If not, please write to:
 * contact@alifsemi.com, or visit: https://alifsemi.com/license
 *
 */

/*******************************************************************************
 * @file         Driver_ISP.c
 * @author       Yogender Kumar Arya
 * @email        yogender.kumar@alifsemi.com
 * @version      V1.0.0
 * @date         06-September-2025
 * @brief        CMSIS-Driver for Image Signal Processor
 * @bug          None.
 * @Note         None.
 ******************************************************************************/

/* System Includes */
#include "RTE_Device.h"
#include "sys_utils.h"
#include "soc.h"
#include "Driver_ISP_Private.h"

#if defined(RTE_Drivers_ISP)

/* Project Includes */
#include "Camera_Sensor.h"

/* ISP Includes */
#include "vsi_comm_isp.h"
#include "vsios_type.h"
#include "isp.h"

#include "mpi_isp_calib.h"
#include "vsi_comm_awb.h"

/* CMSIS ISP driver Includes */
#include "Driver_ISP.h"

extern void VSI_ISP_IrqProcessFrameEnd(ISP_PORT IspPort);

#if (RTE_ISP_WB_MODULE)
extern ISP_AWB_FUNC_S vsiAwbAlgo;
#endif /* RTE_ISP_WB_MODULE */

#define ARM_ISP_DRV_VERSION ARM_DRIVER_VERSION_MAJOR_MINOR(0, 1) /* driver version */

/* Driver Version */
static const ARM_DRIVER_VERSION DriverVersion = { ARM_ISP_API_VERSION, ARM_ISP_DRV_VERSION };

/* Driver Capabilities */
static const ARM_ISP_CAPABILITIES DriverCapabilities = {
    0, /* AE is not supported,
        * In this mode, ISP can do Auto-Exposure control.
        */
    0, /* BLS */
    0, /* Demosaic */
    0, /* Filter */
    0, /* CCM */
    0, /* CSM */
    0, /* White Balancing */
    0, /* ae_stat - Auto-Exposure Statistics */
    0, /* Gamma-out */
    0, /* wb_stat - White-Balancing Statistics */
    0, /* Binning */
    0  /* Reserved (must be zero) */
};

/*
 * fn        ARM_DRIVER_VERSION ISP_GetVersion(void)
 * brief     get ISP version
 * return    ISP version
 */
static ARM_DRIVER_VERSION ISP_GetVersion(void)
{
    return DriverVersion;
}

/*
 * fn        ARM_ISP_CAPABILITIES ISP_GetCapabilities(void)
 * brief     get ISP capabilities
 * return    ISP driver capabilities
 */
static ARM_ISP_CAPABILITIES ISP_GetCapabilities(void)
{
    return DriverCapabilities;
}

/*
 * fn        int32_t ISP_Init (ARM_ISP_SignalEvent_t cb_event,
 *                              ISP_RESOURCES *isp)
 * brief     Initialize ISP Interface.
 * param[in] cb_event pointer to ARM_ISP_SignalEvent_t.
 * param[in] isp Pointer to ISP resource.
 * return    @ref execution_status.
 */
static int32_t ISP_Init(ARM_ISP_SignalEvent_t cb_event, CAMERA_SENSOR_DEVICE *cam_sensor,
                        ISP_RESOURCES *isp)
{
    int32_t         ret;
    ISP_DEV_ATTR_S  devAttr;
    ISP_PORT_ATTR_S isp_port_config = {};

    if (isp->state.initialized == 1) {
        return ARM_DRIVER_OK;
    }

    if (!cb_event) {
        return ARM_DRIVER_ERROR_PARAMETER;
    }

    isp->cb_event = cb_event;
    isp->state.streaming = 0;

    /* Init ISP system. */
    ret           = VSI_MPI_ISP_Init(isp->isp_dev_id);
    if (ret) {
        return ARM_DRIVER_ERROR;
    }

    /* Configure ISP work-mode. */
    devAttr.ispWorkMode = WORK_MODE_NORMAL;
    ret                 = VSI_MPI_ISP_SetDevAttr(isp->isp_dev_id, &devAttr);
    if (ret) {
        return ARM_DRIVER_ERROR;
    }

    // Technically it should be set when we are setting AWB.
    // The algorithm must be first registered and then Init.
#if (RTE_ISP_WB_MODULE)
    if (isp->isp_calib_info->modules.wb.opType == OP_TYPE_AUTO) {
        ret = VSI_MPI_ISP_AwbRegCallBack(isp->isp_port_id, &vsiAwbAlgo);
        if (ret) {
            return ARM_DRIVER_ERROR;
        }
    }
#endif /* RTE_ISP_WB_MODULE */

    ret = VSI_MPI_ISP_SetCalib(isp->isp_port_id, isp->isp_calib_info);
    if (ret) {
        return ARM_DRIVER_ERROR;
    }

    ret = VSI_MPI_ISP_GetPortAttr(isp->isp_port_id, &isp_port_config);
    if (ret) {
        return ARM_DRIVER_ERROR;
    }

    // Update the sensor configuration here:
    isp->isp_port_attr->snsRect.width  = cam_sensor->width;
    isp->isp_port_attr->snsRect.height = cam_sensor->height;

    isp_port_config.ispInputType       = isp->isp_port_attr->ispInputType;
    isp_port_config.ispMode            = isp->isp_port_attr->ispMode;
    isp_port_config.hdrMode            = isp->isp_port_attr->hdrMode;
    isp_port_config.stichMode          = isp->isp_port_attr->stichMode;
    isp_port_config.pixelFormat        = isp->isp_port_attr->pixelFormat;

    isp_port_config.snsRect.top        = isp->isp_port_attr->snsRect.top;
    isp_port_config.snsRect.left       = isp->isp_port_attr->snsRect.left;
    isp_port_config.snsRect.width      = isp->isp_port_attr->snsRect.width;
    isp_port_config.snsRect.height     = isp->isp_port_attr->snsRect.height;

    isp_port_config.inFormRect.top     = isp->isp_port_attr->inFormRect.top;
    isp_port_config.inFormRect.left    = isp->isp_port_attr->inFormRect.left;
    isp_port_config.inFormRect.width   = isp->isp_port_attr->inFormRect.width;
    isp_port_config.inFormRect.height  = isp->isp_port_attr->inFormRect.height;

    isp_port_config.outFormRect.top    = isp->isp_port_attr->outFormRect.top;
    isp_port_config.outFormRect.left   = isp->isp_port_attr->outFormRect.left;
    isp_port_config.outFormRect.width  = isp->isp_port_attr->outFormRect.width;
    isp_port_config.outFormRect.height = isp->isp_port_attr->outFormRect.height;

    isp_port_config.iSRect.top         = isp->isp_port_attr->iSRect.top;
    isp_port_config.iSRect.left        = isp->isp_port_attr->iSRect.left;
    isp_port_config.iSRect.width       = isp->isp_port_attr->iSRect.width;
    isp_port_config.iSRect.height      = isp->isp_port_attr->iSRect.height;

    isp_port_config.snsFps             = 0;

    ret = VSI_MPI_ISP_SetPortAttr(isp->isp_port_id, &isp_port_config);
    if (ret) {
        return ARM_DRIVER_ERROR;
    }

    if (!isp->isp_chan_attr->chnFormat.width) {
        isp->isp_chan_attr->chnFormat.width = cam_sensor->width;
    }
    if (!isp->isp_chan_attr->chnFormat.height) {
        isp->isp_chan_attr->chnFormat.height = cam_sensor->height;
    }

    // Set-up ISP channel attributes which control the way ISP output data to memory.
    ret = VSI_MPI_ISP_SetChnAttr(isp->isp_chn_id, isp->isp_chan_attr);
    if (ret) {
        return ARM_DRIVER_ERROR;
    }

    isp->state.initialized = 1;

    return ARM_DRIVER_OK;
}

/*
 *  fn        int32_t ISP_Uninit (ISP_RESOURCES *isp)
 *  brief     Uninitialize ISP Interface.
 *  param[in] isp Pointer to ISP resource.
 *  return    @ref execution_status.
 */
static int32_t ISP_Uninit(ISP_RESOURCES *isp)
{
    int32_t ret;

    if (isp->state.initialized == 0) {
        return ARM_DRIVER_OK;
    }

#if (RTE_ISP_WB_MODULE)
    ret = VSI_MPI_ISP_AwbUnRegCallBack(isp->isp_port_id);
    if (ret) {
        return ARM_DRIVER_ERROR;
    }
#endif /* RTE_ISP_WB_MODULE */

    ret = VSI_MPI_ISP_Exit(isp->isp_dev_id);
    if (ret) {
        return ARM_DRIVER_ERROR;
    }

    isp->cb_event = NULL;

    return ARM_DRIVER_OK;
}

/*
 * fn        int32_t ISP_PowerCtrl (ARM_POWER_STATE state,
 *                                   ISP_RESOURCES *isp)
 * brief     Control ISP Interface Power.
 * param[in] state Power state.
 * return    @ref execution_status.
 */
static int32_t ISP_PowerCtrl(ARM_POWER_STATE state, ISP_RESOURCES *isp)
{
    if (isp->state.initialized == 0) {
        return ARM_DRIVER_ERROR;
    }

    switch (state) {
    case ARM_POWER_OFF:
        if (isp->state.powered == 0) {
            return ARM_DRIVER_OK;
        }

        /* Disabling IRQ */
        NVIC_DisableIRQ(ISP_IRQ_IRQn);
        NVIC_DisableIRQ(ISP_MI_IRQ_IRQn);
        NVIC_ClearPendingIRQ(ISP_IRQ_IRQn);
        NVIC_ClearPendingIRQ(ISP_MI_IRQ_IRQn);

        break;

    case ARM_POWER_FULL:
        if (isp->state.powered == 1) {
            return ARM_DRIVER_OK;
        }

        /* Enabling IRQ */
        NVIC_ClearPendingIRQ(ISP_IRQ_IRQn);
        NVIC_ClearPendingIRQ(ISP_MI_IRQ_IRQn);
        NVIC_SetPriority(ISP_IRQ_IRQn, isp->irq_priority);
        NVIC_SetPriority(ISP_MI_IRQ_IRQn, isp->irq_priority);
        NVIC_EnableIRQ(ISP_IRQ_IRQn);
        NVIC_EnableIRQ(ISP_MI_IRQ_IRQn);

        isp->state.powered = 1;
        break;

    case ARM_POWER_LOW:
    default:
        return ARM_DRIVER_ERROR_UNSUPPORTED;
    }

    return ARM_DRIVER_OK;
}

/*
 * fn        int32_t ISP_start(ISP_RESOURCES *isp)
 * brief     Start ISP Image capture.
 * return    @ref execution_status.
 */
static int32_t ISP_start(ISP_RESOURCES *isp)
{
    int32_t ret = 0;

    if (isp->state.streaming == 1)  {
        return ARM_DRIVER_OK;
    }

    // Library call to start capture using ISP library

    ret = VSI_MPI_ISP_EnableDev(isp->isp_dev_id);
    if (ret) {
        return ARM_DRIVER_ERROR;
    }

    ret = VSI_MPI_ISP_EnablePort(isp->isp_port_id);
    if (ret) {
        return ARM_DRIVER_ERROR;
    }

    ret = VSI_MPI_ISP_EnableChn(isp->isp_chn_id);
    if (ret) {
        return ARM_DRIVER_ERROR;
    }

    isp->state.streaming = 1;
    return ARM_DRIVER_OK;
}

/*
 * fn        int32_t ISP_stop(ISP_RESOURCES *isp)
 * brief     Stop ISP Image capture.
 * return    @ref execution_status.
 */
static int32_t ISP_stop(ISP_RESOURCES *isp)
{
    int32_t ret = 0;

    if (isp->state.streaming == 0) {
        return ARM_DRIVER_OK;
    }

    // Library call to stop capture using ISP library
    ret = VSI_MPI_ISP_DisableChn(isp->isp_chn_id);
    if (ret) {
        return ARM_DRIVER_ERROR;
    }

    ret = VSI_MPI_ISP_DisablePort(isp->isp_port_id);
    if (ret) {
        return ARM_DRIVER_ERROR;
    }

    ret = VSI_MPI_ISP_DisableDev(isp->isp_dev_id);
    if (ret) {
        return ARM_DRIVER_ERROR;
    }

    isp->state.streaming = 0;

    return ARM_DRIVER_OK;
}

/*
 * fn        int32_t ISP_control (uint32_t control,
 *                                 uint32_t arg,
 *                                 ISP_RESOURCES *isp)
 * brief     Control the ISP.
 * param[in] control ISP control code operation
 * param[in] arg Argument of operation.
 * param[in] isp Pointer to ISP resources.
 * return    \ref execution_status.
 */
static int32_t ISP_control(uint32_t control, uint32_t arg, ISP_RESOURCES *isp)
{
    int32_t ret = 0;

    if (isp->state.initialized == 0) {
        return ARM_DRIVER_ERROR;
    }

    switch (control) {
    case ISP_CONTROL_QBUF:
        ret = VSI_MPI_ISP_QBUF(isp->isp_chn_id, (VIDEO_BUF_S *) arg);
        break;
    case ISP_CONTROL_DQBUF:
        ret = VSI_MPI_ISP_DQBUF(isp->isp_chn_id, (VIDEO_BUF_S *) arg, 0);
        break;
    case ISP_PROCESS_FRAME_END:
        VSI_ISP_IrqProcessFrameEnd(isp->isp_port_id);
        break;
    default:
        return ARM_DRIVER_ERROR_UNSUPPORTED;
    }
    return (ret == VSI_SUCCESS) ? ARM_DRIVER_OK : ARM_DRIVER_ERROR;
}

#if (RTE_ISP)
/* ISP sensor access structure */
static CAMERA_SENSOR_DEVICE *sensor;

ISP_CALIB_DATA_S calibration_data = {
    .modules = {
#if (RTE_ISP_WBM_MODULE)
        .wbm = {
            .enable   = 1,
            .measMode = ISP_AWB_MEAS_MODE_RGB,
            .measRect = {
                .hOffs = 0,
                .vOffs = 0,
                .hSize = 1920,
                .vSize = 1080,
            },
            // RGB mode of measurement
            .wpRange = {
                .maxY       = 0xEB,
                .refCr_MaxR = 0x80,
                .minY_MaxG  = 0xC0,
                .refCb_MaxB = 0x80,
                .maxCSum    = 0x14,
                .minC       = 0x14,
            },
        },
#endif /* RTE_ISP_WBM_MODULE */

#if (RTE_ISP_WB_MODULE)
        .wb = {
            .enable = 1,
            .opType = OP_TYPE_AUTO,
            //.opType = OP_TYPE_MANUAL,
            .manualAttr = {
                .wbGain = {0x100, 0x100, 0x100, 0x100},
            },
            .autoAttr = {
                .runInterval   = 1,
                .speed         = 64,
                .tolerance     = 1,
                .initColorTemp = 4500,
                .calibParam    = {
                    .centLine = {-756822, -540795, -2485600},
                    .rgMin    = 995000,
                    .rgMax    = 2434700,
                    .wpRange0 = {
                        .wpLCurve = {
                            .rg   = {
                                800000,   918317, 1036630, 1154950, 1273270, 1391580, 1509900,
                                1628220, 1746530, 1864850, 1983170, 2101480, 2219800, 2291520,
                                2391170, 2475300
                            },
                            .dist = {
                                4346,    54815, 101574, 145633, 185416, 220875, 250999, 274321,
                                290001, 295548, 288132, 263269, 215941, 174965, 100294, 13567
                            },
                        },
                        .wpRCurve = {
                            .rg   = {
                                800000,  918317,  1036630, 1154950, 1273270, 1391580, 1516450,
                                1565950, 1690140, 1847260, 1983170, 2101480, 2204780, 2296030,
                                2373560, 2475800
                            },
                            .dist = {
                                355654, 305185, 258426, 214367, 174584, 139125, 170938, 317311,
                                332925, 169218, 71868,  96731,  135492, 190735, 258194, 366710
                            },
                        },
                    },
                    .wpRange1 = {
                        .wpLCurve = {
                            .rg   = {
                                995000,  1128750, 1250800, 1349560, 1448330, 1547100, 1645860,
                                1744630, 1843390, 1942160, 2040920, 2140900, 2216330, 2305560,
                                2381430, 2434900
                            },
                            .dist = {
                                -39503,  14716,  58032,  88970, 115812, 139005, 157346, 169761,
                                175397, 172450, 158581, 139910, 109243,  58664,   7042, -56770
                            },
                        },
                        .wpRCurve = {
                            .rg   = {
                                995000,  1115620, 1250800, 1349560, 1448330, 1552250, 1590720,
                                1670660, 1783930, 1884870, 2040920, 2139690, 2223960, 2296240,
                                2354920, 2434900
                            },
                            .dist = {
                                154354, 106619,  61968, 31031,  4188,  62094, 238853, 263946,
                                166529, 48583,  -38581, -10696, 28146, 79446, 127288, 208410
                            },
                        },
                    },
                    .illuminant[ILLUMINANT_A] = {
                        .illuType  = ILLUMINANT_A,
                        .colorTemp = 2856,
                        .wbGain    = {
                            0x14c, 0x100, 0x100, 0x2b8
                        },
                    },
                    .illuminant[ILLUMINANT_TL84] = {
                        .illuType  = ILLUMINANT_TL84,
                        .colorTemp = 4000,
                        .wbGain    = {
                            0x18c, 0x100, 0x100, 0x238
                        },
                    },
                    .illuminant[ILLUMINANT_CWF] = {
                        .illuType  = ILLUMINANT_CWF,
                        .colorTemp = 4100,
                        .wbGain    = {
                            0x1df, 0x100, 0x100, 0x245
                        },
                    },
                    .illuminant[ILLUMINANT_D50] = {
                        .illuType  = ILLUMINANT_D50,
                        .colorTemp = 5000,
                        .wbGain    = {
                            0x1E0, 0x100, 0x100, 0x1D0
                        },
                    },
                    .illuminant[ILLUMINANT_D65] = {
                        .illuType  = ILLUMINANT_D65,
                        .colorTemp = 6500,
                        .wbGain    = {
                            0x200, 0x100, 0x100, 0x1A0
                        },
                    },
                },
            },
        },
#endif /* RTE_ISP_WB_MODULE */

#if (RTE_ISP_CCM_MODULE)
        .ccm = {
            .opType = OP_TYPE_AUTO,
            .manualAttr = {
                .colorMatrix = {
                    0x80, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x80
                },
                .rOffset = 0,
                .gOffset = 0,
                .bOffset = 0,
            },
            .autoAttr = {
                .illuminantCCM[ILLUMINANT_A] = {
                    .colorTemp = 2856,
                    .colorMatrix = {
                        192, -6, -57, -70, 230, -24, 11, -149, 276
                    },
                    .rOffset = 0,
                    .gOffset = 0,
                    .bOffset = 0,
                },
                .illuminantCCM[ILLUMINANT_TL84] = {
                    .colorTemp = 4000,
                    .colorMatrix = {
                        222, -60, -32, -64, 236, -35, 1, -78, 222
                    },
                    .rOffset = 0,
                    .gOffset = 0,
                    .bOffset = 0,
                },
                .illuminantCCM[ILLUMINANT_CWF] = {
                    .colorTemp = 4100,
                    .colorMatrix = {
                        257, -90, -37, -65, 219, -18, 2, -82, 223
                    },
                    .rOffset = 0,
                    .gOffset = 0,
                    .bOffset = 0,
                },
                .illuminantCCM[ILLUMINANT_D50] = {
                    .colorTemp = 5000,
                    .colorMatrix = {
                        207, -55, -23, -48, 224, -34, 6, -89, 222
                    },
                    .rOffset = 0,
                    .gOffset = 0,
                    .bOffset = 0,
                },
                .illuminantCCM[ILLUMINANT_D65] = {
                    .colorTemp = 6500,
                    .colorMatrix = {
                        229, -82, -17, -39, 215, -47, 0, -68, 207
                    },
                    .rOffset = 0,
                    .gOffset = 0,
                    .bOffset = 0,
                },
            },
        },
#endif /* RTE_ISP_CCM_MODULE */
#if (RTE_ISP_DMSC_MODULE)
        .dmsc = {
            .enable    = 1,
            .threshold = 10,
            .cacAttr   = {
                .enable      = 0,
                .hClipMode   = 0,
                .vClipMode   = 0,
                .hStart      = 0,
                .vStart      = 0,
                .aBlue       = 0,
                .aRed        = 0,
                .bBlue       = 0,
                .bRed        = 0,
                .cBlue       = 0,
                .cRed        = 0,
                .xNormShift  = 7,
                .xNormFactor = 29,
                .yNormShift  = 7,
                .yNormFactor = 29,
            },
        },
#endif /* RTE_ISP_DMSC_MODULE */
    },
};

// Configuration of what is coming through the pipeline to the ISP IP.
ISP_PORT_ATTR_S port_attr = {
    .ispInputType = INPUT_TYPE_SENSOR,
    .ispMode      = ISP_MODE_RAW,
    .hdrMode      = HDR_MODE_LINEAR,
    .pixelFormat  = PIXEL_FORMAT_GRBG8,
    .snsRect = {
        .top    = 0,
        .left   = 0,
        .width  = 560,
        .height = 560,
    },
    .inFormRect = {
        .top    = 0,
        .left   = 0,
        .width  = 560,
        .height = 560,
    },
    .iSRect = {
        .top    = 0,
        .left   = 0,
        .width  = 560,
        .height = 560,
    },
    .outFormRect = {
        .top    = 0,
        .left   = 0,
        .width  = 560,
        .height = 560,
    },
};

ISP_CHN_ATTR_S chan_attr = {
    .transBus = TRANS_BUS_ONLINE,
    .chnFormat = {
        .width       = RTE_ISP_OUTPUT_WIDTH,
        .height      = RTE_ISP_OUTPUT_HEIGHT,
        .pixelFormat = RTE_ISP_OUTPUT_FORMAT,
    },
};

ISP_RESOURCES ISP_RES = {
    .isp_calib_info = &calibration_data,
    .isp_port_attr  = &port_attr,
    .isp_chan_attr  = &chan_attr,
    .cb_event       = NULL,
    .irq_priority   = RTE_ISP_IRQ_PRIORITY,
    .isp_dev_id     = 0,
    .isp_port_id    = {
        0, /* Device ID */
        0, /* Port ID */
    },
    .isp_chn_id     = {
        0, /* Device ID */
        0, /* Port ID */
        0, /* Channel ID */
    },
    .state = {0},
};

/*
 * fn        int32_t ISP_Initialize (ARM_ISP_SignalEvent_t cb_event)
 * brief     Initialize ISP Interface.
 * param[in] cb_event Pointer to ARM_ISP_SignalEvent_t.
 * return    @ref execution_status.
 */
static int32_t ISP_Initialize(ARM_ISP_SignalEvent_t cb_event)
{
    sensor = Get_Camera_Sensor();
    return ISP_Init(cb_event, sensor, &ISP_RES);
}

/*
 * fn        int32_t ISP_Uninitialize (void)
 * brief     Uninitialize ISP Interface.
 * return    @ref execution_status.
 */
static int32_t ISP_Uninitialize(void)
{
    return ISP_Uninit(&ISP_RES);
}

/*
 * fn        int32_t ISP_PowerControl (ARM_POWER_STATE state)
 * brief     Control ISP Interface Power.
 * param[in] state Power state.
 * return    @ref execution_status.
 */
static int32_t ISP_PowerControl(ARM_POWER_STATE state)
{
    return ISP_PowerCtrl(state, &ISP_RES);
}

/*
 * fn        int32_t ISP_Start()
 * brief     ISP Start Image Capture.
 * return    @ref execution_status.
 */
static int32_t ISP_Start(void)
{
    return ISP_start(&ISP_RES);
}

/*
 * fn        int32_t ISP_StopCapture()
 * brief     ISP Stop Image Capture.
 * return    @ref execution_status.
 */
static int32_t ISP_Stop(void)
{
    return ISP_stop(&ISP_RES);
}

/*
 * fn        int32_t ISP_Control(uint32_t control, uint32_t arg)
 * brief     Control ISP.
 * param[in] control ISP configuration.
 * param[in] arg Argument of operation (optional).
 * return    @ref execution_status.
 */
static int32_t ISP_Control(uint32_t control, uint32_t arg)
{
    return ISP_control(control, arg, &ISP_RES);
}

void ISP_ISRHandler(ISP_RESOURCES *isp)
{
    uint32_t reg;
    uint32_t event = 0;

    reg            = isp_get_interrupt_status(isp->isp_port_id);

    if (reg) {
        isp_irq_handler_clear_intr_status(isp->isp_port_id, reg);
    }

    if (reg & ISP_INTR_DATALOSS) {
        event |= ARM_ISP_EVENT_DATALOSS_DETECTED;
    }

    if (reg & ISP_INTR_SIZE_ERR) {
        event |= ARM_ISP_EVENT_SIZE_ERR_DETECTED;
    }

    if (reg & ISP_INTR_AWB_DONE) {
        event |= ARM_ISP_EVENT_AWB_DONE;
    }

    if (reg & ISP_INTR_FRAME_IN) {
        event |= ARM_ISP_EVENT_FRAME_IN_DETECTED;
    }

    if (reg & ISP_INTR_VSYNC) {
        event |= ARM_ISP_EVENT_FRAME_VSYNC_DETECTED;
    }

    if (reg & ISP_INTR_HSYNC) {
        event |= ARM_ISP_EVENT_FRAME_HSYNC_DETECTED;
    }

    if (reg & ISP_INTR_EXPM_COMPLETE) {
        event |= ARM_ISP_EVENT_FRAME_IN_DETECTED;
    }

    if (event && isp->cb_event) {
        isp->cb_event(event);
    }
}

void ISP_MI_ISRHandler(ISP_RESOURCES *isp)
{
    uint32_t event = 0;
    uint32_t reg;

    reg = isp_mi_get_interrupt_status(isp->isp_port_id);

    if (reg) {
        isp_mi_irq_handler_clear_intr_status(isp->isp_port_id, reg);
    }

    if (reg & ISP_MI_INTR_MP_FRAME_END) {
        event |= ARM_ISP_MI_EVENT_MP_FRAME_END_DETECTED;
    }

    if (reg & ISP_MI_INTR_MBLK_LINE) {
        event |= ARM_ISP_MI_EVENT_MBLK_LINE_DETECTED;
    }

    if (reg & ISP_MI_INTR_FILL_MP_Y) {
        event |= ARM_ISP_MI_EVENT_FILL_MP_Y_DETECTED;
    }

    if (reg & ISP_MI_INTR_WRAP_MP_Y) {
        event |= ARM_ISP_MI_EVENT_MP_Y_WRAP_DETECTED;
    }

    if (reg & ISP_MI_INTR_WRAP_MP_CB) {
        event |= ARM_ISP_MI_EVENT_MP_CB_WRAP_DETECTED;
    }

    if (reg & ISP_MI_INTR_WRAP_MP_CR) {
        event |= ARM_ISP_MI_EVENT_MP_CR_WRAP_DETECTED;
    }

    if (event && isp->cb_event) {
        isp->cb_event(event);
    }
}

void ISP_IRQHandler(void)
{
    ISP_ISRHandler(&ISP_RES);
}

void ISP_MI_IRQHandler(void)
{
    ISP_MI_ISRHandler(&ISP_RES);
}

extern ARM_DRIVER_ISP Driver_ISP;
ARM_DRIVER_ISP        Driver_ISP = {
    ISP_GetVersion,
    ISP_GetCapabilities,
    ISP_Initialize,
    ISP_Uninitialize,
    ISP_PowerControl,
    ISP_Start,
    ISP_Stop,
    ISP_Control,
};
#endif
/************************ (C) COPYRIGHT ALIF SEMICONDUCTOR *****END OF FILE****/
#endif /* RTE_Drivers_ISP */
