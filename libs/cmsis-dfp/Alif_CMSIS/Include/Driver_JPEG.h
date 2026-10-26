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
 * @file     Driver_JPEG.h
 * @author   Chandra Bhushan Singh
 * @email    chandrabhushan.singh@alifsemi.com
 * @version  V1.0.0
 * @date     14-April-2026
 * @brief    JPEG Encoder Driver definitions.
 ******************************************************************************/

#ifndef DRIVER_JPEG_H_
#define DRIVER_JPEG_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "Driver_Common.h"

#define ARM_JPEG_API_VERSION   ARM_DRIVER_VERSION_MAJOR_MINOR(1, 0)

/* Pixel formats — value of ARM_JPEG_FORMAT.pixel_format.
 * Each value names one in-memory layout. The number of planes the
 * driver expects in ARM_JPEG_INPUT_BUFFER follows from this value.
 */
#define ARM_JPEG_FORMAT_NV12        (0x01UL)         ///< Semi-planar 4:2:0  (2 planes: Y, interleaved CbCr)
#define ARM_JPEG_FORMAT_NV21        (0x02UL)         ///< Semi-planar 4:2:0  (2 planes: Y, interleaved CrCb)
#define ARM_JPEG_FORMAT_YUV420P     (0x03UL)         ///< Planar 4:2:0       (3 planes: Y, Cb, Cr)
#define ARM_JPEG_FORMAT_YUV422P     (0x04UL)         ///< Planar 4:2:2       (3 planes: Y, Cb, Cr)
#define ARM_JPEG_FORMAT_YUYV        (0x05UL)         ///< Packed 4:2:2       (1 plane,  YUY2 order)
#define ARM_JPEG_FORMAT_UYVY        (0x06UL)         ///< Packed 4:2:2       (1 plane,  UYVY order)
#define ARM_JPEG_FORMAT_YUV444      (0x07UL)         ///< Packed 4:4:4       (1 plane,  Y Cb Cr per pixel)
#define ARM_JPEG_FORMAT_GRAY8       (0x08UL)         ///< Monochrome 4:0:0   (1 plane,  8 bpp luma only)
#define ARM_JPEG_FORMAT_RGB24       (0x09UL)         ///< Packed RGB 8:8:8   (1 plane,  R G B per pixel)
#define ARM_JPEG_FORMAT_BGR24       (0x0AUL)         ///< Packed BGR 8:8:8   (1 plane,  B G R per pixel)
#define ARM_JPEG_FORMAT_RGB565      (0x0BUL)         ///< Packed RGB 5:6:5   (1 plane,  16 bpp)
#define ARM_JPEG_FORMAT_ARGB8888    (0x0CUL)         ///< Packed ARGB 8:8:8:8 (1 plane, 32 bpp; alpha discarded)
#define ARM_JPEG_FORMAT_BGRA8888    (0x0DUL)         ///< Packed BGRA 8:8:8:8 (1 plane, 32 bpp; alpha discarded)

/* Chroma subsampling — value passed to ARM_JPEG_CTRL_SET_SUBSAMPLING
 * (selects the OUTPUT subsampling; input subsampling is implied by
 * the pixel format).
 */
#define ARM_JPEG_SUBSAMP_444        (0x01UL)         ///< 4:4:4 (no chroma decimation)
#define ARM_JPEG_SUBSAMP_422        (0x02UL)         ///< 4:2:2 (horizontal 2:1)
#define ARM_JPEG_SUBSAMP_420        (0x03UL)         ///< 4:2:0 (horizontal & vertical 2:1)
#define ARM_JPEG_SUBSAMP_400        (0x04UL)         ///< 4:0:0 (monochrome, chroma dropped)
#define ARM_JPEG_SUBSAMP_440        (0x05UL)         ///< 4:4:0 (vertical 2:1)

/* Color space / range — for RGB→YCbCr conversion path.
 * Value passed to ARM_JPEG_CTRL_SET_COLOR_SPACE.
 */
