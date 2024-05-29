/* Copyright (C) 2024 Alif Semiconductor - All Rights Reserved.
 * Use, distribution and modification of this code is permitted under the
 * terms stated in the Alif Semiconductor Software License Agreement
 *
 * You should have received a copy of the Alif Semiconductor Software
 * License Agreement with this file. If not, please write to:
 * contact@alifsemi.com, or visit: https://alifsemi.com/license
 *
 */

/*
 * Entrypoint for the MCUBoot bootloader.
 * */

#include <bootutil/bootutil.h>
#include "bootutil/bootutil_log.h"
#include "uart_tracelib.h"
#include "fault_handler.h"
#include "pinconf.h"

#include "RTE_Components.h"
#include CMSIS_device_header

#include <stdio.h>

struct arm_vector_table {
    uint32_t msp;
    uint32_t reset;
};

extern void clk_init(void);

// Overwrites the default MPU table from Alif CMSIS-dfp to make own execution area
// read only with normal and cached and make the Application execution area
// writable and device to enable writing to MRAM in update scenario.
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
        {   /* MRAM - Own execution area : RO-1, NP-1, XN-0  */
            .RBAR = ARM_MPU_RBAR(0x80000000, ARM_MPU_SH_NON, 1, 1, 0),
            .RLAR = ARM_MPU_RLAR(0x8000FFFF, MEMATTRIDX_NORMAL_WT_RA)
        },
        {   /* MRAM - Application execution area : RO-0, NP-1, XN-0  */
            .RBAR = ARM_MPU_RBAR(0x80010000, ARM_MPU_SH_NON, 0, 1, 0),
            .RLAR = ARM_MPU_RLAR(0x8057FFFF, MEMATTRIDX_DEVICE_nGnRE)
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

void hw_init(void)
{
    uint32_t config_uart_rx =
			PADCTRL_READ_ENABLE |
			PADCTRL_SCHMITT_TRIGGER_ENABLE |
			PADCTRL_DRIVER_DISABLED_PULL_UP;
    pinconf_set(PORT_1, PIN_0, PINMUX_ALTERNATE_FUNCTION_1, config_uart_rx);	// P1_0: RX  (mux mode 1)
	pinconf_set(PORT_1, PIN_1, PINMUX_ALTERNATE_FUNCTION_1, 0);					// P1_1: TX  (mux mode 1)
}

int main(void)
{
    hw_init();
    tracelib_init(0, 0);
    fault_dump_enable(true);
    clk_init();

    struct arm_vector_table *vt;
    struct boot_rsp rsp;

    int rv = boot_go(&rsp);

    if (rv == 0)
    {
        /* Jump to the starting point of the image */
        vt = (struct arm_vector_table *)(rsp.br_image_off + rsp.br_hdr->ih_hdr_size);
        if ((uint32_t)vt & 0x7FF) {
            printf("\n ERROR: vector table alignment not correct (0x%" PRIx32 ")\n", (uint32_t)vt);
        }
        else {
            // uninitialize stuff?
            // set SP & VTOR as convenience?

            // Don't use PRIu8 for printing as nano spec doesn't support that.
            printf("\nLoading image, version %" PRIu16 ".%" PRIu16 ".%" PRIu16 " (build: %" PRIu32 ")\n", rsp.br_hdr->ih_ver.iv_major, rsp.br_hdr->ih_ver.iv_minor, rsp.br_hdr->ih_ver.iv_revision, rsp.br_hdr->ih_ver.iv_build_num);
            printf("  image size: %" PRIu32 ".\n", rsp.br_hdr->ih_img_size);
            printf("\n");
            ((void (*)(void))vt->reset)();
        }
    }
    else
    {
        printf("\n ERROR: %d \n" , rv);
    }
    while(1);
}
