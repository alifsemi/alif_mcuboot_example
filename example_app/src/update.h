/* Copyright (C) 2024 Alif Semiconductor - All Rights Reserved.
 * Use, distribution and modification of this code is permitted under the
 * terms stated in the Alif Semiconductor Software License Agreement
 *
 * You should have received a copy of the Alif Semiconductor Software
 * License Agreement with this file. If not, please write to:
 * contact@alifsemi.com, or visit: https://alifsemi.com/license
 *
 */

#ifndef UPDATE_H
#define UPDATE_H

#include <inttypes.h>

int read_image_state(uint8_t* test_boot, uint8_t* update_available);
void set_pending(void);
void confirm_update(void);

#endif
