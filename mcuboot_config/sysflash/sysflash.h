/* Copyright (C) 2024 Alif Semiconductor - All Rights Reserved.
 * Use, distribution and modification of this code is permitted under the
 * terms stated in the Alif Semiconductor Software License Agreement
 *
 * You should have received a copy of the Alif Semiconductor Software
 * License Agreement with this file. If not, please write to:
 * contact@alifsemi.com, or visit: https://alifsemi.com/license
 *
 */
#ifndef _SYSFLASH_H_
#define _SYSFLASH_H_

#include <mcuboot_config/mcuboot_config.h>

#define FLASH_DEVICE_MRAM                   0

#define FLASH_SLOT_DOES_NOT_EXIST           255
#define MRAM_SECTOR_SIZE                    1024
#define FLASH_AREA_BOOTLOADER               0
#define FLASH_AREA_IMAGE_0_PRIMARY          1
#define FLASH_AREA_IMAGE_0_SECONDARY        2
#if MCUBOOT_SWAP_USING_SCRATCH
#define FLASH_AREA_IMAGE_SCRATCH            3
#endif
#if MCUBOOT_IMAGE_NUMBER > 1
#define FLASH_AREA_IMAGE_1_PRIMARY          4
#define FLASH_AREA_IMAGE_1_SECONDARY        5
#endif
#if MCUBOOT_IMAGE_NUMBER > 2
#define FLASH_AREA_IMAGE_2_PRIMARY          6
#define FLASH_AREA_IMAGE_2_SECONDARY        7
#endif

#define MRAM_START                          (0x80000000)

#define IMAGE_0_START                       (0x0)
#define IMAGE_1_START                       (0x40000)

#ifndef BOOT_BOOTLOADER_SIZE
#define BOOT_BOOTLOADER_SIZE                (0x10000)
#endif

#ifndef BOOT_SLOT_SIZE
#define BOOT_SLOT_SIZE                      (0x10000)
#endif

#ifndef BOOT_SERAM_SLOT_SIZE
#define BOOT_SERAM_SLOT_SIZE                (0x50000)
#endif

#ifndef BOOT_SCRATCH_SIZE
#define BOOT_SCRATCH_SIZE                   (0x1000)
#endif

#ifdef M55_HE
#define ALIF_SCRATCH_START (MRAM_BASE +\
                IMAGE_0_START +\
                BOOT_BOOTLOADER_SIZE +\
                BOOT_SLOT_SIZE +\
                BOOT_SLOT_SIZE)
#elif defined(M55_HP)
#define ALIF_SCRATCH_START (IMAGE_1_START - BOOT_SCRATCH_SIZE)
#else
#error "No core selected"
#endif

#if MCUBOOT_IMAGE_NUMBER == 1
#define FLASH_AREA_IMAGE_PRIMARY(x)    (((x) == 0) ?          \
                                         FLASH_AREA_IMAGE_0_PRIMARY : \
                                         FLASH_SLOT_DOES_NOT_EXIST)
#define FLASH_AREA_IMAGE_SECONDARY(x)  (((x) == 0) ?          \
                                         FLASH_AREA_IMAGE_0_SECONDARY : \
                                         FLASH_SLOT_DOES_NOT_EXIST)
#elif MCUBOOT_IMAGE_NUMBER == 2
#define FLASH_AREA_IMAGE_PRIMARY(x)    (((x) == 0) ?          \
                                         FLASH_AREA_IMAGE_0_PRIMARY : \
                                         ((x) == 1) ? FLASH_AREA_IMAGE_1_PRIMARY : \
                                         FLASH_SLOT_DOES_NOT_EXIST)
#define FLASH_AREA_IMAGE_SECONDARY(x)  (((x) == 0) ?          \
                                         FLASH_AREA_IMAGE_0_SECONDARY : \
                                         ((x) == 1) ? FLASH_AREA_IMAGE_1_SECONDARY : \
                                         FLASH_SLOT_DOES_NOT_EXIST)
#elif MCUBOOT_IMAGE_NUMBER == 3
#define FLASH_AREA_IMAGE_PRIMARY(x)    (((x) == 0) ?          \
                                         FLASH_AREA_IMAGE_0_PRIMARY : \
                                         ((x) == 1) ? FLASH_AREA_IMAGE_1_PRIMARY : \
                                         ((x) == 2) ? FLASH_AREA_IMAGE_2_PRIMARY : \
                                         FLASH_SLOT_DOES_NOT_EXIST)
#define FLASH_AREA_IMAGE_SECONDARY(x)  (((x) == 0) ?          \
                                         FLASH_AREA_IMAGE_0_SECONDARY : \
                                         ((x) == 1) ? FLASH_AREA_IMAGE_1_SECONDARY : \
                                         ((x) == 2) ? FLASH_AREA_IMAGE_2_SECONDARY : \
                                         FLASH_SLOT_DOES_NOT_EXIST)
#else
#error Unsupported number of images.
#endif
#endif /* _SYSFLASH_H_ */
