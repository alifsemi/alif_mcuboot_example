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
 * @@file     jpeg_hantro_vc9000e.h
 * @@author   Chandra Bhushan Singh
 * @@email    chandrabhushan.singh@alifsemi.com
 * @@version  V1.0.0
 * @@date     14-April-2026
 * @@brief    Low level driver Specific header file.
 ******************************************************************************/

#ifndef JPEG_HANTRO_VC9000E_H_
#define JPEG_HANTRO_VC9000E_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include "soc.h"

/* JPEG_SWREG1 Register bit Definition, Macros, Offsets and Masks
 * these include interrupt enable/disable, interrupt status, timeout,
 * slice/line/segment interrupt enable, and interrupt type selection.
 */
/* JPEG_SWREG1 SW_ENC_IRQ bit[0] */
#define JPEG_SWREG1_IRQ                         (1U << 0)

/* JPEG_SWREG1 SW_ENC_IRQ_DIS bit[1] */
#define JPEG_SWREG1_IRQ_DIS                     (1U << 1)

/* JPEG_SWREG1 SW_ENC_FRAME_RDY_STATUS bit[2] */
#define JPEG_SWREG1_FRAME_RDY_STATUS            (1U << 2)

/* JPEG_SWREG1 SW_ENC_BUS_ERROR_STATUS bit[3] */
#define JPEG_SWREG1_BUS_ERROR_STATUS            (1U << 3)

/* JPEG_SWREG1 SW_ENC_SW_RESET bit[4] */
#define JPEG_SWREG1_SW_RESET                    (1U << 4)

/* JPEG_SWREG1 SW_ENC_BUFFER_FULL bit[5] */
#define JPEG_SWREG1_BUFFER_FULL                 (1U << 5)

/* JPEG_SWREG1 SW_ENC_TIMEOUT bit[6] */
#define JPEG_SWREG1_TIMEOUT                     (1U << 6)

/* JPEG_SWREG1 SW_ENC_IRQ_LINE_BUFFER bit[7] */
#define JPEG_SWREG1_IRQ_LINE_BUFFER             (1U << 7)

/* JPEG_SWREG1 SW_ENC_SLICE_RDY_STATUS bit[8] */
#define JPEG_SWREG1_SLICE_RDY_STATUS            (1U << 8)

/* JPEG_SWREG1 SW_ENC_IRQ_FUSE_ERROR bit[9] */
#define JPEG_SWREG1_IRQ_FUSE_ERROR              (1U << 9)

/* JPEG_SWREG1 SW_ENC_TIMEOUT_INT bit[11] */
#define JPEG_SWREG1_TIMEOUT_INT                 (1U << 11)

/* JPEG_SWREG1 SW_ENC_STRM_SEGMENT_RDY_INT bit[12] */
#define JPEG_SWREG1_STRM_SEGMENT_RDY_INT        (1U << 12)

/* JPEG_SWREG1 SW_ENC_IRQ_TYPE_FRAME_RDY bit[16] */
#define JPEG_SWREG1_IRQ_TYPE_FRAME_RDY          (0U << 16)

/* JPEG_SWREG1 SW_ENC_IRQ_TYPE_SLICE_RDY bit[17] */
#define JPEG_SWREG1_IRQ_TYPE_SLICE_RDY          (1U << 17)

/* JPEG_SWREG1 SW_ENC_IRQ_TYPE_LINE_BUFFER bit[18] */
#define JPEG_SWREG1_IRQ_TYPE_LINE_BUFFER        (1U << 18)

/* JPEG_SWREG1 SW_ENC_IRQ_TYPE_STRM_SEGMENT bit[19] */
#define JPEG_SWREG1_IRQ_TYPE_STRM_SEGMENT       (1U << 19)

/* JPEG_SWREG1 SW_ENC_IRQ_TYPE_TIMEOUT bit[20] */
#define JPEG_SWREG1_IRQ_TYPE_TIMEOUT            (1U << 20)

