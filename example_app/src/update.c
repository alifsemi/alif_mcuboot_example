/* Copyright (C) 2024 Alif Semiconductor - All Rights Reserved.
 * Use, distribution and modification of this code is permitted under the
 * terms stated in the Alif Semiconductor Software License Agreement
 *
 * You should have received a copy of the Alif Semiconductor Software
 * License Agreement with this file. If not, please write to:
 * contact@alifsemi.com, or visit: https://alifsemi.com/license
 *
 */

#include "update.h"

#include "bootutil/bootutil_public.h"
#include "sysflash/sysflash.h"

#include <stdio.h>


static int read_single_image_state(int id, uint8_t* update_available)
{
    const struct flash_area* fa;
    int err = flash_area_open(id, &fa);
    if (err != 0) {
        printf("flash_area_open error %d\n", err);
        return -1;
    }

    printf("  offset:    0x%lX\n", fa->fa_off);
    printf("  size:      0x%lX\n", fa->fa_size);

    struct boot_swap_state sstate;

    err = boot_read_swap_state(fa, &sstate);
    if(err != 0) {
        printf("boot_read_swap_state error: %d\n", err);
        return -1;
    }

    printf("  magic:     %u\n", sstate.magic);
    printf("  swap_type: %u\n", sstate.swap_type);
    printf("  copy_done: %u\n", sstate.copy_done);
    printf("  image_ok:  %u\n", sstate.image_ok);
    printf("  image_num: %u\n", sstate.image_num);

    struct image_header header;
    err = boot_image_load_header(fa, &header);
    if(!err) {
        printf("  version:   %u.%u.%u\n", header.ih_ver.iv_major, header.ih_ver.iv_minor, header.ih_ver.iv_revision);
        if (update_available) {
            *update_available = 1;
        }
    }
    
    flash_area_close(fa);
    return 0;
}

int read_image_state(uint8_t* update_available)
{
    printf("PRIMARY slot:\n");
    int err = read_single_image_state(FLASH_AREA_IMAGE_PRIMARY(0), 0);
    if (err) {
        return err;
    }
    printf("SECONDARY slot:\n");
    return read_single_image_state(FLASH_AREA_IMAGE_SECONDARY(0), update_available);
}

void set_pending(void)
{
    int err = boot_set_pending_multi(0, 1);
    if(err) {
        printf("set_pending error: %d\n", err);
    }
    else {
        printf("Update set pending. Reboot the device to apply the update.\n");
    }
}
