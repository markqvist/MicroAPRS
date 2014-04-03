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
 * \defgroup io_kblock KBlock interface
 * \ingroup core
 * \{
 *
 * \brief KBlock interface
 *
 * A block device is a device which can only be read/written
 * with data blocks of constant size: flash memories,
 * SD cards, hard disks, etc...
 * This interface is designed to adapt to most block devices and
 * use peculiar features in order to save CPU time and memory space.
 *
 * There is no init function because you do not have to use this
 * structure directly, specific implementations will supply their own init
 * functions.
 *
 * Error handling is done in a way similar to standard C library: whenever a
 * function (eg. kblock_flush()) returns error, you need to check the error
 * code, which is implementation specific.
 *
 * Example of code flow:
 * \code
 * // init a KBlock-derived class
 * Flash fls;
 * flash_init(&fls.blk, 0);
 *
 * // use kblock_* functions to access the derived class
 * kblock_write(&fls.blk, ...);
 * if (kblock_flush(&fls.blk) == EOF)
 * {
 *     // oops, error occurred!
 *     int err = kblock_error(&fls.blk);
 *     // handle Flash specific error conditions
 *     // ...
 *     // clear error condition
 *     kblock_clearerr(&fls.blk);
 * }
 * \endcode
 *
 * \note The KBlock interface is optimized for block reads. If you need a
 * file-like access, you can use \ref kfile_block.
 *
 * \author Francesco Sacchi <batt@develer.com>
 *
 * $WIZ$ module_name = "kblock"
 */

#ifndef IO_KBLOCK_H
#define IO_KBLOCK_H

#include <cfg/compiler.h>
#include <cfg/debug.h>
#include <cfg/macros.h>

/** Type for addressing blocks in the device. */
typedef uint32_t block_idx_t;

// Fwd Declaration
struct KBlock;

/**
 * \name Prototypes for KBlock low level access functions.
 *
 * When writing a driver implementing the KBlock interface you can choose which
 * function subset to implement, but you have to set to NULL unimplemented
 * features.
 *
 *  \{
 */
typedef size_t (* kblock_read_direct_t)  (struct KBlock *b, block_idx_t index, void *buf, size_t offset, size_t size);
typedef size_t (* kblock_write_direct_t) (struct KBlock *b, block_idx_t index, const void *buf, size_t offset, size_t size);

typedef size_t (* kblock_read_t)        (struct KBlock *b, void *buf, size_t offset, size_t size);
typedef size_t (* kblock_write_t)       (struct KBlock *b, const void *buf, size_t offset, size_t size);
typedef int    (* kblock_load_t)        (struct KBlock *b, block_idx_t index);
typedef int    (* kblock_store_t)       (struct KBlock *b, block_idx_t index);

typedef int    (* kblock_error_t)       (struct KBlock *b);
typedef void   (* kblock_clearerr_t)    (struct KBlock *b);
typedef int    (* kblock_close_t)       (struct KBlock *b);
/* \} */

/*
 * Table of interface functions for a KBlock device.
 */
typedef struct KBlockVTable
{
	kblock_read_direct_t readDirect;
	kblock_write_direct_t writeDirect;

	kblock_read_t  readBuf;
	kblock_write_t writeBuf;
	kblock_load_t  load;
	kblock_store_t store;

	kblock_error_t    error;    // \sa kblock_error()
	kblock_clearerr_t clearerr; // \sa kblock_clearerr()

	kblock_close_t  close; // \sa kblock_close()
} KBlockVTable;


#define KB_BUFFERED        BV(0) ///< Internal flag: true if the KBlock has a buffer
#define KB_CACHE_DIRTY     BV(1) ///< Internal flag: true if the cache is dirty
#define KB_PARTIAL_WRITE   BV(2) ///< Internal flag: true if the device allows partial block write


/*
 * KBlock private members.
 * These are the private members of the KBlock interface, please do not
 * access these directly, use the KBlock API.
 */
typedef struct KBlockPriv
{
	DB(id_t type);         // Used to keep track, at runtime, of the class type.
	int flags;             // Status and error flags.
	void *buf;             // Pointer to the page buffer for RAM-cached KBlocks.
	block_idx_t blk_start; // Start block number when the device is trimmed. \sa kblock_trim().
	block_idx_t curr_blk;  // Current cached block number in cached KBlocks.

	const struct KBlockVTable *vt; // Virtual table of interface functions.
} KBlockPriv;

/**
 * KBlock: interface for a generic block device.
 *
 */
typedef struct KBlock
{
	KBlockPriv priv;         ///< Interface private data, do not use directly.

	/* Public access members */
	size_t blk_size;         ///< Block size.
	block_idx_t blk_cnt;     ///< Number of blocks available in the device.
} KBlock;


/**
 * Use a subset of the blocks on the device.
 *
 * This function is useful for partitioning a device and use it for
 * different purposes at the same time.
 *
 * This function will limit the number of blocks used on the device by setting
 * a start index and a number of blocks to be used counting from that index.
 *
 * The blocks outside this range are no more accessible.
 *
 * Logical block indexes will be mapped to physical indexes inside this new
 * range automatically. Even following calls to kblock_trim() will use logical
 * indexes, so, once trimmed, access can only be limited further and never
 * expanded back.
 *
 * Example:
 * \code
 * //...init KBlock device dev
 * kblock_trim(dev, 200, 1500); // Restrict access to the 200-1700 physical block range.
 * kblock_read(dev, 0, buf, 0, dev->blk_size);  // Read from physical block #200.
 * kblock_trim(dev, 0, 300); // Restrict access to the 200-500 physical block range.
 * \endcode
 *
 * \param b KBlock device.
 * \param start The index of the start block for the limiting window in logical addressing units.
 * \param count The number of blocks to be used.
 *
 * \return 0 if all is OK, EOF on errors.
 */
int kblock_trim(struct KBlock *b, block_idx_t start, block_idx_t count);


#define KB_ASSERT_METHOD(b, method) \
	do \
	{ \
		ASSERT(b); \
		ASSERT((b)->priv.vt); \
		ASSERT((b)->priv.vt->method); \
	} \
	while (0)


/**
 * Get the current errors for the device.
 *
 * \note Calling this function will not clear the errors.
 *
 * \param b KBlock device.
 *
 * \return 0 if no error is present, a driver specific mask of errors otherwise.
 *
 * \sa kblock_clearerr()
 */
INLINE int kblock_error(struct KBlock *b)
{
	KB_ASSERT_METHOD(b, error);
	return b->priv.vt->error(b);
}

/**
 * Clear the errors of the device.
 *
 * \param b KBlock device.
 *
 *
 * \sa kblock_error()
 */
INLINE void kblock_clearerr(struct KBlock *b)
{
	KB_ASSERT_METHOD(b, clearerr);
	b->priv.vt->clearerr(b);
}


/**
 * Flush the cache (if any) to the device.
 *
 * This function will write any pending modifications to the device.
 * If the device does not have a cache, this function will do nothing.
 *
 * \return 0 if all is OK, EOF on errors.
 * \sa kblock_read(), kblock_write(), kblock_buffered().
 */
int kblock_flush(struct KBlock *b);

/**
 * Close the device.
 *
 * \param b KBlock device.
 *
 * \return 0 on success, EOF on errors.
 */
INLINE int kblock_close(struct KBlock *b)
{
	KB_ASSERT_METHOD(b, close);
	return kblock_flush(b) | b->priv.vt->close(b);
}

/**
 * \return true if the device \a b is buffered, false otherwise.
 * \param b KBlock device.
 * \sa kblock_cachedBlock(), kblock_cacheDirty().
 */
INLINE bool kblock_buffered(struct KBlock *b)
{
	ASSERT(b);
	return (b->priv.flags & KB_BUFFERED);
}


/**
 * \return The current cached block number if the device is buffered.
 * \param b KBlock device.
 * \note   This function will throw an ASSERT if called on a non buffered KBlock.
 * \sa kblock_buffered(), kblock_cacheDirty().
 */
INLINE block_idx_t kblock_cachedBlock(struct KBlock *b)
{
	ASSERT(kblock_buffered(b));
	return b->priv.curr_blk;
}


/**
 * Return the status of the internal cache.
 *
 * \param b KBlock device.
 * \return If the device supports buffering, returns true if the cache is dirty,
 *         false if the cache is clean and coherent with device content.
 * \note   This function will throw an ASSERT if called on a non buffered KBlock.
 * \sa kblock_cachedBlock(), kblock_buffered().
 */
INLINE bool kblock_cacheDirty(struct KBlock *b)
{
	ASSERT(kblock_buffered(b));
	return kblock_buffered(b) && (b->priv.flags & KB_CACHE_DIRTY);
}

/**
 * \return true if the device \a b supports partial block write. That is, you
 *         can call kblock_write() with a size which is lesser than the block
 *         size.
 * \param b KBlock device.
 * \sa kblock_write().
 */
INLINE bool kblock_partialWrite(struct KBlock *b)
{
	ASSERT(b);
	return (b->priv.flags & KB_PARTIAL_WRITE);
}

/**
 * Read data from the block device.
 *
 * This function will read \a size bytes from block \a idx starting at
 * address \a offset inside the block.
 *
 * Most block devices (almost all flash memories, for instance),
 * can efficiently read even a part of the block.
 *
 * \note This function can be slow if you try to partial read a block from
 *       a device which does not support partial block reads and is opened
 *       in unbuffered mode.
 *
 * \param b KBlock device.
 * \param idx the block number where you want to read.
 * \param buf a buffer where the data will be read.
 * \param offset the offset inside the block from which data reading will start.
 * \param size the size of data to be read.
 *
 * \return the number of bytes read.
 *
 * \sa kblock_write().
 */
size_t kblock_read(struct KBlock *b, block_idx_t idx, void *buf, size_t offset, size_t size);


/**
 * Write data to the block device.
 *
 * This function will write \a size bytes to block \a idx starting at
 * address \a offset inside the block.
 *
 * \note Partial block writes are supported only on certain devices.
 *       You can use kblock_partialWrite() in order to check if the device
 *       has this feature or not.
 *
 * \note If the device is opened in buffered mode, this function will use
 *       efficiently and trasparently the cache provided.
 *       In order to be sure that all modifications are actually written
 *       to the device you have to call kblock_flush().
 *
 * \param b KBlock device.
 * \param idx the block number where you want to write.
 * \param buf a pointer to the data to be written.
 * \param offset the offset inside the block from which data writing will start.
 * \param size the size of data to be written.
 *
 * \return the number of bytes written.
 *
 * \sa kblock_read(), kblock_flush(), kblock_buffered(), kblock_partialWrite().
 */
size_t kblock_write(struct KBlock *b, block_idx_t idx, const void *buf, size_t offset, size_t size);

/**
 * Copy one block to another.
 *
 * This function will copy the content of block \a src to block \a dest.
 *
 * \note This function is available only on devices which support partial
 *       block write or are opened in buffered mode.
 *
 * \param b KBlock device.
 * \param src source block number.
 * \param dest destination block number.
 *
 * \return 0 if all is OK, EOF on errors.
 */
int kblock_copy(struct KBlock *b, block_idx_t src, block_idx_t dest);

int kblock_swLoad(struct KBlock *b, block_idx_t index);
int kblock_swStore(struct KBlock *b, block_idx_t index);
size_t kblock_swReadBuf(struct KBlock *b, void *buf, size_t offset, size_t size);
size_t kblock_swWriteBuf(struct KBlock *b, const void *buf, size_t offset, size_t size);
int kblock_swClose(struct KBlock *b);

/** \} */ //defgroup io_kblock


#endif /* IO_KBLOCK_H */