/* JPEG_SWREG1 SW_ENC_IRQ_TYPE_BUS_ERROR bit[21] */
#define JPEG_SWREG1_IRQ_TYPE_BUS_ERROR          (1U << 21)

/* JPEG_SWREG1 SW_ENC_IRQ_TYPE_BUFFER_FULL bit[22] */
#define JPEG_SWREG1_IRQ_TYPE_BUFFER_FULL        (1U << 22)

/* JPEG_SWREG1 SW_ENC_IRQ_TYPE_FUSE_ERROR bit[23] */
#define JPEG_SWREG1_IRQ_TYPE_FUSE_ERROR         (1U << 23)

/* JPEG_SWREG1 SW_ENC_IRQ_TYPE_SW_RESET bit[24] */
#define JPEG_SWREG1_IRQ_TYPE_SW_RESET           (1U << 24)

/* SWREG1 writable status bits (W1C) */
#define JPEG_SWREG1_STATUS_W1C_Msk    (JPEG_SWREG1_IRQ                  | \
                                       JPEG_SWREG1_FRAME_RDY_STATUS     | \
                                       JPEG_SWREG1_BUS_ERROR_STATUS     | \
                                       JPEG_SWREG1_SW_RESET             | \
                                       JPEG_SWREG1_BUFFER_FULL          | \
                                       JPEG_SWREG1_TIMEOUT              | \
                                       JPEG_SWREG1_IRQ_FUSE_ERROR)

/* JPEG_SWREG4 Register bit Definition, Macros, Offsets and Masks
 * these include JPEG encoder mode selection.
 */
/* JPEG_SWREG4 SW_ENC_MODE bit[31:29] */
#define JPEG_SWREG4_MODE                        4U
#define JPEG_SWREG4_MODE_Pos                    29U
#define JPEG_SWREG4_MODE_Msk                    (0x7UL << JPEG_SWREG4_MODE_Pos)

/* JPEG_SWREG5 Register bit Definition, Macros, Offsets and Masks
 * these include encoded JPEG width, encoded JPEG height, and encoder enable.
 */
/* JPEG_SWREG5 SW_ENC_JPEG_PIC_WIDTH bit[31:20] */
#define JPEG_SWREG5_JPEG_PIC_WIDTH_Pos          20U
#define JPEG_SWREG5_JPEG_PIC_WIDTH_Msk          (0xFFFUL << JPEG_SWREG5_JPEG_PIC_WIDTH_Pos)

/* JPEG_SWREG5 SW_ENC_JPEG_PIC_HEIGHT bit[19:8] */
#define JPEG_SWREG5_JPEG_PIC_HEIGHT_Pos         8U
#define JPEG_SWREG5_JPEG_PIC_HEIGHT_Msk         (0xFFFUL << JPEG_SWREG5_JPEG_PIC_HEIGHT_Pos)

/* JPEG_SWREG5 SW_ENC_E bit[0] */
#define JPEG_SWREG5_E_Pos                       0U
#define JPEG_SWREG5_E_Msk                       (1UL << JPEG_SWREG5_E_Pos)

/* JPEG_SWREG8 Register bit Definition, Macros, Offsets and Masks
 * these include output stream base address.
 */
/* JPEG_SWREG8 SW_ENC_OUTPUT_STRM_BASE bit[31:0] */
#define JPEG_SWREG8_OUTPUT_STRM_BASE_Pos        0U
#define JPEG_SWREG8_OUTPUT_STRM_BASE_Msk        (0xFFFFFFFFUL << JPEG_SWREG8_OUTPUT_STRM_BASE_Pos)

/* JPEG_SWREG18 Register bit Definition, Macros, Offsets and Masks
 * these include stream start offset, JPEG slice enable, JPEG mode,
 * reset interrupt value, and reset value.
 */
/* JPEG_SWREG18 SW_ENC_JPEG_SLICE bit[25] */
#define JPEG_SWREG18_JPEG_SLICE                 (1U << 25)

