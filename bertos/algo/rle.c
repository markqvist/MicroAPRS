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
 * Copyright 2004 Develer S.r.l. (http://www.develer.com/)
 * Copyright 1999, 2000, 2001 Bernie Innocenti <bernie@codewiz.org>
 *
 * -->
 *
 * \brief General-purpose run-length {en,de}coding algorithm (implementation)
 *
 * Original source code from http://www.compuphase.com/compress.htm
 *
 * \author Bernie Innocenti <bernie@codewiz.org>
 */

#include "rle.h"


/**
 * Run-length encode \a len bytes from the \a input buffer
 * to the \a output buffer.
 */
int rle(unsigned char *output, const unsigned char *input, int len)
{
	int count, index, i;
	unsigned char first;
	unsigned char *out;


	out = output;
	count = 0;
	while (count < len)
	{
		index = count;
		first = input[index++];

		/* Scan for bytes identical to the first one */
		while ((index < len) && (index - count < 127) && (input[index] == first))
			index++;

		if (index - count == 1)
		{
			/* Failed to "replicate" the current byte. See how many to copy.
			 */
			while ((index < len) && (index - count < 127))
			{
				/* Avoid a replicate run of only 2-bytes after a literal run.
				 * There is no gain in this, and there is a risc of loss if the
				 * run after the two identical bytes is another literal run.
				 * So search for 3 identical bytes.
				 */
				if ((input[index] == input[index - 1]) &&
					((index > 1) && (input[index] == input[index - 2])))
				{
					/* Reset the index so we can back up these three identical
					 * bytes in the next run.
					 */
					index -= 2;
					break;
				}

				index++;
			}

			/* Output a run of uncompressed bytes: write length and values */
			*out++ = (unsigned char)(count - index);
			for (i = count; i < index; i++)
				*out++ = input[i];
	    }
		else
		{
			/* Output a compressed run: write length and value */
			*out++ = (unsigned char)(index - count);
			*out++ = first;
	    }

		count = index;
	}

	/* Output EOF marker */
	*out++ = 0;

	return (out - output);
}


/**
 * Run-length decode from the \a input buffer to the \a output
 * buffer.
 *
 * \note The output buffer must be large enough to accomodate
 *       all decoded output.
 */
int unrle(unsigned char *output, const unsigned char *input)
{
	signed char count;
	unsigned char *out;
	unsigned char value;


	out = output;

	for (;;)
	{
		count = (signed char)*input++;
		if (count > 0)
		{
			/* replicate run */
			value = *input++;
			while (count--)
				*out++ = value;
		}
		else if (count < 0)
		{
			/* literal run */
			while (count++)
				*out++ = *input++;
		}
		else
			/* EOF */
			break;
	}

	return (out - output);
}
