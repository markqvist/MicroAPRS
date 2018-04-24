// Based on work by Francesco Sacchi

#include <string.h>
#include <ctype.h>
#include "AX25.h"
#include "protocol/HDLC.h"
#include "util/CRC-CCIT.h"
#include "../hardware/AFSK.h"

#define countof(a) sizeof(a)/sizeof(a[0])
#define MIN(a,b) ({ typeof(a) _a = (a); typeof(b) _b = (b); ((typeof(_a))((_a < _b) ? _a : _b)); })
#define DECODE_CALL(buf, addr) for (unsigned i = 0; i < sizeof((addr)); i++) { char c = (*(buf)++ >> 1); (addr)[i] = (c == ' ') ? '\x0' : c; }
#define AX25_SET_REPEATED(msg, idx, val) do { if (val) { (msg)->rpt_flags |= _BV(idx); } else { (msg)->rpt_flags &= ~_BV(idx) ; } } while(0)

void ax25_init(AX25Ctx *ctx, Afsk *modem, FILE *channel, ax25_callback_t hook) {
    memset(ctx, 0, sizeof(*ctx));
    ctx->ch = channel;
    ctx->modem = modem;
    ctx->hook = hook;
    ctx->crc_in = ctx->crc_out = CRC_CCIT_INIT_VAL;
    ctx->ready_for_data = true;
}

static void ax25_decode(AX25Ctx *ctx) {
    #if SERIAL_PROTOCOL == PROTOCOL_KISS
        if (ctx->hook) ctx->hook(ctx);
    #endif

    #if SERIAL_PROTOCOL == PROTOCOL_SIMPLE_SERIAL
        AX25Msg msg;
        uint8_t *buf = ctx->buf;

        DECODE_CALL(buf, msg.dst.call);
        msg.dst.ssid = (*buf++ >> 1) & 0x0F;

        DECODE_CALL(buf, msg.src.call);
        msg.src.ssid = (*buf >> 1) & 0x0F;

        for (msg.rpt_count = 0; !(*buf++ & 0x01) && (msg.rpt_count < countof(msg.rpt_list)); msg.rpt_count++) {
            DECODE_CALL(buf, msg.rpt_list[msg.rpt_count].call);
            msg.rpt_list[msg.rpt_count].ssid = (*buf >> 1) & 0x0F;
            AX25_SET_REPEATED(&msg, msg.rpt_count, (*buf & 0x80));
        }

        msg.ctrl = *buf++;
        if (msg.ctrl != AX25_CTRL_UI) { return; }

        msg.pid = *buf++;
        if (msg.pid != AX25_PID_NOLAYER3) { return; }

        msg.len = ctx->frame_len - 2 - (buf - ctx->buf);
        msg.info = buf;

        if (ctx->hook) ctx->hook(&msg);        

    #endif
}

void ax25_poll(AX25Ctx *ctx) {
    int c;
    
    while ((c = fgetc(ctx->ch)) != EOF) {
        if (!ctx->escape && c == HDLC_FLAG) {
            if (ctx->frame_len >= AX25_MIN_FRAME_LEN) {
                if (ctx->crc_in == AX25_CRC_CORRECT) {
                    #if OPEN_SQUELCH == true
                        LED_RX_ON();
                    #endif
                    ax25_decode(ctx);
                }
            }
            ctx->sync = true;
            ctx->crc_in = CRC_CCIT_INIT_VAL;
            ctx->frame_len = 0;
            continue;
        }

        if (!ctx->escape && c == HDLC_RESET) {
            ctx->sync = false;
            continue;
        }

        if (!ctx->escape && c == AX25_ESC) {
            ctx->escape = true;
            continue;
        }

        if (ctx->sync) {
            if (ctx->frame_len < AX25_MAX_FRAME_LEN) {
                ctx->buf[ctx->frame_len++] = c;
                ctx->crc_in = update_crc_ccit(c, ctx->crc_in);
            } else {
                ctx->sync = false;
            }
        }
        ctx->escape = false;
    }

}

static void ax25_putchar(AX25Ctx *ctx, uint8_t c)
{
    if (c == HDLC_FLAG || c == HDLC_RESET || c == AX25_ESC) fputc(AX25_ESC, ctx->ch);
    ctx->crc_out = update_crc_ccit(c, ctx->crc_out);
    fputc(c, ctx->ch);
}

void ax25_sendRaw(AX25Ctx *ctx, void *_buf, size_t len) {
    ctx->ready_for_data = false;
    ctx->crc_out = CRC_CCIT_INIT_VAL;
    fputc(HDLC_FLAG, ctx->ch);
    const uint8_t *buf = (const uint8_t *)_buf;
    while (len--) ax25_putchar(ctx, *buf++);

    uint8_t crcl = (ctx->crc_out & 0xff) ^ 0xff;
    uint8_t crch = (ctx->crc_out >> 8) ^ 0xff;
    ax25_putchar(ctx, crcl);
    ax25_putchar(ctx, crch);

    fputc(HDLC_FLAG, ctx->ch);

    ctx->ready_for_data = true;
}

#if SERIAL_PROTOCOL == PROTOCOL_SIMPLE_SERIAL
    static void ax25_sendCall(AX25Ctx *ctx, const AX25Call *addr, bool last){
        unsigned len = MIN(sizeof(addr->call), strlen(addr->call));

        for (unsigned i = 0; i < len; i++) {
            uint8_t c = addr->call[i];
            c = toupper(c);
            ax25_putchar(ctx, c << 1);
        }

        if (len < sizeof(addr->call)) {
            for (unsigned i = 0; i < sizeof(addr->call) - len; i++) {
                ax25_putchar(ctx, ' ' << 1);
            }
        }

        uint8_t ssid = 0x60 | (addr->ssid << 1) | (last ? 0x01 : 0);
        ax25_putchar(ctx, ssid);
    }

    void ax25_sendVia(AX25Ctx *ctx, const AX25Call *path, size_t path_len, const void *_buf, size_t len) {
        const uint8_t *buf = (const uint8_t *)_buf;

        ctx->crc_out = CRC_CCIT_INIT_VAL;
        fputc(HDLC_FLAG, ctx->ch);

        for (size_t i = 0; i < path_len; i++) {
            ax25_sendCall(ctx, &path[i], (i == path_len - 1));
        }

        ax25_putchar(ctx, AX25_CTRL_UI);
        ax25_putchar(ctx, AX25_PID_NOLAYER3);

        while (len--) {
            ax25_putchar(ctx, *buf++);
        }

        uint8_t crcl = (ctx->crc_out & 0xff) ^ 0xff;
        uint8_t crch = (ctx->crc_out >> 8) ^ 0xff;
        ax25_putchar(ctx, crcl);
        ax25_putchar(ctx, crch);

        fputc(HDLC_FLAG, ctx->ch);
    }
#endif