/* JPEG_SWREG18 SW_ENC_JPEG_MODE bit[24] */
#define JPEG_SWREG18_JPEG_MODE_Pos              24U
#define JPEG_SWREG18_JPEG_MODE_Msk              (1UL << JPEG_SWREG18_JPEG_MODE_Pos)

/* JPEG_SWREG18 SW_ENC_JPEG_RSTINT bit[23:16] */
#define JPEG_SWREG18_JPEG_RSTINT_Pos            16U
#define JPEG_SWREG18_JPEG_RSTINT_Msk            (0xFFUL << JPEG_SWREG18_JPEG_RSTINT_Pos)

/* JPEG_SWREG18 SW_ENC_JPEG_RST bit[15:0] */
#define JPEG_SWREG18_JPEG_RST_Pos               0U
#define JPEG_SWREG18_JPEG_RST_Msk               (0xFFFFUL << JPEG_SWREG18_JPEG_RST_Pos)

/* JPEG_SWREG20 Register bit Definition, Macros, Offsets and Masks
 * these include input row length, lossless JPEG enable, lossless format,
 * predictor selection value, and point transform.
 */
/* JPEG_SWREG20 SW_ENC_JPEG_ROWLENGTH bit[31:17] */
#define JPEG_SWREG20_JPEG_ROWLENGTH_Pos         17U
#define JPEG_SWREG20_JPEG_ROWLENGTH_Msk         (0x7FFFUL << JPEG_SWREG20_JPEG_ROWLENGTH_Pos)

/* JPEG_SWREG20 SW_ENC_LJPEG_FORMAT bit[15:14] */
#define JPEG_SWREG20_LJPEG_FORMAT_Pos           14U
#define JPEG_SWREG20_LJPEG_FORMAT_Msk           (0x3UL << JPEG_SWREG20_LJPEG_FORMAT_Pos)

/* JPEG_SWREG38 Register bit Definition, Macros, Offsets and Masks
 * these include input format, input rotation, mirror, Y fill and X fill.
 */
/* JPEG_SWREG38 SW_ENC_INPUT_FORMAT bit[31:28] */
#define JPEG_SWREG38_INPUT_FORMAT_Pos           28U
#define JPEG_SWREG38_INPUT_FORMAT_Msk           (0xFUL << JPEG_SWREG38_INPUT_FORMAT_Pos)

/* JPEG_SWREG38 SW_ENC_XFILL bit[5:4] */
#define JPEG_SWREG38_XFILL_Pos                  4U
#define JPEG_SWREG38_XFILL_FIELD_Msk            0x3UL
#define JPEG_SWREG38_XFILL_Msk                  (0x3UL << JPEG_SWREG38_XFILL_Pos)

/* JPEG_SWREG38 SW_ENC_YFILL bit[3:1] */
#define JPEG_SWREG38_YFILL_Pos                  1U
#define JPEG_SWREG38_YFILL_FIELD_Msk            0x7UL
#define JPEG_SWREG38_YFILL_Msk                  (0x7UL << JPEG_SWREG38_YFILL_Pos)

/* JPEG_SWREG81 Register bit Definition, Macros, Offsets and Masks
 * these include maximum AXI burst length.
 */
/* JPEG_SWREG81 SW_ENC_MAX_BURST bit[31:24] */
#define JPEG_SWREG81_MAX_BURST_Pos              24U
#define JPEG_SWREG81_MAX_BURST_Msk              (0xFFUL << JPEG_SWREG81_MAX_BURST_Pos)

/* JPEG_SWREG193 Register bit Definition, Macros, Offsets and Masks
 * these include X fill MSB and Y fill MSB.
 */
/* JPEG_SWREG193 SW_ENC_XFILL_MSB bit[7:6] */
#define JPEG_SWREG193_XFILL_MSB_Pos             6U
#define JPEG_SWREG193_XFILL_MSB_Msk             (0x3UL << JPEG_SWREG193_XFILL_MSB_Pos)

