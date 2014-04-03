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
 * Copyright 2003, 2004, 2005, 2008 Develer S.r.l. (http://www.develer.com/)
 *
 * -->
 *
 *
 * \brief Basic "printf", "sprintf" and "fprintf" formatter.
 *
 * This module is 100% reentrant and can be adapted to user-defined routines
 * that needs formatters with special properties like different output
 * channels or new format specifiers.
 *
 * To reduce size in applications not using real numbers or long integers
 * the formatter may be compiled to exclude certain parts.  This is
 * controlled by giving a -D option a compilation time:
 *
 * \code
 *  -D CONFIG_PRINTF=PRINTF_FULL         Full ANSI printf formatter, with some C99 extensions
 *  -D CONFIG_PRINTF=PRINTF_NOFLOAT      Exclude support for floats
 *  -D CONFIG_PRINTF=PRINTF_REDUCED      Simplified formatter (see below)
 *  -D CONFIG_PRINTF=PRINTF_NOMODIFIERS  Exclude "l", "z" and "h" modifiers in reduced version
 *  -D CONFIG_PRINTF=PRINTF_DISABLED     No formatter at all
 * \endcode
 *
 * Code size on AVR4 with GCC 3.4.1 (-O2):
 * \li  PRINTF_FULL        2912byte (0xB60)
 * \li  PRINTF_NOFLOAT     1684byte (0x694)
 * \li  PRINTF_REDUCED      924byte (0x39C)
 * \li  PRINTF_NOMODIFIERS  416byte (0x1A0)
 *
 * Code/data size in words on DSP56K with CodeWarrior 6.0:
 * \li  PRINTF_FULL         1493/45
 * \li  PRINTF_NOFLOAT      795/45
 * \li  PRINTF_REDUCED      482/0
 * \li  PRINTF_NOMODIFIERS  301/0
 *
 * The reduced version of formatter is suitable when program size is critical
 * rather than formatting power.  This routine uses less than 20 bytes of
 * stack space which makes it practical even in systems with less than 256
 * bytes of user RAM.
 *
 * The only formatting specifiers supported by the reduced formatter are:
 * \code
 *    %% %c %s %d %o %x %X and %hd %ho %hx %hX %ld %lo %lx %lX
 * \endcode
 *
 * It means that real variables are not supported as well as field
 * width and precision arguments.
 */


#include "formatwr.h"

#include "cfg/cfg_formatwr.h"  /* CONFIG_ macros */
#include <cfg/debug.h>         /* ASSERT */

#include <cpu/pgm.h>
#include <mware/hex.h>

#ifndef CONFIG_PRINTF_N_FORMATTER
	/** Disable the arcane %n formatter. */
	#define CONFIG_PRINTF_N_FORMATTER 0
#endif

#ifndef CONFIG_PRINTF_OCTAL_FORMATTER
	/** Disable the %o formatter. */
	#define CONFIG_PRINTF_OCTAL_FORMATTER 0
#endif

/* True if we must keep a count of the number of characters we print. */
#define CONFIG_PRINTF_COUNT_CHARS (CONFIG_PRINTF_RETURN_COUNT || CONFIG_PRINTF_N_FORMATTER)

#if CONFIG_PRINTF

#if CONFIG_PRINTF > PRINTF_NOFLOAT
	#include <float.h>

	/* Maximum precision for floating point values */
	typedef long double max_float_t;

	#if CONFIG_FRMWRI_BUFSIZE
		#define FRMWRI_BUFSIZE CONFIG_FRMWRI_BUFSIZE
	#else
		/* Conservative estimate. Max float is 3.40282e+038, so %f (but not %e or %g) must have
		 * space for: sign + all 38 digits + '.' + 6 decimal digits (default)
		 * Use a high value to avoid unexpected buffer overflows.
		 */
		#define FRMWRI_BUFSIZE 134
	#endif
#else
	#if CONFIG_FRMWRI_BUFSIZE
		#define FRMWRI_BUFSIZE CONFIG_FRMWRI_BUFSIZE
	#else
		/*
		 * Conservative estimate. Should be (probably) 12 (which is the size necessary
		 * to represent (2^32-1) in octal plus the sign bit.
		 */
		#define FRMWRI_BUFSIZE 16
	#endif
#endif

/* Probably useful for fancy microcontrollers such as the PIC, nobody knows. */
#ifndef MEM_ATTRIBUTE
#define MEM_ATTRIBUTE
#endif

#if CONFIG_PRINTF > PRINTF_NOMODIFIERS
	#define IS_SHORT (h_modifier || (sizeof(int) == 2 && !l_modifier))
#else
	#define IS_SHORT (sizeof(int) == 2)
#endif /* CONFIG_PRINTF > PRINTF_NOMODIFIERS */


#if CONFIG_PRINTF > PRINTF_NOFLOAT

static char *float_conversion(MEM_ATTRIBUTE max_float_t value,
		MEM_ATTRIBUTE short nr_of_digits,
		MEM_ATTRIBUTE char *buf,
		MEM_ATTRIBUTE char format_flag,
		MEM_ATTRIBUTE char g_flag,
		MEM_ATTRIBUTE bool alternate_flag)
{
	MEM_ATTRIBUTE char *cp;
	MEM_ATTRIBUTE char *buf_pointer;
	MEM_ATTRIBUTE short n, i, dec_point_pos, integral_10_log;

	buf_pointer = buf;
	integral_10_log = 0;

	if (value >= 1)
	{
		while (value >= 1e11) /* To speed up things a bit */
		{
			value /= 1e10;
			integral_10_log += 10;
		}
		while (value >= 10)
		{
			value /= 10;
			integral_10_log++;
		}
	}
	else if (value) /* Not just 0.0 */
	{
		while (value <= 1e-10) /* To speed up things a bit */
		{
			value *= 1e10;
			integral_10_log -= 10;
		}
		while (value < 1)
		{
			value *= 10;
			integral_10_log--;
		}
	}
	if (g_flag)
	{
		if (integral_10_log < nr_of_digits && integral_10_log >= -4)
		{
			format_flag = 0;
			nr_of_digits -= integral_10_log;
		}
		nr_of_digits--;
		if (alternate_flag)
			/* %#G - No removal of trailing zeros */
			g_flag = 0;
		else
			/* %G - Removal of trailing zeros */
			alternate_flag = true;
	}

	/* %e or %E */
	if (format_flag)
	{
		dec_point_pos = 0;
	}
	else
	{
		/* Less than one... */
		if (integral_10_log < 0)
		{
			*buf_pointer++ = '0';
			if ((n = nr_of_digits) || alternate_flag)
				*buf_pointer++ = '.';
			i = 0;
			while (--i > integral_10_log && nr_of_digits)
			{
				*buf_pointer++ = '0';
				nr_of_digits--;
			}
			if (integral_10_log < (-n - 1))
				/* Nothing more to do */
				goto CLEAN_UP;
			dec_point_pos = 1;
		}
		else
		{
			dec_point_pos = - integral_10_log;
		}
	}

	i = dec_point_pos;
	while (i <= nr_of_digits )
	{
		value -= (max_float_t)(n = (short)value); /* n=Digit value=Remainder */
		value *= 10; /* Prepare for next shot */
		*buf_pointer++ = n + '0';
		if ( ! i++ && (nr_of_digits || alternate_flag))
			*buf_pointer++ = '.';
	}

	/* Rounding possible */
	if (value >= 5)
	{
		n = 1; /* Carry */
		cp = buf_pointer - 1;
		do
		{
			if (*cp != '.')
			{
				if ( (*cp += n) == ('9' + 1) )
				{
					*cp = '0';
					n = 1;
				}
				else
					n = 0;
			}
		} while (cp-- > buf);
		if (n)
		{
			/* %e or %E */
			if (format_flag)
			{
				cp = buf_pointer;
				while (cp > buf)
				{
					if (*(cp - 1) == '.')
					{
						*cp = *(cp - 2);
						cp--;
					}
					else
						*cp = *(cp - 1);
					cp--;
				}
				integral_10_log++;
			}
			else
			{
				cp = ++buf_pointer;
				while (cp > buf)
				{
					*cp = *(cp - 1);
					cp--;
				}
			}
			*buf = '1';
		}
	}

CLEAN_UP:
	/* %G - Remove trailing zeros */
	if (g_flag)
	{
		while (*(buf_pointer - 1) == '0')
			buf_pointer--;
		if (*(buf_pointer - 1) == '.')
			buf_pointer--;
	}

	/* %e or %E */
	if (format_flag)
	{
		*buf_pointer++ = format_flag;
		if (integral_10_log < 0)
		{
			*buf_pointer++ = '-';
			integral_10_log = -integral_10_log;
		}
		else
			*buf_pointer++ = '+';
		n = 0;
		buf_pointer +=10;
		do
		{
			n++;
			*buf_pointer++ = (integral_10_log % 10) + '0';
			integral_10_log /= 10;
		} while ( integral_10_log || n < 2 );
		for ( i = n ; n > 0 ; n-- )
			*(buf_pointer - 11 - i + n) = *(buf_pointer - n);
		buf_pointer -= 10;
	}
	return (buf_pointer);
}

#endif /* CONFIG_PRINTF > PRINTF_NOFLOAT */

/**
 * This routine forms the core and entry of the formatter.
 *
 * The conversion performed conforms to the ANSI specification for "printf".
 */
int
PGM_FUNC(_formatted_write)(const char * PGM_ATTR format,
		void put_one_char(char, void *),
		void *secret_pointer,
		va_list ap)
{
#if CONFIG_PRINTF > PRINTF_REDUCED
	MEM_ATTRIBUTE static char bad_conversion[] = "???";
	MEM_ATTRIBUTE static char null_pointer[] = "<NULL>";

	MEM_ATTRIBUTE int precision;
	MEM_ATTRIBUTE int n;
#if CONFIG_PRINTF_COUNT_CHARS
	MEM_ATTRIBUTE int nr_of_chars;
#endif
	MEM_ATTRIBUTE int field_width;
	MEM_ATTRIBUTE char format_flag;
	enum PLUS_SPACE_FLAGS {
		PSF_NONE, PSF_PLUS, PSF_MINUS
	};
	enum DIV_FACTOR {
		DIV_DEC, DIV_HEX,
#if CONFIG_PRINTF_OCTAL_FORMATTER
		DIV_OCT,
#endif
	};
	MEM_ATTRIBUTE struct {
		enum PLUS_SPACE_FLAGS plus_space_flag : 2;
#if CONFIG_PRINTF_OCTAL_FORMATTER
		enum DIV_FACTOR div_factor : 2;
#else
		enum DIV_FACTOR div_factor : 1;
#endif
		bool left_adjust : 1;
		bool l_L_modifier : 1;
		bool h_modifier : 1;
		bool alternate_flag : 1;
		bool nonzero_value : 1;
		bool zeropad : 1;
#if CPU_HARVARD
		bool progmem : 1;
#endif
	} flags;
	MEM_ATTRIBUTE unsigned long ulong;

#if CONFIG_PRINTF >  PRINTF_NOFLOAT
	MEM_ATTRIBUTE max_float_t fvalue;
#endif

	MEM_ATTRIBUTE char *buf_pointer;
	MEM_ATTRIBUTE char *ptr;
	MEM_ATTRIBUTE const char *hex;
	MEM_ATTRIBUTE char buf[FRMWRI_BUFSIZE];

#if CONFIG_PRINTF_COUNT_CHARS
	nr_of_chars = 0;
#endif
	for (;;)    /* Until full format string read */
	{
		while ((format_flag = PGM_READ_CHAR(format++)) != '%')    /* Until '%' or '\0' */
		{
			if (!format_flag)
#if CONFIG_PRINTF_RETURN_COUNT
				return (nr_of_chars);
#else
				return 0;
#endif
			put_one_char(format_flag, secret_pointer);
#if CONFIG_PRINTF_COUNT_CHARS
			nr_of_chars++;
#endif
		}
		if (PGM_READ_CHAR(format) == '%')    /* %% prints as % */
		{
			format++;
			put_one_char('%', secret_pointer);
#if CONFIG_PRINTF_COUNT_CHARS
			nr_of_chars++;
#endif
			continue;
		}

		flags.left_adjust = false;
		flags.alternate_flag = false;
		flags.plus_space_flag = PSF_NONE;
		flags.zeropad = false;
#if CPU_HARVARD
		flags.progmem = false;
#endif
		ptr = buf_pointer = &buf[0];
		hex = HEX_tab;

		/* check for leading '-', '+', ' ','#' or '0' flags  */
		for (;;)
		{
			switch (PGM_READ_CHAR(format))
			{
				case ' ':
					if (flags.plus_space_flag)
						goto NEXT_FLAG;
				case '+':
					flags.plus_space_flag = PSF_PLUS;
					goto NEXT_FLAG;
				case '-':
					flags.left_adjust = true;
					goto NEXT_FLAG;
				case '#':
					flags.alternate_flag = true;
					goto NEXT_FLAG;
				case '0':
					flags.zeropad = true;
					goto NEXT_FLAG;
			}
			break;
NEXT_FLAG:
			format++;
		}

		/* Optional field width (may be '*') */
		if (PGM_READ_CHAR(format) == '*')
		{
			field_width = va_arg(ap, int);
			if (field_width < 0)
			{
				field_width = -field_width;
				flags.left_adjust = true;
			}
			format++;
		}
		else
		{
			field_width = 0;
			while (PGM_READ_CHAR(format) >= '0' && PGM_READ_CHAR(format) <= '9')
				field_width = field_width * 10 + (PGM_READ_CHAR(format++) - '0');
		}

		if (flags.left_adjust)
			flags.zeropad = false;

		/* Optional precision (or '*') */
		if (PGM_READ_CHAR(format) == '.')
		{
			if (PGM_READ_CHAR(++format) == '*')
			{
				precision = va_arg(ap, int);
				format++;
			}
			else
			{
				precision = 0;
				while (PGM_READ_CHAR(format) >= '0' && PGM_READ_CHAR(format) <= '9')
					precision = precision * 10 + (PGM_READ_CHAR(format++) - '0');
			}
		}
		else
			precision = -1;

		/* At this point, "left_adjust" is nonzero if there was
		 * a sign, "zeropad" is 1 if there was a leading zero
		 * and 0 otherwise, "field_width" and "precision"
		 * contain numbers corresponding to the digit strings
		 * before and after the decimal point, respectively,
		 * and "plus_space_flag" is either 0 (no flag) or
		 * contains a plus or space character. If there was no
		 * decimal point, "precision" will be -1.
		 */

		flags.l_L_modifier = false;
		flags.h_modifier = false;

		/* Optional 'l','L','z' or 'h' modifier? */
		switch (PGM_READ_CHAR(format))
		{
			case 'l':
			case 'L':
		#if SIZEOF_SIZE_T == SIZEOF_LONG
			case 'z':
				flags.l_L_modifier = true;
		#elif SIZEOF_SIZE_T == SIZEOF_INT
				flags.l_L_modifier = true;
			case 'z':
		#endif
				format++;
				break;

			case 'h':
				flags.h_modifier = true;
				format++;
				break;

		}

		/*
		 * At exit from the following switch, we will emit
		 * the characters starting at "buf_pointer" and
		 * ending at "ptr"-1
		 */
		switch (format_flag = PGM_READ_CHAR(format++))
		{
#if CONFIG_PRINTF_N_FORMATTER
			case 'n':
				if (sizeof(short) != sizeof(int))
				{
					if (sizeof(int) != sizeof(long))
					{
						if (h_modifier)
							*va_arg(ap, short *) = nr_of_chars;
						else if (flags.l_L_modifier)
							*va_arg(ap, long *) = nr_of_chars;
						else
							*va_arg(ap, int *) = nr_of_chars;
					}
					else
					{
						if (h_modifier)
							*va_arg(ap, short *) = nr_of_chars;
						else
							*va_arg(ap, int *) = nr_of_chars;
					}
				}
				else
				{
					if (flags.l_L_modifier)
						*va_arg(ap, long *) = nr_of_chars;
					else
						*va_arg(ap, int *) = nr_of_chars;
				}
				continue;
#endif
			case 'c':
				buf[0] = va_arg(ap, int);
				ptr++;
				break;

			/* Custom formatter for strings in program memory. */
			case 'S':
#if CPU_HARVARD
				flags.progmem = true;
#endif
				/* Fall trough */

			case 's':
				if ( !(buf_pointer = va_arg(ap, char *)) )
					buf_pointer = null_pointer;
				if (precision < 0)
					precision = 10000;

				/*
				 * Move `ptr' to the last character of the
				 * string that will be actually printed.
				 */
				ptr = buf_pointer;
#if CPU_HARVARD
				if (flags.progmem)
				{
					for (n=0; pgm_read_char(ptr) && n < precision; n++)
						++ptr;
				}
				else
#endif
				for (n=0; *ptr && n < precision; n++)
					++ptr;
				break;

#if CONFIG_PRINTF_OCTAL_FORMATTER
			case 'o':
				if (flags.alternate_flag && !precision)
					precision++;
#endif
			case 'x':
				hex = hex_tab;
			case 'u':
			case 'p':
			case 'X':
				if (format_flag == 'p')
#if defined(__AVR__) || defined(__I196__) || defined(__MSP430__) /* 16bit pointers */
					ulong = (unsigned long)(unsigned short)va_arg(ap, char *);
#else /* 32bit pointers */
					ulong = (unsigned long)va_arg(ap, char *);
#endif /* 32bit pointers */
				else if (flags.l_L_modifier)
					ulong = va_arg(ap, unsigned long);
				else if (flags.h_modifier)
					ulong = (unsigned long)(unsigned short)va_arg(ap, unsigned int);
				else
					ulong = va_arg(ap, unsigned int);

				flags.div_factor =
#if CONFIG_PRINTF_OCTAL_FORMATTER
					(format_flag == 'o') ? DIV_OCT :
#endif
					(format_flag == 'u') ? DIV_DEC : DIV_HEX;
				flags.plus_space_flag = PSF_NONE;
				goto INTEGRAL_CONVERSION;

			case 'd':
			case 'i':
				if (flags.l_L_modifier)
					ulong = (unsigned long)(long)va_arg(ap, long);
				else
					ulong = (unsigned long)(long)va_arg(ap, int);

				/* Extract sign */
				if ((signed long)ulong < 0)
				{
					flags.plus_space_flag = PSF_MINUS;
					ulong = (unsigned long)(-((signed long)ulong));
				}

				flags.div_factor = DIV_DEC;

				/* Now convert to digits */
INTEGRAL_CONVERSION:
				ptr = buf_pointer = &buf[FRMWRI_BUFSIZE - 1];
				flags.nonzero_value = (ulong != 0);

				/* No char if zero and zero precision */
				if (precision != 0 || flags.nonzero_value)
				{
					switch (flags.div_factor)
					{
					case DIV_DEC:
						do
							*--buf_pointer = hex[ulong % 10];
						while (ulong /= 10);
						break;

					case DIV_HEX:
						do
							*--buf_pointer = hex[ulong % 16];
						while (ulong /= 16);
						break;
#if CONFIG_PRINTF_OCTAL_FORMATTER
					case DIV_OCT:
						do
							*--buf_pointer = hex[ulong % 8];
						while (ulong /= 8);
						break;
#endif
					}
				}

				/* "precision" takes precedence */
				if (precision < 0)
					if (flags.zeropad)
						precision = field_width - (flags.plus_space_flag != PSF_NONE);
				while (precision > (int)(ptr - buf_pointer))
					*--buf_pointer = '0';

				if (flags.alternate_flag && flags.nonzero_value)
				{
					if (format_flag == 'x' || format_flag == 'X')
					{
						*--buf_pointer = format_flag;
						*--buf_pointer = '0';
					}
#if CONFIG_PRINTF_OCTAL_FORMATTER
					else if ((format_flag == 'o') && (*buf_pointer != '0'))
					{
						*--buf_pointer = '0';
					}
#endif
				}
				ASSERT(buf_pointer >= buf);
				break;

#if CONFIG_PRINTF > PRINTF_NOFLOAT
			case 'g':
			case 'G':
				n = 1;
				format_flag -= 2;
				if (! precision)
				{
					precision = 1;
				}
				goto FLOATING_CONVERSION;
			case 'f':
				format_flag = 0;
			case 'e':
			case 'E':
				n = 0;
FLOATING_CONVERSION:
				if (precision < 0)
				{
					precision = 6;
				}

				if (sizeof(double) != sizeof(max_float_t))
				{
					fvalue = flags.l_L_modifier ?
						va_arg(ap,max_float_t) : va_arg(ap,double);
				}
				else
					fvalue = va_arg(ap,max_float_t);

				if (fvalue < 0)
				{
					flags.plus_space_flag = PSF_MINUS;
					fvalue = -fvalue;
				}
				ptr = float_conversion (fvalue,
						(short)precision,
						buf_pointer += field_width,
						format_flag,
						(char)n,
						flags.alternate_flag);
				if (flags.zeropad)
				{
					precision = field_width - (flags.plus_space_flag != PSF_NONE);
					while (precision > ptr - buf_pointer)
						*--buf_pointer = '0';
				}
				break;

#endif /* CONFIG_PRINTF <= PRINTF_NOFLOAT */

			case '\0': /* Really bad place to find NUL in */
				format--;

			default:
				/* Undefined conversion! */
				ptr = buf_pointer = bad_conversion;
				ptr += sizeof(bad_conversion) - 1;
				break;

		}

		/*
		 * This part emittes the formatted string to "put_one_char".
		 */

		/* If field_width == 0 then nothing should be written. */
		precision = ptr - buf_pointer;

		if ( precision > field_width)
		{
			n = 0;
		}
		else
		{
			n = field_width - precision - (flags.plus_space_flag != PSF_NONE);
		}

		/* emit any leading pad characters */
		if (!flags.left_adjust)
			while (--n >= 0)
			{
				put_one_char(' ', secret_pointer);
#if CONFIG_PRINTF_COUNT_CHARS
				nr_of_chars++;
#endif
			}

		/* emit flag characters (if any) */
		if (flags.plus_space_flag)
		{
			put_one_char(flags.plus_space_flag == PSF_PLUS ? '+' : '-', secret_pointer);
#if CONFIG_PRINTF_COUNT_CHARS
			nr_of_chars++;
#endif
		}

#if CPU_HARVARD
		if (flags.progmem)
		{
			while (--precision >= 0)
			{
				put_one_char(pgm_read_char(buf_pointer++), secret_pointer);
#if CONFIG_PRINTF_COUNT_CHARS
				nr_of_chars++;
#endif
			}
		}
		else
#endif /* CPU_HARVARD */
		{
			/* emit the string itself */
			while (--precision >= 0)
			{
				put_one_char(*buf_pointer++, secret_pointer);
#if CONFIG_PRINTF_COUNT_CHARS
				nr_of_chars++;
#endif
			}
		}

		/* emit trailing space characters */
		if (flags.left_adjust)
			while (--n >= 0)
			{
				put_one_char(' ', secret_pointer);
#if CONFIG_PRINTF_COUNT_CHARS
				nr_of_chars++;
#endif
			}
	}

#else /* PRINTF_REDUCED starts here */

#if CONFIG_PRINTF > PRINTF_NOMODIFIERS
	bool l_modifier, h_modifier;
	unsigned long u_val, div_val;
#else
	unsigned int u_val, div_val;
#endif /* CONFIG_PRINTF > PRINTF_NOMODIFIERS */

	char format_flag;
	unsigned int nr_of_chars, base;
	char outChar;
	char *ptr;

	nr_of_chars = 0;
	for (;;)    /* Until full format string read */
	{
		while ((format_flag = PGM_READ_CHAR(format++)) != '%')    /* Until '%' or '\0' */
		{
			if (!format_flag)
				return (nr_of_chars);
			put_one_char(format_flag, secret_pointer);
			nr_of_chars++;
		}

#if CONFIG_PRINTF > PRINTF_NOMODIFIERS
		/*
		 * Optional 'l', 'z' or 'h' modifiers?
		 */
		l_modifier = h_modifier = false;
		switch (PGM_READ_CHAR(format))
		{
			case 'l':
		#if SIZEOF_SIZE_T == SIZEOF_LONG
			case 'z':
				l_modifier = true;
		#elif SIZEOF_SIZE_T == SIZEOF_INT
				l_modifier = true;
			case 'z':
		#endif
				format++;
				break;

			case 'h':
				h_modifier = true;
				format++;
				break;
		}
#endif /* CONFIG_PRINTF > PRINTF_NOMODIFIERS */

		switch (format_flag = PGM_READ_CHAR(format++))
		{
			case 'c':
				format_flag = va_arg(ap, int);
			default:
				put_one_char(format_flag, secret_pointer);
				nr_of_chars++;
				continue;

			case 's':
				ptr = va_arg(ap, char *);
				while ((format_flag = *ptr++))
				{
					put_one_char(format_flag, secret_pointer);
					nr_of_chars++;
				}
				continue;

			case 'o':
				base = 8;
				if (IS_SHORT)
					div_val = 0x8000;
				else
					div_val = 0x40000000;
				goto CONVERSION_LOOP;

			case 'd':
				base = 10;
				if (IS_SHORT)
					div_val = 10000;
				else
					div_val = 1000000000;
				goto CONVERSION_LOOP;

			case 'X':
			case 'x':
				base = 16;
				if (IS_SHORT)
					div_val = 0x1000;
				else
					div_val = 0x10000000;

CONVERSION_LOOP:
#if CONFIG_PRINTF > PRINTF_NOMODIFIERS
				if (h_modifier)
				{
					if (format_flag == 'd')
						u_val = (short)va_arg(ap, int);
					else
						u_val = (unsigned short)va_arg(ap, int);
				}
				else if (l_modifier)
					u_val = va_arg(ap, long);
				else
				{
					if (format_flag == 'd')
						u_val = va_arg(ap, int);
					else
						u_val = va_arg(ap, unsigned int);
				}

#else /* CONFIG_PRINTF > PRINTF_NOMODIFIERS */
				u_val = va_arg(ap,int);
#endif /* CONFIG_PRINTF > PRINTF_NOMODIFIERS */
				if (format_flag == 'd')
				{
					if (((int)u_val) < 0)
					{
						u_val = - u_val;
						put_one_char('-', secret_pointer);
						nr_of_chars++;
					}
				}
				while (div_val > 1 && div_val > u_val)
				{
					div_val /= base;
				}
				do
				{
					outChar = (u_val / div_val) + '0';
					if (outChar > '9')
					{
						if (format_flag == 'x')
							outChar += 'a'-'9'-1;
						else
							outChar += 'A'-'9'-1;
					}
					put_one_char(outChar, secret_pointer);
					nr_of_chars++;
					u_val %= div_val;
					div_val /= base;
				}
				while (div_val);

		} /* end switch(format_flag...) */
	}
#endif /* CONFIG_PRINTF > PRINTF_REDUCED */
}

#endif /* CONFIG_PRINTF */
