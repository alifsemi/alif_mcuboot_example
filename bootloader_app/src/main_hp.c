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
#include "sysflash/sysflash.h"
#include "uart_tracelib.h"
#include "fault_handler.h"
#include "pinconf.h"
#include "Driver_HWSEM.h"

#include "RTE_Components.h"
#include CMSIS_device_header

#include <stdio.h>

struct arm_vector_table {
    uint32_t msp;
    uint32_t reset;
};

extern void clk_init(void);
extern void flush_uart(void);

#if HE_UPDATES_BOTH
extern ARM_DRIVER_HWSEM ARM_Driver_HWSEM_(0);
static ARM_DRIVER_HWSEM* hwsem = &ARM_Driver_HWSEM_(0);
#endif

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
        {   /* Host Peripherals - 16MB : RO-0, NP-1, XN-1 */
            .RBAR = ARM_MPU_RBAR(0x1A000000, ARM_MPU_SH_NON, 0, 1, 1),
            .RLAR = ARM_MPU_RLAR(0x1AFFFFFF, MEMATTRIDX_DEVICE_nGnRE)
        },
        {   /* MRAM - earlier stuff : RO-1, NP-1, XN-1  */
            .RBAR = ARM_MPU_RBAR(MRAM_START, ARM_MPU_SH_NON, 1, 1, 1),
            .RLAR = ARM_MPU_RLAR(MRAM_START + IMAGE_1_START - BOOT_SCRATCH_SIZE - 1, MEMATTRIDX_NORMAL_WT_RA)
        },
        {   /* MRAM - scratch : RO-0, NP-1, XN-0  */
            .RBAR = ARM_MPU_RBAR(MRAM_START + IMAGE_1_START - BOOT_SCRATCH_SIZE, ARM_MPU_SH_NON, 0, 1, 1),
            .RLAR = ARM_MPU_RLAR(MRAM_START + IMAGE_1_START - 1, MEMATTRIDX_DEVICE_nGnRE)
        },
        {   /* MRAM - Own execution area : RO-1, NP-1, XN-0  */
            .RBAR = ARM_MPU_RBAR(MRAM_START + IMAGE_1_START, ARM_MPU_SH_NON, 1, 1, 0),
            .RLAR = ARM_MPU_RLAR(MRAM_START + IMAGE_1_START + BOOT_BOOTLOADER_SIZE - 1, MEMATTRIDX_NORMAL_WT_RA)
        },
        {   /* MRAM - Application execution area + candidate slot : RO-0, NP-1, XN-0  */
            .RBAR = ARM_MPU_RBAR(MRAM_START + IMAGE_1_START + BOOT_BOOTLOADER_SIZE, ARM_MPU_SH_NON, 0, 1, 0),
            .RLAR = ARM_MPU_RLAR(MRAM_START + MRAM_SIZE - 1, MEMATTRIDX_DEVICE_nGnRE)
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

    pinconf_set(PORT_12, PIN_1, PINMUX_ALTERNATE_FUNCTION_2, config_uart_rx); // P12_1: RX  (mux mode 2)
    pinconf_set(PORT_12, PIN_2, PINMUX_ALTERNATE_FUNCTION_2, 0);              // P12_2: TX  (mux mode 2)

#if HE_UPDATES_BOTH
    hwsem->Initialize(NULL);
#endif
}

void hw_uninit()
{
#if HE_UPDATES_BOTH
    hwsem->Uninitialize();
#endif

    // configure UART4
    uint32_t config_default =
            PADCTRL_OUTPUT_DRIVE_STRENGTH_4MA |
            PADCTRL_SCHMITT_TRIGGER_ENABLE;
    pinconf_set(PORT_12, PIN_1, PINMUX_ALTERNATE_FUNCTION_0, config_default);
    pinconf_set(PORT_12, PIN_2, PINMUX_ALTERNATE_FUNCTION_0, config_default);
}

void uninit()
{
    fault_dump_enable(false);
    flush_uart();

    // hack to wait for the final characters to be transmitted via uart before unitializing the driver
    extern void wait_for_uart_empty(void);
    wait_for_uart_empty();

    tracelib_uninit();
    hw_uninit();
}

#if HE_UPDATES_BOTH
static const struct flash_area *_fa_p;
static struct image_header _hdr = { 0 };
#define BOOT_TMPBUF_SZ  256
static uint8_t tmpbuf[BOOT_TMPBUF_SZ];