/* JPEG_SWREG193 SW_ENC_YFILL_MSB bit[5:4] */
#define JPEG_SWREG193_YFILL_MSB_Pos             4U
#define JPEG_SWREG193_YFILL_MSB_Msk             (0x3UL << JPEG_SWREG193_YFILL_MSB_Pos)

/* JPEG_SWREG210 Register bit Definition, Macros, Offsets and Masks
 * these include input luma stride.
 */
/* JPEG_SWREG210 SW_ENC_INPUT_LUMA_STRIDE bit[31:12] */
#define JPEG_SWREG210_INPUT_LUMA_STRIDE_Pos     12U
#define JPEG_SWREG210_INPUT_LUMA_STRIDE_Msk     (0xFFFFFUL << JPEG_SWREG210_INPUT_LUMA_STRIDE_Pos)

/* JPEG_SWREG211 Register bit Definition, Macros, Offsets and Masks
 * these include input chroma stride.
 */
/* JPEG_SWREG211 SW_ENC_INPUT_CHROMA_STRIDE bit[31:12] */
#define JPEG_SWREG211_INPUT_CHROMA_STRIDE_Pos   12U
#define JPEG_SWREG211_INPUT_CHROMA_STRIDE_Msk   \
        (0xFFFFFUL << JPEG_SWREG211_INPUT_CHROMA_STRIDE_Pos)

/* JPEG_SWREG246 Register bit Definition, Macros, Offsets and Masks
 * these include AXI write outstanding number.
 */
/* JPEG_SWREG246 SW_ENC_AXI_WRITE_OUTSTANDING_NUM bit[13:6] */
#define JPEG_SWREG246_AXI_WRITE_OUTSTANDING_NUM_Pos   6U
#define JPEG_SWREG246_AXI_WRITE_OUTSTANDING_NUM_Msk   \
        (0xFFUL << JPEG_SWREG246_AXI_WRITE_OUTSTANDING_NUM_Pos)

/* JPEG_SWREG249 Register bit Definition, Macros, Offsets and Masks
 * these include JPEG width MSB, JPEG height MSB, and row length MSB.
 */
/* JPEG_SWREG249 SW_ENC_JPEG_PIC_WIDTH_MSB bit[10:9] */
#define JPEG_SWREG249_JPEG_PIC_WIDTH_MSB_Pos   9U
#define JPEG_SWREG249_JPEG_PIC_WIDTH_MSB_Msk   (0x3UL << JPEG_SWREG249_JPEG_PIC_WIDTH_MSB_Pos)

/* JPEG_SWREG249 SW_ENC_JPEG_PIC_HEIGHT_MSB bit[8:7] */
#define JPEG_SWREG249_JPEG_PIC_HEIGHT_MSB_Pos  7U
#define JPEG_SWREG249_JPEG_PIC_HEIGHT_MSB_Msk  (0x3UL << JPEG_SWREG249_JPEG_PIC_HEIGHT_MSB_Pos)

/* JPEG_SWREG249 SW_ENC_JPEG_ROWLENGTH_MSB bit[6:5] */
#define JPEG_SWREG249_JPEG_ROWLENGTH_MSB_Pos   5U
#define JPEG_SWREG249_JPEG_ROWLENGTH_MSB_Msk   (0x3UL << JPEG_SWREG249_JPEG_ROWLENGTH_MSB_Pos)

/* JPEG_SWREG261 Register bit Definition, Macros, Offsets and Masks
 * these include AXI read outstanding number.
 */
/* JPEG_SWREG261 SW_ENC_AXI_READ_OUTSTANDING_NUM bit[11:4] */
#define JPEG_SWREG261_AXI_READ_OUTSTANDING_NUM_Pos   4U
#define JPEG_SWREG261_AXI_READ_OUTSTANDING_NUM_Msk   \
        (0xFFUL << JPEG_SWREG261_AXI_READ_OUTSTANDING_NUM_Pos)

