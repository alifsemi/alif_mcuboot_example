/* Copyright (C) 2024 Alif Semiconductor - All Rights Reserved.
 * Use, distribution and modification of this code is permitted under the
 * terms stated in the Alif Semiconductor Software License Agreement
 *
 * You should have received a copy of the Alif Semiconductor Software
 * License Agreement with this file. If not, please write to:
 * contact@alifsemi.com, or visit: https://alifsemi.com/license
 *
 */

#include "mcumgr_handler.h"

#include "mcumgr_transport.h"

#include "img_mgmt/img_mgmt.h"
#include "os_mgmt/os_mgmt.h"
#include "smp/smp.h"
#include "mbedtls/base64.h"
#include "tinycbor/cbor_buf_reader.h"
#include "tinycbor/cbor_buf_writer.h"
#include "tinycbor/extract_number_p.h"

#include "RTE_Components.h"
#include CMSIS_device_header

#include "Driver_CRC.h"

// OS command implementation init
extern int32_t os_mgmt_impl_init(void);

// CRC calculation
extern ARM_DRIVER_CRC Driver_CRC1;
static ARM_DRIVER_CRC *CRCdrv = &Driver_CRC1;
static uint16_t calculate_crc(unsigned char* data, uint32_t len);
static volatile int32_t crc_global_call_back_event = 0;

// Buffers
#define MAX_REQUEST_SIZE 1024
static unsigned char request_buffer[MAX_REQUEST_SIZE];
static unsigned char* request_ptr = request_buffer;
static bool packet_receive_ongoing = false;

#define MAX_BODY_SIZE_FOR_SINGLE_FRAME 90
// must be able to hold single frame's data unencoded
static unsigned char smp_buf[MAX_BODY_SIZE_FOR_SINGLE_FRAME];

#define TRANSMIT_BUF_SIZE 121
// needs to able to hold MAX_BODY_SIZE_FOR_SINGLE_FRAME encoded in BASE64 + null byte that mbedtls_base64encode 
// insist on adding (but won't get transmitted)
static unsigned char transmit_buf[TRANSMIT_BUF_SIZE];

// must be able to hold full response contents. needs adjustment?
#define PACKET_BUF_SIZE 256
static char response_buf[PACKET_BUF_SIZE];

// SMP handling
#define LENGTH_FIELD_SIZE 2     // uint16
#define CRC_FIELD_SIZE 2        // uint16

static mgmt_alloc_rsp_fn smp_alloc_rsp;
static mgmt_trim_front_fn smp_trim_front;
static mgmt_reset_buf_fn smp_reset_buf;
static mgmt_write_at_fn smp_write_at;
static mgmt_init_reader_fn smp_init_reader;
static mgmt_init_writer_fn smp_init_writer;
static mgmt_free_buf_fn smp_free_buf;
static smp_tx_rsp_fn smp_tx_rsp_cb;

struct cbor_buf_writer_ext {
    struct cbor_buf_writer enc;
    unsigned char* whole_buffer_start;
};

static struct cbor_buf_writer_ext buf_writer;
static struct cbor_buf_reader buf_reader;

static const struct mgmt_streamer_cfg g_smp_cbor_cfg = {
    .alloc_rsp = smp_alloc_rsp,
    .trim_front = smp_trim_front,
    .reset_buf = smp_reset_buf,
    .write_at = smp_write_at,
    .init_reader = smp_init_reader,
    .init_writer = smp_init_writer,
    .free_buf = smp_free_buf,
};

struct smp_streamer_ext {
    struct smp_streamer enc;
    struct smp_streamer_ext* self;
    uint16_t* len;
};

static struct smp_streamer_ext streamer = {
    .enc.mgmt_stmr.cb_arg = 0,
    .enc.mgmt_stmr.cfg = &g_smp_cbor_cfg,
    .enc.mgmt_stmr.reader = (struct cbor_decoder_reader*)&buf_reader,
    .enc.mgmt_stmr.writer = (struct cbor_encoder_writer*)&buf_writer,
    .enc.tx_rsp_cb = smp_tx_rsp_cb,
    .self = 0,
    .len = 0
};

static int smp_write_at(struct cbor_encoder_writer *writer, size_t offset,
                             const void *data, size_t len, void *arg)
{
    (void)arg;
    struct cbor_buf_writer_ext* buf_writer_ext = (struct cbor_buf_writer_ext*)writer;

    // check if the write fits to the buffer
    if(buf_writer_ext->whole_buffer_start + offset + len > buf_writer_ext->enc.end) {
        return MGMT_ERR_ENOMEM;
    }

    memcpy(buf_writer_ext->whole_buffer_start + offset, data, len);

    // adjust the current position of writer if necessary
    if(buf_writer_ext->enc.ptr < buf_writer_ext->whole_buffer_start + len) {
        buf_writer_ext->enc.ptr = buf_writer_ext->whole_buffer_start + len;
    }

    // adjust the bytes written if necessary
    if(writer->bytes_written < (int)len) {
        writer->bytes_written = len;
    }

    return MGMT_ERR_EOK;
}

