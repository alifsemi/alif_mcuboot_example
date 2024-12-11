/* Copyright (C) 2024 Alif Semiconductor - All Rights Reserved.
 * Use, distribution and modification of this code is permitted under the
 * terms stated in the Alif Semiconductor Software License Agreement
 *
 * You should have received a copy of the Alif Semiconductor Software
 * License Agreement with this file. If not, please write to:
 * contact@alifsemi.com, or visit: https://alifsemi.com/license
 *
 */

#include "mcumgr_transport.h"

#include "tusb.h"

#define CDC_BUF_SIZE 255 // needs to be able to hold full max size frame
static unsigned char cdc_buffer[CDC_BUF_SIZE];
static unsigned char* cdc_buffer_ptr = cdc_buffer;

void transmit_synchronous(const unsigned char* data, const uint32_t length)
{
    uint32_t written = 0;
    do {
        uint32_t av = tud_cdc_write_available();
        uint32_t to_write = av < length - written ? av : length - written;
        if (to_write) {
            written += tud_cdc_write(data + written, to_write);
            tud_cdc_write_flush();
        }
        // some transmits are so long that usb might time out if
        // not given time in between the transmit
        tud_task();

    } while((length - written) > 0);
}

int32_t get_frame_from_receive_buffer(unsigned char* frame_buf, const uint32_t maxlength, uint32_t* frame_length, const bool packet_receive_ongoing)
{
    int32_t ret = MCUMGR_TRANSPORT_NO_FRAME;
    tud_task();
    if (tud_cdc_available()) {
        uint32_t len = tud_cdc_read(cdc_buffer_ptr, CDC_BUF_SIZE - (cdc_buffer_ptr - cdc_buffer));
        cdc_buffer_ptr += len;
        ret = scan_and_consume(cdc_buffer, &cdc_buffer_ptr, frame_buf, maxlength, frame_length, packet_receive_ongoing);
    }
    return ret;
}

int32_t init_transport()
{
    bool ret = tusb_init();

    return !ret;
}