#define ARM_JPEG_CSC_BT601_LIMITED  (0x01UL)         ///< ITU-R BT.601, studio (16..235) range
#define ARM_JPEG_CSC_BT601_FULL     (0x02UL)         ///< ITU-R BT.601, full   (0..255)  range
#define ARM_JPEG_CSC_BT709_LIMITED  (0x03UL)         ///< ITU-R BT.709, studio (16..235) range
#define ARM_JPEG_CSC_BT709_FULL     (0x04UL)         ///< ITU-R BT.709, full   (0..255)  range

/* Container / header emission — value passed to
 * ARM_JPEG_CTRL_SET_HEADER_MODE.
 */
#define ARM_JPEG_HEADER_NONE        (0x00UL)         ///< Raw entropy-coded data only (no SOI/EOI/JFIF)
#define ARM_JPEG_HEADER_JFIF        (0x01UL)         ///< Standard JFIF (SOI..EOI with APP0)
#define ARM_JPEG_HEADER_EXIF        (0x02UL)         ///< EXIF container (SOI..EOI with APP1)

/* Slice operating model — value passed to ARM_JPEG_CTRL_SET_SLICE_MODEL.
 */
#define ARM_JPEG_SLICE_MODEL_REFILL (0x00UL)         ///< Single output buffer refilled between slices
#define ARM_JPEG_SLICE_MODEL_RING   (0x01UL)         ///< Ring of pre-enqueued output buffers

/* Thumbnail source formats — value of ARM_JPEG_THUMBNAIL.format */
#define ARM_JPEG_THUMB_RGB24        (0x01UL)         ///< Thumbnail data is raw RGB888
#define ARM_JPEG_THUMB_JPEG         (0x02UL)         ///< Thumbnail data is a pre-encoded JPEG
#define ARM_JPEG_THUMB_PALETTE8     (0x03UL)         ///< Thumbnail data is 8-bit palette indexed

/* Data types */

typedef struct _ARM_JPEG_FORMAT {
    uint32_t pixel_format;          /* ARM_JPEG_FORMAT_xxx                  */
    uint32_t width;                 /* image width in pixels                */
    uint32_t height;                /* image height in pixels               */
    uint32_t pitch;                 /* row stride in bytes; 0 = packed      */
} ARM_JPEG_FORMAT;

typedef struct _ARM_JPEG_CROP {
    uint32_t x;                     /* x offset in pixels                   */
    uint32_t y;                     /* y offset in pixels                   */
    uint32_t width;                 /* Output image width in pixels         */
    uint32_t height;                /* Output image height in pixels        */
} ARM_JPEG_CROP;

typedef struct _ARM_JPEG_THUMBNAIL {
    uint32_t    format;             /* ARM_JPEG_THUMB_xxx                   */
    uint32_t    width;              /* Thumb width in pixels                */
    uint32_t    height;             /* Thumb height in pixels               */
    const void *data;               /* Thumb data buffer                    */
    uint32_t    data_length;        /* Thumb data buffer length in bytes    */
} ARM_JPEG_THUMBNAIL;

/*
 * ARM_JPEG_BUFFER — descriptor for the encoded JPEG bitstream. *
 *   IN   : `size` is the buffer capacity in bytes.
 *   OUT  : `size` is overwritten by the driver with the
 *                    actual encoded JPEG byte count.
 */
typedef struct _ARM_JPEG_BUFFER {
    void    *buf;                   /* Output Data buffer                   */
    uint32_t size;                  /* Output Data buffer Output limit size */
} ARM_JPEG_BUFFER;

/*
 * ARM_JPEG_INPUT_BUFFER — raw pixel input descriptor.
 *
 * Plane count and ordering follow the active pixel format:
 *   Packed   (RGB24/GRAY8/YUYV/...) : num_planes = 1 → plane[0] only
 *   NV12 / NV21                     : num_planes = 2 → plane[0]=Y, plane[1]=interleaved chroma
 *   YUV420P / YUV422P               : num_planes = 3 → plane[0]=Y, plane[1]=Cb, plane[2]=Cr
 *
 * plane_pitch[i] is the row stride (in bytes) of plane i. For tightly
 * packed sources, plane_pitch = width_of_plane_in_bytes.
 *
 * When ARM_JPEG_CTRL_SET_CROP is active, crop (x, y, w, h) are given in
 * luma-plane pixel coordinates. The driver applies the equivalent
 * subsampled crop to chroma planes.
 */
