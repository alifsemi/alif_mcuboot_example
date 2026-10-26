/*
 * Copyright (c) 2026 Alif Semiconductor
 * SPDX-License-Identifier: Apache-2.0
 *
 * OS agnostic VeriSilicon Hantro VC9000E JPEG Hardware Encoder Driver
 */

#ifndef VIDEO_JPEG_HANTRO_VC9000E_SW_H_
#define VIDEO_JPEG_HANTRO_VC9000E_SW_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Library version */
#define JPEG_SW_LIB_VERSION_MAJOR    1
#define JPEG_SW_LIB_VERSION_MINOR    0
#define JPEG_SW_LIB_VERSION_PATCH    0
#define JPEG_SW_LIB_VERSION_STRING   "1.0.0"

/* Packed version for compile-time comparison:
 *   #if JPEG_SW_LIB_VERSION_CODE >= JPEG_SW_LIB_MAKE_VERSION(1, 0, 0)
 */
#define JPEG_SW_LIB_MAKE_VERSION(major, minor, patch) \
	(((major) << 16) | ((minor) << 8) | (patch))
#define JPEG_SW_LIB_VERSION_CODE \
	JPEG_SW_LIB_MAKE_VERSION(JPEG_SW_LIB_VERSION_MAJOR, \
				    JPEG_SW_LIB_VERSION_MINOR, \
				    JPEG_SW_LIB_VERSION_PATCH)

const char *jpeg_sw_lib_get_version(void);

int jpeg_qf_scaling(int quality);
void jpeg_calc_q_table(int scale_factor);
void jpeg_set_q_table(uint32_t base);

struct jpeg_header_info {
	uint8_t *buffer;
	uint32_t width;
	uint32_t height;
	uint8_t num_components;
};

int jpeg_header_generation(struct jpeg_header_info header);

#ifdef __cplusplus
}
#endif

#endif /* VIDEO_JPEG_HANTRO_VC9000E_SW_H_ */
