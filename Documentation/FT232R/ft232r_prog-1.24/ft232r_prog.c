/*
 * ft232r_prog.c by Mark Lord.  Copyright 2010-2013.
 *
 * This is a Linux command-line alternative to the FTDI MProg/FTProg utilities.
 * It is known to work only for FT232R chips at this time.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; see the file LICENSE.txt.  If not, write to
 * the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <ftdi.h>

#define MYVERSION	"1.24"

static struct ftdi_context ftdi;
static int verbose = 0;
static const char *save_path = NULL, *restore_path = NULL;

enum cbus_mode {
	cbus_txden	=  0, 
	cbus_pwren	=  1, 
	cbus_rxled	=  2, 
	cbus_txled	=  3, 
	cbus_txrxled	=  4, 
	cbus_sleep	=  5, 
	cbus_clk48	=  6, 
	cbus_clk24	=  7, 
	cbus_clk12	=  8, 
	cbus_clk6	=  9, 
	cbus_io		= 10, 
	cbus_wr		= 11,
	cbus_rd		= 12,
	cbus_rxf	= 13,
};

static const char *cbus_mode_strings[] = {
	"TxDEN",
	"PwrEn",
	"RxLED",
	"TxLED",
	"TxRxLED",
	"Sleep",
	"Clk48",
	"Clk24",
	"Clk12",
	"Clk6",
	"IO",
	"WR",
	"RD",
	"RxF",
	NULL
};

enum arg_type {
	arg_help,
	arg_dump,
	arg_verbose,
	arg_save,
	arg_restore,
	arg_cbus0,
	arg_cbus1,
	arg_cbus2,
	arg_cbus3,
	arg_cbus4,
	arg_manufacturer,
	arg_product,
	arg_old_serno,
	arg_new_serno,
	arg_self_powered,
	arg_max_bus_power,
	arg_high_current_io,
	arg_suspend_pull_down,
	arg_old_vid,
	arg_old_pid,
	arg_new_vid,
	arg_new_pid,
	arg_invert_txd,
	arg_invert_rxd,
	arg_invert_rts,
	arg_invert_cts,
	arg_invert_dtr,
	arg_invert_dsr,
	arg_invert_dcd,
	arg_invert_ri,
};

static const char *arg_type_strings[] = {
	"--help",
	"--dump",
	"--verbose",
	"--save",
	"--restore",
	"--cbus0",
	"--cbus1",
	"--cbus2",
	"--cbus3",
	"--cbus4",
	"--manufacturer",
	"--product",
	"--old-serial-number",
	"--new-serial-number",
	"--self-powered",
	"--max-bus-power",
	"--high-current-io",
	"--suspend-pull-down",
	"--old-vid",
	"--old-pid",
	"--new-vid",
	"--new-pid",
	"--invert_txd",
	"--invert_rxd",
	"--invert_rts",
	"--invert_cts",
	"--invert_dtr",
	"--invert_dsr",
	"--invert_dcd",
	"--invert_ri",
	NULL
};

static const char *arg_type_help[] = {
	"   # (show this help text)",
	"   # (dump eeprom settings to stdout))",
	"# (show debug info and raw eeprom contents)",
	"   # (save original eeprom contents to file)",
	"# (restore initial eeprom contents from file)",
	"",
	"",
	"",
	"",
	"",
	"     <string>  # (new USB manufacturer string)",
	"          <string>  # (new USB product name string)",
	"<string>  # (current serial number of device to be reprogrammed)",
	"<string>  # (new USB serial number string)",
	"     [on|off]  # (self powered)",
	"    <number>  # (max bus current in milli-amperes)",
	"  [on|off]  # (enable high [6mA @ 5V] drive current on CBUS pins)",
	"[on|off]  # (force I/O pins into logic low state on suspend)",
	"  <number>  # (current vendor id of device to be reprogrammed, eg. 0x0403)",
	"  <number>  # (current product id of device to be reprogrammed, eg. 0x6001)",
	"  <number>  # (new/custom vendor id to be programmed)",
	"  <number>  # (new/custom product id be programmed)",
	" Inverts the current value of TXD",
	" Inverts the current value of RXD",
	" Inverts the current value of RTS",
	" Inverts the current value of CTS",
	" Inverts the current value of DTR",
	" Inverts the current value of DSR",
	" Inverts the current value of DCD",
	"  Inverts the current value of RI",
};

static const char *bool_strings[] = {
	"off",
	"on",
	"0",
	"1",
	"no",
	"yes",
	"disable",
	"enable",
};

struct eeprom_fields {
	unsigned char		byte01;
	unsigned char		high_current_io;	/* bool */
	unsigned char		load_d2xx_driver;	/* bool */
	unsigned char		txd_inverted;		/* bool */
	unsigned char		rxd_inverted;		/* bool */
	unsigned char		rts_inverted;		/* bool */
	unsigned char		cts_inverted;		/* bool */
	unsigned char		dtr_inverted;		/* bool */
	unsigned char		dsr_inverted;		/* bool */
	unsigned char		dcd_inverted;		/* bool */
	unsigned char		ri_inverted;		/* bool */
	unsigned char		pnp_enabled;		/* bool */
	enum cbus_mode		cbus[5];
	unsigned char		extras[112];		/* extra, undefined fields */
	struct ftdi_eeprom	libftdi;		/* stuff known to libftdi */
	unsigned char		BM_type_chip;		/* from libftdi-0.18, missing in 0.19 */

	/* These are not actually eeprom values; here for convenience */
	unsigned short		old_vid;
	unsigned short		old_pid;
	const char		*old_serno;
	unsigned short		new_vid;
	unsigned short		new_pid;
};