typedef struct _ARM_JPEG_INPUT_BUFFER {
    void    *plane[3];
    uint32_t plane_pitch[3];
    uint32_t num_planes;
} ARM_JPEG_INPUT_BUFFER;

/*
 * ARM_JPEG_SLICE_INFO — populated by ARM_JPEG_CTRL_GET_SLICE_INFO
 * immediately after a successful DEQUEUE in slice mode.
 */
typedef struct _ARM_JPEG_SLICE_INFO {
    uint32_t slice_index;     /* 0-based slice number within the frame */
    uint32_t slice_count;     /* total slices in this frame            */
    uint32_t is_last      : 1;
    uint32_t reserved     : 31;
} ARM_JPEG_SLICE_INFO;

/* Capabilities (returned by GetCapabilities()) */
typedef struct _ARM_JPEG_CAPABILITIES {
    /* Direction */
    uint32_t encode             : 1;   /* encoder direction supported      */
    uint32_t decode             : 1;   /* decoder direction (deferred)     */

    /* Input pixel formats */
    uint32_t input_nv12         : 1;
    uint32_t input_nv21         : 1;
    uint32_t input_yuv420p      : 1;
    uint32_t input_yuv422p      : 1;
    uint32_t input_yuv444       : 1;
    uint32_t input_yuyv         : 1;
    uint32_t input_uyvy         : 1;
    uint32_t input_gray8        : 1;
    uint32_t input_rgb24        : 1;
    uint32_t input_bgr24        : 1;
    uint32_t input_rgb565       : 1;
    uint32_t input_argb8888     : 1;
    uint32_t input_bgra8888     : 1;

    /* Thumbnail source formats */
    uint32_t thumb_rgb24        : 1;
    uint32_t thumb_jpeg         : 1;
    uint32_t thumb_palette8     : 1;

    /* Encode features */
    uint32_t hw_crop            : 1;
    uint32_t restart_markers    : 1;
    uint32_t rate_control       : 1;   /* SET_TARGET_SIZE supported        */

    /* Slice mode */
    uint32_t slice_output       : 1;   /* slice mode supported             */
    uint32_t slice_refill       : 1;   /* refill-in-place model supported  */
    uint32_t slice_ring         : 1;   /* ring-of-buffers model supported  */

    uint32_t reserved_bits      : 7;   /* future capability flags          */

    /* Dimensional limits */
    uint32_t max_width;
    uint32_t max_height;
    uint32_t min_width;
    uint32_t min_height;

    /* Slice limits — valid only if slice_output == 1, in MCU rows */
    uint32_t slice_height_min;
    uint32_t slice_height_max;
} ARM_JPEG_CAPABILITIES;

/* JPEG Events */
#define ARM_JPEG_EVENT_BUS_RESET       (1UL << 0)     ///< IP completed bus reset (post-init or recovery)
#define ARM_JPEG_EVENT_FRAME_READY     (1UL << 1)     ///< Full frame encoded; output buffer ready to DEQUEUE
#define ARM_JPEG_EVENT_BUS_ERROR       (1UL << 2)     ///< AXI/AHB bus fault during DMA
#define ARM_JPEG_EVENT_BUFFER_FULL     (1UL << 3)     ///< Encoded output exceeded enqueued buffer capacity; frame discarded
#define ARM_JPEG_EVENT_TIMEOUT         (1UL << 4)     ///< Encode watchdog expired
#define ARM_JPEG_EVENT_SLICE_READY     (1UL << 5)     ///< One slice encoded; output buffer holds slice bytes
#define ARM_JPEG_EVENT_SLICE_OVERFLOW  (1UL << 6)     ///< A slice exceeded its buffer; frame aborted

