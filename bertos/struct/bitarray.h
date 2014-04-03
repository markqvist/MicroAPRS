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
 * Copyright 2010 Develer S.r.l. (http://www.develer.com/)
 * -->
 *
 * \brief Bitarray module
 *
 * \author Daniele Basile <asterix@develer.com>
 *
 * $WIZ$ module_name = "bitarray"
 */

#ifndef STRUCT_BITARRAY_H
#define STRUCT_BITARRAY_H

#include <cfg/compiler.h>
#include <cfg/macros.h>
#include <cfg/debug.h>

#include <cpu/types.h>

typedef struct BitArray
{
	size_t size;          /// Size in bytes of the bitarray
	size_t bitarray_len;  /// Number of bits used
	uint8_t *array;       /// Pointer to memory occupied by the bitarray
} BitArray;

/**
 * Convenience macro to create a memory area for the BitArray.
 * \param name Name of the variable.
 * \param size Number of bits requested. It will be rounded to the nearest
 *             byte
 */
#define BITARRAY_ALLOC(name, size)   uint8_t name[DIV_ROUNDUP((size), 8)]

/**
 * Set one bit into the bit array.
 * \param bitx BitArray context
 * \param idx The bit to set
 */
INLINE void bitarray_set(BitArray *bitx, int idx)
{
	ASSERT((size_t)idx <= bitx->bitarray_len);

	int page = idx / 8;
	uint8_t bit = idx % 8;

	bitx->array[page] |= BV(bit);
}

/**
 * Clear one bit in the bit array.
 * \param bitx BitArray context
 * \param idx The bit to clear
 */
INLINE void bitarray_clear(BitArray *bitx, int idx)
{
	ASSERT((size_t)idx <= bitx->bitarray_len);

	int page = idx / 8;
	uint8_t bit = idx % 8;

	bitx->array[page] &= ~BV(bit);
}

/**
 * Set a range of bits.
 *
 * The range starts from \a idx (inclusive) and spans \a offset bits.
 *
 * \param bitx BitArray context
 * \param idx Starting bit
 * \param offset Number of bit to set
 */
INLINE void bitarray_setRange(BitArray *bitx, int idx, int offset)
{
	ASSERT((size_t)idx <= bitx->bitarray_len);

	for (int i = idx; i < offset + idx; i++)
		bitarray_set(bitx, i);
}

/**
 * Clear a range of bits.
 *
 * The range starts from \a idx (inclusive) and spans \a offset bits.
 *
 * \param bitx BitArray context
 * \param idx Starting bit
 * \param offset Number of bits to clear
 */
INLINE void bitarray_clearRange(BitArray *bitx, int idx, int offset)
{
	ASSERT((size_t)idx <= bitx->bitarray_len);

	for (int i = idx; i < offset + idx; i++)
		bitarray_clear(bitx, i);
}

/**
 * Test a bit.
 *
 * \param bitx BitArray context
 * \param idx Bit to test
 * \return True if bit is set, false otherwise.
 */
INLINE bool bitarray_test(BitArray *bitx, int idx)
{
	ASSERT((size_t)idx <= bitx->bitarray_len);
	int page = idx / 8;
	uint8_t bit = idx % 8;

	return (bitx->array[page] & BV(bit));
}

/**
 * Check if the bitarray is full
 *
 * Only \a bitarray_len bits are tested.
 *
 * \param bitx BitArray to test
 * \return True if \a bitx is full, false otherwise
 */
INLINE bool bitarray_isFull(BitArray *bitx)
{
	// test full bytes except the last one
	for (size_t page = 0; page <= bitx->size - 2; page++)
	{
		if (!(bitx->array[page] == 0xff))
			return 0;
	}
	// test the last byte using the correct bitmask
	uint8_t mask = BV(bitx->bitarray_len >> 3) - 1;
	if (!(bitx->array[bitx->size - 1] & mask))
		return 0;

	return 1;
}

/*
 * Ugly!.. reformat it.
 */
/**
 * Test if a range of bit is full.
 *
 * \param bitx BitArray context
 * \param idx Starting bit
 * \param offset Number of bits to test
 * \return True if range is full, false otherwise
 */
INLINE bool bitarray_isRangeFull(BitArray *bitx, int idx, int offset)
{
	ASSERT((size_t)(idx + offset) <= bitx->bitarray_len);

	for (int i = idx; i <= idx + offset; i++)
		if (!bitarray_test(bitx, i))
			return 0;

	return 1;
}

/*
 * Ugly!.. reformat it.
 */
/**
 * Test if a range of bit is empty.
 *
 * \param bitx BitArray context
 * \param idx Starting bit
 * \param offset Number of bits to test
 * \return True if range is empty, false otherwise
 */
INLINE bool bitarray_isRangeEmpty(BitArray *bitx, int idx, int offset)
{
	ASSERT((size_t)(idx + offset) <= bitx->bitarray_len);

	for (int i = idx; i <= idx + offset; i++)
		if (bitarray_test(bitx, i))
			return 0;

	return 1;
}

/**
 * Print on debug serial a BitArray.
 * \note This module does not use the logging module, so you
 *       can't decide the logging level.
 * \param bitx BitArray to be printed.
 */
INLINE void bitarray_dump(BitArray *bitx)
{
	kprintf("bitarray size[%zu]bits on [%zu]bytes\n", bitx->bitarray_len, bitx->size);

	int i = 0;
	int j = 0;
	int count = bitx->bitarray_len;

	while (count--)
	{
		kprintf("%d", bitarray_test(bitx, i++));
		if (j == 7)
		{
			kprintf("..%02x [%d]\n", bitx->array[(i / 8) - 1], i);
			j = 0;
			continue;
		}
		j++;
	}

	if (j != 0)
		kprintf("..%02x [%d]\n", bitx->array[i / 8], i);
}

/**
 * Init a BitArray.
 *
 * The BitArray uses an external array for storage. You can use the macro
 * BITARRAY_ALLOC to declare an appropriate memory size. Example usage:
 * \code
 * BITARRAY_ALLOC(bits_mem, 17);
 * BitArray bits;
 * bitarray_init(&bits, 17, bits_mem, sizeof(bits_mem))
 * \endcode
 *
 * \param bitx BitArray context
 * \param bitarray_len Number of bits in the BitArray
 * \param array Memory area for the BitArray
 * \param size Size (in bytes) of the memory area \a array
 */
INLINE void bitarray_init(BitArray *bitx, size_t bitarray_len, uint8_t *array, size_t size)
{
	bitx->size = size;
	bitx->array = array;
	bitx->bitarray_len = bitarray_len;
}


int bitarray_testSetup(void);
int bitarray_testRun(void);
int bitarray_testTearDown(void);

#endif /* STRUCT_BITARRAY_H */
