/* Copyright (C) 2022-2024 Alif Semiconductor - All Rights Reserved.
 * Use, distribution and modification of this code is permitted under the
 * terms stated in the Alif Semiconductor Software License Agreement
 *
 * You should have received a copy of the Alif Semiconductor Software
 * License Agreement with this file. If not, please write to:
 * contact@alifsemi.com, or visit: https://alifsemi.com/license
 *
 */

/**************************************************************************//**
 * @file     mpu_M55.c
 * @author   Sudhir Sreedharan
 * @email    sudhir@alifsemi.com
 * @version  V1.0.0
 * @date     14-December-2021
 * @brief    MPU Configuration Source
 *
 *           This file load the MPU regions from the table and provides the function to enable it.
 * @bug      None.
 ******************************************************************************/

/* Includes ------------------------------------------------------------------*/

#if defined (M55_HP)
  #include "M55_HP.h"
#elif defined (M55_HE)
  #include "M55_HE.h"
#else
  #error device not specified!
#endif

#include <stdint.h>

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

    static const ARM_MPU_Region_t mpu_table[] __STARTUP_RO_DATA_ATTRIBUTE =
    {
        {   /* SRAM0 - 4MB : RO-0, NP-1, XN-0 */
            .RBAR = ARM_MPU_RBAR(0x02000000, ARM_MPU_SH_NON, 0, 1, 0),
            .RLAR = ARM_MPU_RLAR(0x023FFFFF, MEMATTRIDX_NORMAL_WT_RA_TRANSIENT)
        },
        {   /* SRAM1 - 2.5MB : RO-0, NP-1, XN-0 */
            .RBAR = ARM_MPU_RBAR(0x08000000, ARM_MPU_SH_NON, 0, 1, 0),
            .RLAR = ARM_MPU_RLAR(0x0827FFFF, MEMATTRIDX_NORMAL_WB_RA_WA)
        },
        {   /* Host Peripherals - 16MB : RO-0, NP-1, XN-1 */
            .RBAR = ARM_MPU_RBAR(0x1A000000, ARM_MPU_SH_NON, 0, 1, 1),
            .RLAR = ARM_MPU_RLAR(0x1AFFFFFF, MEMATTRIDX_DEVICE_nGnRE)
        },
#if defined (M55_HP)
        {   /* RTSS HE ITCM - 256K(SRAM4) : RO-0, NP-1, XN-0  */
            .RBAR = ARM_MPU_RBAR(0x58000000, ARM_MPU_SH_OUTER, 0, 1, 0),
            .RLAR = ARM_MPU_RLAR(0x5803FFFF, MEMATTRIDX_NORMAL_WB_RA_WA)
        },
        {   /* RTSS HE DTCM - 256K(SRAM5) : RO-0, NP-1, XN-0  */
            .RBAR = ARM_MPU_RBAR(0x58800000, ARM_MPU_SH_OUTER, 0, 1, 0),
            .RLAR = ARM_MPU_RLAR(0x5883FFFF, MEMATTRIDX_NORMAL_WB_RA_WA)
        },
#elif defined (M55_HE)
        {   /* RTSS HP ITCM - 256K(SRAM2) : RO-0, NP-1, XN-0  */
            .RBAR = ARM_MPU_RBAR(0x50000000, ARM_MPU_SH_OUTER, 0, 1, 0),
            .RLAR = ARM_MPU_RLAR(0x5003FFFF, MEMATTRIDX_NORMAL_WB_RA_WA)
        },
        {   /* RTSS HP DTCM - 1MB(SRAM3) : RO-0, NP-1, XN-0  */
            .RBAR = ARM_MPU_RBAR(0x50800000, ARM_MPU_SH_OUTER, 0, 1, 0),
            .RLAR = ARM_MPU_RLAR(0x508FFFFF, MEMATTRIDX_NORMAL_WB_RA_WA)
        },
#endif
        {   /* MRAM - BL + PRIMARY => RO  */
            .RBAR = ARM_MPU_RBAR(0x80000000, ARM_MPU_SH_NON, 1, 1, 0),
            .RLAR = ARM_MPU_RLAR(0x8001FFDF, MEMATTRIDX_NORMAL_WT_RA)
        },
        {   /* MRAM - PRIMARY TRAILER + SECONDARY => RW  */
            .RBAR = ARM_MPU_RBAR(0x8001FFE0, ARM_MPU_SH_NON, 0, 1, 0),
            .RLAR = ARM_MPU_RLAR(0x8002FFFF, MEMATTRIDX_DEVICE_nGnRE)
        },
        {   /* MRAM - the rest => RO  */
            .RBAR = ARM_MPU_RBAR(0x80030000, ARM_MPU_SH_NON, 1, 1, 0),
            .RLAR = ARM_MPU_RLAR(0x8057FFFF, MEMATTRIDX_NORMAL_WT_RA)
        },
        {   /* OSPI Regs - 16MB : RO-0, NP-1, XN-1  */
            .RBAR = ARM_MPU_RBAR(0x83000000, ARM_MPU_SH_NON, 0, 1, 1),
            .RLAR = ARM_MPU_RLAR(0x83FFFFFF, MEMATTRIDX_DEVICE_nGnRE)
        },
        {   /* OSPI0 XIP(eg:hyperram) - 512MB : RO-0, NP-1, XN-0  */
            .RBAR = ARM_MPU_RBAR(0xA0000000, ARM_MPU_SH_NON, 0, 1, 0),
            .RLAR = ARM_MPU_RLAR(0xBFFFFFFF, MEMATTRIDX_NORMAL_WB_RA_WA)
        },
        {   /* OSPI1 XIP(eg:flash) - 512MB : RO-1, NP-1, XN-0  */
            .RBAR = ARM_MPU_RBAR(0xC0000000, ARM_MPU_SH_NON, 1, 1, 0),
            .RLAR = ARM_MPU_RLAR(0xDFFFFFFF, MEMATTRIDX_NORMAL_WT_RA)
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

