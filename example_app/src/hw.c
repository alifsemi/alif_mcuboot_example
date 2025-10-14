/* Copyright (C) 2024 Alif Semiconductor - All Rights Reserved.
 * Use, distribution and modification of this code is permitted under the
 * terms stated in the Alif Semiconductor Software License Agreement
 *
 * You should have received a copy of the Alif Semiconductor Software
 * License Agreement with this file. If not, please write to:
 * contact@alifsemi.com, or visit: https://alifsemi.com/license
 *
 */

#include "hw.h"

#include "pinconf.h"
#include "RTE_Components.h"
#include CMSIS_device_header

#include <stdio.h>


#define LED_PORT  12

#define LED_PIN_ORIGINAL 3 // LED0_R, Same ports and pins with DevKit-e7,DevKit-e8, DevKit-e4, AppKit-e7 and DevKit-e1c.
#define LED_PIN_UPDATED  0 // LED0_B, Same ports and pins with DevKit-e7,DevKit-e8, DevKit-e4, AppKit-e7 and DevKit-e1c.

#ifdef EXAMPLE_APP_UPDATE_TARGET
#define LED_PIN   LED_PIN_UPDATED
#else
#define LED_PIN   LED_PIN_ORIGINAL
#endif

#ifdef ENSEMBLE_SOC_E1C
#define UART2_RX_PORT   PORT_5
#define UART2_TX_PORT   PORT_5
#define UART2_RX_PIN    PIN_2
#define UART2_TX_PIN    PIN_3

#define UART3_RX_PORT   PORT_2
#define UART3_TX_PORT   PORT_2
#define UART3_RX_PIN    PIN_4
#define UART3_TX_PIN    PIN_5

#define BUTTON_PORT PORT_5
#define BUTTON_PIN  PIN_7 // JOY_SW5
#else // Same ports and pins with DevKit-e7,DevKit-e8, DevKit-e4, AppKit-e7
#define UART2_RX_PORT   PORT_1
#define UART2_TX_PORT   PORT_1
#define UART2_RX_PIN    PIN_0
#define UART2_TX_PIN    PIN_1

#define UART3_RX_PORT   PORT_1
#define UART3_TX_PORT   PORT_1
#define UART3_RX_PIN    PIN_2
#define UART3_TX_PIN    PIN_3

#define BUTTON_PORT PORT_15
#define BUTTON_PIN  PIN_4 // JOY_SW5
#endif

extern ARM_DRIVER_GPIO ARM_Driver_GPIO_(LED_PORT);
static ARM_DRIVER_GPIO* Led = &ARM_Driver_GPIO_(LED_PORT);

extern ARM_DRIVER_GPIO ARM_Driver_GPIO_(15);
static ARM_DRIVER_GPIO* Button = &ARM_Driver_GPIO_(15);


void hw_init(void)
{
    uint32_t config_uart_rx =
			PADCTRL_READ_ENABLE |
			PADCTRL_SCHMITT_TRIGGER_ENABLE |
			PADCTRL_DRIVER_DISABLED_PULL_UP;

    uint32_t config_button =
            PADCTRL_READ_ENABLE |
            PADCTRL_SCHMITT_TRIGGER_ENABLE |
			PADCTRL_DRIVER_DISABLED_PULL_UP;

    // uart 2 = trace uart
    pinconf_set(UART2_RX_PORT, UART2_RX_PORT, PINMUX_ALTERNATE_FUNCTION_1, config_uart_rx);
    pinconf_set(UART2_TX_PORT, UART2_TX_PORT, PINMUX_ALTERNATE_FUNCTION_1, 0);

    // uart 3 = mcumgr transport uart
    pinconf_set(UART3_RX_PORT,  UART3_RX_PIN, PINMUX_ALTERNATE_FUNCTION_1, config_uart_rx);
    pinconf_set(UART3_TX_PORT,  UART3_TX_PIN, PINMUX_ALTERNATE_FUNCTION_1, 0);
    pinconf_set(LED_PORT, LED_PIN_ORIGINAL, PINMUX_ALTERNATE_FUNCTION_0, 0);
    pinconf_set(LED_PORT, LED_PIN_UPDATED, PINMUX_ALTERNATE_FUNCTION_0, 0);
    pinconf_set(BUTTON_PORT, BUTTON_PIN, PINMUX_ALTERNATE_FUNCTION_0, config_button);
}

void led_button_init(ARM_GPIO_SignalEvent_t cb)
{
    Led->Initialize(LED_PIN, 0);
    Led->PowerControl(LED_PIN, ARM_POWER_FULL);

    Led->SetDirection(LED_PIN_ORIGINAL, GPIO_PIN_DIRECTION_OUTPUT);
    Led->SetDirection(LED_PIN_UPDATED, GPIO_PIN_DIRECTION_OUTPUT);

    Led->SetValue(LED_PIN_ORIGINAL, GPIO_PIN_OUTPUT_STATE_HIGH);
    Led->SetValue(LED_PIN_UPDATED, GPIO_PIN_OUTPUT_STATE_HIGH);

    uint32_t err;

    err = Button->Initialize(BUTTON_PIN, cb);
    if(err) {
        printf("1: %lu\n", err);
    }
    err = Button->PowerControl(BUTTON_PIN, ARM_POWER_FULL);
    if(err) {
        printf("1: %lu\n", err);
    }
    err = Button->SetDirection(BUTTON_PIN, GPIO_PIN_DIRECTION_INPUT);
    if(err) {
        printf("1: %lu\n", err);
    }
    uint32_t irq_config = ARM_GPIO_IRQ_POLARITY_LOW | ARM_GPIO_IRQ_SENSITIVE_EDGE;
    err = Button->Control(BUTTON_PIN, ARM_GPIO_ENABLE_INTERRUPT, &irq_config);
    if(err) {
        printf("1: %lu\n", err);
    }
}

void led_toggle(void)
{
    Led->SetValue(LED_PIN, GPIO_PIN_OUTPUT_STATE_TOGGLE);
}

void led_off(void)
{
    Led->SetValue(LED_PIN, GPIO_PIN_OUTPUT_STATE_HIGH);
}
