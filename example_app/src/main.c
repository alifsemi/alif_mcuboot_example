/* Copyright (C) 2024 Alif Semiconductor - All Rights Reserved.
 * Use, distribution and modification of this code is permitted under the
 * terms stated in the Alif Semiconductor Software License Agreement
 *
 * You should have received a copy of the Alif Semiconductor Software
 * License Agreement with this file. If not, please write to:
 * contact@alifsemi.com, or visit: https://alifsemi.com/license
 *
 */

#include "uart_tracelib.h"
#include "fault_handler.h"
#include "system_utils.h"
#include <stdio.h>

#include "hw.h"
#include "update.h"
#include "mcumgr_handler.h"


extern void clk_init(void);
extern void flush_uart(void);

extern void mpu_init(void);

static volatile uint8_t button_pressed = 0;
static uint8_t update_available = 0;
static uint8_t test_boot = 0;

static void button_callback(uint32_t event)
{
    (void)event;
    if(!button_pressed)
        button_pressed = 1;
}

static void do_button_pressed(void)
{
    if(button_pressed) {

        if(test_boot) {
            confirm_update();
        }
        else if(update_available) {
            set_pending();
        }
        button_pressed = 0;
    }
}

int main(void)
{
    mpu_init(); // pull mpu in
    hw_init();
    tracelib_init(0, 0);
    fault_dump_enable(true);
    clk_init();
    sys_busy_loop_init();

    int32_t ret = mcumgr_handler_init();
    if(ret) {
        printf("Failed to initialize mcumgr: %" PRId32 "\n", ret);
        while(1);
    }

#ifdef EXAMPLE_APP_UPDATE_TARGET
    printf("Updated app running!\n");
#else
    printf("Example app running!\n");
#endif

    read_image_state(&test_boot, &update_available);

    if(test_boot) {
        printf("Confirm update by pressing button or reboot the device to revert.\n");
    }
    else if(update_available) {
        printf("Press button to pend update.\n");
    }
    else {
        printf("No update available.\n");
    }
    led_button_init(button_callback);

    uint32_t start = S32K_CNTRead->CNTCVL;

    while(1)
    {
        if(S32K_CNTRead->CNTCVL - start > 32768) {
            led_toggle();
            start = S32K_CNTRead->CNTCVL;
        }

        do_button_pressed();
        mcumgr_handler_process();
        flush_uart();
    }
}
