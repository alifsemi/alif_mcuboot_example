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
#include "pinconf.h"
#include "Driver_GPIO.h"
#include "system_utils.h"


#include <stdio.h>

extern void clk_init(void);

#ifdef EXAMPLE_APP_UPDATE_TARGET
#define LED_PORT  7
#define LED_PIN 4
#else
#define LED_PORT  12
#define LED_PIN 3
#endif
extern ARM_DRIVER_GPIO ARM_Driver_GPIO_(LED_PORT);
static ARM_DRIVER_GPIO* Led = &ARM_Driver_GPIO_(LED_PORT);

void hw_init(void)
{
    uint32_t config_uart_rx =
			PADCTRL_READ_ENABLE |
			PADCTRL_SCHMITT_TRIGGER_ENABLE |
			PADCTRL_DRIVER_DISABLED_PULL_UP;
    pinconf_set(PORT_1, PIN_0, PINMUX_ALTERNATE_FUNCTION_1, config_uart_rx);	// P1_0: RX  (mux mode 1)
	pinconf_set(PORT_1, PIN_1, PINMUX_ALTERNATE_FUNCTION_1, 0);					// P1_1: TX  (mux mode 1)
    pinconf_set(LED_PORT, LED_PIN, PINMUX_ALTERNATE_FUNCTION_0, 0);
}

int main(void)
{
    hw_init();
    tracelib_init(0, 0);
    fault_dump_enable(true);
    clk_init();
    sys_busy_loop_init();

#ifdef EXAMPLE_APP_UPDATE_TARGET
    printf("Updated app running!\n");
#else
    printf("Example app running!\n");
#endif

    Led->Initialize(LED_PIN, 0);
    Led->PowerControl(LED_PIN, ARM_POWER_FULL);

    Led->SetValue(LED_PIN, GPIO_PIN_OUTPUT_STATE_LOW);
	Led->SetDirection(LED_PIN, GPIO_PIN_DIRECTION_OUTPUT);

    while(1)
    {
        Led->SetValue(LED_PIN, GPIO_PIN_OUTPUT_STATE_TOGGLE);
        for(int i = 0; i < 10; i++) {
            sys_busy_loop_us(100000);
        }
    }
}
