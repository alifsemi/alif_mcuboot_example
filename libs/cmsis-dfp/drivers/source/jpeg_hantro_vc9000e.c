/* Copyright (C) 2026 Alif Semiconductor - All Rights Reserved.
 * @Use, distribution and modification of this code is permitted under the
 * @terms stated in the Alif Semiconductor Software License Agreement
 *
 * @You should have received a copy of the Alif Semiconductor Software
 * @License Agreement with this file. If not, please write to:
 * @contact@alifsemi.com, or visit: https://alifsemi.com/license
 *
 */

/*******************************************************************************
 * @@file     jpeg_hantro_vc9000e.c
 * @@author   Chandra Bhushan Singh
 * @@email    chandrabhushan.singh@alifsemi.com
 * @@version  V1.0.0
 * @@date     14-April-2026
 * @@brief    Low level driver Specific source file.
 ******************************************************************************/

#include "jpeg_hantro_vc9000e_sw.h"
#include "jpeg_hantro_vc9000e.h"

/**
 * \fn           bool jpeg_init(JPEG_Type *jpeg)
 * \brief        Validates the JPEG hardware ID and hardware version
 *               registers to ensure that the expected JPEG IP is
 *               present.
 * \param[in]    jpeg  Pointer to the JPEG register map.
 * \return       true  JPEG hardware ID and version match the expected values.
 *               false JPEG hardware ID or version does not match.
 */
bool jpeg_init(JPEG_Type *jpeg)
{
	if (jpeg->JPEG_SWREG0 != JPEG_HW_ID) {
		return false;
	}

	if (jpeg->JPEG_SWREG80 != JPEG_HW_VERSION) {
		return false;
	}
	return true;
}

/**
 * \fn           jpeg_set_format(JPEG_Type *jpeg, jpeg_image_info_t img_info)
 * \brief        Configure JPEG format registers.
 * \param[in]    jpeg       Pointer to the JPEG register map.
 * \param[in]    img_info   Pointer to JPEG image configuration.
 * \return       none.
 */
void jpeg_set_format(JPEG_Type *jpeg, jpeg_image_info_t img_info)
{
    uint16_t width;
    uint16_t height;
    uint8_t  xfill;
    uint8_t  yfill;
    uint32_t aligned_width;
    uint32_t aligned_height;

    jpeg->JPEG_SWREG18 &= ~JPEG_SWREG18_JPEG_MODE_Msk;
    jpeg->JPEG_SWREG18 |= ((img_info.mode << JPEG_SWREG18_JPEG_MODE_Pos)
                           & JPEG_SWREG18_JPEG_MODE_Msk);

    jpeg->JPEG_SWREG20 &= ~JPEG_SWREG20_LJPEG_FORMAT_Msk;
    jpeg->JPEG_SWREG20 |= ((img_info.coding_mode << JPEG_SWREG20_LJPEG_FORMAT_Pos)
                           & JPEG_SWREG20_LJPEG_FORMAT_Msk);

    jpeg->JPEG_SWREG38 &= ~JPEG_SWREG38_INPUT_FORMAT_Msk;
    jpeg->JPEG_SWREG38 |= ((img_info.input_fmt << JPEG_SWREG38_INPUT_FORMAT_Pos)
                           & JPEG_SWREG38_INPUT_FORMAT_Msk);

    /* Align encoding dimensions to multiple of 16 */
    aligned_width = ((img_info.width + JPEG_MCU_ALIGNMENT - 1)
                     / JPEG_MCU_ALIGNMENT) * JPEG_MCU_ALIGNMENT;
    aligned_height = ((img_info.height + JPEG_MCU_ALIGNMENT - 1)
                     / JPEG_MCU_ALIGNMENT) * JPEG_MCU_ALIGNMENT;

    width  = (uint16_t)(aligned_width >> JPEG_PIC_WH_PIXEL_SHIFT);
    height = (uint16_t)(aligned_height >> JPEG_PIC_WH_PIXEL_SHIFT);

    /* SWREG5: picture width LSB */
    jpeg->JPEG_SWREG5 &= ~JPEG_SWREG5_JPEG_PIC_WIDTH_Msk;
    jpeg->JPEG_SWREG5 |= (((uint32_t)(width & JPEG_PIC_WH_MASK)
                           << JPEG_SWREG5_JPEG_PIC_WIDTH_Pos)
                           & JPEG_SWREG5_JPEG_PIC_WIDTH_Msk);

    /* SWREG249: picture width MSB */
    jpeg->JPEG_SWREG249 &= ~JPEG_SWREG249_JPEG_PIC_WIDTH_MSB_Msk;
    jpeg->JPEG_SWREG249 |= (((uint32_t)(width >> JPEG_PIC_WH_FIELD_WIDTH)
                             << JPEG_SWREG249_JPEG_PIC_WIDTH_MSB_Pos)
                             & JPEG_SWREG249_JPEG_PIC_WIDTH_MSB_Msk);

    /* SWREG5: picture height LSB */
    jpeg->JPEG_SWREG5 &= ~JPEG_SWREG5_JPEG_PIC_HEIGHT_Msk;
    jpeg->JPEG_SWREG5 |= (((uint32_t)(height & JPEG_PIC_WH_MASK)
                           << JPEG_SWREG5_JPEG_PIC_HEIGHT_Pos)
                           & JPEG_SWREG5_JPEG_PIC_HEIGHT_Msk);

    /* SWREG249: picture height MSB */
    jpeg->JPEG_SWREG249 &= ~JPEG_SWREG249_JPEG_PIC_HEIGHT_MSB_Msk;
    jpeg->JPEG_SWREG249 |= (((uint32_t)(height >> JPEG_PIC_WH_FIELD_WIDTH)
                             << JPEG_SWREG249_JPEG_PIC_HEIGHT_MSB_Pos)
                             & JPEG_SWREG249_JPEG_PIC_HEIGHT_MSB_Msk);

    width  = (uint16_t)img_info.width;
    height = (uint16_t)img_info.height;

    xfill = (width  % JPEG_MCU_ALIGNMENT) ? (uint8_t)((JPEG_MCU_ALIGNMENT
             - (width  % JPEG_MCU_ALIGNMENT)) / JPEG_YUV420_CHROMA_DIV) : 0U;
    yfill = (height % JPEG_MCU_ALIGNMENT) ? (uint8_t)(JPEG_MCU_ALIGNMENT
             - (height % JPEG_MCU_ALIGNMENT)) : 0U;

    /* SWREG38: X fill */
    jpeg->JPEG_SWREG38 &= ~JPEG_SWREG38_XFILL_Msk;
    jpeg->JPEG_SWREG38 |= (((uint32_t)(xfill & JPEG_SWREG38_XFILL_FIELD_Msk)
                            << JPEG_SWREG38_XFILL_Pos)
                            & JPEG_SWREG38_XFILL_Msk);

    /* SWREG38: Y fill */
    jpeg->JPEG_SWREG38 &= ~JPEG_SWREG38_YFILL_Msk;
    jpeg->JPEG_SWREG38 |= (((uint32_t)(yfill & JPEG_SWREG38_YFILL_FIELD_Msk)
                            << JPEG_SWREG38_YFILL_Pos)
                            & JPEG_SWREG38_YFILL_Msk);

    /* SWREG193: X fill MSB */
    jpeg->JPEG_SWREG193 &= ~JPEG_SWREG193_XFILL_MSB_Msk;
    jpeg->JPEG_SWREG193 |= (((uint32_t)(xfill >> JPEG_XFILL_FIELD_WIDTH)
                             << JPEG_SWREG193_XFILL_MSB_Pos)
                             & JPEG_SWREG193_XFILL_MSB_Msk);

    /* SWREG193: Y fill MSB */
    jpeg->JPEG_SWREG193 &= ~JPEG_SWREG193_YFILL_MSB_Msk;
    jpeg->JPEG_SWREG193 |= (((uint32_t)(yfill >> JPEG_YFILL_FIELD_WIDTH)
                             << JPEG_SWREG193_YFILL_MSB_Pos)
                             & JPEG_SWREG193_YFILL_MSB_Msk);
}

