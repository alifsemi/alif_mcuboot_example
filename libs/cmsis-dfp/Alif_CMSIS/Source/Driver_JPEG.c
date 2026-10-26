/* Copyright (C) 2026 Alif Semiconductor - All Rights Reserved.
 * Use, distribution and modification of this code is permitted under the
 * terms stated in the Alif Semiconductor Software License Agreement
 *
 * You should have received a copy of the Alif Semiconductor Software
 * License Agreement with this file. If not, please write to:
 * contact@alifsemi.com, or visit: https://alifsemi.com/license
 *
 */

/*******************************************************************************
 * @file         Driver_JPEG.c
 * @author       Chandra Bhushan Singh
 * @email        chandrabhushan.singh@alifsemi.com
 * @version      V1.0.0
 * @date         14-April-2026
 * @brief        CMSIS-Driver for JPEG Encoder
 * @bug          None.
 * @Note         None.
 ******************************************************************************/

/* System Includes */
#include "RTE_Device.h"
#include "sys_utils.h"

/* Driver JPEG includes */
#include "Driver_JPEG_Private.h"
#include "jpeg_hantro_vc9000e_sw.h"

/* Driver Version */
#define ARM_JPEG_DRV_VERSION ARM_DRIVER_VERSION_MAJOR_MINOR(1, 0)

static const ARM_DRIVER_VERSION DriverVersion = {ARM_JPEG_API_VERSION, ARM_JPEG_DRV_VERSION};

/* Driver Capabilities */
static const ARM_JPEG_CAPABILITIES DriverCapabilities = {
	    .encode      = 1U,
	    .decode      = 0U,
	    .input_nv12  = 1U,

	    .max_width   = 16384U,
	    .max_height  = 16384U,
	    .min_width   = 32U,
	    .min_height  = 32U,
};

/*
 * fn        ARM_DRIVER_VERSION JPEG_GetVersion(void)
 * brief     get JPEG version
 * return    JPEG version
 */
static ARM_DRIVER_VERSION JPEG_GetVersion(void)
{
    return DriverVersion;
}

/*
 * fn        ARM_JPEG_CAPABILITIES JPEG_GetCapabilities(void)
 * brief     get JPEG capabilities
 * return    JPEG driver capabilities
 */
static ARM_JPEG_CAPABILITIES JPEG_GetCapabilities(void)
{
    return DriverCapabilities;
}

/*
 * fn        int32_t JPEGx_Initialize(JPEG_RESOURCES *JPEG_RES, ARM_JPEG_SignalEvent_t cb_event)
 * brief     Initialize JPEG Interface.
 * param[in] JPEG_RES Pointer to JPEG resource.
 * param[in] cb_event pointer to ARM_JPEG_SignalEvent_t.
 * return    @ref execution_status.
 */
static int32_t JPEGx_Initialize(JPEG_RESOURCES *JPEG_RES, ARM_JPEG_SignalEvent_t cb_event)
{
    bool ret;

    if (JPEG_RES->state.initialized == 1) {
        return ARM_DRIVER_OK;
    }

    if (!cb_event) {
        return ARM_DRIVER_ERROR_PARAMETER;
    }

    JPEG_RES->cb_event = cb_event;

    JPEG_RES->state.initialized = 1;

    return ARM_DRIVER_OK;
}

/*
 *  fn        int32_t JPEGx_Uninitialize (JPEG_RESOURCES *JPEG_RES)
 *  brief     Uninitialize JPEG Interface.
 *  param[in] JPEG_RES Pointer to JPEG resource.
 *  return    @ref execution_status.
 */
static int32_t JPEGx_Uninitialize(JPEG_RESOURCES *JPEG_RES)
{
    if (JPEG_RES->state.initialized == 0) {
        return ARM_DRIVER_OK;
    }

    JPEG_RES->cb_event = NULL;

    JPEG_RES->state.initialized = 0;

    return ARM_DRIVER_OK;
}

/*
 * fn        int32_t JPEGx_PowerControl(JPEG_RESOURCES *JPEG_RES, ARM_POWER_STATE state)
 * brief     Control JPEG Interface Power.
 * param[in] JPEG_RES Pointer to JPEG resource.
 * param[in] state    Power state.
 * return    @ref execution_status.
 */