/* JPEG encoder constants */
#define JPEG_MCU_ALIGNMENT                           16U
#define JPEG_YUV420_CHROMA_DIV                       2U
#define JPEG_PIC_WH_PIXEL_SHIFT                      3U
#define JPEG_PIC_WH_MASK                             0xFFF
#define JPEG_PIC_WH_FIELD_WIDTH                      12U
#define JPEG_ROWLENGTH_FIELD_MASK                    0x7FFF
#define JPEG_ROWLENGTH_FIELD_WIDTH                   15U
#define JPEG_QUALITY_FACTOR_MIN                      0U
#define JPEG_QUALITY_FACTOR_MAX                      100U

/* JPEG header limits */
#define JPEG_HEADER_SIZE                             623U

/* Register field split widths */
#define JPEG_XFILL_FIELD_WIDTH                       2U
#define JPEG_YFILL_FIELD_WIDTH                       3U

/* JPEG Hardware Version */
#define JPEG_HW_VERSION                              0x00C0C200

/*JPEG Hardware ID */
#define JPEG_HW_ID                                   0x90001000

/* JPEG maximum planes */
#define JPEG_MAX_PLANES                              3U

/**
 * enum  JPEG_SUBSAMPLING_MODE
 * JPEG chroma subsampling mode configuration.
 */
typedef enum _JPEG_SUBSAMPLING_MODE {
    JPEG_SUBSAMPLING_MODE_420 = 0,      /* Select JPEG subsampling mode 4:2:0      */
    JPEG_SUBSAMPLING_MODE_422           /* Select JPEG subsampling mode 4:2:2      */
} JPEG_SUBSAMPLING_MODE;

/**
 * enum  JPEG_CODING_MODE
 * JPEG coding mode configuration.
 */
typedef enum _JPEG_CODING_MODE {
    JPEG_CODING_MODE_420 = 0,           /* JPEG coding mode YUV420                 */
    JPEG_CODING_MODE_422,               /* JPEG coding mode YUV422                 */
    JPEG_CODING_MODE_MONO               /* JPEG coding mode monochrome             */
} JPEG_CODING_MODE;

/**
 * enum  JPEG_INPUT_FORMAT
 * JPEG input format configuration.
 */
typedef enum _JPEG_INPUT_FORMAT {
    JPEG_INPUT_FORMAT_NV12      = 1U,   /* YUV420 semi-planar (Y + UV)             */
    JPEG_INPUT_FORMAT_NV21      = 2U,   /* YUV420 semi-planar (Y + VU)             */
} JPEG_INPUT_FORMAT;

/**
 * enum JPEG_ENCODE_STATUS.
 * Status of an ongoing JPEG ENCODING.
 */
typedef enum _JPEG_ENCODE_STATUS {
    JPEG_ENCODE_STATUS_NONE             = 0U,       /* JPEG encode status none           */
    JPEG_ENCODE_STATUS_FRAME_RDY        = (1 << 0), /* JPEG encode status frame ready    */
    JPEG_ENCODE_STATUS_BUFFER_FULL      = (1 << 1), /* JPEG encode status buffer full    */
    JPEG_ENCODE_STATUS_BUS_ERROR        = (1 << 2), /* JPEG encode status bus error      */
    JPEG_ENCODE_STATUS_SW_RESET         = (1 << 3), /* JPEG encode status software reset */
    JPEG_ENCODE_STATUS_TIMEOUT          = (1 << 4), /* JPEG encode status frame ready    */
} JPEG_ENCODE_STATUS;

/**
 * @struct  _jpeg_image_info_t
 * @brief   JPEG image configuration information.
 */
typedef struct _jpeg_image_info_t {
    JPEG_SUBSAMPLING_MODE  mode;         /* JPEG chroma subsampling mode (4:2:0 / 4:2:2) */
    JPEG_CODING_MODE       coding_mode;  /* JPEG encoder coding mode                     */
    JPEG_INPUT_FORMAT      input_fmt;    /* Input pixel format                           */
    uint32_t               width;        /* Image width in pixels                        */
    uint32_t               height;       /* Image height in pixels                       */
} jpeg_image_info_t;