static void dumpmem (const char *msg, void *addr, int len)
{
	char *data = addr, hex[3 * 16 + 1], ascii[17];
	unsigned int i, offset = 0;

	if (msg)
		printf("%s:\n", msg);
	for (i = 0; i < len;) {
		unsigned int i16 = i % 16;
		unsigned char c = data[i];
		sprintf(hex + (3 * i16), " %02x", c);
		ascii[i16] = (c < ' ' || c > '~') ? '.' : c;
		if (++i == len || i16 == 15) {
			ascii[i16 + 1] = '\0';
			for (; i16 != 15; ++i16)
				strcat(hex, "   ");
			printf("%04x:%s  %s\n", offset, hex, ascii);
			offset = i;
		}
	}
}

static unsigned short calc_crc (void *addr, int len)
{
	unsigned int i;
	unsigned short crc = 0xaaaa;
	unsigned char *d8 = addr;

	for (i = 0; i < len - 2; i += 2) {
		crc ^= d8[i] | (d8[i+1] << 8);
		crc  = (crc << 1) | (crc >> 15);
	}
	return crc;
}

static void do_deinit (void)
{
	ftdi_deinit(&ftdi);
}

static void do_close (void)
{
	ftdi_usb_close(&ftdi);
}

static unsigned short verify_crc (void *addr, int len)
{
	unsigned short crc    = calc_crc(addr, len);
	unsigned char *d8     = addr;
	unsigned short actual = d8[len-2] | (d8[len-1] << 8);

	if (crc != actual) {
		fprintf(stderr, "Bad CRC: crc=0x%04x, actual=0x%04x\n", crc, actual);
		exit(EINVAL);
	}
	if (verbose) printf("CRC: Okay (0x%04x)\n", crc);
	return crc;
}

static unsigned short update_crc (void *addr, int len)
{
	unsigned short crc = calc_crc(addr, len);
	unsigned char *d8  = addr;

	d8[len-2] = crc;
	d8[len-1] = crc >> 8;
	return crc;
}

static int match_arg (const char *arg, const char **possibles)
{
	int i;

	for (i = 0; possibles[i]; ++i) {
		if (0 == strcasecmp(possibles[i], arg))
			return i;
	}
	fprintf(stderr, "unrecognized arg: \"%s\"\n", arg);
	exit(EINVAL);
	return -1;  /* never reached */
}

