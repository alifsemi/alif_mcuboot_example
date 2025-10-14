/* Copyright (C) 2022-2024 Alif Semiconductor - All Rights Reserved.
 * Use, distribution and modification of this code is permitted under the
 * terms stated in the Alif Semiconductor Software License Agreement
 *
 * You should have received a copy of the Alif Semiconductor Software
 * License Agreement with this file. If not, please write to:
 * contact@alifsemi.com, or visit: https://alifsemi.com/license
 *
 */

/* Includes ------------------------------------------------------------------*/

#include "RTE_Components.h"
#include CMSIS_device_header

#include <stdint.h>
#include "sysflash/sysflash.h"

void mpu_init(void)
{

}

/* Public functions ----------------------------------------------------------*/
/**
 * @brief  Load the MPU regions from the given table
 * @note   This function loads the region and also sets the
 *         attributes for the regions.
 *         Set this function as weak, so that User can
 *         override from application.
 * @param  None
 * @retval None
 */
void MPU_Load_Regions(void)
{

/* Define the memory attribute index with the below properties */
#define MEMATTRIDX_NORMAL_WT_RA_TRANSIENT    0
#define MEMATTRIDX_DEVICE_nGnRE              1
#define MEMATTRIDX_NORMAL_WB_RA_WA           2
#define MEMATTRIDX_NORMAL_WT_RA              3

    static const ARM_MPU_Region_t mpu_table[] =
    {
        {   /* Host Peripherals - 16MB : RO-0, NP-1, XN-1 */
            .RBAR = ARM_MPU_RBAR(0x1A000000, ARM_MPU_SH_NON, 0, 1, 1),
            .RLAR = ARM_MPU_RLAR(0x1AFFFFFF, MEMATTRIDX_DEVICE_nGnRE)
        },
#if defined (M55_HE)
        {   /* RTSS HE ITCM - 256K(SRAM4) : RO-0, NP-1, XN-0  */
            .RBAR = ARM_MPU_RBAR(0x58000000, ARM_MPU_SH_OUTER, 0, 1, 0),
            .RLAR = ARM_MPU_RLAR(0x5803FFFF, MEMATTRIDX_NORMAL_WB_RA_WA)
        },
        {   /* RTSS HE DTCM - 256K(SRAM5) : RO-0, NP-1, XN-0  */
            .RBAR = ARM_MPU_RBAR(0x58800000, ARM_MPU_SH_OUTER, 0, 1, 0),
            .RLAR = ARM_MPU_RLAR(0x5883FFFF, MEMATTRIDX_NORMAL_WB_RA_WA)
        },
#elif defined (M55_HP)
        {   /* RTSS HP ITCM - 256K(SRAM2) : RO-0, NP-1, XN-0  */
            .RBAR = ARM_MPU_RBAR(0x50000000, ARM_MPU_SH_OUTER, 0, 1, 0),
            .RLAR = ARM_MPU_RLAR(0x5003FFFF, MEMATTRIDX_NORMAL_WB_RA_WA)
        },
        {   /* RTSS HP DTCM - 1MB(SRAM3) : RO-0, NP-1, XN-0  */
            .RBAR = ARM_MPU_RBAR(0x50800000, ARM_MPU_SH_OUTER, 0, 1, 0),
            .RLAR = ARM_MPU_RLAR(0x508FFFFF, MEMATTRIDX_NORMAL_WB_RA_WA)
        },
#endif
        {   /* MRAM => earlier stuff  */
            .RBAR = ARM_MPU_RBAR(MRAM_START, ARM_MPU_SH_NON, 1, 1, 1),
            .RLAR = ARM_MPU_RLAR(MRAM_START + IMAGE_1_START + BOOT_BOOTLOADER_SIZE - 1, MEMATTRIDX_NORMAL_WT_RA)
        },
        {   /* MRAM => own execution area  */
            .RBAR = ARM_MPU_RBAR(MRAM_START + IMAGE_1_START + BOOT_BOOTLOADER_SIZE, ARM_MPU_SH_NON, 1, 1, 0),
            .RLAR = ARM_MPU_RLAR(MRAM_START + IMAGE_1_START + BOOT_BOOTLOADER_SIZE + BOOT_SLOT_SIZE - 0x21, MEMATTRIDX_NORMAL_WT_RA)
        },
        {   /* MRAM => primary trailer (for confirming the update) */
            .RBAR = ARM_MPU_RBAR(MRAM_START + IMAGE_1_START + BOOT_BOOTLOADER_SIZE + BOOT_SLOT_SIZE - 0x20, ARM_MPU_SH_NON, 0, 1, 0),
            .RLAR = ARM_MPU_RLAR(MRAM_START + IMAGE_1_START + BOOT_BOOTLOADER_SIZE + BOOT_SLOT_SIZE - 1, MEMATTRIDX_DEVICE_nGnRE)
        },
        {   /* MRAM => rest of MRAM */
            .RBAR = ARM_MPU_RBAR(MRAM_START + IMAGE_1_START + BOOT_BOOTLOADER_SIZE + BOOT_SLOT_SIZE, ARM_MPU_SH_NON, 1, 1, 1),
            .RLAR = ARM_MPU_RLAR(MRAM_START + SOC_FEAT_MRAM_SIZE - 1, MEMATTRIDX_NORMAL_WT_RA)
        },
    };

    /* Mem Attribute for 0th index */
    ARM_MPU_SetMemAttr(MEMATTRIDX_NORMAL_WT_RA_TRANSIENT, ARM_MPU_ATTR(
                                         /* NT=0, WB=0, RA=1, WA=0 */
                                         ARM_MPU_ATTR_MEMORY_(0,0,1,0),
                                         ARM_MPU_ATTR_MEMORY_(0,0,1,0)));

    /* Mem Attribute for 1st index */
    ARM_MPU_SetMemAttr(MEMATTRIDX_DEVICE_nGnRE, ARM_MPU_ATTR(
                                         /* Device Memory */
                                         ARM_MPU_ATTR_DEVICE,
                                         ARM_MPU_ATTR_DEVICE_nGnRE));

    /* Mem Attribute for 2nd index */
    ARM_MPU_SetMemAttr(MEMATTRIDX_NORMAL_WB_RA_WA, ARM_MPU_ATTR(
                                         /* NT=1, WB=1, RA=1, WA=1 */
                                         ARM_MPU_ATTR_MEMORY_(1,1,1,1),
                                         ARM_MPU_ATTR_MEMORY_(1,1,1,1)));

    /* Mem Attribute for 3th index */
    ARM_MPU_SetMemAttr(MEMATTRIDX_NORMAL_WT_RA, ARM_MPU_ATTR(
                                         /* NT=1, WB=0, RA=1, WA=0 */
                                         ARM_MPU_ATTR_MEMORY_(1,0,1,0),
                                         ARM_MPU_ATTR_MEMORY_(1,0,1,0)));

    /* Load the regions from the table */
    ARM_MPU_Load(0, mpu_table, sizeof(mpu_table)/sizeof(ARM_MPU_Region_t));
}