/**
 * @struct  _jpeg_encode_info_t
 * @brief   JPEG encode information(Run-Time).
 */
typedef struct _jpeg_encode_info_t {
    JPEG_ENCODE_STATUS status;               /* JPEG encode status      */
    uint32_t           encoded_size;         /* JPEG encoded image size */
} jpeg_encode_info_t;

/**
 * \fn           void jpeg_enable_enc_interrupts(JPEG_Type *jpeg)
 * \brief        Enable JPEG interrupt.
 * \param[in]    jpeg        Pointer to JPEG register map.
 * \return       none.
 */
static inline void jpeg_enable_enc_interrupts(JPEG_Type *jpeg)
{
    jpeg->JPEG_SWREG1 |= (JPEG_SWREG1_IRQ_TYPE_FRAME_RDY |
                          JPEG_SWREG1_IRQ_TYPE_BUFFER_FULL |
                          JPEG_SWREG1_TIMEOUT_INT);
}

/**
 * \fn           void jpeg_set_encoding_mode(JPEG_Type *jpeg)
 * \brief        Set JPEG encoding mode.
 * \param[in]    jpeg   Pointer to the JPEG register map.
 * \return       none.
 */
static inline void jpeg_set_encoding_mode(JPEG_Type *jpeg)
{
    jpeg->JPEG_SWREG4 = ((JPEG_SWREG4_MODE << JPEG_SWREG4_MODE_Pos)
                         & JPEG_SWREG4_MODE_Msk);
}

/**
 * \fn           void jpeg_set_axi_max_burst_len(JPEG_Type *jpeg, uint8_t burst_len)
 * \brief        Set JPEG AXI max burst length.
 * \param[in]    jpeg        Pointer to the JPEG register map.
 * \param[in]    burst_len   JPEG AXI max burst length
 * \return       none.
 */
static inline void jpeg_set_axi_max_burst_len(JPEG_Type *jpeg, uint8_t burst_len)
{
    jpeg->JPEG_SWREG81 &= ~JPEG_SWREG81_MAX_BURST_Msk;
    jpeg->JPEG_SWREG81 |= ((burst_len << JPEG_SWREG81_MAX_BURST_Pos)
                           & JPEG_SWREG81_MAX_BURST_Msk);
}

/**
 * \fn           void jpeg_set_axi_write_outstanding_num(JPEG_Type *jpeg, uint8_t number)
 * \brief        Set JPEG AXI write outstanding number.
 * \param[in]    jpeg     Pointer to the JPEG register map.
 * \param[in]    number   Number of AXI write outstanding transactions.
 * \return       none.
 */
static inline void jpeg_set_axi_write_outstanding_num(JPEG_Type *jpeg, uint8_t number)
{
    jpeg->JPEG_SWREG246 = ((number << JPEG_SWREG246_AXI_WRITE_OUTSTANDING_NUM_Pos)
                           & JPEG_SWREG246_AXI_WRITE_OUTSTANDING_NUM_Msk);
}

/**
 * \fn           void jpeg_set_axi_read_outstanding_num(JPEG_Type *jpeg, uint8_t number)
 * \brief        Set JPEG AXI read outstanding number.
 * \param[in]    jpeg     Pointer to the JPEG register map.
 * \param[in]    number   Number of AXI read outstanding transactions.
 * \return       none.
 */
static inline void jpeg_set_axi_read_outstanding_num(JPEG_Type *jpeg, uint8_t number)
{
    jpeg->JPEG_SWREG261 &= ~JPEG_SWREG261_AXI_READ_OUTSTANDING_NUM_Msk;
    jpeg->JPEG_SWREG261 |= ((number << JPEG_SWREG261_AXI_READ_OUTSTANDING_NUM_Pos)
                            & JPEG_SWREG261_AXI_READ_OUTSTANDING_NUM_Msk);
}