static unsigned long unsigned_val (const char *arg, unsigned long max)
{
	unsigned long val;

	errno = 0;
	val = strtoul(arg, NULL, 0);
	if (errno || val > max) {
		fprintf(stderr, "%s: bad value (max=0x%lx)\n", arg, max);
		exit(EINVAL);
	}
	return val;
}

static void ee_dump (struct eeprom_fields *ee)
{
	unsigned int c;

	printf("       eeprom_size = %d\n",	ee->libftdi.size);
	printf("         vendor_id = 0x%04x\n",	ee->libftdi.vendor_id);
	printf("        product_id = 0x%04x\n",	ee->libftdi.product_id);
	printf("      self_powered = %d\n",	ee->libftdi.self_powered);
	printf("     remote_wakeup = %d\n",	ee->libftdi.remote_wakeup);
	printf("suspend_pull_downs = %d\n",	ee->libftdi.suspend_pull_downs);
	printf("     max_bus_power = %d mA\n",	2 * ee->libftdi.max_power);
	printf("      manufacturer = %s\n",	ee->libftdi.manufacturer);
	printf("           product = %s\n",	ee->libftdi.product);
	printf("         serialnum = %s\n",	ee->libftdi.serial);
	printf("   high_current_io = %u\n",	ee->high_current_io);
	printf("  load_d2xx_driver = %u\n",	ee->load_d2xx_driver);
	printf("      txd_inverted = %u\n",	ee->txd_inverted);
	printf("      rxd_inverted = %u\n",	ee->rxd_inverted);
	printf("      rts_inverted = %u\n",	ee->rts_inverted);
	printf("      cts_inverted = %u\n",	ee->cts_inverted);
	printf("      dtr_inverted = %u\n",	ee->dtr_inverted);
	printf("      dsr_inverted = %u\n",	ee->dsr_inverted);
	printf("      dcd_inverted = %u\n",	ee->dcd_inverted);
	printf("       ri_inverted = %u\n",	ee->ri_inverted);

	for (c = 0; c < 5; ++c)
		printf("           cbus[%u] = %s\n", c, cbus_mode_strings[ee->cbus[c]]);

	if (verbose) {
		/* These fields are non-applicable for FT232R devices */
		printf("       usb_version = %d\n",		ee->libftdi.usb_version);
		printf("     use_serialnum = %d    (n/a)\n",	ee->libftdi.use_serial);
		printf("change_usb_version = %d    (n/a)\n",	ee->libftdi.change_usb_version);
		printf("       pnp_enabled = %u    (n/a)\n",	ee->pnp_enabled);
		printf("      BM_type_chip = 0x%02x (n/a)\n",	ee->BM_type_chip);
		printf(" in_is_isochronous = %d    (n/a)\n",	ee->libftdi.in_is_isochronous);
		printf("out_is_isochronous = %d    (n/a)\n",	ee->libftdi.out_is_isochronous);
	}
};

static unsigned int calc_extras_offset (unsigned char *eeprom)
{
	unsigned int str1 = (eeprom[0x0e] & 0x7f) + eeprom[0x0f];
	unsigned int str2 = (eeprom[0x10] & 0x7f) + eeprom[0x11];
	unsigned int str3 = (eeprom[0x12] & 0x7f) + eeprom[0x13];
	unsigned int offset;

	if (str3 > str2)
		offset = (str3 > str1) ? str3 : str1;
	else
		offset = (str2 > str1) ? str2 : str1;
	return offset;
}

static unsigned int encode_string (void *eeprom, int desc, int offset, char *s)
{
	unsigned char c, *u8 = eeprom, slen = (strlen(s) + 1) * 2;

	if (!s || !*s)
		return offset;
	u8[desc + 0] = offset | 0x80;	/* offset of string */
	u8[desc + 1] = slen;		/* length */

	u8[offset++] = slen;	/* length */
	u8[offset++] = 0x03;	/* "type" == string */
	while ((c = *s++)) {
		u8[offset++] = c;
		u8[offset++] = 0;
	}
	return offset;
}

