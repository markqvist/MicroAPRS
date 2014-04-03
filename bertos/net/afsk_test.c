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
 *
 * \brief AFSK demodulator test.
 *
 * \author Francesco Sacchi <batt@develer.com>
 * $test$: cp bertos/cfg/cfg_ax25.h $cfgdir/
 * $test$: echo "#undef AX25_LOG_LEVEL" >> $cfgdir/cfg_ax25.h
 * $test$: echo "#define AX25_LOG_LEVEL LOG_LVL_INFO" >> $cfgdir/cfg_ax25.h
 * $test$: cp bertos/cfg/cfg_afsk.h $cfgdir/
 * $test$: echo "#undef CONFIG_AFSK_TX_BUFLEN" >> $cfgdir/cfg_afsk.h
 * $test$: echo "#define CONFIG_AFSK_TX_BUFLEN 512" >> $cfgdir/cfg_afsk.h
 */


#include "afsk.h"
#include "cfg/cfg_afsk.h"

#include <drv/timer.h>
#include <net/ax25.h>

#include <cfg/test.h>
#include <cfg/debug.h>
#include <cfg/kfile_debug.h>

#include <cpu/byteorder.h>

#include <stdio.h>
#include <string.h>

FILE *fp_adc;
FILE *fp_dac;
uint32_t data_size;
uint32_t data_written;
Afsk afsk_fd;
AX25Ctx ax25;
KFileDebug dbg;

int msg_cnt;
static void message_hook(struct AX25Msg *msg)
{
	msg_cnt++;
	ax25_print(&dbg.fd, msg);
}

static FILE *afsk_fileOpen(const char *name)
{
	FILE *fp = 0;
	#if CPU_AVR
		(void)name;
		#warning TODO: open the file?
	#else
		fp = fopen(name, "rb");
	#endif
	ASSERT(fp);

	char snd[5];
	ASSERT(fread(snd, 1, 4, fp) == 4);
	snd[4] = 0;
	ASSERT(strcmp(snd, ".snd") == 0);

	uint32_t offset;
	ASSERT(fread(&offset, 1, sizeof(offset), fp) == sizeof(offset));
	offset = be32_to_cpu(offset);
	kprintf("AU file offset: %ld\n", (long)offset);
	ASSERT(offset >= 24);

	ASSERT(fread(&data_size, 1, sizeof(data_size), fp) == sizeof(data_size));
	data_size = be32_to_cpu(data_size);
	kprintf("AU file data_size: %ld\n", (long)data_size);
	ASSERT(data_size);

	uint32_t encoding;
	ASSERT(fread(&encoding, 1, sizeof(encoding), fp) == sizeof(encoding));
	encoding = be32_to_cpu(encoding);
	kprintf("AU file encoding: %ld\n", (long)encoding);
	ASSERT(encoding == 2); // 8 bit linear PCM

	uint32_t sample_rate;
	ASSERT(fread(&sample_rate, 1, sizeof(sample_rate), fp) == sizeof(sample_rate));
	sample_rate = be32_to_cpu(sample_rate);
	kprintf("AU file sample_rate: %ld\n", (long)sample_rate);
	ASSERT(sample_rate == 9600);

	uint32_t channels;
	ASSERT(fread(&channels, 1, sizeof(channels), fp) == sizeof(channels));
	channels = be32_to_cpu(channels);
	kprintf("AU file channels: %ld\n", (long)channels);
	ASSERT(channels == 1);

	#if CPU_AVR
		#warning TODO: fseek?
	#else
		ASSERT(fseek(fp, offset, SEEK_SET) == 0);
	#endif
	return fp;
}

int afsk_testSetup(void)
{
	kdbg_init();
	kfiledebug_init(&dbg);
	fp_adc = afsk_fileOpen("test/afsk_test.au");
	#if CPU_AVR
		#warning TODO: open the file?
	#else
		fp_dac = fopen("test/afsk_test_out.au", "w+b");
	#endif
	ASSERT(fp_dac);
	#define FS_HH (((uint32_t)CONFIG_AFSK_DAC_SAMPLERATE) >> 24)
	#define FS_HL ((((uint32_t)CONFIG_AFSK_DAC_SAMPLERATE) >> 16) & 0xff)
	#define FS_LH ((((uint32_t)CONFIG_AFSK_DAC_SAMPLERATE) >> 8) & 0xff)
	#define FS_LL (((uint32_t)CONFIG_AFSK_DAC_SAMPLERATE) & 0xff)

	uint8_t snd_header[] = { '.','s','n','d', 0,0,0,24, 0,0,0,0, 0,0,0,2, FS_HH,FS_HL,FS_LH,FS_LL, 0,0,0,1};

	ASSERT(fwrite(snd_header, 1, sizeof(snd_header), fp_dac) == sizeof(snd_header));

	timer_init();
	afsk_init(&afsk_fd, 0 ,0);
	ax25_init(&ax25, &afsk_fd.fd, message_hook);
	return 0;
}


static void messageout_hook(struct AX25Msg *msg)
{
	ASSERT(strncmp(msg->dst.call, "ABCDEF", 6) == 0);
	ASSERT(strncmp(msg->src.call, "123456", 6) == 0);
	ASSERT(msg->src.ssid == 1);
	ASSERT(msg->dst.ssid == 0);
	ASSERT(msg->ctrl == AX25_CTRL_UI);
	ASSERT(msg->pid == AX25_PID_NOLAYER3);
	ASSERT(msg->len == 256);
	for (int i = 0; i < 256; i++)
		ASSERT(msg->info[i] == i);
}

int afsk_testRun(void)
{
	int c;
	while ((c = fgetc(fp_adc)) != EOF)
	{
		afsk_adc_isr(&afsk_fd, (int8_t)c);

		ax25_poll(&ax25);
	}
	kprintf("Messages correctly received: %d\n", msg_cnt);
	ASSERT(msg_cnt >= 15);

	char buf[256];
	for (unsigned i = 0; i < sizeof(buf); i++)
		buf[i] = i;

	ax25_send(&ax25, AX25_CALL("abcdef", 0), AX25_CALL("123456", 1), buf, sizeof(buf));

	do
	{
		int8_t val = afsk_dac_isr(&afsk_fd) - 128;
		ASSERT(fwrite(&val, 1, sizeof(val), fp_dac) == sizeof(val));
		data_written++;
	}
	while (afsk_fd.sending);

	#define SND_DATASIZE_OFF 8
	#if CPU_AVR
		#warning TODO: fseek?
	#else
		ASSERT(fseek(fp_dac, SND_DATASIZE_OFF, SEEK_SET) == 0);
	#endif
	data_written = cpu_to_be32(data_written);
	ASSERT(fwrite(&data_written, 1, sizeof(data_written), fp_dac) == sizeof(data_written));
	ASSERT(fclose(fp_adc) + fclose(fp_dac) == 0);

	fp_adc = afsk_fileOpen("test/afsk_test_out.au");
	ax25_init(&ax25, &afsk_fd.fd, messageout_hook);

	while ((c = fgetc(fp_adc)) != EOF)
	{
		afsk_adc_isr(&afsk_fd, (int8_t)c);

		ax25_poll(&ax25);
	}

	return 0;
}

int afsk_testTearDown(void)
{
	return fclose(fp_adc);
}

TEST_MAIN(afsk);