static int32_t JPEGx_PowerControl(JPEG_RESOURCES *JPEG_RES, ARM_POWER_STATE state)
{
    if (JPEG_RES->state.initialized == 0) {
        return ARM_DRIVER_ERROR;
    }

    switch (state) {
    case ARM_POWER_OFF:
        {
            if (JPEG_RES->state.powered == 0) {
                return ARM_DRIVER_OK;
            }

            /* Disabling IRQ */
            NVIC_DisableIRQ(JPEG_IRQ_IRQn);
            NVIC_ClearPendingIRQ(JPEG_IRQ_IRQn);

            JPEG_RES->state.powered = 0;
            break;
        }

    case ARM_POWER_FULL:
        {
            if (JPEG_RES->state.powered == 1) {
                return ARM_DRIVER_OK;
            }

            /* Enabling IRQ */
            NVIC_ClearPendingIRQ(JPEG_IRQ_IRQn);
            NVIC_SetPriority(JPEG_IRQ_IRQn, JPEG_RES->irq_priority);
            NVIC_EnableIRQ(JPEG_IRQ_IRQn);

            if (!jpeg_init(JPEG_RES->regs)) {
                return ARM_DRIVER_ERROR;
            }

            jpeg_set_encoding_mode(JPEG_RES->regs);

            jpeg_set_axi_max_burst_len(
                JPEG_RES->regs,
                JPEG_RES->enc_cfg->max_burst_length);

            jpeg_set_axi_write_outstanding_num(
                JPEG_RES->regs,
                JPEG_RES->enc_cfg->axi_write_outstanding);

            jpeg_set_axi_read_outstanding_num(
                JPEG_RES->regs,
                JPEG_RES->enc_cfg->axi_read_outstanding);

            JPEG_RES->state.powered = 1;
            break;
        }

    case ARM_POWER_LOW:
    default:
        return ARM_DRIVER_ERROR_UNSUPPORTED;
    }

    return ARM_DRIVER_OK;
}

/*
 * fn        int32_t JPEGx_Start(JPEG_RESOURCES *JPEG_RES,
                                 ARM_JPEG_INPUT_BUFFER *in_buf,
                                 ARM_JPEG_BUFFER *out_buf)
 * brief     Start JPEG encoding.
 * param[in] JPEG_RES Pointer to JPEG resource.
 * param[in] in_buf  Pointer to input buffer.
 * param[in] out_buf pointer to output buffer.
 * return    @ref     execution_status.
 */
static int32_t JPEGx_Start(JPEG_RESOURCES *JPEG_RES,
                           ARM_JPEG_INPUT_BUFFER *in_buf,
                           ARM_JPEG_BUFFER *out_buf)
{
    struct jpeg_header_info header_info;
    uint8_t *out_ptr;

    if ((JPEG_RES->state.initialized == 0U) ||
        (JPEG_RES->state.powered == 0U)) {
        return ARM_DRIVER_ERROR;
    }

    if ((in_buf == NULL) || (out_buf == NULL)) {
        return ARM_DRIVER_ERROR_PARAMETER;
    }

    if ((in_buf->num_planes == 0U) ||
        (in_buf->num_planes > JPEG_MAX_PLANES)) {
        return ARM_DRIVER_ERROR_PARAMETER;
    }

    JPEG_RES->active_output = out_buf;

    jpeg_set_input_luma_base_addr(JPEG_RES->regs,
                                  (uint32_t)(uintptr_t)in_buf->plane[0]);
    jpeg_set_input_cb_base_addr(JPEG_RES->regs,
                                (uint32_t)(uintptr_t)in_buf->plane[1]);
    jpeg_set_input_cr_base_addr(JPEG_RES->regs,
                                (uint32_t)(uintptr_t)in_buf->plane[2]);

    jpeg_set_input_row_len_lsb(JPEG_RES->regs, in_buf->plane_pitch[0]);
    jpeg_set_input_row_len_msb(JPEG_RES->regs, in_buf->plane_pitch[0]);
    jpeg_set_input_luma_stride(JPEG_RES->regs, in_buf->plane_pitch[0]);
    jpeg_set_input_chroma_stride(JPEG_RES->regs, in_buf->plane_pitch[1]);

    out_ptr = (uint8_t *)out_buf->buf;

    jpeg_set_output_stream_addr(JPEG_RES->regs,
                                (uint32_t)(uintptr_t)(out_ptr + JPEG_HEADER_SIZE));
    jpeg_set_output_stream_size(JPEG_RES->regs, out_buf->size);

    /* JPEG header generation */
    header_info.buffer = out_buf->buf;
    header_info.width  = JPEG_RES->format.width;
    header_info.height = JPEG_RES->format.height;
    header_info.num_components = in_buf->num_planes;

    jpeg_header_generation(header_info);

    /* clean the header buffer before encoding starts. */
    SCB_CleanDCache_by_Addr(out_buf->buf, JPEG_HEADER_SIZE);

    /* Enable JPEG interrupt */
    jpeg_enable_enc_interrupts(JPEG_RES->regs);

    jpeg_encode_start(JPEG_RES->regs);

    return ARM_DRIVER_OK;
}

