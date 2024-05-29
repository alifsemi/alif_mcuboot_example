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

#define FLASH_AREA_BOOTLOADER               0
#define FLASH_AREA_IMAGE_0_PRIMARY          1
#define FLASH_AREA_IMAGE_0_SECONDARY        2
#define FLASH_AREA_IMAGE_SCRATCH            3

#ifndef BOOTLOADER_START_ADDRESS
#define BOOTLOADER_START_ADDRESS            (0x0)
#endif

#ifndef BOOT_BOOTLOADER_SIZE
#define BOOT_BOOTLOADER_SIZE                (0x10000)
#endif

#ifndef BOOT_PRIMARY_1_SIZE
#define BOOT_PRIMARY_1_SIZE                 (0x10000)
#endif

#ifndef BOOT_SECONDARY_1_SIZE
#define BOOT_SECONDARY_1_SIZE               (0x10000)
#endif

#ifndef BOOT_SCRATCH_SIZE
#define BOOT_SCRATCH_SIZE                   (0x1000)
#endif


#define FLASH_AREA_IMAGE_PRIMARY(x)    (((x) == 0) ?          \
                                         FLASH_AREA_IMAGE_0_PRIMARY : \
                                         FLASH_SLOT_DOES_NOT_EXIST)
#define FLASH_AREA_IMAGE_SECONDARY(x)  (((x) == 0) ?          \
                                         FLASH_AREA_IMAGE_0_SECONDARY : \
                                         FLASH_SLOT_DOES_NOT_EXIST)
#endif /* _SYSFLASH_H_ */