static void ft232r_eprom_build (struct eeprom_fields *ee, unsigned char *eeprom)
{
	unsigned int len = ee->libftdi.size;
	int offset = 0x18;

	memset(eeprom, 0, len);
	if (strlen(ee->libftdi.serial) > 16) {
		fprintf(stderr, "Serial number string exceeds limit of 16 chars, aborting.\n");
		exit(EINVAL);
	}
	if ((strlen(ee->libftdi.manufacturer) + strlen(ee->libftdi.product) + strlen(ee->libftdi.serial)) > 46) {
		fprintf(stderr, "Total string sizes exceed limit of 46 chars, aborting.\n");
		exit(EINVAL);
	}
	offset = encode_string(eeprom, 0x0e, offset, ee->libftdi.manufacturer);
	offset = encode_string(eeprom, 0x10, offset, ee->libftdi.product);
	offset = encode_string(eeprom, 0x12, offset, ee->libftdi.serial);
	eeprom[0x02] = ee->libftdi.vendor_id;
	eeprom[0x03] = ee->libftdi.vendor_id >> 8;
	eeprom[0x04] = ee->libftdi.product_id;
	eeprom[0x05] = ee->libftdi.product_id >> 8;
	eeprom[0x07] = ee->BM_type_chip;
	eeprom[0x08] = 0x80;
	if (ee->libftdi.remote_wakeup)
		eeprom[0x08] |= 0x20;
	if (ee->libftdi.self_powered)
		eeprom[0x08] |= 0x40;
	eeprom[0x09] = ee->libftdi.max_power;
	if (ee->libftdi.in_is_isochronous)
		eeprom[0x0a] |= 0x01;
	if (ee->libftdi.out_is_isochronous)
		eeprom[0x0a] |= 0x02;
	if (ee->libftdi.suspend_pull_downs)
		eeprom[0x0a] |= 0x04;
	if (ee->libftdi.use_serial)
		eeprom[0x0a] |= 0x08;
	if (ee->libftdi.change_usb_version)
		eeprom[0x0a] |= 0x10;
	eeprom[0x0c] = ee->libftdi.usb_version;
	eeprom[0x0d] = ee->libftdi.usb_version >> 8;
}

/*
 * There are some undefined "extra features" bytes after the strings.
 * So blindly preserve them from the original eeprom image.
 */
static void ee_encode_extras (unsigned char *eeprom, int len, struct eeprom_fields *ee)
{
	unsigned int extras_offset = calc_extras_offset(eeprom);

	memcpy(eeprom + extras_offset, ee->extras, len - extras_offset - 2);
	if (ee->pnp_enabled)
		eeprom[extras_offset + 2] |=  1;
	else
		eeprom[extras_offset + 2] &= ~1;
}

static unsigned short ee_encode (unsigned char *eeprom, int len, struct eeprom_fields *ee)
{
	int ret;

	memset(eeprom, 0, len);
	ee->libftdi.size = len;

	if (ee->new_vid)
		ee->libftdi.vendor_id  = ee->new_vid;
	if (ee->new_pid)
		ee->libftdi.product_id = ee->new_pid;

	/* Unfortunately, ftdi_eeprom_build() is buggy and puts things in the wrong places */
	if (0) {
		ret = ftdi_eeprom_build(&ee->libftdi, eeprom);
		if (ret < 0) {
			fprintf(stderr, "ftdi_eeprom_build() failed, ret=%d\n", ret);
			exit(EINVAL);
		}
		printf("ftdi_eeprom_build() ret=%d\n", ret);
	} else {
		ft232r_eprom_build(ee, eeprom);
	}
	eeprom[1] = ee->byte01;
	if (ee->high_current_io)
		eeprom[0x00] |= 0x04;
	if (ee->load_d2xx_driver)
		eeprom[0x00] |= 0x08;
	if (ee->txd_inverted)
		eeprom[0x0b] |= 0x01;
	if (ee->rxd_inverted)
		eeprom[0x0b] |= 0x02;
	if (ee->rts_inverted)
		eeprom[0x0b] |= 0x04;
	if (ee->cts_inverted)
		eeprom[0x0b] |= 0x08;
	if (ee->dtr_inverted)
		eeprom[0x0b] |= 0x10;
	if (ee->dsr_inverted)
		eeprom[0x0b] |= 0x20;
	if (ee->dcd_inverted)
		eeprom[0x0b] |= 0x40;
	if (ee->ri_inverted)
		eeprom[0x0b] |= 0x80;
	eeprom[0x14] = (ee->cbus[1] << 4) | ee->cbus[0];
	eeprom[0x15] = (ee->cbus[3] << 4) | ee->cbus[2];
	eeprom[0x16] = ee->cbus[4];
	ee_encode_extras(eeprom, len, ee);
	return update_crc(eeprom, len);
}