/*
 * fn        int32_t JPEGx_Control (JPEG_RESOURCES *JPEG_RES, uint32_t control, uint32_t arg)
 * brief     Control the JPEG.
 * param[in] JPEG_RES Pointer to JPEG resources.
 * param[in] control JPEG control code operation
 * param[in] arg      Argument of operation.
 * return    \ref execution_status.
 */
static int32_t JPEGx_Control(JPEG_RESOURCES *JPEG_RES, uint32_t control, uint32_t arg)
{
    if (JPEG_RES->state.initialized == 0) {
        return ARM_DRIVER_ERROR;
    }

    switch (control) {
    case ARM_JPEG_CTRL_SET_FORMAT:
    {
        ARM_JPEG_FORMAT *fmt =
            (ARM_JPEG_FORMAT *)(uintptr_t)arg;

        if (fmt == NULL) {
            return ARM_DRIVER_ERROR_PARAMETER;
        }

        JPEG_RES->format = *fmt;

        if ((JPEG_RES->format.width < DriverCapabilities.min_width) ||
            (JPEG_RES->format.width > DriverCapabilities.max_width)) {
            return ARM_DRIVER_ERROR_PARAMETER;
        }

        if ((JPEG_RES->format.height < DriverCapabilities.min_height) ||
            (JPEG_RES->format.height > DriverCapabilities.max_height)) {
            return ARM_DRIVER_ERROR_PARAMETER;
        }

        JPEG_RES->img_cfg.mode          = JPEG_RES->enc_cfg->jpeg_mode;
        JPEG_RES->img_cfg.coding_mode   = JPEG_RES->enc_cfg->jpeg_coding_mode;
        JPEG_RES->img_cfg.input_fmt     = fmt->pixel_format;
        JPEG_RES->img_cfg.width         = fmt->width;
        JPEG_RES->img_cfg.height        = fmt->height;

        jpeg_set_format(JPEG_RES->regs, JPEG_RES->img_cfg);

        break;
    }

    case ARM_JPEG_CTRL_SET_QUALITY:
    {
        if ((arg < JPEG_QUALITY_FACTOR_MIN) || (arg > JPEG_QUALITY_FACTOR_MAX)) {
            return ARM_DRIVER_ERROR_PARAMETER;
        }

        JPEG_RES->quality = (uint8_t)arg;
        jpeg_config_quality(JPEG_RES->regs, JPEG_RES->quality);

        break;
    }

    default:
        return ARM_DRIVER_ERROR_UNSUPPORTED;
    }

    return ARM_DRIVER_OK;
}

/*
 * \fn        void JPEG_HandleIRQStatus(JPEG_RESOURCES *JPEG_RES)
 * \brief     CMSIS-Driver JPEG IRQ status handler
 * \param[in] JPEG_RES  Pointer to JPEG resources structure
 * \return    none
 */
static void JPEG_HandleIRQStatus(JPEG_RESOURCES *JPEG_RES)
{
    uint32_t event = 0U;
    jpeg_encode_info_t *enc_info = &JPEG_RES->enc_info;

    if (enc_info->status & JPEG_ENCODE_STATUS_FRAME_RDY) {
        event |= ARM_JPEG_EVENT_FRAME_READY;

        if (JPEG_RES->active_output) {
            JPEG_RES->active_output->size = enc_info->encoded_size;
        }
    }

    if (enc_info->status & JPEG_ENCODE_STATUS_BUFFER_FULL) {
        event |= ARM_JPEG_EVENT_BUFFER_FULL;
    }

    if (enc_info->status & JPEG_ENCODE_STATUS_BUS_ERROR) {
        event |= ARM_JPEG_EVENT_BUS_ERROR;
    }

    if (enc_info->status & JPEG_ENCODE_STATUS_SW_RESET) {
        event |= ARM_JPEG_EVENT_BUS_RESET;
    }

    if (enc_info->status & JPEG_ENCODE_STATUS_TIMEOUT) {
        event |= ARM_JPEG_EVENT_TIMEOUT;
    }

    if (event != 0U) {
        JPEG_RES->cb_event(event);
    }

    enc_info->status = JPEG_ENCODE_STATUS_NONE;
}