/**
 * \fn           void jpeg_set_input_luma_base_addr(JPEG_Type *jpeg, uint32_t addr)
 * \brief        Set JPEG input luma base address.
 * \param[in]    jpeg     Pointer to the JPEG register map.
 * \param[in]    addr     JPEG input luma base address
 * \return       none.
 */
static inline void jpeg_set_input_luma_base_addr(JPEG_Type *jpeg, uint32_t addr)
{
    jpeg->JPEG_SWREG12 = addr;
}

/**
 * \fn           void jpeg_set_input_cb_base_addr(JPEG_Type *jpeg, uint32_t addr)
 * \brief        Set JPEG input CB base address.
 * \param[in]    jpeg     Pointer to the JPEG register map.
 * \param[in]    addr     JPEG input CB base address
 * \return       none.
 */
static inline void jpeg_set_input_cb_base_addr(JPEG_Type *jpeg, uint32_t addr)
{
    jpeg->JPEG_SWREG13 = addr;
}

/**
 * \fn           void jpeg_set_input_cr_base_addr(JPEG_Type *jpeg, uint32_t addr)
 * \brief        Set JPEG input CR base address.
 * \param[in]    jpeg     Pointer to the JPEG register map.
 * \param[in]    addr     JPEG input CR base address
 * \return       none.
 */
static inline void jpeg_set_input_cr_base_addr(JPEG_Type *jpeg, uint32_t addr)
{
    jpeg->JPEG_SWREG14 = addr;
}

/**
 * \fn           void jpeg_set_input_row_len_lsb(JPEG_Type *jpeg, uint32_t row_len)
 * \brief        Set lower bits of JPEG input row length field.
 * \param[in]    jpeg     Pointer to the JPEG register map.
 * \param[in]    row_len  JPEG input row length
 * \return       none.
 */
static inline void jpeg_set_input_row_len_lsb(JPEG_Type *jpeg, uint32_t row_len)
{
    jpeg->JPEG_SWREG20 &= ~JPEG_SWREG20_JPEG_ROWLENGTH_Msk;
    jpeg->JPEG_SWREG20 |= (((row_len & JPEG_ROWLENGTH_FIELD_MASK)
                           << JPEG_SWREG20_JPEG_ROWLENGTH_Pos)
                           & JPEG_SWREG20_JPEG_ROWLENGTH_Msk);
}

/**
 * \fn           void jpeg_set_input_row_len_msb(JPEG_Type *jpeg, uint32_t row_len)
 * \brief        Set upper bits of JPEG input row length field.
 * \param[in]    jpeg     Pointer to the JPEG register map.
 * \param[in]    row_len  JPEG input row length
 * \return       none.
 */
static inline void jpeg_set_input_row_len_msb(JPEG_Type *jpeg, uint32_t row_len)
{
    jpeg->JPEG_SWREG249 &= ~JPEG_SWREG249_JPEG_ROWLENGTH_MSB_Msk;
    jpeg->JPEG_SWREG249 |= (((row_len >> JPEG_ROWLENGTH_FIELD_WIDTH)
                            << JPEG_SWREG249_JPEG_ROWLENGTH_MSB_Pos)
                            & JPEG_SWREG249_JPEG_ROWLENGTH_MSB_Msk);
}

/**
 * \fn           void jpeg_set_input_luma_stride(JPEG_Type *jpeg, uint32_t stride)
 * \brief        Set JPEG input luma stride.
 * \param[in]    jpeg     Pointer to the JPEG register map.
 * \param[in]    stride   JPEG input luma stride.
 * \return       none.
 */
static inline void jpeg_set_input_luma_stride(JPEG_Type *jpeg, uint32_t stride)
{
    jpeg->JPEG_SWREG210 = ((stride << JPEG_SWREG210_INPUT_LUMA_STRIDE_Pos)
                           & JPEG_SWREG210_INPUT_LUMA_STRIDE_Msk);
}