int just_go(struct boot_rsp* rsp)
{
    hwsem->Lock();

    int rc = -1;

    rc = flash_area_open(FLASH_AREA_IMAGE_PRIMARY(1), &_fa_p);
    assert(rc == 0);

    rc = boot_image_load_header(_fa_p, &_hdr);
    if (rc != 0) {
        goto returning;
    }

    rc = bootutil_img_validate(NULL, 0, &_hdr, _fa_p, tmpbuf, BOOT_TMPBUF_SZ, NULL, 0, NULL);

    rsp->br_flash_dev_id = flash_area_get_device_id(_fa_p);
    rsp->br_image_off = flash_area_get_off(_fa_p);
    rsp->br_hdr = &_hdr;

returning:
    flash_area_close(_fa_p);
    hwsem->Unlock();
    return rc;
}
#endif // #if HE_UPDATES_BOTH

int main(void)
{
    hw_init();
    tracelib_init(0, 0);
    fault_dump_enable(true);
    clk_init();
    sys_busy_loop_init();

    struct arm_vector_table *vt;
    struct boot_rsp rsp;

#if HE_UPDATES_BOTH
    // HE will update us
    int rv = just_go(&rsp);
#else
    // both cores update themselves
    int rv = boot_go_for_image_id(&rsp, 1);
#endif // #if HE_UPDATES_BOTH

    if (rv == 0)
    {
        /* Jump to the starting point of the image */
        if (rsp.br_hdr->ih_load_addr) {
            // RAM LOAD build
            vt = (struct arm_vector_table *)(rsp.br_hdr->ih_load_addr + rsp.br_hdr->ih_hdr_size);
        }
        else {
            // XIP from slot
            vt = (struct arm_vector_table *)(rsp.br_image_off + rsp.br_hdr->ih_hdr_size);
        }

        if ((uint32_t)vt & 0x7FF) {
            printf("\n ERROR: vector table alignment not correct (0x%" PRIx32 ")\n", (uint32_t)vt);
        }
        else {
            // Don't use PRIu8 for printing as nano spec doesn't support that.
            printf("\nLoading image, version %" PRIu16 ".%" PRIu16 ".%" PRIu16 " (build: %" PRIu32 ")\n", rsp.br_hdr->ih_ver.iv_major, rsp.br_hdr->ih_ver.iv_minor, rsp.br_hdr->ih_ver.iv_revision, rsp.br_hdr->ih_ver.iv_build_num);
            printf("  image size: %" PRIu32 ".\n", rsp.br_hdr->ih_img_size);
            printf("  VT: 0x%08" PRIX32 ", RH: 0x%08" PRIX32 "\n", (uint32_t)vt, (uint32_t)(vt->reset));
            printf("\n");
            uninit();

            // set vector table to application side
            SCB->VTOR = (uint32_t)vt;

            // reset MSPLIM, set MSP from app vector table and jump to app
            __asm(
                "MOV  R0, #0                \n\t"
                "MSR  MSPLIM, R0            \n\t"
                "MSR  MSP, %[stack_pointer] \n\t"
                "BX   %[reset_handler]          " : : [stack_pointer] "r"(vt->msp), [reset_handler] "r"(vt->reset)
            );
        }
    }
    else
    {
        printf("\n ERROR: %d \n" , rv);
    }
    while(1) __WFE();
}

int boot_read_swap_state_primary_slot_hook(int image_index, struct boot_swap_state *state)
{
    (void)image_index;
    (void)state;
    return BOOT_HOOK_REGULAR;
}

int boot_read_image_header_hook(int img_index, int slot, struct image_header *img_head)
{
    (void)img_index;
    (void)slot;
    (void)img_head;
    return BOOT_HOOK_REGULAR;
}

int boot_copy_region_post_hook(int img_index, const struct flash_area *area, size_t size)
{
    (void)img_index;
    (void)area;
    (void)size;
    return 0;
}

fih_ret boot_image_check_hook(int img_index, int slot)
{
    (void)img_index;
    (void)slot;
    return BOOT_HOOK_REGULAR;
}

int boot_perform_update_hook(int img_index, struct image_header *img_head, const struct flash_area *area)
{
    (void)img_index;
    (void)img_head;
    (void)area;
    return BOOT_HOOK_REGULAR;
}