static int smp_init_reader(struct cbor_decoder_reader *reader, void *buf, void *arg)
{
    struct smp_streamer_ext* streamer = (struct smp_streamer_ext*)arg;
    cbor_buf_reader_init((struct cbor_buf_reader*)reader, buf, *(streamer->len));
    
    return MGMT_ERR_EOK;
}

static int smp_init_writer(struct cbor_encoder_writer *writer, void *buf, void *arg)
{
    (void)arg;
    struct cbor_buf_writer_ext* buf_writer_ext = (struct cbor_buf_writer_ext*)writer;
    buf_writer_ext->whole_buffer_start = buf;
    cbor_buf_writer_init((struct cbor_buf_writer*)writer, buf, PACKET_BUF_SIZE);
    return MGMT_ERR_EOK;
}

static void smp_free_buf(void *buf, void *arg)
{
    (void)arg;
    (void)buf;
}

static int smp_tx_rsp_cb(struct smp_streamer *ss, void *buf, void *arg)
{
    (void)arg;
    int body_left = ss->mgmt_stmr.writer->bytes_written;
    const int packet_length = body_left + CRC_FIELD_SIZE; // length includes CRC
    int body_sent = 0;
    bool first_frame = true;
    bool last_frame = false;

    while(!last_frame) {
        if(first_frame) {
            // transmit packet starting bytes
            transmit_buf[0] = 0x06;
            transmit_buf[1] = 0x09;
            transmit_synchronous(transmit_buf, 2);
        }
        else {
            // transmit packet continuation bytes
            transmit_buf[0] = 0x04;
            transmit_buf[1] = 0x14;
            transmit_synchronous(transmit_buf, 2);
        }

        // prepare packet content
        unsigned char* ptr = smp_buf;
        int bytes_for_this_frame = MAX_BODY_SIZE_FOR_SINGLE_FRAME;

        if(first_frame) {
            // first frame includes length of the packet (length of the body + the final crc)
            *((uint16_t*)ptr) = cbor_ntohs(packet_length);
            ptr += LENGTH_FIELD_SIZE;
            bytes_for_this_frame -= LENGTH_FIELD_SIZE; // length added
        }

        if(body_left <= bytes_for_this_frame) {
            // less than maximum bytes left so adjust send length

            if(body_left <= (bytes_for_this_frame - CRC_FIELD_SIZE)) {
                // there's two bytes space in the end
                // so this can be made the last packet by adding
                // the CRC.
                last_frame = true;
            }
            bytes_for_this_frame = body_left;
        }
        // adjust unsent bytes
        body_left -= bytes_for_this_frame;

        memcpy(ptr, buf + body_sent, bytes_for_this_frame);
        ptr += bytes_for_this_frame;
        body_sent += bytes_for_this_frame;

        if(last_frame) {
            // add CRC to the last frame
            uint16_t crc = calculate_crc(buf, ss->mgmt_stmr.writer->bytes_written);
            *((uint16_t*)(ptr)) = cbor_ntohs(crc);
            ptr += CRC_FIELD_SIZE;
        }

        // base64 encode response buffer into transmit buffer
        size_t encoded_len;
        int ret = mbedtls_base64_encode(transmit_buf, TRANSMIT_BUF_SIZE, &encoded_len, smp_buf, (ptr - smp_buf));
        if(ret) {
            printf("mbedtls_base64_encode err: %d\n", ret);
            return ret;
        }

        // send the base64 content
        transmit_synchronous(transmit_buf, encoded_len);

        // send newline
        transmit_buf[0] = '\n';
        transmit_synchronous(transmit_buf, 1);
        first_frame = false;
    }
    return MGMT_ERR_EOK;
}

static void* smp_alloc_rsp(const void *src_buf, void *arg)
{
    (void)arg;
    if(src_buf) {
        memcpy(response_buf, src_buf, TRANSMIT_BUF_SIZE);
    }
    return response_buf;
}

static void smp_trim_front(void *buf, size_t len, void *arg)
{
    (void)buf;
    struct smp_streamer_ext* streamer = (struct smp_streamer_ext*)arg;
    struct cbor_buf_reader* reader = (struct cbor_buf_reader*)streamer->enc.mgmt_stmr.reader;
    if (reader->r.message_size < len) {
        len = reader->r.message_size;
    }
    memcpy((unsigned char*)(reader->buffer), reader->buffer + len, reader->r.message_size - len);
    reader->r.message_size -= len;
    *(streamer->len) -= len;
}

static void smp_reset_buf(void *buf, void *arg)
{
    (void)arg;
    (void)buf;
}

