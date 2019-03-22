// Copyright Mark Qvist / unsigned.io
// https://unsigned.io/microaprs
//
// Licensed under GPL-3.0. For full info,
// read the LICENSE file.

#ifndef PROTOCOL_AX25_H
#define PROTOCOL_AX25_H

#include <stdio.h>
#include <stdbool.h>
#include "device.h"
#include "hardware/AFSK.h"

#define AX25_MIN_FRAME_LEN 18
#ifndef CUSTOM_FRAME_SIZE
    #define AX25_MAX_FRAME_LEN 792
#else
    #define AX25_MAX_FRAME_LEN CUSTOM_FRAME_SIZE
#endif

#define AX25_CRC_CORRECT  0xF0B8

#define AX25_CTRL_UI      0x03
#define AX25_PID_NOLAYER3 0xF0

struct AX25Ctx;     // Forward declarations
struct AX25Msg;

#if SERIAL_PROTOCOL == PROTOCOL_KISS
    typedef void (*ax25_callback_t)(struct AX25Ctx *ctx);
#endif

#if SERIAL_PROTOCOL == PROTOCOL_SIMPLE_SERIAL
    typedef void (*ax25_callback_t)(struct AX25Msg *msg);
#endif

typedef struct AX25Ctx {
    uint8_t buf[AX25_MAX_FRAME_LEN];
    Afsk *modem;
    FILE *ch;
    size_t frame_len;
    uint16_t crc_in;
    uint16_t crc_out;
    ax25_callback_t hook;
    bool sync;
    bool escape;
    bool ready_for_data;
} AX25Ctx;

#if SERIAL_PROTOCOL == PROTOCOL_SIMPLE_SERIAL
    #define AX25_CALL(str, id) {.call = (str), .ssid = (id) }
    #define AX25_MAX_RPT 8
    #define AX25_REPEATED(msg, n) ((msg)->rpt_flags & BV(n))

    typedef struct AX25Call {
        char call[6];
        uint8_t ssid;
    } AX25Call;

    typedef struct AX25Msg {
        AX25Call src;
        AX25Call dst;
        AX25Call rpt_list[AX25_MAX_RPT];
        uint8_t  rpt_count;
        uint8_t  rpt_flags;
        uint16_t ctrl;
        uint8_t  pid;
        const uint8_t *info;
        size_t len;
    } AX25Msg;

    void ax25_sendVia(AX25Ctx *ctx, const AX25Call *path, size_t path_len, const void *_buf, size_t len);
    #define ax25_send(ctx, dst, src, buf, len) ax25_sendVia(ctx, ({static AX25Call __path[]={dst, src}; __path;}), 2, buf, len)
    
#endif

void ax25_poll(AX25Ctx *ctx);
void ax25_sendRaw(AX25Ctx *ctx, void *_buf, size_t len);
void ax25_init(AX25Ctx *ctx, Afsk *modem, FILE *channel, ax25_callback_t hook);

#endif