typedef void (*ARM_JPEG_SignalEvent_t)(uint32_t event);

/* Control codes */

/* --- Configuration ---------------------------------------------------- */
#define ARM_JPEG_CTRL_SET_FORMAT             (0x01UL) ///< Set input image format; arg: (uint32_t)&ARM_JPEG_FORMAT
#define ARM_JPEG_CTRL_SET_QUALITY            (0x02UL) ///< Set encode quality; arg: 1..100
#define ARM_JPEG_CTRL_SET_THUMBNAIL          (0x03UL) ///< Configure embedded thumbnail; arg: (uint32_t)&ARM_JPEG_THUMBNAIL or 0 to clear
#define ARM_JPEG_CTRL_SET_CROP               (0x04UL) ///< Configure hardware crop window; arg: (uint32_t)&ARM_JPEG_CROP or 0 to clear
#define ARM_JPEG_CTRL_SET_RESTART_INTERVAL   (0x05UL) ///< Set JPEG restart interval in MCUs; arg: MCU count (0 = none)
#define ARM_JPEG_CTRL_SET_SUBSAMPLING        (0x06UL) ///< Set output chroma subsampling; arg: ARM_JPEG_SUBSAMP_xxx
#define ARM_JPEG_CTRL_SET_COLOR_SPACE        (0x07UL) ///< Set RGB→YCbCr conversion matrix/range; arg: ARM_JPEG_CSC_xxx
#define ARM_JPEG_CTRL_SET_HEADER_MODE        (0x08UL) ///< Select container/header emission; arg: ARM_JPEG_HEADER_xxx
#define ARM_JPEG_CTRL_SET_TARGET_SIZE        (0x09UL) ///< Rate-control target bytes per frame; arg: bytes (0 = quality-only)

/* --- Slice mode (see SLICE_MODE.md) ----------------------------------- */
#define ARM_JPEG_CTRL_SET_SLICE_HEIGHT       (0x0AUL) ///< Slice height in MCU rows (>= 1); 0 disables slice mode
#define ARM_JPEG_CTRL_SET_SLICE_MODEL        (0x0BUL) ///< Pick refill or ring; arg: ARM_JPEG_SLICE_MODEL_xxx
#define ARM_JPEG_CTRL_GET_SLICE_INFO         (0x0CUL) ///< Read metadata for the most recent DEQUEUE; arg: (uint32_t)&ARM_JPEG_SLICE_INFO

/*  Access struct — ARM_DRIVER_JPEG */
typedef struct _ARM_DRIVER_JPEG {
    ARM_DRIVER_VERSION    (*GetVersion)      (void);  ///< Pointer to \ref GetVersion        :
                                                      ///< Get JPEG driver version.
    ARM_JPEG_CAPABILITIES (*GetCapabilities) (void);  ///< Pointer to \ref GetCapabilities   :
                                                      ///< Get JPEG driver capabilities.
    int32_t               (*Initialize)      (ARM_JPEG_SignalEvent_t cb_event);
                                                      ///< Pointer to \ref Initialize : Initialize JPEG Interface
    int32_t               (*Uninitialize)    (void);  ///< Pointer to \ref Uninitialize      :
                                                      ///< De-initialize JPEG Interface.
    int32_t               (*PowerControl)    (ARM_POWER_STATE state);  ///< Pointer to \ref PowerControl  :
                                                                       ///< Control JPEG Interface Power.
    int32_t               (*Start)           (ARM_JPEG_INPUT_BUFFER *in_buf, ARM_JPEG_BUFFER *out_buf);
                                                      ///< Pointer to \ref Start : Start JPEG Interface.
    int32_t               (*Control)         (uint32_t control, uint32_t arg);
                                                      ///< Pointer to \ref Control : Control JPEG Interface.
} const ARM_DRIVER_JPEG;

#ifdef __cplusplus
}
#endif

#endif /* DRIVER_JPEG_H_ */