static void mcumgr_handler_process_frame(const uint32_t received_len)
{
    // decode received data in place
    size_t decoded_len;
    int ret = mbedtls_base64_decode(request_ptr, MAX_REQUEST_SIZE - (request_ptr - request_buffer), &decoded_len, request_ptr, received_len);
    if(ret) {
        printf("Base64 decode failed: %d\n", ret);
        return;
    }

    uint16_t packet_length = get16(request_buffer); // get full packet length (body + crc)
    request_ptr += decoded_len;

    if (request_ptr - request_buffer == packet_length + LENGTH_FIELD_SIZE) {
        // all content for packet received
        uint16_t body_length = packet_length - CRC_FIELD_SIZE;
        uint16_t crc_calculated = calculate_crc(request_buffer + LENGTH_FIELD_SIZE, body_length);
        uint16_t crc_messaged = get16(request_buffer + packet_length);
        if(crc_calculated != crc_messaged)
        {
            printf("CRC failed, messaged: 0x%" PRIX16 " calculated: 0x%" PRIX16 "\n", crc_messaged, crc_calculated);
            return;
        }

        streamer.enc.mgmt_stmr.cb_arg = &streamer;
        streamer.len = &body_length;

        // process the received packet's body
        smp_process_request_packet((struct smp_streamer*)&streamer, request_buffer + LENGTH_FIELD_SIZE);

        // reset to be ready for new packet
        packet_receive_ongoing = false;
        request_ptr = request_buffer;
    }
    else {
        // continuation expected
        packet_receive_ongoing = true;
    }
}

void mcumgr_handler_process()
{
    uint32_t received_len;
    int32_t ret = get_frame_from_receive_buffer(request_ptr, MAX_REQUEST_SIZE - (request_ptr - request_buffer), &received_len, packet_receive_ongoing);

    switch(ret) {
        case MCUMGR_TRANSPORT_FRAME_FOUND:
            mcumgr_handler_process_frame(received_len);
            break;
        case MCUMGR_TRANSPORT_NO_FRAME:
            // no full frame yet.
            break;
        case MCUMGR_TRANSPORT_FRAME_INVALID:
            printf("INVALID DATA\n");
            break;
        case MCUMGR_TRANSPORT_FRAME_TOO_BIG:
            printf("TOO BIG FRAME\n");
            break;
        default:
            printf("UNKNOWN RET FROM TRANSPORT\n");
            break;
    }
}

static void crc_compute_callback(uint32_t event)
{
    if (event & ARM_CRC_COMPUTE_EVENT_DONE) {
        crc_global_call_back_event = 1;
    }
}

static uint16_t calculate_crc(unsigned char* data, uint32_t len)
{
    CRCdrv->Seed(0);

    crc_global_call_back_event = 0;

    uint32_t data_out;
    CRCdrv->Compute(data, len, &data_out);

    while(!crc_global_call_back_event) __WFE();
    return data_out;
}

static int32_t init_crc()
{
    // Do the init uninit sequence first to clear the registers. This can be removed after issue
    // https://alifsemi.atlassian.net/browse/PSBT-272 is fixed.
    CRCdrv->Initialize(crc_compute_callback);
    CRCdrv->Uninitialize();

    // Initialize CRC driver
    int32_t ret = CRCdrv->Initialize(crc_compute_callback);
    if (ret != ARM_DRIVER_OK) {
         printf("\r\n Error: CRC init failed\n");
        return ret;
    }

    // Enable the power for CRC
    ret = CRCdrv->PowerControl(ARM_POWER_FULL);
    if (ret != ARM_DRIVER_OK) {
        printf("\r\n Error: CRC Power up failed\n");
        return ret;
    }

    // To select the 16 bit CRC algorithm
    ret= CRCdrv->Control(ARM_CRC_ALGORITHM_SEL, ARM_CRC_ALGORITHM_SEL_16_BIT_CCITT);
    if (ret != ARM_DRIVER_OK) {
        printf("\r\n Error: CRC Control Algorithm failed\n");
    }

    return ret;
}

void mcumgr_evt_callback(uint8_t opcode, uint16_t group, uint8_t id, void *arg)
{
    if(opcode == MGMT_EVT_OP_CMD_DONE)
    {
        struct mgmt_evt_op_cmd_done_arg* cmd_done_arg = (struct mgmt_evt_op_cmd_done_arg*)arg;
        printf("Handled MCUMGR command, group: %" PRId16 " id: %" PRId16 " result code: %d\n", group, id, cmd_done_arg->err);
    }
}

int32_t mcumgr_handler_init(void)
{
    int32_t ret = os_mgmt_impl_init();
    if(ret) {
        printf("mcumgr_handler_init - os_mgmt_impl_init failed: %" PRId32 "\n", ret);
        return ret;
    }

    img_mgmt_register_group();
    os_mgmt_register_group();
    mgmt_register_evt_cb(mcumgr_evt_callback);

    ret = init_transport();
    if(ret) {
        printf("mcumgr_handler_init - init_transport failed: %" PRId32 "\n", ret);
        return ret;
    }
    ret = init_crc();
    if(ret) {
        printf("mcumgr_handler_init - init_crc failed: %" PRId32 "\n", ret);
        return ret;
    }

    return ret;
}
