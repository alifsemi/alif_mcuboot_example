/* Copyright (C) 2024 Alif Semiconductor - All Rights Reserved.
 * Use, distribution and modification of this code is permitted under the
 * terms stated in the Alif Semiconductor Software License Agreement
 *
 * You should have received a copy of the Alif Semiconductor Software
 * License Agreement with this file. If not, please write to:
 * contact@alifsemi.com, or visit: https://alifsemi.com/license
 *
 */

#include <bootutil/bootutil.h>
#include <sysflash/sysflash.h>

#include "RTE_Components.h"
#include CMSIS_device_header

#define BOOTLOADER_START_ADDR           (MRAM_BASE + BOOTLOADER_START_ADDRESS)
#define BOOTLOADER_END_ADDR             (BOOTLOADER_START_ADDR + \
                                         BOOT_BOOTLOADER_SIZE - 1)

#define APP_IMAGE_SLOTS_START_ADDR      (BOOTLOADER_START_ADDR + BOOT_BOOTLOADER_SIZE)
#define APP_IMAGE_SLOTS_END_ADDR        (APP_IMAGE_SLOTS_START_ADDR + \
                                         BOOT_PRIMARY_1_SIZE + \
                                         BOOT_SECONDARY_1_SIZE + \
                                         BOOT_SCRATCH_SIZE - 1)

struct arm_vector_table {
    uint32_t msp;
    uint32_t reset;
};

/*
 * Override the weak implementation of MPU_Load_Regions and setup the
 * memory regions accessed by the bootloader and their attributes.
 */
void MPU_Load_Regions(void)
{
#define MEMATTRIDX_DEVICE_nGnRE              0
#define MEMATTRIDX_NORMAL_WT_RA              1

    static const ARM_MPU_Region_t mpu_table[] __STARTUP_RO_DATA_ATTRIBUTE =
    {
        {   /* Host Peripherals - 16MB : RO-0, NP-1, XN-1 */
            .RBAR = ARM_MPU_RBAR(0x1A000000, ARM_MPU_SH_NON, 0, 1, 1),
            .RLAR = ARM_MPU_RLAR(0x1AFFFFFF, MEMATTRIDX_DEVICE_nGnRE)
        },
        {   /* MRAM region 1 (bootloader) : RO-1, NP-1, XN-0  */
            .RBAR = ARM_MPU_RBAR(BOOTLOADER_START_ADDR, ARM_MPU_SH_NON, 1, 1, 0),
            .RLAR = ARM_MPU_RLAR(BOOTLOADER_END_ADDR, MEMATTRIDX_NORMAL_WT_RA)
        },
        {   /* MRAM region 2 (Application image slots and scratch area) : RO-0, NP-1, XN-0 */
            .RBAR = ARM_MPU_RBAR(APP_IMAGE_SLOTS_START_ADDR, ARM_MPU_SH_NON, 0, 1, 0),
            .RLAR = ARM_MPU_RLAR(APP_IMAGE_SLOTS_END_ADDR, MEMATTRIDX_DEVICE_nGnRE)
        },
    };

    /* Mem Attribute for 0th index */
    ARM_MPU_SetMemAttr(MEMATTRIDX_DEVICE_nGnRE, ARM_MPU_ATTR(
                                         /* Device Memory */
                                         ARM_MPU_ATTR_DEVICE,
                                         ARM_MPU_ATTR_DEVICE_nGnRE));

    /* Mem Attribute for 1st index */
    ARM_MPU_SetMemAttr(MEMATTRIDX_NORMAL_WT_RA, ARM_MPU_ATTR(
                                         /* NT=1, WB=0, RA=1, WA=0 */
                                         ARM_MPU_ATTR_MEMORY_(1,0,1,0),
                                         ARM_MPU_ATTR_MEMORY_(1,0,1,0)));

    /* Load the regions from the table */
    ARM_MPU_Load(0, mpu_table, sizeof(mpu_table)/sizeof(ARM_MPU_Region_t));
}

int main(void)
{
    struct arm_vector_table *vt;
    struct boot_rsp rsp;

    int rv = boot_go(&rsp);

    if (rv == 0)
    {
        /* Jump to the starting point of the image */
        vt = (struct arm_vector_table *)(rsp.br_image_off + rsp.br_hdr->ih_hdr_size);
        ((void (*)(void))vt->reset)();
    }
    while(1);
}
