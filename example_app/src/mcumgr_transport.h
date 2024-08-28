/* Copyright (C) 2024 Alif Semiconductor - All Rights Reserved.
 * Use, distribution and modification of this code is permitted under the
 * terms stated in the Alif Semiconductor Software License Agreement
 *
 * You should have received a copy of the Alif Semiconductor Software
 * License Agreement with this file. If not, please write to:
 * contact@alifsemi.com, or visit: https://alifsemi.com/license
 *
 */

#ifndef MCUMGR_TRANSPORT_H
#define MCUMGR_TRANSPORT_H

#include <inttypes.h>
#include <stdbool.h>

// return values for get_frame_from_receive_buffer
#define MCUMGR_TRANSPORT_FRAME_FOUND   1
#define MCUMGR_TRANSPORT_NO_FRAME      2
#define MCUMGR_TRANSPORT_FRAME_INVALID 3
#define MCUMGR_TRANSPORT_FRAME_TOO_BIG 4

// API that transport implementations needs to implement.

// Initialize the transport layer and start receiving data
// Return: 0 on success or error code.
int32_t init_transport();

// Send given data via the transport mechanism. On return, transport layer must be ready to receive a new transmit request.
// Input:
//    data a buffer containing the data to be sent
//    length the length of the sent data contained in the buffer
void transmit_synchronous(const unsigned char* data, const uint32_t length);

// Copy full frame of received data to the given frame buffer.
// Input:
//    frame_buf buffer where frame data is to be copied
//    maxlength maximum space available in the frame_buf
//    frame_length must be set to amount of data copied into frame_buf
//    packet_receive_ongoing whether caller is expecting a starting frame or continuation frame
// Return: One of MCUMGR_TRANPORT_... defines
int32_t get_frame_from_receive_buffer(unsigned char* frame_buf, const uint32_t maxlength, uint32_t* frame_length, const bool packet_receive_ongoing);


// API for the transport implementation to use on get_frame_from_receive_buffer to detect frame and copy it to the frame buffer
// Input:
//    buf_start the transport buffer that is scanned for frame data
//    buf_end end of transport buffer, will be set to new end if data is consumed from the transport buffer
//    frame_buf buffer where frame data is to be copied
//    maxlength maximum space available in the frame_buf
//    frame_length will be set to amount of data copied into frame_buf
//    packet_receive_ongoing whether caller is expecting a starting frame or continuation frame
// Return: One of MCUMGR_TRANPORT_... defines
int32_t scan_and_consume(unsigned char* buf_start, unsigned char** buf_end, unsigned char* frame_buf, const uint32_t maxlength, uint32_t* frame_length, const bool packet_receive_ongoing);

#endif
