/*---------------------------------------------------------------------------
 * Copyright (c) 2025 Arm Limited (or its affiliates). All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the License); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *---------------------------------------------------------------------------*/

#include "RTE_Components.h"
#include CMSIS_device_header

#include "ext_init.h"

#include "Driver_IO.h"

/* Defines required for manual pin control (reset) */
#define MT9M114_CAMERA_RESET_GPIO_PORT      9
#define MT9M114_CAMERA_RESET_PIN_NO         1
#define MT9M114_CAMERA_POWER_GPIO_PORT      7
#define MT9M114_CAMERA_POWER_PIN_NO         5

#define GT911_TOUCH_RESET_GPIO_PORT         4
#define GT911_TOUCH_RESET_PIN_NO            0

#define ILI9488_PANEL_BL_LED_PIN_NO         1
#define ILI9488_PANEL_BL_LED_GPIO_PORT      6

/* MT9M114 Camera reset GPIO port */
extern ARM_DRIVER_GPIO  ARM_Driver_GPIO_(MT9M114_CAMERA_RESET_GPIO_PORT);
static ARM_DRIVER_GPIO *IO_Driver_CAM_RST =
    &ARM_Driver_GPIO_(MT9M114_CAMERA_RESET_GPIO_PORT);

/* MT9M114 Camera power GPIO port */
extern ARM_DRIVER_GPIO  ARM_Driver_GPIO_(MT9M114_CAMERA_POWER_GPIO_PORT);
static ARM_DRIVER_GPIO *IO_Driver_CAM_PWR =
    &ARM_Driver_GPIO_(MT9M114_CAMERA_POWER_GPIO_PORT);

/* GT911 Touch Controller reset port */
extern ARM_DRIVER_GPIO  ARM_Driver_GPIO_(GT911_TOUCH_RESET_GPIO_PORT);
static ARM_DRIVER_GPIO *IO_Driver_TOUCH_RST = &ARM_Driver_GPIO_(GT911_TOUCH_RESET_GPIO_PORT);

/* GLCD reset port */
#define BOARD_LCD_RESET_GPIO_PORT LP
extern ARM_DRIVER_GPIO  ARM_Driver_GPIO_(BOARD_LCD_RESET_GPIO_PORT);
static ARM_DRIVER_GPIO *IO_Driver_LCD_RST = &ARM_Driver_GPIO_(BOARD_LCD_RESET_GPIO_PORT);

/*
  Initialize reset and power I/Os to the external devices.
*/
void ext_init(void)
{
    IO_Driver_CAM_PWR->Initialize(MT9M114_CAMERA_POWER_PIN_NO, NULL);
    IO_Driver_CAM_PWR->PowerControl(MT9M114_CAMERA_POWER_PIN_NO, ARM_POWER_FULL);
    IO_Driver_CAM_PWR->SetValue(MT9M114_CAMERA_POWER_PIN_NO, GPIO_PIN_OUTPUT_STATE_LOW);
    IO_Driver_CAM_PWR->SetDirection(MT9M114_CAMERA_POWER_PIN_NO, GPIO_PIN_DIRECTION_OUTPUT);

    IO_Driver_CAM_RST->Initialize(MT9M114_CAMERA_RESET_PIN_NO, NULL);
    IO_Driver_CAM_RST->PowerControl(MT9M114_CAMERA_RESET_PIN_NO, ARM_POWER_FULL);
    IO_Driver_CAM_RST->SetValue(MT9M114_CAMERA_RESET_PIN_NO, GPIO_PIN_OUTPUT_STATE_LOW);
    IO_Driver_CAM_RST->SetDirection(MT9M114_CAMERA_RESET_PIN_NO, GPIO_PIN_DIRECTION_OUTPUT);

    IO_Driver_TOUCH_RST->Initialize(GT911_TOUCH_RESET_PIN_NO, NULL);
    IO_Driver_TOUCH_RST->PowerControl(GT911_TOUCH_RESET_PIN_NO, ARM_POWER_FULL);
    IO_Driver_TOUCH_RST->SetValue(GT911_TOUCH_RESET_PIN_NO, GPIO_PIN_OUTPUT_STATE_LOW);
    IO_Driver_TOUCH_RST->SetDirection(GT911_TOUCH_RESET_PIN_NO, GPIO_PIN_DIRECTION_OUTPUT);

    IO_Driver_LCD_RST->Initialize(ILI9488_PANEL_BL_LED_PIN_NO, NULL);
    IO_Driver_LCD_RST->PowerControl(ILI9488_PANEL_BL_LED_PIN_NO, ARM_POWER_FULL);
    IO_Driver_LCD_RST->SetValue(ILI9488_PANEL_BL_LED_PIN_NO, GPIO_PIN_OUTPUT_STATE_LOW);
    IO_Driver_LCD_RST->SetDirection(ILI9488_PANEL_BL_LED_PIN_NO, GPIO_PIN_DIRECTION_OUTPUT);
}
