/* Copyright (C) 2024 Alif Semiconductor - All Rights Reserved.
 * Use, distribution and modification of this code is permitted under the
 * terms stated in the Alif Semiconductor Software License Agreement
 *
 * You should have received a copy of the Alif Semiconductor Software
 * License Agreement with this file. If not, please write to:
 * contact@alifsemi.com, or visit: https://alifsemi.com/license
 *
 */

#ifndef HW_H
#define HW_H

#include "Driver_GPIO.h"

void hw_init(void);
void led_button_init(ARM_GPIO_SignalEvent_t cb);
void led_toggle(void);

#endif
