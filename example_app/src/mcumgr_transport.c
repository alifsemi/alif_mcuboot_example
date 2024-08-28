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
#include <inttypes.h>
#include <string.h>

#define FRAME_START_BYTES_LEN 2

int32_t scan_and_consume(unsigned char* buf_start, unsigned char** buf_end, unsigned char* frame_buf, const uint32_t maxlength, uint32_t* frame_length, const bool packet_receive_ongoing)
{
    *frame_length = 0;
    int32_t ret = MCUMGR_TRANSPORT_NO_FRAME;
    for (unsigned char* character = buf_start; character < *buf_end; character++) {
        if(*character == '\n') {
            // found, check for frame markers
            if((!packet_receive_ongoing && buf_start[0] == 0x06 && buf_start[1] == 0x09) ||
                (packet_receive_ongoing && buf_start[0] == 0x04 && buf_start[1] == 0x14)) {

                *frame_length = (character - buf_start) - FRAME_START_BYTES_LEN;

                if(*frame_length <= maxlength) {
                    // copy frame data to frame buffer
                    memcpy(frame_buf, buf_start + FRAME_START_BYTES_LEN, *frame_length);
                    ret = MCUMGR_TRANSPORT_FRAME_FOUND;

                } else {
                    *frame_length = 0; // nothing copied
                    ret = MCUMGR_TRANSPORT_FRAME_TOO_BIG;
                }
            }
            else {
                ret = MCUMGR_TRANSPORT_FRAME_INVALID;
            }

            // consume data from buffer regardless of valid frame received
            memcpy(buf_start, character + 1, *buf_end - character - 1);
            *buf_end = buf_start + (*buf_end - character - 1);
        }
    }
    return ret;
}
