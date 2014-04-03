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
 * Copyright 2004, 2005, 2006, 2007 Develer S.r.l. (http://www.develer.com/)
 * Copyright 1999, 2001 Bernie Innocenti <bernie@codewiz.org>
 *
 * -->
 *
 * \brief X-Modem serial transmission protocol (implementation)
 *
 * Supports the CRC-16 and 1K-blocks variants of the standard.
 * \see ymodem.txt for the protocol description.
 *
 * \todo Break xmodem_send() and xmodem_recv() in smaller functions.
 *
 * \author Bernie Innocenti <bernie@codewiz.org>
 * \author Francesco Sacchi <batt@develer.com>
 */


#include "xmodem.h"

#include "cfg/cfg_xmodem.h"

#include <cfg/debug.h>
// Define log settings for cfg/log.h
#define LOG_LEVEL    CONFIG_XMODEM_LOG_LEVEL
#define LOG_FORMAT   CONFIG_XMODEM_LOG_FORMAT
#include <cfg/log.h>


#include <algo/crc.h>

#include <string.h> /* for memset() */

/**
 * \name Protocol control codes
 * \{
 */
#define XM_SOH  0x01  /**< Start Of Header (128-byte block) */
#define XM_STX  0x02  /**< Start Of Header (1024-byte block) */
#define XM_EOT  0x04  /**< End Of Transmission */
#define XM_ACK  0x06  /**< Acknowledge block */
#define XM_NAK  0x15  /**< Negative Acknowledge */
#define XM_C    0x43  /**< Request CRC-16 transmission */
#define XM_CAN  0x18  /**< CANcel transmission */
/*\}*/

#if CONFIG_XMODEM_1KCRC == 1
	#define XM_BUFSIZE       1024  /**< 1024 bytes of block buffer */
#else
	#define XM_BUFSIZE       128   /**< 128 bytes of block buffer */
#endif


#if CONFIG_XMODEM_RECV
/**
 * \brief Receive a file using the XModem protocol.
 *
 * \param ch Channel to use for transfer
 * \param fd Destination file
 *
 * \note This function allocates a large amount of stack (\see XM_BUFSIZE).
 */
bool xmodem_recv(KFile *ch, KFile *fd)
{
	char block_buffer[XM_BUFSIZE]; /* Buffer to hold a block of data */
	int c, i, blocksize;
	int blocknr = 0, last_block_done = 0, retries = 0;
	char *buf;
	uint8_t checksum;
	uint16_t crc;
	bool purge = false;
	bool usecrc = true;


	LOG_INFO("Starting Transfer...\n");
	purge = true;
	kfile_clearerr(ch);

	/* Send initial NAK to start transmission */
	for(;;)
	{
		if (XMODEM_CHECK_ABORT)
		{
			kfile_putc(XM_CAN, ch);
			kfile_putc(XM_CAN, ch);
			LOG_INFO("Transfer aborted\n");
			return false;
		}

		/*
		 * Discard incoming input until a timeout occurs, then send
		 * a NAK to the transmitter.
		 */
		if (purge)
		{
			purge = false;

			if (kfile_error(ch))
			{
				LOG_ERR("Retries %d\n", retries);
			}

			kfile_resync(ch, 200);
			retries++;

			if (retries >= CONFIG_XMODEM_MAXRETRIES)
			{
				kfile_putc(XM_CAN, ch);
				kfile_putc(XM_CAN, ch);
				LOG_INFO("Transfer aborted\n");
				return false;
			}

			/* Transmission start? */
			if (blocknr == 0)
			{
				if (retries < CONFIG_XMODEM_MAXCRCRETRIES)
				{
					LOG_INFO("Request Tx (CRC)\n");
					kfile_putc(XM_C, ch);
				}
				else
				{
					/* Give up with CRC and fall back to checksum */
					usecrc = false;
					LOG_INFO("Request Tx (BCC)\n");
					kfile_putc(XM_NAK, ch);
				}
			}
			else
				kfile_putc(XM_NAK, ch);
		}

		switch (kfile_getc(ch))
		{
		#if XM_BUFSIZE >= 1024
		case XM_STX:  /* Start of header (1024-byte block) */
			blocksize = 1024;
			goto getblock;
		#endif

		case XM_SOH:  /* Start of header (128-byte block) */
			blocksize = 128;
			/* Needed to avoid warning if XM_BUFSIZE < 1024 */

		getblock:
			/* Get block number */
			c = kfile_getc(ch);

			/* Check complemented block number */
			if ((~c & 0xff) != kfile_getc(ch))
			{
				LOG_WARN("Bad blk (%d)\n", c);
				purge = true;
				break;
			}

			/* Determine which block is being sent */
			if (c == (blocknr & 0xff))
			{
				/* Last block repeated */
				LOG_INFO("Repeat blk %d\n", blocknr);
			}
			else if (c == ((blocknr + 1) & 0xff))
			{
				/* Next block */
				LOG_INFO("Recv blk %d\n", ++blocknr);
			}
			else
			{
				/* Sync lost */
				LOG_WARN("Sync lost (%d/%d)\n", c, blocknr);
				purge = true;
				break;
			}

			buf = block_buffer;	/* Reset pointer to start of buffer */
			checksum = 0;
			crc = 0;
			for (i = 0; i < blocksize; i++)
			{
				if ((c = kfile_getc(ch)) == EOF)
				{
					purge = true;
					break;
				}

				/* Store in buffer */
				*buf++ = (char)c;

				/* Calculate block checksum or CRC */
				if (usecrc)
					crc = UPDCRC16(c, crc);
				else
					checksum += (char)c;
			}

			if (purge)
				break;

			/* Get the checksum byte or the CRC-16 MSB */
			if ((c = kfile_getc(ch)) == EOF)
			{
				purge = true;
				break;
			}

			if (usecrc)
			{
				crc = UPDCRC16(c, crc);

				/* Get CRC-16 LSB */
				if ((c = kfile_getc(ch)) == EOF)
				{
					purge = true;
					break;
				}

				crc = UPDCRC16(c, crc);

				if (crc)
				{
					LOG_ERR("Bad CRC: %04x\n", crc);
					purge = true;
					break;
				}
			}
			/* Compare the checksum */
			else if (c != checksum)
			{
				LOG_ERR("Bad sum: %04x/%04x\n", checksum, c);
				purge = true;
				break;
			}

			/*
			 * Avoid flushing the same block twice.
			 * This could happen when the sender does not receive our
			 * acknowledge and resends the same block.
			 */
			if (last_block_done < blocknr)
			{
				/* Call user function to flush the buffer */
				if (kfile_write(fd, block_buffer, blocksize) == (size_t)blocksize)
				{
					/* Acknowledge block and clear error counter */
					kfile_putc(XM_ACK, ch);
					retries = 0;
					last_block_done = blocknr;
				}
				else
				{
					/* User callback failed: abort transfer immediately */
					retries = CONFIG_XMODEM_MAXRETRIES;
					purge = true;
				}
			}
			break;

		case XM_EOT:	/* End of transmission */
			kfile_putc(XM_ACK, ch);
			LOG_INFO("Transfer completed\n");
			return true;

		case EOF: /* Timeout or serial error */
			purge = true;
			break;

		default:
			LOG_INFO("Skipping garbage\n");
			purge = true;
			break;
		}
	} /* End forever */
}
#endif


#if CONFIG_XMODEM_SEND
/**
 * \brief Transmit some data using the XModem protocol.
 *
 * \param ch Channel to use for transfer
 * \param fd Source file
 *
 * \note This function allocates a large amount of stack for
 *       the XModem transfer buffer (\see XM_BUFSIZE).
 */
bool xmodem_send(KFile *ch, KFile *fd)
{
	char block_buffer[XM_BUFSIZE]; /* Buffer to hold a block of data */
	size_t size = -1;
	int blocknr = 1, retries = 0, c, i;
	bool proceed, usecrc = false;
	uint16_t crc;
	uint8_t sum;

	/*
	 * Reading a block can be very slow, so we read the first block early
	 * to avoid receiving double XM_C char.
	 * This could happen if we check for XM_C and then read the block, giving
	 * the receiving device time to send another XM_C char misinterpretating
	 * the blocks sent.
	 */
	size = kfile_read(fd, block_buffer, XM_BUFSIZE);

	kfile_clearerr(ch);
	LOG_INFO("Wait remote host\n");

	for(;;)
	{
		proceed = false;
		do
		{
			if (XMODEM_CHECK_ABORT)
				return false;

			switch (c = kfile_getc(ch))
			{
			case XM_NAK:
				LOG_INFO("Resend blk %d\n", blocknr);
				proceed = true;
				break;

			case XM_C:
				if (c == XM_C)
				{
					LOG_INFO("Tx start (CRC)\n");
					usecrc = true;
				}
				else
				{
					LOG_INFO("Tx start (BCC)\n");
				}

				proceed = true;
				break;

			case XM_ACK:
				/* End of transfer? */
				if (!size)
					return true;

				/* Call user function to read in one block */
				size = kfile_read(fd, block_buffer, XM_BUFSIZE);
				LOG_INFO("Send blk %d\n", blocknr);
				blocknr++;
				retries = 0;
				proceed = true;
				break;

			case EOF:
				kfile_clearerr(ch);
				retries++;
				LOG_INFO("Retries %d\n", retries);
				if (retries <= CONFIG_XMODEM_MAXRETRIES)
					break;
				/* falling through! */

			case XM_CAN:
				LOG_INFO("Transfer aborted\n");
				return false;

			default:
				LOG_INFO("Skipping garbage\n");
				break;
			}
		}
		while (!proceed);

		if (!size)
		{
			kfile_putc(XM_EOT, ch);
			continue;
		}

		/* Pad block with 0xFF if it's partially full */
		memset(block_buffer + size, 0xFF, XM_BUFSIZE - size);

		/* Send block header (STX, blocknr, ~blocknr) */
		#if XM_BUFSIZE == 128
			kfile_putc(XM_SOH, ch);
		#else
			kfile_putc(XM_STX, ch);
		#endif
		kfile_putc(blocknr & 0xFF, ch);
		kfile_putc(~blocknr & 0xFF, ch);

		/* Send block and compute its CRC/checksum */
		sum = 0;
		crc = 0;
		for (i = 0; i < XM_BUFSIZE; i++)
		{
			kfile_putc(block_buffer[i], ch);
			crc = UPDCRC16(block_buffer[i], crc);
			sum += block_buffer[i];
		}

		/* Send CRC/Checksum */
		if (usecrc)
		{
			kfile_putc(crc >> 8, ch);
			kfile_putc(crc & 0xFF, ch);
		}
		else
			kfile_putc(sum, ch);
	}
}
#endif