/*
 * \fn        void JPEG_ISRHandler(JPEG_RESOURCES *JPEG_RES)
 * \brief     JPEG interrupt handler.
 *                This function will
 *                    - read interrupt status from JPEG
 *                    - clear interrupt status (W1C)
 *                    - map hardware interrupts to driver events
 *                    - call user callback if event occurs
 * \param[in] JPEG_RES   Pointer to JPEG resources structure
 * \return    none
 */
void JPEG_ISRHandler(JPEG_RESOURCES *JPEG_RES)
{
    jpeg_encode_info_t *enc_info = &(JPEG_RES->enc_info);

    jpeg_irq_handler(JPEG_RES->regs, enc_info);

    if (JPEG_RES->enc_info.status) {
        JPEG_HandleIRQStatus(JPEG_RES);
    }
}

#if (RTE_JPEG)

/* JPEG Encoder Configuration */
JPEG_ENC_CONFIG jpeg_enc_cfg = {
    .jpeg_mode              = RTE_JPEG_MODE,
    .jpeg_coding_mode       = RTE_JPEG_CODING_MODE,
    .max_burst_length       = RTE_JPEG_AXI_BURST_LENGTH,
    .axi_write_outstanding  = RTE_AXI_WRITE_OUTSTANDING_NUM,
    .axi_read_outstanding   = RTE_AXI_READ_OUTSTANDING_NUM,
};

/* JPEG Device Resource */
JPEG_RESOURCES JPEG_RES = {
    .regs                   = (JPEG_Type *) JPEG_BASE,
    .cb_event               = NULL,
    .irq_priority           = RTE_JPEG_IRQ_PRIORITY,
    .enc_cfg                = &jpeg_enc_cfg,
    .state                  = {0},
};

/*
 * fn        int32_t JPEG_Initialize (ARM_JPEG_SignalEvent_t cb_event)
 * brief     Initialize JPEG Interface.
 * param[in] cb_event Pointer to ARM_JPEG_SignalEvent_t.
 * return    @ref execution_status.
 */
static int32_t JPEG_Initialize(ARM_JPEG_SignalEvent_t cb_event)
{
    return JPEGx_Initialize(&JPEG_RES, cb_event);
}

/*
 * fn        int32_t JPEG_Uninitialize (void)
 * brief     Uninitialize JPEG Interface.
 * return    @ref execution_status.
 */
static int32_t JPEG_Uninitialize(void)
{
    return JPEGx_Uninitialize(&JPEG_RES);
}

/*
 * fn        int32_t JPEG_PowerControl (ARM_POWER_STATE state)
 * brief     Control JPEG Interface Power.
 * param[in] state Power state.
 * return    @ref execution_status.
 */
static int32_t JPEG_PowerControl(ARM_POWER_STATE state)
{
    return JPEGx_PowerControl(&JPEG_RES, state);
}

/*
 * fn        int32_t JPEG_Start(ARM_JPEG_INPUT_BUFFER *in_buf,
                          ARM_JPEG_BUFFER *out_buf)
 * brief     JPEG Start Encoding.
 * param[in] in_buf  Pointer to input buffer.
 * param[in] out_buf pointer to output buffer.
 * return    @ref execution_status.
 */
static int32_t JPEG_Start(ARM_JPEG_INPUT_BUFFER *in_buf,
                          ARM_JPEG_BUFFER *out_buf)
{
    return JPEGx_Start(&JPEG_RES, in_buf, out_buf);
}

/*
 * fn        int32_t JPEG_Control(uint32_t control, uint32_t arg)
 * brief     Control JPEG.
 * param[in] control JPEG configuration.
 * param[in] arg Argument of operation (optional).
 * return    @ref execution_status.
 */
static int32_t JPEG_Control(uint32_t control, uint32_t arg)
{
    return JPEGx_Control(&JPEG_RES, control, arg);
}

void JPEG_IRQHandler(void)
{
    JPEG_ISRHandler(&JPEG_RES);
}

extern ARM_DRIVER_JPEG Driver_JPEG;
ARM_DRIVER_JPEG Driver_JPEG = {
    JPEG_GetVersion,
    JPEG_GetCapabilities,
    JPEG_Initialize,
    JPEG_Uninitialize,
    JPEG_PowerControl,
    JPEG_Start,
    JPEG_Control,
};
#endif
/************************ (C) COPYRIGHT ALIF SEMICONDUCTOR *****END OF FILE****/