/**
 * \fn           void jpeg_config_quality(JPEG_Type *jpeg, uint8_t quality)
 * \brief        Configure JPEG quantization tables for given quality factor.
 * \param[in]    jpeg      Pointer to the JPEG register map.
 * \param[in]    quality   JPEG quality factor (1-100).
 * \return       none.
 */
void jpeg_config_quality(JPEG_Type *jpeg, uint8_t quality)
{
    uint16_t scale_factor = jpeg_qf_scaling(quality);

    jpeg_calc_q_table(scale_factor);

    jpeg_set_q_table((uint32_t)jpeg);
}

/**
  \fn          void jpeg_irq_handler(JPEG_Type *jpeg, jpeg_encode_info_t *transfer)
  \brief       Handle interrupts for the JPEG.
  \param[in]   jpeg        Pointer to the JPEG register map.
  \param[in]   enc_info    The encode structure for the JPEG.
  \return      none
*/
void jpeg_irq_handler(JPEG_Type *jpeg, jpeg_encode_info_t *enc_info)
{
    uint32_t jpeg_int_status;

    jpeg_int_status = (jpeg->JPEG_SWREG1 & JPEG_SWREG1_STATUS_W1C_Msk);

    if (jpeg_int_status & JPEG_SWREG1_FRAME_RDY_STATUS) {
        enc_info->status |= JPEG_ENCODE_STATUS_FRAME_RDY;
        enc_info->encoded_size = (jpeg->JPEG_SWREG9 + JPEG_HEADER_SIZE);
    }

    if (jpeg_int_status & JPEG_SWREG1_BUFFER_FULL) {
        enc_info->status |= JPEG_ENCODE_STATUS_BUFFER_FULL;
    }

    if (jpeg_int_status & JPEG_SWREG1_BUS_ERROR_STATUS) {
        enc_info->status |= JPEG_ENCODE_STATUS_BUS_ERROR;
    }

    if (jpeg_int_status & JPEG_SWREG1_SW_RESET) {
        enc_info->status |= JPEG_ENCODE_STATUS_SW_RESET;
    }

    if (jpeg_int_status & JPEG_SWREG1_TIMEOUT) {
        enc_info->status |= JPEG_ENCODE_STATUS_TIMEOUT;
    }

    if (jpeg_int_status != 0U) {
        JPEG->JPEG_SWREG1 = (jpeg_int_status & JPEG_SWREG1_STATUS_W1C_Msk);
        (void)JPEG->JPEG_SWREG1;
    }
}
