/* Copyright (C) 2024 Alif Semiconductor - All Rights Reserved.
 * Use, distribution and modification of this code is permitted under the
 * terms stated in the Alif Semiconductor Software License Agreement
 *
 * You should have received a copy of the Alif Semiconductor Software
 * License Agreement with this file. If not, please write to:
 * contact@alifsemi.com, or visit: https://alifsemi.com/license
 *
 */
#include "pinconf.h"
#include "Driver_GPIO.h"
#include "RTE_Components.h"
#include CMSIS_device_header
#include "uart_tracelib.h"
#include "fault_handler.h"
#include "bootutil/bootutil_public.h"
#include <stdio.h>
#include "pm.h"

extern void mpu_init(void);
extern void clk_init(void);
extern void flush_uart(void);

#define LED_PORT 6

#define LED_PIN_ORIGINAL 2
#define LED_PIN_UPDATED  6

#ifdef BLINKY_APP_UPDATE_TARGET
#define LED_PIN   LED_PIN_UPDATED
#else
#define LED_PIN   LED_PIN_ORIGINAL
#endif

#define BUTTON_PIN PIN_4

extern ARM_DRIVER_GPIO ARM_Driver_GPIO_(LED_PORT);
static ARM_DRIVER_GPIO* Led = &ARM_Driver_GPIO_(LED_PORT);

// never updated on !HE_UPDATES_BOTH...
static volatile bool running = true;

#if HE_UPDATES_BOTH
// HE acts as single updater, so HP must go down when requested.

#include "mhu_driver.h"

#define SHUTDOWN_MESSAGE 0xDEADBEEF

static uint32_t sender_addresses[] =
{
    MHU_RTSS_S_TX_BASE,
};

static uint32_t receiver_addresses[] =
{
    MHU_RTSS_S_RX_BASE,
};

static void msg_acked_callback(uint32_t sender_id, uint32_t channel_number)
{
    (void)sender_id;
    (void)channel_number;
}

static void message_received_callback(uint32_t receiver_id, uint32_t channel_number, uint32_t service_data)
{
    (void)receiver_id;
    (void)channel_number;
    if (service_data == SHUTDOWN_MESSAGE) {
        running = false;
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

void MHU_RTSS_S_TX_IRQHandler(void)
{
    mhu_driver_out.sender_irq_handler(0);
}

void MHU_RTSS_S_RX_IRQHandler(void)
{
    mhu_driver_out.receiver_irq_handler(0);
}
#endif // #if HE_UPDATES_BOTH

static void hwinit()
{
    uint32_t config_uart_rx =
			PADCTRL_READ_ENABLE |
			PADCTRL_SCHMITT_TRIGGER_ENABLE |
			PADCTRL_DRIVER_DISABLED_PULL_UP;

    // configure UART4 for output (same as HP bootloader has)
    pinconf_set(PORT_12, PIN_1, PINMUX_ALTERNATE_FUNCTION_2, config_uart_rx); // P12_1: RX  (mux mode 2)
    pinconf_set(PORT_12, PIN_2, PINMUX_ALTERNATE_FUNCTION_2, 0);              // P12_2: TX  (mux mode 2)
}

int main(void)
{
    mpu_init(); // pull mpu in
    hwinit();
    tracelib_init(0, 0);
    fault_dump_enable(true);
    clk_init();
#if HE_UPDATES_BOTH
    MHU_driver_initialize(&mhu_driver_in, &mhu_driver_out);
    MHU_driver_initialize(&mhu_driver_in, &mhu_driver_out);
    NVIC_DisableIRQ(MHU_RTSS_S_RX_IRQ_IRQn);
    NVIC_SetPriority(MHU_RTSS_S_RX_IRQ_IRQn, 10);
    NVIC_EnableIRQ(MHU_RTSS_S_RX_IRQ_IRQn);

    NVIC_DisableIRQ(MHU_RTSS_S_TX_IRQ_IRQn);
    NVIC_SetPriority(MHU_RTSS_S_TX_IRQ_IRQn, 10);
    NVIC_EnableIRQ(MHU_RTSS_S_TX_IRQ_IRQn);
#endif
    pinconf_set(LED_PORT, LED_PIN_ORIGINAL, PINMUX_ALTERNATE_FUNCTION_0, 0);
    pinconf_set(LED_PORT, LED_PIN_UPDATED, PINMUX_ALTERNATE_FUNCTION_0, 0);
    int32_t ret = Led->Initialize(LED_PIN, 0);
    while(ret);
    ret = Led->PowerControl(LED_PIN, ARM_POWER_FULL);
    while(ret);
    ret = Led->SetDirection(LED_PIN_ORIGINAL, GPIO_PIN_DIRECTION_OUTPUT);
    while(ret);
    ret = Led->SetDirection(LED_PIN_UPDATED, GPIO_PIN_DIRECTION_OUTPUT);
    while(ret);
    ret = Led->SetValue(LED_PIN_ORIGINAL, GPIO_PIN_OUTPUT_STATE_LOW);
    while(ret);
    ret = Led->SetValue(LED_PIN_UPDATED, GPIO_PIN_OUTPUT_STATE_LOW);
    while(ret);

    uint32_t start = S32K_CNTRead->CNTCVL;
    printf("Blink start\n");

    boot_set_confirmed_multi(1);

    while(running)
    {
        if(S32K_CNTRead->CNTCVL - start > 32768) {
            Led->SetValue(LED_PIN, GPIO_PIN_OUTPUT_STATE_TOGGLE);
            start = S32K_CNTRead->CNTCVL;
        }
        flush_uart();
    }

    Led->SetValue(LED_PIN, GPIO_PIN_OUTPUT_STATE_LOW);

    __disable_irq();
    while(1) {
        pm_core_enter_deep_sleep_request_subsys_off();
    }
}
