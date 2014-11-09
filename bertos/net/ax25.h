/**
 * \file
 * <!--
 * This file is part of BeRTOS.
 *
 * Bertos is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * As a special exception, you may use this file as part of a free software
 * library without restriction.  Specifically, if other files instantiate
 * templates or use macros or inline functions from this file, or you compile
 * this file and link it with other files to produce an executable, this
 * file does not by itself cause the resulting executable to be covered by
 * the GNU General Public License.  This exception does not however
 * invalidate any other reasons why the executable file might be covered by
 * the GNU General Public License.
 *
 * Copyright 2009 Develer S.r.l. (http://www.develer.com/)
 *
 * -->
 * \brief Simple AX25 data link layer implementation.
 *
 * For now, only UI frames without any Layer 3 protocol are handled.
 * This however is enough to send/receive APRS packets.
 *
 * \author Francesco Sacchi <batt@develer.com>
 *
 * $WIZ$ module_name = "ax25"
 * $WIZ$ module_configuration = "bertos/cfg/cfg_ax25.h"
 * $WIZ$ module_depends = "kfile", "crc-ccitt"
 */


#ifndef NET_AX25_H
#define NET_AX25_H

#include "config.h"             // Various configuration values
#include "cfg/cfg_ax25.h"

#include <cfg/compiler.h>
#include <io/kfile.h>

/**
 * Maximum size of a AX25 frame.
 */
#define AX25_MIN_FRAME_LEN 18

/**
 * CRC computation on correct AX25 packets should
 * give this result (don't ask why).
 */
#define AX25_CRC_CORRECT 0xF0B8

struct AX25Msg; // fwd declaration
struct AX25Ctx; // fwd declaration

/**
 * Type for AX25 messages callback.
 */
#if SERIAL_PROTOCOL == PROTOCOL_SIMPLE_SERIAL
	typedef void (*ax25_callback_t)(struct AX25Msg *msg);
#endif

#if SERIAL_PROTOCOL == PROTOCOL_KISS
	typedef void (*ax25_callback_t)(struct AX25Ctx *ctx);
#endif


/**
 * AX25 Protocol context.
 */
typedef struct AX25Ctx
{
	uint8_t buf[CONFIG_AX25_FRAME_BUF_LEN]; ///< buffer for received chars
	KFile *ch;        ///< KFile used to access the physical medium
	size_t frm_len;   ///< received frame length.
	uint16_t crc_in;  ///< CRC for current received frame
	uint16_t crc_out; ///< CRC of current sent frame
	ax25_callback_t hook; ///< Hook function to be called when a message is received
	bool sync;   ///< True if we have received a HDLC flag.
	bool escape; ///< True when we have to escape the following char.
} AX25Ctx;


/**
 * AX25 Call sign.
 */
typedef struct AX25Call
{
	char call[6]; ///< Call string, max 6 character
	uint8_t ssid; ///< SSID (secondary station ID) for the call
} AX25Call;

/**
 * Create an AX25Call structure on the fly.
 * \param str callsign, can be 6 characters or shorter.
 * \param id  ssid associated with the callsign.
 */
#define AX25_CALL(str, id) {.call = (str), .ssid = (id) }

/**
 * Maximum number of Repeaters in a AX25 message.
 */
#define AX25_MAX_RPT 8

/*
 * Has to be lesser than 8 in order to fit in one byte
 * change AX25Msg.rpt_flags if you need more repeaters.
 */
STATIC_ASSERT(AX25_MAX_RPT <= 8);

/**
 * AX25 Message.
 * Used to handle AX25 sent/received messages.
 */
typedef struct AX25Msg
{
	AX25Call src;  ///< Source adress
	AX25Call dst;  ///< Destination address
	#if CONFIG_AX25_RPT_LST
	AX25Call rpt_lst[AX25_MAX_RPT]; ///< List of repeaters
	uint8_t rpt_cnt; ///< Number of repeaters in this message
	uint8_t rpt_flags; ///< Has-been-repeated flags for each repeater (bit-mapped)
	#define AX25_REPEATED(msg, idx) ((msg)->rpt_flags & BV(idx))
	#endif
	uint16_t ctrl; ///< AX25 control field
	uint8_t pid;   ///< AX25 PID field
	const uint8_t *info; ///< Pointer to the info field (payload) of the message
	size_t len;    ///< Payload length
} AX25Msg;


#define AX25_CTRL_UI      0x03
#define AX25_PID_NOLAYER3 0xF0

/**
 * \name HDLC flags.
 * These should be moved in
 * a separated HDLC related file one day...
 * \{
 */
#define HDLC_FLAG  0x7E
#define HDLC_RESET 0x7F
#define AX25_ESC   0x1B
/* \} */


/**
 * Declare an AX25 path.
 * \param dst the destination callsign for the path, \see AX25_CALL
 *        for a handy way to create a callsign on the fly.
 * \param src the source callsign for the path, \see AX25_CALL
 *        for a handy way to create a callsign on the fly.
 *
 * Additional optional callsigns can be specified at the end of this macro
 * in order to add repeater callsigns or specific unproto paths.
 *
 * This macro can be used to simply path array declaration.
 * Should be used in this way:
 * \code
 * AX25Call path[] = AX25_PATH(AX25_CALL("abcdef", 0), AX25_CALL("ghjklm", 0), AX25_CALL("wide1", 1), AX25_CALL("wide2", 2));
 * \endcode
 *
 * The declared path can then be passed to ax25_sendVia().
 */
#define AX25_PATH(dst, src, ...) { dst, src, ## __VA_ARGS__ }

void ax25_poll(AX25Ctx *ctx);
void ax25_sendVia(AX25Ctx *ctx, const AX25Call *path, size_t path_len, const void *_buf, size_t len);
void ax25_sendRaw(AX25Ctx *ctx, void *_buf, size_t len);

/**
 * Send an AX25 frame on the channel.
 * \param ctx AX25 context to operate on.
 * \param dst the destination callsign for the frame, \see AX25_CALL
 *        for a handy way to create a callsign on the fly.
 * \param src the source callsign for the frame, \see AX25_CALL
 *        for a handy way to create a callsign on the fly.
 * \param buf payload buffer.
 * \param len length of the payload.
 *
 * \see ax25_sendVia() if you want to send a frame with a specific path.
 */
#define ax25_send(ctx, dst, src, buf, len) ax25_sendVia(ctx, ({static AX25Call __path[]={dst, src}; __path;}), 2, buf, len)
void ax25_init(AX25Ctx *ctx, KFile *channel, ax25_callback_t hook);

void ax25_print(KFile *ch, const AX25Msg *msg);

int ax25_testSetup(void);
int ax25_testTearDown(void);
int ax25_testRun(void);

#endif /* NET_AX25_H */