/*
 * There are some undefined "extra features" bytes after the strings.
 * So blindly preserve them from the original eeprom image.
 */
static void ee_decode_extras (unsigned char *eeprom, int len, struct eeprom_fields *ee)
{
	unsigned int extras_offset = calc_extras_offset(eeprom);

	memcpy(ee->extras, eeprom + extras_offset, len - extras_offset - 2);
	ee->pnp_enabled = eeprom[extras_offset + 2] & 0x01;
}

static void ee_decode (unsigned char *eeprom, int len, struct eeprom_fields *ee)
{
	memset(ee, 0, sizeof(*ee));
	if (eeprom[0] & 0x04)
		ee->high_current_io = 1;
	if (eeprom[0x00] & 0x08)
		ee->load_d2xx_driver = 1;
	ee->byte01 = eeprom[0x01];
	if (eeprom[0x0b] & 0x01)
		ee->txd_inverted = 1;
	if (eeprom[0x0b] & 0x02)
		ee->rxd_inverted = 1;
	if (eeprom[0x0b] & 0x04)
		ee->rts_inverted = 1;
	if (eeprom[0x0b] & 0x08)
		ee->cts_inverted = 1;
	if (eeprom[0x0b] & 0x10)
		ee->dtr_inverted = 1;
	if (eeprom[0x0b] & 0x20)
		ee->dsr_inverted = 1;
	if (eeprom[0x0b] & 0x40)
		ee->dcd_inverted = 1;
	if (eeprom[0x0b] & 0x80)
		ee->ri_inverted  = 1;
	ee->cbus[0] = eeprom[0x14] & 0xf;
	ee->cbus[1] = eeprom[0x14] >> 4;
	ee->cbus[2] = eeprom[0x15] & 0xf;
	ee->cbus[3] = eeprom[0x15] >> 4;
	ee->cbus[4] = eeprom[0x16] & 0xf;
	ee_decode_extras(eeprom, len, ee);

	/* Use libftdi to decode the remaining fields, which it knows about */
	if (ftdi_eeprom_decode(&ee->libftdi, eeprom, len)) {
		fprintf(stderr, "ftdi_eeprom_decode() failed\n");
		exit(EINVAL);
	}
	ee->BM_type_chip = eeprom[0x07];	/* buggy ftdi_eeprom_decode() */
	if (eeprom[0x0a] & 0x10)		/* more buggy ftdi_eeprom_decode() */
		ee->libftdi.change_usb_version = 1;
	else
		ee->libftdi.change_usb_version = 0;
	ee->libftdi.usb_version = (eeprom[0x0d] << 8) | eeprom[0x0c];;
}

static const char *myname;

static void show_help (FILE *fp)
{
	int i;

	fprintf(fp, "\nUsage:  %s [<arg> <val>]..\n", myname);
	fprintf(fp, "\nwhere <arg> must be any of:\n");

	for (i = 0; arg_type_strings[i]; ++i) {
		const char *val = arg_type_help[i];
		fprintf(fp, "    %s", arg_type_strings[i]);
		if (val) {
			if (*val) {
				fprintf(fp, "  %s", val);
			} else {  /* cbus args */
				int j;
				fprintf(fp, "  [");
				for (j = 0; cbus_mode_strings[j];) {
					fprintf(fp, "%s", cbus_mode_strings[j]);
					if (cbus_mode_strings[++j])
						fprintf(fp, "|");
				}
				fprintf(fp, "]");
			}
		}
		fputc('\n', fp);
	}
	fputc('\n', fp);
}

