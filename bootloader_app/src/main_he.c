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
#include "services_lib_bare_metal.h"

#include "RTE_Components.h"
#include CMSIS_device_header

#include <stdio.h>

#include "mhu_driver.h"

#define SHUTDOWN_MESSAGE 0xDEADBEEF

static volatile bool msg_acked = false;
static volatile bool hp_updated = false;

static uint32_t sender_addresses[] =
{
    MHU_RTSS_S_TX_BASE,
    MHU_SESS_S_TX_BASE,
};

static uint32_t receiver_addresses[] =
{
    MHU_RTSS_S_RX_BASE,
    MHU_SESS_S_RX_BASE,
};

static void msg_acked_callback(uint32_t sender_id, uint32_t channel_number)
{
    if(sender_id == 0) {
        msg_acked = true;
    }
    else {
        SERVICES_send_msg_acked_callback(sender_id, channel_number);
    }
}

static void message_received_callback(uint32_t receiver_id, uint32_t channel_number, uint32_t service_data)
{
    if(receiver_id == 1) {
        SERVICES_rx_msg_callback(receiver_id, channel_number, service_data);
    }
}

static mhu_driver_in_t  mhu_driver_in = {
    .sender_base_address_list = sender_addresses,
    .receiver_base_address_list = receiver_addresses,
    .mhu_count = sizeof(sender_addresses) / sizeof(uint32_t),
    .send_msg_acked_callback = msg_acked_callback,
    .rx_msg_callback = message_received_callback,
    .debug_print = 0
};
static mhu_driver_out_t mhu_driver_out;

static uint32_t se_services_s_handle;

void MHU_RTSS_S_TX_IRQHandler(void)
{
    mhu_driver_out.sender_irq_handler(0);
}

void MHU_RTSS_S_RX_IRQHandler(void)
{
    mhu_driver_out.receiver_irq_handler(0);
}

void MHU_SESS_S_TX_IRQHandler(void)
{
    mhu_driver_out.sender_irq_handler(1);
}

void MHU_SESS_S_RX_IRQHandler(void)
{
    mhu_driver_out.receiver_irq_handler(1);
}

static uint8_t
  se_services_packet_buffer[SERVICES_MAX_PACKET_BUFFER_SIZE] __attribute__ ((aligned (4)));

static int32_t se_services_wait_ms(uint32_t wait_time_ms)
{
    for(uint32_t count = 0; count < wait_time_ms; count++) {
        sys_busy_loop_us(1000);
    }

    return 0;
}

static int se_services_print(const char * fmt, ...)
{
  (void)fmt;
  return 0;
}

static services_lib_t  services_init_params = {
    .packet_buffer_address = (uint32_t)se_services_packet_buffer,
    .fn_send_mhu_message   = 0,
    .fn_wait_ms            = &se_services_wait_ms,
    .wait_timeout          = 0x01000000,
    .fn_print_msg          = &se_services_print,
};

#if HE_UPDATES_BOTH
extern ARM_DRIVER_HWSEM ARM_Driver_HWSEM_(0);
static ARM_DRIVER_HWSEM* hwsem = &ARM_Driver_HWSEM_(0);
#endif

static run_profile_t runp = {
    .power_domains = PD_VBAT_AON_MASK | PD_SSE700_AON_MASK | PD_SYST_MASK | PD_DBSS_MASK | PD_SESS_MASK,
	.dcdc_voltage = DCDC_VOUT_0825,
	.dcdc_mode = DCDC_MODE_PWM,
	.aon_clk_src = CLK_SRC_LFXO,
	.run_clk_src = CLK_SRC_PLL,
	.cpu_clk_freq = CLOCK_FREQUENCY_160MHZ,
	.scaled_clk_freq = SCALED_FREQ_XO_HIGH_DIV_38_4_MHZ,
	.memory_blocks = MRAM_MASK | SERAM_MASK | SRAM0_MASK,
	.ip_clock_gating = NPU_HP_MASK | NPU_HE_MASK | OSPI_1_MASK | CANFD_MASK | USB_MASK | CDC200_MASK | CAMERA_MASK | MIPI_DSI_MASK | MIPI_CSI_MASK | LP_PERIPH_MASK,
	.phy_pwr_gating = LDO_PHY_MASK | USB_PHY_MASK | MIPI_TX_DPHY_MASK | MIPI_RX_DPHY_MASK | MIPI_PLL_DPHY_MASK,
	.vdd_ioflex_3V3 = IOFLEX_LEVEL_1V8,
};

struct arm_vector_table {
    uint32_t msp;
    uint32_t reset;
};

extern void clk_init(void);
extern void flush_uart(void);


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
        {   /* MRAM - Own execution area : RO-1, NP-1, XN-0  */
            .RBAR = ARM_MPU_RBAR(MRAM_START, ARM_MPU_SH_NON, 1, 1, 0),
            .RLAR = ARM_MPU_RLAR(MRAM_START + BOOT_BOOTLOADER_SIZE - 1, MEMATTRIDX_NORMAL_WT_RA)
        },
        {   /* MRAM - Application execution area + candidate + scratch : RO-0, NP-1, XN-0  */
            .RBAR = ARM_MPU_RBAR(MRAM_START + BOOT_BOOTLOADER_SIZE, ARM_MPU_SH_NON, 0, 1, 0),
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
    // configure UART2 for logging
    pinconf_set(PORT_1, PIN_0, PINMUX_ALTERNATE_FUNCTION_1, config_uart_rx);  // P1_0:  RX  (mux mode 1)
    pinconf_set(PORT_1, PIN_1, PINMUX_ALTERNATE_FUNCTION_1, 0);               // P1_1:  TX  (mux mode 1)

#if HE_UPDATES_BOTH
    hwsem->Initialize(NULL);
#endif
}

