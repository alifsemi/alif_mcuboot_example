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

#include <stdio.h>
#include <string.h>

#include "RTE_Components.h"
#include CMSIS_device_header
#include "board.h"

#include "Driver_USART.h"

extern ARM_DRIVER_USART ARM_Driver_USART_(BOARD_UART2_INSTANCE);
static ARM_DRIVER_USART *USARTdrv = &ARM_Driver_USART_(BOARD_UART2_INSTANCE);
#define UART_BUF_SIZE 255 // needs to be able to hold full max size frame
static unsigned char uart_buffer[UART_BUF_SIZE];
static unsigned char* uart_next_char = &uart_buffer[0];
static char uart_receive_char;
static volatile int32_t uart_transmit_global_call_back_event = 0;

static void uart_callback(uint32_t event)
{
    if (event & ARM_USART_EVENT_RECEIVE_COMPLETE) {

        if(uart_receive_char) {
            // store received char to uart buffer if there is space, otherwise drop
            if(uart_next_char < uart_buffer + UART_BUF_SIZE) {
                *uart_next_char++ = uart_receive_char;
            }
            else {
                printf("<UART RCV OVERFLOW>\n");
            }
        }

        // schedule to receive next char
        int32_t ret = USARTdrv->Receive(&uart_receive_char, 1);
        if (ret) {
            printf("<UART RCV ERROR>\n");
        }
    }
    if(event & ARM_USART_EVENT_SEND_COMPLETE) {
        uart_transmit_global_call_back_event = 1;
    }
}

int32_t get_frame_from_receive_buffer(unsigned char* frame_buf, const uint32_t maxlength, uint32_t* frame_length, const bool packet_receive_ongoing)
{
    __disable_irq();
    int32_t ret = scan_and_consume(uart_buffer, &uart_next_char, frame_buf, maxlength, frame_length, packet_receive_ongoing);
    __enable_irq();
    return ret;
}

void transmit_synchronous(const unsigned char* data, const uint32_t length)
{
    uart_transmit_global_call_back_event = 0;
    USARTdrv->Send(data, length);
    while(!uart_transmit_global_call_back_event) __WFE();
}

int32_t init_transport()
{
    int32_t ret = USARTdrv->Initialize(uart_callback);
    if (ret != ARM_DRIVER_OK)
    {
        return ret;
    }

    /* Power up UART peripheral */
    ret = USARTdrv->PowerControl(ARM_POWER_FULL);
    if (ret != ARM_DRIVER_OK)
    {
        return ret;
    }

    /* Configure UART to 115200 Bits/sec */
    ret =  USARTdrv->Control(ARM_USART_MODE_ASYNCHRONOUS |
                             ARM_USART_DATA_BITS_8       |
                             ARM_USART_PARITY_NONE       |
                             ARM_USART_STOP_BITS_1       |
                             ARM_USART_FLOW_CONTROL_NONE, 115200);
    if (ret != ARM_DRIVER_OK)
    {
        return ret;
    }

    /* Transmitter line */
    ret =  USARTdrv->Control(ARM_USART_CONTROL_TX, 1);
    if (ret != ARM_DRIVER_OK)
    {
        return ret;
    }

    /* Receiver line */
    ret =  USARTdrv->Control(ARM_USART_CONTROL_RX, 1);
    if (ret != ARM_DRIVER_OK)
    {
        return ret;
    }

    // schedule to receive first char
    ret = USARTdrv->Receive(&uart_receive_char, 1);

    return ret;
}