static unsigned short ee_read_and_verify (void *eeprom, int len)
{
	if (ftdi_read_eeprom(&ftdi, eeprom)) {
		fprintf(stderr, "ftdi_read_eeprom() failed: %s\n", ftdi_get_error_string(&ftdi));
		exit(EIO);
	}
	return verify_crc(eeprom, len);
}

static void process_args (int argc, char *argv[], struct eeprom_fields *ee)
{
	int i;

	for (i = 1; i < argc;) {
		int arg;
		arg = match_arg(argv[i++], arg_type_strings);
		switch (arg) {
		case arg_help:
			show_help(stdout);
			exit(1);
		case arg_dump:
			continue;
		case arg_verbose:
			verbose = 1;
			continue;
		case arg_invert_txd:
			ee->txd_inverted = !ee->txd_inverted;
			continue;
		case arg_invert_rxd:
			ee->rxd_inverted = !ee->rxd_inverted;
			continue;
		case arg_invert_rts:
			ee->rts_inverted = !ee->rts_inverted;
			continue;
		case arg_invert_cts:
			ee->cts_inverted = !ee->cts_inverted;
			continue;
		case arg_invert_dtr:
			ee->dtr_inverted = !ee->dtr_inverted;
			continue;
		case arg_invert_dsr:
			ee->dsr_inverted = !ee->dsr_inverted;
			continue;
		case arg_invert_dcd:
			ee->dcd_inverted = !ee->dcd_inverted;
			continue;
		case arg_invert_ri:
			ee->ri_inverted = !ee->ri_inverted;
			continue;
		}
		if (i == argc) {
			fprintf(stderr, "%s: missing %s value\n", argv[i-2], argv[i-1]);
			exit(EINVAL);
		}
		switch (arg) {
		case arg_save:
			save_path = argv[i++];
			break;
		case arg_restore:
			restore_path = argv[i++];
			break;
		case arg_cbus0:
		case arg_cbus1:
		case arg_cbus2:
		case arg_cbus3:
		case arg_cbus4:
			ee->cbus[arg - arg_cbus0] = match_arg(argv[i++], cbus_mode_strings);
			break;
		case arg_manufacturer:
			ee->libftdi.manufacturer = argv[i++];
			break;
		case arg_product:
			ee->libftdi.product = argv[i++];
			break;
		case arg_new_serno:
			ee->libftdi.serial = argv[i++];
			break;
		case arg_high_current_io:
			ee->high_current_io = match_arg(argv[i++], bool_strings) & 1;
			break;
		case arg_self_powered:
			ee->libftdi.self_powered = match_arg(argv[i++], bool_strings) & 1;
			break;
		case arg_max_bus_power:
			ee->libftdi.max_power = unsigned_val(argv[i++], 0x1ff) / 2;
			break;
		case arg_suspend_pull_down:
			ee->libftdi.suspend_pull_downs = unsigned_val(argv[i++], 0xff);
			break;
		case arg_old_vid:
			ee->old_vid = unsigned_val(argv[i++], 0xffff);
			break;
		case arg_old_pid:
			ee->old_pid = unsigned_val(argv[i++], 0xffff);
			break;
		case arg_old_serno:
			ee->old_serno = argv[i++];
			break;
		case arg_new_vid:
			ee->new_vid = unsigned_val(argv[i++], 0xffff);
			break;
		case arg_new_pid:
			ee->new_pid = unsigned_val(argv[i++], 0xffff);
			break;
		default:
			fprintf(stderr, "bad args\n");
			exit(EINVAL);
		}
	}
}

static void save_eeprom_to_file (const char *path, void *eeprom, int len)
{
	int count, fd = open(path, O_CREAT|O_WRONLY|O_TRUNC, 0644);

	if (fd == -1) {
		int err = errno;
		perror(path);
		exit(err);
	}
	count = write(fd, eeprom, len);
	if (count < 0) {
		int err = errno;
		perror(path);
		exit(err);
	}
	close(fd);
	if (count != len) {
		fprintf(stderr, "%s: wrong size, wrote %d/%d bytes\n", path, count, len);
		exit(EINVAL);
	}
	printf("%s: wrote %d bytes\n", path, count);
}