void hw_uninit()
{
#if HE_UPDATES_BOTH
    hwsem->Uninitialize();
#endif

    uint32_t config_default =
            PADCTRL_OUTPUT_DRIVE_STRENGTH_4MA |
            PADCTRL_SCHMITT_TRIGGER_ENABLE;
    pinconf_set(PORT_1, PIN_0, PINMUX_ALTERNATE_FUNCTION_0, config_default);
    pinconf_set(PORT_1, PIN_1, PINMUX_ALTERNATE_FUNCTION_0, config_default);
}

void uninit()
{
#if HE_UPDATES_BOTH
    NVIC_DisableIRQ(MHU_RTSS_S_RX_IRQ_IRQn);
    NVIC_ClearPendingIRQ(MHU_RTSS_S_RX_IRQ_IRQn);
    NVIC_DisableIRQ(MHU_RTSS_S_TX_IRQ_IRQn);
    NVIC_ClearPendingIRQ(MHU_RTSS_S_TX_IRQ_IRQn);
#endif
    NVIC_DisableIRQ(MHU_SESS_S_RX_IRQ_IRQn);
    NVIC_ClearPendingIRQ(MHU_SESS_S_RX_IRQ_IRQn);
    NVIC_DisableIRQ(MHU_SESS_S_TX_IRQ_IRQn);
    NVIC_ClearPendingIRQ(MHU_SESS_S_TX_IRQ_IRQn);
    fault_dump_enable(false);
    flush_uart();

    // wait for the final characters to be transmitted via uart before unitializing the driver
    extern void wait_for_uart_empty(void);
    wait_for_uart_empty();
    
    tracelib_uninit();
    hw_uninit();
}

int main(void)
{
    hw_init();
    tracelib_init(0, 0);
    fault_dump_enable(true);
    clk_init();
    sys_busy_loop_init();

    se_services_s_handle = SERVICES_register_channel(1, 0);
    MHU_driver_initialize(&mhu_driver_in, &mhu_driver_out);

#if HE_UPDATES_BOTH
    NVIC_DisableIRQ(MHU_RTSS_S_RX_IRQ_IRQn);
    NVIC_ClearPendingIRQ(MHU_RTSS_S_RX_IRQ_IRQn);
    NVIC_SetPriority(MHU_RTSS_S_RX_IRQ_IRQn, 10);
    NVIC_EnableIRQ(MHU_RTSS_S_RX_IRQ_IRQn);

    NVIC_DisableIRQ(MHU_RTSS_S_TX_IRQ_IRQn);
    NVIC_ClearPendingIRQ(MHU_RTSS_S_TX_IRQ_IRQn);
    NVIC_SetPriority(MHU_RTSS_S_TX_IRQ_IRQn, 10);
    NVIC_EnableIRQ(MHU_RTSS_S_TX_IRQ_IRQn);
#endif

    NVIC_DisableIRQ(MHU_SESS_S_RX_IRQ_IRQn);
    NVIC_ClearPendingIRQ(MHU_SESS_S_RX_IRQ_IRQn);
    NVIC_SetPriority(MHU_SESS_S_RX_IRQ_IRQn, 10);
    NVIC_EnableIRQ(MHU_SESS_S_RX_IRQ_IRQn);

    NVIC_DisableIRQ(MHU_SESS_S_TX_IRQ_IRQn);
    NVIC_ClearPendingIRQ(MHU_SESS_S_TX_IRQ_IRQn);
    NVIC_SetPriority(MHU_SESS_S_TX_IRQ_IRQn, 10);
    NVIC_EnableIRQ(MHU_SESS_S_TX_IRQ_IRQn);

    services_init_params.fn_send_mhu_message = mhu_driver_out.send_message;

    SERVICES_initialize(&services_init_params);
    SERVICES_synchronize_with_se(se_services_s_handle);
    uint32_t err;
    uint32_t er = SERVICES_set_run_cfg(se_services_s_handle, &runp, &err);
    if(er || err) {
        printf("SERVICES_set_run_cfg %" PRIu32 "    %" PRIu32 "\n", er, err);
        while(1) __WFE();
    }

    struct arm_vector_table *vt;
    struct boot_rsp rsp;

#if HE_UPDATES_BOTH
    // this core is the single updater so run update for all images
    int rv = boot_go(&rsp);
#else
    // both cores handle themselves
    int rv = boot_go_for_image_id(&rsp, 0);
#endif

    if (rv == 0)
    {
#if HE_UPDATES_BOTH
        if(hp_updated) {
            er = SERVICES_boot_reset_cpu(se_services_s_handle, EXTSYS_0, &err);
            er = SERVICES_boot_process_toc_entry(se_services_s_handle, (unsigned char*)"HP_BL", &err);
        }

        hwsem->Unlock();
#endif
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
#if HE_UPDATES_BOTH
    printf("boot_perform_update_hook: %d\n", img_index);
    if(img_index != 0) {
        // updating HP image.
        hwsem->Lock();
        msg_acked = false;
        mhu_driver_out.send_message(0, 0, SHUTDOWN_MESSAGE);
        while(!msg_acked) __WFE();
        sys_busy_loop_us(31); // minimum delay
        hp_updated = true;
    }
#endif

    return BOOT_HOOK_REGULAR;
}
