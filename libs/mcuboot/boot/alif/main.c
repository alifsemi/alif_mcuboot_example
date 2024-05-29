/* Copyright (C) 2024 Alif Semiconductor - All Rights Reserved.
 * Use, distribution and modification of this code is permitted under the
 * terms stated in the Alif Semiconductor Software License Agreement
 *
 * You should have received a copy of the Alif Semiconductor Software
 * License Agreement with this file. If not, please write to:
 * contact@alifsemi.com, or visit: https://alifsemi.com/license
 *
 */

#include <bootutil/bootutil.h>

struct arm_vector_table {
    uint32_t msp;
    uint32_t reset;
};

int main(void)
{
    struct arm_vector_table *vt;
    struct boot_rsp rsp;

    int rv = boot_go(&rsp);

    if (rv == 0)
    {
        /* Jump to the starting point of the image */
        vt = (struct arm_vector_table *)(rsp.br_image_off + rsp.br_hdr->ih_hdr_size);
        ((void (*)(void))vt->reset)();
    }
    while(1);
}