static void restore_eeprom_from_file (const char *path, void *eeprom, int len, int max)
{
	int count, fd = open(path, O_RDONLY);

	if (fd == -1) {
		int err = errno;
		perror(path);
		exit(err);
	}
	count = read(fd, eeprom, max);
	if (count < 0) {
		int err = errno;
		perror(path);
		exit(err);
	}
	close(fd);
	if (count != len ) {
		fprintf(stderr, "%s: wrong size, read %d/%d bytes\n", path, count, len);
		exit(EINVAL);
	}
	printf("%s: read %d bytes\n", path, count);
	verify_crc(eeprom, len);
}

int main (int argc, char *argv[])
{
	const char *slash;
	unsigned char old[256] = {0,}, new[256] = {0,};
	unsigned short new_crc;
	struct eeprom_fields ee;
	unsigned int len = 128;

	myname = argv[0];
	slash = strrchr(myname, '/');
	if (slash)
		myname = slash + 1;

	printf("\n%s: version %s, by Mark Lord.\n", myname, MYVERSION);
	if (argc < 2) {
		show_help(stdout);
		exit(0);
	}

	ftdi_init(&ftdi);
	atexit(&do_deinit);

	memset(&ee, 0, sizeof(ee));
	ee.old_vid = 0x0403;;	/* default; override with --old_vid arg */
	ee.old_pid = 0x6001;	/* default; override with --old_pid arg */
	process_args(argc, argv, &ee);	/* handle --help and --old-* args */

	if (ftdi_usb_open_desc(&ftdi, ee.old_vid, ee.old_pid, NULL, ee.old_serno)) {
		fprintf(stderr, "ftdi_usb_open() failed for %04x:%04x:%s %s\n",
			ee.old_vid, ee.old_pid, ee.old_serno ? ee.old_serno : "", ftdi_get_error_string(&ftdi));
		exit(ENODEV);
	}
	atexit(&do_close);

	/* First, read the original eeprom from the device */
	(void) ee_read_and_verify(old, len);
	if (verbose) dumpmem("existing eeprom", old, len);

	/* Save old contents to a file, if requested (--save) */
	if (save_path)
		save_eeprom_to_file(save_path, old, len);

	/* Restore contents from a file, if requested (--restore) */
	if (restore_path) {
		restore_eeprom_from_file(restore_path, new, len, sizeof(new));
		if (verbose) dumpmem(restore_path, new, len);
		/* Decode file contents into ee struct */
		ee_decode(new, len, &ee);
	} else {
		/* Decode eeprom contents into ee struct */
		ee_decode(old, len, &ee);

		/* Reencode without any changes, to ensure we can reconstruct the original eeprom from ee */
		new_crc = ee_encode(new, len, &ee);
		if (memcmp(old, new, len)) {
			if (verbose) dumpmem("reconstructed eeprom", new, len);
			fprintf(stderr, "eeprom reconstruction self-test failed, aborting.\n");
			exit(EINVAL);
		}
	}

	/* process args, and dump new settings */
	process_args(argc, argv, &ee);	/* Handle value-change args */
	ee_dump(&ee);

	/* Build new eeprom image */
	new_crc = ee_encode(new, len, &ee);

	/* If different from original, then write it back to the device */
	if (0 == memcmp(old, new, len)) {
		printf("No change from existing eeprom contents.\n");
	} else {
		if (verbose) dumpmem("new eeprom", new, len);
		printf("Rewriting eeprom with new contents.\n");
		if (ftdi_write_eeprom(&ftdi, new)) {
			fprintf(stderr, "ftdi_write_eeprom() failed: %s\n", ftdi_get_error_string(&ftdi));
			exit(EIO);
		}
		/* Read it back again, and check for differences */
		if (ee_read_and_verify(new, len) != new_crc) {
			fprintf(stderr, "Readback test failed, results may be botched\n");
			exit(EINVAL);
		}
		ftdi_usb_reset(&ftdi);  /* reset the device to force it to load the new settings */
	}
	return 0;
}
