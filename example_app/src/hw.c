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

#include <stdio.h>


#ifdef EXAMPLE_APP_UPDATE_TARGET
#define LED_PORT  7
#define LED_PIN 4
#else
#define LED_PORT  12
#define LED_PIN 3
#endif

#define BUTTON_PIN PIN_4

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

    pinconf_set(PORT_1, PIN_0, PINMUX_ALTERNATE_FUNCTION_1, config_uart_rx);	// P1_0: RX  (mux mode 1)
	pinconf_set(PORT_1, PIN_1, PINMUX_ALTERNATE_FUNCTION_1, 0);					// P1_1: TX  (mux mode 1)
    pinconf_set(LED_PORT, LED_PIN, PINMUX_ALTERNATE_FUNCTION_0, 0);
    pinconf_set(PORT_15, BUTTON_PIN, PINMUX_ALTERNATE_FUNCTION_0, config_button);
}

void led_button_init(ARM_GPIO_SignalEvent_t cb)
{
    Led->Initialize(LED_PIN, 0);
    Led->PowerControl(LED_PIN, ARM_POWER_FULL);

    Led->SetValue(LED_PIN, GPIO_PIN_OUTPUT_STATE_LOW);
	Led->SetDirection(LED_PIN, GPIO_PIN_DIRECTION_OUTPUT);

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