/**
 * \fn           void jpeg_set_input_chroma_stride(JPEG_Type *jpeg, uint32_t stride)
 * \brief        Set JPEG input chroma stride.
 * \param[in]    jpeg     Pointer to the JPEG register map.
 * \param[in]    stride   JPEG input chroma stride.
 * \return       none.
 */
static inline void jpeg_set_input_chroma_stride(JPEG_Type *jpeg, uint32_t stride)
{
    jpeg->JPEG_SWREG211 = ((stride << JPEG_SWREG211_INPUT_CHROMA_STRIDE_Pos)
                           & JPEG_SWREG211_INPUT_CHROMA_STRIDE_Msk);
}

/**
 * \fn           void jpeg_set_output_stream_addr(JPEG_Type *jpeg, uint32_t addr)
 * \brief        Set JPEG output stream address.
 * \param[in]    jpeg     Pointer to the JPEG register map.
 * \param[in]    addr     JPEG output stream address.
 * \return       none.
 */
static inline void jpeg_set_output_stream_addr(JPEG_Type *jpeg, uint32_t addr)
{
    jpeg->JPEG_SWREG8 = addr;
}

/**
 * \fn           void jpeg_set_output_stream_size(JPEG_Type *jpeg, uint32_t size)
 * \brief        Set JPEG output stream size.
 * \param[in]    jpeg     Pointer to the JPEG register map.
 * \param[in]    size     JPEG output stream size.
 * \return       none.
 */
static inline void jpeg_set_output_stream_size(JPEG_Type *jpeg, uint32_t size)
{
    jpeg->JPEG_SWREG9 = size;
}

/**
 * \fn           void jpeg_encode_start(JPEG_Type *jpeg)
 * \brief        Start JPEG encoding.
 * \param[in]    jpeg  Pointer to the JPEG register map.
 * \return       none.
 */
static inline void jpeg_encode_start(JPEG_Type *jpeg)
{
    jpeg->JPEG_SWREG5 |= JPEG_SWREG5_E_Msk;
}

/**
 * \fn           bool jpeg_init(JPEG_Type *jpeg)
 * \brief        Validates the JPEG hardware ID and hardware version
 *               registers to ensure that the expected JPEG IP is
 *               present.
 * \param[in]    jpeg  Pointer to the JPEG register map.
 * \return       true  JPEG hardware ID and version match the expected values.
 *               false JPEG hardware ID or version does not match.
 */
bool jpeg_init(JPEG_Type *jpeg);

/**
 * \fn           jpeg_set_format(JPEG_Type *jpeg, jpeg_image_info_t img_info)
 * \brief        Configure JPEG format registers.
 * \param[in]    jpeg       Pointer to the JPEG register map.
 * \param[in]    img_info   Pointer to JPEG image configuration.
 * \return       none.
 */
void jpeg_set_format(JPEG_Type *jpeg, jpeg_image_info_t img_info);

/**
 * \fn           void jpeg_config_quality(JPEG_Type *jpeg, uint8_t quality)
 * \brief        Configure JPEG quantization tables for given quality factor.
 * \param[in]    jpeg      Pointer to the JPEG register map.
 * \param[in]    quality   JPEG quality factor (1-100).
 * \return       none.
 */
void jpeg_config_quality(JPEG_Type *jpeg, uint8_t quality);

/**
  \fn          void jpeg_irq_handler(JPEG_Type *jpeg, jpeg_encode_info_t *transfer)
  \brief       Handle interrupts for the JPEG.
  \param[in]   jpeg        Pointer to the JPEG register map.
  \param[in]   enc_info    The encode structure for the JPEG.
  \return      none
*/
void jpeg_irq_handler(JPEG_Type *jpeg, jpeg_encode_info_t *enc_info);

#ifdef __cplusplus
}
#endif

#endif /* JPEG_HANTRO_VC9000E_H_ */
