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
 * Copyright 2007 Develer S.r.l. (http://www.develer.com/)
 *
 * -->
 *
 * \brief BattFS: a filesystem for embedded platforms (implementation).
 *
 *
 * \author Francesco Sacchi <batt@develer.com>
 *
 */

#include "battfs.h"
#include "cfg/cfg_battfs.h"
#include <cfg/debug.h>
#include <cfg/macros.h> /* MIN, MAX */
#include <cfg/test.h>
#include <cpu/byteorder.h> /* cpu_to_xx */

#define LOG_LEVEL       BATTFS_LOG_LEVEL
#define LOG_FORMAT      BATTFS_LOG_FORMAT
#include <cfg/log.h>

#include <string.h> /* memset, memmove */

#if LOG_LEVEL >= LOG_LVL_INFO
static void dumpPageArray(struct BattFsSuper *disk)
{
	kprintf("Page array dump, free_page_start %d:", disk->free_page_start);
	for (pgcnt_t i = 0; i < disk->dev->blk_cnt; i++)
	{
		if (!(i % 16))
			kputchar('\n');
		kprintf("%04d ", disk->page_array[i]);
	}
	kputchar('\n');
}
#endif

/**
 * Convert from memory representation to disk structure.
 * \note filesystem is in little-endian format.
 */
INLINE void battfs_to_disk(struct BattFsPageHeader *hdr, uint8_t *buf)
{
	STATIC_ASSERT(BATTFS_HEADER_LEN == 12);
	buf[0] = hdr->inode;

	buf[1] = hdr->fill;
	buf[2] = hdr->fill >> 8;

	buf[3] = hdr->pgoff;
	buf[4] = hdr->pgoff >> 8;

	/*
	 * Sequence number is 40 bits long.
	 * No need to take care of wraparonds: the memory will die first!
	 */
	buf[5] = hdr->seq;
	buf[6] = hdr->seq >> 8;
	buf[7] = hdr->seq >> 16;
	buf[8] = hdr->seq >> 24;
	buf[9] = hdr->seq >> 32;

	/*
	 * This field must be the last one!
	 * This is needed because if the page is only partially
	 * written, we can use this to detect it.
	 */
	buf[10] = hdr->fcs;
	buf[11] = hdr->fcs >> 8;
}

/**
 * Convert from disk structure to memory representation.
 * \note filesystem is in little-endian format.
 */
INLINE void disk_to_battfs(uint8_t *buf, struct BattFsPageHeader *hdr)
{
	STATIC_ASSERT(BATTFS_HEADER_LEN == 12);
	hdr->inode = buf[0];
	hdr->fill = buf[2] << 8 | buf[1];
	hdr->pgoff = buf[4] << 8 | buf[3];
	hdr->seq = (seq_t)buf[9] << 32 | (seq_t)buf[8] << 24 | (seq_t)buf[7] << 16 | buf[6] << 8 | buf[5];
	hdr->fcs = buf[11] << 8 | buf[10];
}

/**
 * Compute the fcs of the header.
 */
static fcs_t computeFcs(struct BattFsPageHeader *hdr)
{
	uint8_t buf[BATTFS_HEADER_LEN];
	fcs_t cks;

	battfs_to_disk(hdr, buf);
	rotating_init(&cks);
	/* fcs is at the end of whole header */
	rotating_update(buf, BATTFS_HEADER_LEN - sizeof(fcs_t), &cks);
	return cks;
}

/**
 * Read header of \a page in \a hdr.
 * \return true on success, false otherwise.
 */
static bool readHdr(struct BattFsSuper *disk, pgcnt_t page, struct BattFsPageHeader *hdr)
{
	uint8_t buf[BATTFS_HEADER_LEN];

	/*
	 * Read header from disk.
	 * Header is actually a footer, and so
	 * resides at page end.
	 */
	if (kblock_read(disk->dev, page, buf, disk->data_size, BATTFS_HEADER_LEN)
	    != BATTFS_HEADER_LEN)
	{
		LOG_ERR("page[%d]\n", page);
		return false;
	}

	/* Fill header */
	disk_to_battfs(buf, hdr);

	return true;
}

static bool writeHdr(struct BattFsSuper *disk, pgcnt_t page, struct BattFsPageHeader *hdr)
{
	uint8_t buf[BATTFS_HEADER_LEN];

	#warning FIXME:refactor computeFcs to save time and stack
	hdr->fcs = computeFcs(hdr);
	/* Fill buffer */
	battfs_to_disk(hdr, buf);

	/*
	 * write header to disk.
	 * Header is actually a footer, and so
	 * resides at page end.
	 */
	if (kblock_write(disk->dev, page, buf, disk->data_size, BATTFS_HEADER_LEN)
	    != BATTFS_HEADER_LEN)
	{
		LOG_ERR("writing to buffer\n");
		return false;
	}
	return true;
}


/**
 * Count the number of pages from
 * inode 0 to \a inode in \a filelen_table.
 */
static pgcnt_t countPages(pgoff_t *filelen_table, inode_t inode)
{
	pgcnt_t cnt = 0;

	for (inode_t i = 0; i < inode; i++)
		cnt += filelen_table[i];

	return cnt;
}

/**
 * Move all pages in page allocation array from \a src to \a src + \a offset.
 * The number of pages moved is page_count - MAX(dst, src).
 */
static void movePages(struct BattFsSuper *disk, pgcnt_t src, int offset)
{
	pgcnt_t dst = src + offset;
	LOG_INFO("src %d, offset %d, size %d\n", src, offset, (unsigned int)((disk->dev->blk_cnt - MAX(dst, src)) * sizeof(pgcnt_t)));
	memmove(&disk->page_array[dst], &disk->page_array[src], (disk->dev->blk_cnt - MAX(dst, src)) * sizeof(pgcnt_t));

	if (offset < 0)
	{
		/* Fill empty space in array with sentinel */
		for (pgcnt_t page = disk->dev->blk_cnt + offset; page < disk->dev->blk_cnt; page++)
			disk->page_array[page] = PAGE_UNSET_SENTINEL;
	}
}

/**
 * Count number of pages per file on \a disk.
 * This information is registered in \a filelen_table.
 * Array index represent file inode, while value contained
 * is the number of pages used by that file.
 *
 * \return true if ok, false on disk read errors.
 * \note The whole disk is scanned once.
 */
static bool countDiskFilePages(struct BattFsSuper *disk, pgoff_t *filelen_table)
{
	BattFsPageHeader hdr;
	disk->free_page_start = 0;

	/* Count the number of disk page per file */
	for (pgcnt_t page = 0; page < disk->dev->blk_cnt; page++)
	{
		if (!readHdr(disk, page, &hdr))
			return false;

		/* Increase free space */
		disk->free_bytes += disk->data_size;

		/* Check header FCS */
		if (hdr.fcs == computeFcs(&hdr))
		{
			ASSERT(hdr.fill <= disk->data_size);

			/* Page is valid and is owned by a file */
			filelen_table[hdr.inode]++;

			/* Keep trace of free space */
			disk->free_bytes -= hdr.fill;
			disk->free_page_start++;
		}
	}
	LOG_INFO("free_bytes:%ld, free_page_start:%d\n", (long)disk->free_bytes, disk->free_page_start);

	return true;
}

/**
 * Fill page allocation array of \a disk
 * using file lenghts in \a filelen_table.
 *
 * The page allocation array is an array containings all file infos.
 * Is ordered by file, and within each file is ordered by page offset
 * inside file.
 * e.g. : at page array[0] you will find page address of the first page
 * of the first file (if present).
 * Free blocks are allocated after the last file.
 *
 * \return true if ok, false on disk read errors.
 * \note The whole disk is scanned at max twice.
 */
static bool fillPageArray(struct BattFsSuper *disk, pgoff_t *filelen_table)
{
	BattFsPageHeader hdr;
	pgcnt_t curr_free_page = disk->free_page_start;
	/* Fill page allocation array */
	for (pgcnt_t page = 0; page < disk->dev->blk_cnt; page++)
	{
		if (!readHdr(disk, page, &hdr))
			return false;

		/* Check header FCS */
		if (hdr.fcs == computeFcs(&hdr))
		{
			/* Compute array position */
			pgcnt_t array_pos = countPages(filelen_table, hdr.inode);
			array_pos += hdr.pgoff;


			/* Check if position is already used by another page of the same file */
			if (disk->page_array[array_pos] == PAGE_UNSET_SENTINEL)
				disk->page_array[array_pos] = page;
			else
			{
				BattFsPageHeader hdr_prv;

				if (!readHdr(disk, disk->page_array[array_pos], &hdr_prv))
					return false;

				/* Check header FCS */
				ASSERT(hdr_prv.fcs == computeFcs(&hdr_prv));

				/* Only the very same page with a different seq number can be here */
				ASSERT(hdr.inode == hdr_prv.inode);
				ASSERT(hdr.pgoff == hdr_prv.pgoff);
				ASSERT(hdr.seq != hdr_prv.seq);

				pgcnt_t new_page, old_page;
				fill_t old_fill;

				/*
				 * Sequence number comparison: since
				 * seq is 40 bits wide, it wraps once
				 * every 1.1E12 times.
				 * The memory will not live enough to
				 * see a wraparound, so we can use a simple
				 * compare here.
				 */
				if (hdr.seq > hdr_prv.seq)
				{
					/* Current header is newer than the previuos one */
					old_page = disk->page_array[array_pos];
					new_page = page;
					old_fill = hdr_prv.fill;
				}
				else
				{
					/* Previous header is newer than the current one */
					old_page = page;
					new_page = disk->page_array[array_pos];
					old_fill = hdr.fill;
				}

				/* Set new page */
				disk->page_array[array_pos] = new_page;
				/* Add free space */
				disk->free_bytes += old_fill;
				/* Shift all array one position to the left, overwriting duplicate page */
				array_pos -= hdr.pgoff;
				array_pos += filelen_table[hdr.inode];
				movePages(disk, array_pos, -1);
				/* Move back all indexes */
				filelen_table[hdr.inode]--;
				disk->free_page_start--;
				curr_free_page--;
				/* Set old page as free */
				ASSERT(disk->page_array[curr_free_page] == PAGE_UNSET_SENTINEL);
				disk->page_array[curr_free_page++] = old_page;

			}
		}
		else
		{
			/* Invalid page, keep as free */
			ASSERT(disk->page_array[curr_free_page] == PAGE_UNSET_SENTINEL);
			//LOG_INFO("Page %d invalid, keeping as free\n", page);
			disk->page_array[curr_free_page++] = page;
		}
	}
	return true;
}


/**
 * Initialize and mount disk described by
 * \a disk.
 * \return false on errors, true otherwise.
 */
bool battfs_mount(struct BattFsSuper *disk, struct KBlock *dev, pgcnt_t *page_array, size_t array_size)
{
	pgoff_t filelen_table[BATTFS_MAX_FILES];

	ASSERT(dev);
	ASSERT(kblock_partialWrite(dev));
	disk->dev = dev;

	ASSERT(disk->dev->blk_size > BATTFS_HEADER_LEN);
	/* Fill page_size with the usable space */
	disk->data_size = disk->dev->blk_size - BATTFS_HEADER_LEN;
	ASSERT(disk->dev->blk_cnt);
	ASSERT(disk->dev->blk_cnt < PAGE_UNSET_SENTINEL - 1);
	ASSERT(page_array);
	disk->page_array = page_array;
	ASSERT(array_size >= disk->dev->blk_cnt * sizeof(pgcnt_t));

	memset(filelen_table, 0, BATTFS_MAX_FILES * sizeof(pgoff_t));

	disk->free_bytes = 0;
	disk->disk_size = (disk_size_t)disk->data_size * disk->dev->blk_cnt;

	/* Count pages per file */
	if (!countDiskFilePages(disk, filelen_table))
	{
		LOG_ERR("counting file pages\n");
		return false;
	}

	/* Once here, we have filelen_table filled with file lengths */

	/* Fill page array with sentinel */
	for (pgcnt_t page = 0; page < disk->dev->blk_cnt; page++)
		disk->page_array[page] = PAGE_UNSET_SENTINEL;

	/* Fill page allocation array using filelen_table */
	if (!fillPageArray(disk, filelen_table))
	{
		LOG_ERR("filling page array\n");
		return false;
	}
	#if LOG_LEVEL >= LOG_LVL_INFO
		dumpPageArray(disk);
	#endif
	#if CONFIG_BATTFS_SHUFFLE_FREE_PAGES
		SHUFFLE(&disk->page_array[disk->free_page_start], disk->dev->blk_cnt - disk->free_page_start);

		LOG_INFO("Page array after shuffle:\n");
		#if LOG_LEVEL >= LOG_LVL_INFO
			dumpPageArray(disk);
		#endif
	#endif
	/* Init list for opened files. */
	LIST_INIT(&disk->file_opened_list);
	return true;
}

/**
 * Check the filesystem.
 * \return true if ok, false on errors.
 */
bool battfs_fsck(struct BattFsSuper *disk)
{
	#define FSCHECK(cond) do { if(!(cond)) { LOG_ERR("\"" #cond "\"\n"); return false; } } while (0)

	FSCHECK(disk->free_page_start <= disk->dev->blk_cnt);
	FSCHECK(disk->data_size < disk->dev->blk_size);
	FSCHECK(disk->free_bytes <= disk->disk_size);

	disk_size_t free_bytes = 0;
	BattFsPageHeader hdr, prev_hdr;
	inode_t files = 0;
	pgcnt_t page_used = 0;

	bool start = true;

	/* Uneeded, the first time will be overwritten but useful to silence
	 * the warning for uninitialized value */
	FSCHECK(readHdr(disk, 0, &prev_hdr));
	for (pgcnt_t page = 0; page < disk->dev->blk_cnt; page++)
	{
		FSCHECK(readHdr(disk, disk->page_array[page], &hdr));
		free_bytes += disk->data_size;

		if (page < disk->free_page_start)
		{
			FSCHECK(computeFcs(&hdr) == hdr.fcs);
			page_used++;
			free_bytes -= hdr.fill;
			if (hdr.inode != prev_hdr.inode || start)
			{
				if (LIKELY(!start))
					FSCHECK(hdr.inode > prev_hdr.inode);
				else
					start = false;

				FSCHECK(hdr.pgoff == 0);
				files++;
			}
			else
			{
				FSCHECK(hdr.fill != 0);
				FSCHECK(prev_hdr.fill == disk->data_size);
				FSCHECK(hdr.pgoff == prev_hdr.pgoff + 1);
			}
			prev_hdr = hdr;
		}
	}

	FSCHECK(page_used == disk->free_page_start);
	FSCHECK(free_bytes == disk->free_bytes);

	return true;
}

/**
 * Flush file \a fd.
 * \return 0 if ok, EOF on errors.
 */
static int battfs_flush(struct KFile *fd)
{
	BattFs *fdb = BATTFS_CAST(fd);

	if (kblock_flush(fdb->disk->dev) == 0)
		return 0;
	else
	{
		fdb->errors |= BATTFS_DISK_FLUSHBUF_ERR;
		return EOF;
	}
}

/**
 * Close file \a fd.
 * \return 0 if ok, EOF on errors.
 */
static int battfs_fileclose(struct KFile *fd)
{
	BattFs *fdb = BATTFS_CAST(fd);

	if (battfs_flush(fd) == 0)
	{
		REMOVE(&fdb->link);
		return 0;
	}
	else
		return EOF;
}

#define NO_SPACE PAGE_UNSET_SENTINEL

static pgcnt_t allocateNewPage(struct BattFsSuper *disk, pgcnt_t new_pos, inode_t inode)
{
	if (SPACE_OVER(disk))
	{
		LOG_ERR("No disk space available!\n");
		return NO_SPACE;
	}

	LOG_INFO("Getting new page %d, pos %d\n", disk->page_array[disk->free_page_start], new_pos);
	pgcnt_t new_page = disk->page_array[disk->free_page_start++];
	memmove(&disk->page_array[new_pos + 1], &disk->page_array[new_pos], (disk->free_page_start - new_pos - 1) * sizeof(pgcnt_t));

	Node *n;
	/* Move following file start point one position ahead. */
	FOREACH_NODE(n, &disk->file_opened_list)
	{
		BattFs *file = containerof(n, BattFs, link);
		if (file->inode > inode)
		{
			LOG_INFO("Move file %d start pos\n", file->inode);
			file->start++;
		}
	}

	disk->page_array[new_pos] = new_page;
	return new_page;
}

static pgcnt_t renewPage(struct BattFsSuper *disk, pgcnt_t old_pos)
{
	if (SPACE_OVER(disk))
	{
		LOG_ERR("No disk space available!\n");
		return NO_SPACE;
	}

	/* Get a free page */
	pgcnt_t new_page = disk->page_array[disk->free_page_start];
	movePages(disk, disk->free_page_start + 1, -1);

	/* Insert previous page in free blocks list */
	LOG_INFO("Setting page %d as free\n", old_pos);
	disk->page_array[disk->dev->blk_cnt - 1] = old_pos;
	return new_page;
}

/**
 * Write to file \a fd \a size bytes from \a buf.
 * \return The number of bytes written.
 */
static size_t battfs_write(struct KFile *fd, const void *_buf, size_t size)
{
	BattFs *fdb = BATTFS_CAST(fd);
	BattFsSuper *disk = fdb->disk;
	const uint8_t *buf = (const uint8_t *)_buf;

	size_t total_write = 0;
	pgoff_t pg_offset;
	pgaddr_t addr_offset;
	pgaddr_t wr_len;
	BattFsPageHeader curr_hdr;
	pgcnt_t new_page;

	if (fd->seek_pos < 0)
	{
		fdb->errors |= BATTFS_NEGATIVE_SEEK_ERR;
		return total_write;
	}

	if (fd->seek_pos > fd->size)
	{
		if (!readHdr(disk, fdb->start[fdb->max_off], &curr_hdr))
		{
			fdb->errors |= BATTFS_DISK_READ_ERR;
			return total_write;
		}

		/*
		 * Renew page only if is not in cache.
		 * This avoids rewriting the same page continuously
		 * if the user code keeps writing in the same portion
		 * of the file.
		 */
		if (kblock_buffered(disk->dev)
			&& ((fdb->start[fdb->max_off] != kblock_cachedBlock(disk->dev)) || !kblock_cacheDirty(disk->dev)))
		{
			new_page = renewPage(disk, fdb->start[fdb->max_off]);
			if (new_page == NO_SPACE)
			{
				fdb->errors |= BATTFS_DISK_SPACEOVER_ERR;
				return total_write;
			}

			kblock_copy(disk->dev, fdb->start[fdb->max_off], new_page);
			fdb->start[fdb->max_off] = new_page;
		}
		else
			new_page = fdb->start[fdb->max_off];

		/* Fill unused space of first page with 0s */
		uint8_t dummy = 0;
		// TODO: write in blocks to speed up things
		pgaddr_t zero_bytes = MIN(fd->seek_pos - fd->size, (kfile_off_t)(disk->data_size - curr_hdr.fill));
		while (zero_bytes--)
		{
			if (kblock_write(disk->dev, new_page, &dummy, curr_hdr.fill, 1) != 1)
			{
				fdb->errors |= BATTFS_DISK_WRITE_ERR;
				return total_write;
			}
			curr_hdr.fill++;
			fd->size++;
			disk->free_bytes--;
		}
		curr_hdr.seq++;
		if (!writeHdr(disk, new_page, &curr_hdr))
		{
			fdb->errors |= BATTFS_DISK_WRITE_ERR;
			return total_write;
		}

		/* Allocate the missing pages first. */
		pgoff_t missing_pages = fd->seek_pos / disk->data_size - fdb->max_off;

		LOG_INFO("missing pages: %d\n", missing_pages);

		while (missing_pages--)
		{
			zero_bytes = MIN((kfile_off_t)disk->data_size, fd->seek_pos - fd->size);

			new_page = allocateNewPage(disk, (fdb->start - disk->page_array) + fdb->max_off + 1, fdb->inode);
			if (new_page == NO_SPACE)
			{
				fdb->errors |= BATTFS_DISK_SPACEOVER_ERR;
				return total_write;
			}


			// TODO: write in blocks to speed up things
			/* Fill page buffer with 0 to avoid filling unused pages with garbage */
			for (pgaddr_t off = 0; off < disk->data_size; off++)
			{
				if (kblock_write(disk->dev, new_page, &dummy, off, 1) != 1)
				{
					fdb->errors |= BATTFS_DISK_WRITE_ERR;
					return total_write;
				}
			}
			curr_hdr.inode = fdb->inode;
			curr_hdr.pgoff = ++fdb->max_off;
			curr_hdr.fill = zero_bytes;
			curr_hdr.seq = 0;

			if (!writeHdr(disk, new_page, &curr_hdr))
			{
				fdb->errors |= BATTFS_DISK_WRITE_ERR;
				return total_write;
			}

			/* Update size and free space left */
			fd->size += zero_bytes;
			disk->free_bytes -= zero_bytes;
		}
	}

	while (size)
	{
		pg_offset = fd->seek_pos / disk->data_size;
		addr_offset = fd->seek_pos % disk->data_size;
		wr_len = MIN(size, (size_t)(disk->data_size - addr_offset));

		/* Handle write outside EOF */
		if (pg_offset > fdb->max_off)
		{
			LOG_INFO("New page needed, pg_offset %d, pos %d\n", pg_offset, (int)((fdb->start - disk->page_array) + pg_offset));

			new_page = allocateNewPage(disk, (fdb->start - disk->page_array) + pg_offset, fdb->inode);
			if (new_page == NO_SPACE)
			{
				fdb->errors |= BATTFS_DISK_SPACEOVER_ERR;
				return total_write;
			}

			curr_hdr.inode = fdb->inode;
			curr_hdr.pgoff = pg_offset;
			curr_hdr.fill = 0;
			curr_hdr.seq = 0;
			fdb->max_off = pg_offset;
		}
		else
		{
			if (!readHdr(disk, fdb->start[pg_offset], &curr_hdr))
			{
				fdb->errors |= BATTFS_DISK_READ_ERR;
				return total_write;
			}

			/* Renew page only if is not in cache. */
			if (kblock_buffered(disk->dev)
				&& ((fdb->start[fdb->max_off] != kblock_cachedBlock(disk->dev)) || !kblock_cacheDirty(disk->dev)))
			{
				new_page = renewPage(disk, fdb->start[pg_offset]);
				if (new_page == NO_SPACE)
				{
					fdb->errors |= BATTFS_DISK_SPACEOVER_ERR;
					return total_write;
				}

				LOG_INFO("Re-writing page %d to %d\n", fdb->start[pg_offset], new_page);
				if (kblock_copy(disk->dev, fdb->start[pg_offset], new_page) != 0)
				{
					fdb->errors |= BATTFS_DISK_WRITE_ERR;
					return total_write;
				}
				fdb->start[pg_offset] = new_page;
			}
			else
			{
				LOG_INFO("Using cached block %d\n", fdb->start[pg_offset]);
				new_page = fdb->start[pg_offset];
			}

			curr_hdr.seq++;
		}
		//LOG_INFO("writing to buffer for page %d, offset %d, size %d\n", disk->curr_page, addr_offset, wr_len);
		if (kblock_write(disk->dev, new_page, buf, addr_offset, wr_len) != wr_len)
		{
			fdb->errors |= BATTFS_DISK_WRITE_ERR;
			return total_write;
		}

		size -= wr_len;
		fd->seek_pos += wr_len;
		total_write += wr_len;
		buf += wr_len;
		fill_t fill_delta = MAX((int32_t)(addr_offset + wr_len) - curr_hdr.fill, (int32_t)0);
		disk->free_bytes -= fill_delta;
		fd->size += fill_delta;
		curr_hdr.fill += fill_delta;

		if (!writeHdr(disk, new_page, &curr_hdr))
		{
			fdb->errors |= BATTFS_DISK_WRITE_ERR;
			return total_write;
		}

		//LOG_INFO("free_bytes %d, seek_pos %d, size %d, curr_hdr.fill %d\n", disk->free_bytes, fd->seek_pos, fd->size, curr_hdr.fill);
	}
	return total_write;
}


/**
 * Read from file \a fd \a size bytes in \a buf.
 * \return The number of bytes read.
 */
static size_t battfs_read(struct KFile *fd, void *_buf, size_t size)
{
	BattFs *fdb = BATTFS_CAST(fd);
	BattFsSuper *disk = fdb->disk;
	uint8_t *buf = (uint8_t *)_buf;

	size_t total_read = 0;
	pgoff_t pg_offset;
	pgaddr_t addr_offset;
	pgaddr_t read_len;

	if (fd->seek_pos < 0)
	{
		fdb->errors |= BATTFS_NEGATIVE_SEEK_ERR;
		return total_read;
	}

	size = MIN((kfile_off_t)size, MAX(fd->size - fd->seek_pos, (kfile_off_t)0));

	while (size)
	{
		pg_offset = fd->seek_pos / disk->data_size;
		addr_offset = fd->seek_pos % disk->data_size;
		read_len = MIN(size, (size_t)(disk->data_size - addr_offset));

		//LOG_INFO("reading from page %d, offset %d, size %d\n", fdb->start[pg_offset], addr_offset, read_len);
		/* Read from disk */
		if (kblock_read(disk->dev, fdb->start[pg_offset], buf, addr_offset, read_len) != read_len)
		{
			fdb->errors |= BATTFS_DISK_READ_ERR;
			return total_read;
		}

		#ifdef _DEBUG
			BattFsPageHeader hdr;
			readHdr(disk, fdb->start[pg_offset], &hdr);
			ASSERT(hdr.inode == fdb->inode);
		#endif

		size -= read_len;
		fd->seek_pos += read_len;
		total_read += read_len;
		buf += read_len;
	}
	return total_read;
}


/**
 * Search file \a inode in \a disk using a binary search.
 * \a last is filled with array offset of file start
 * in disk->page_array if file is found, otherwise
 * \a last is filled with the correct insert position
 * for creating a file with the given \a inode.
 * \return true if file is found, false otherwisr.
 */
static bool findFile(BattFsSuper *disk, inode_t inode, pgcnt_t *last)
{
	BattFsPageHeader hdr;
	pgcnt_t first = 0, page;
	*last = disk->free_page_start;
	fcs_t fcs;

	while (first < *last)
	{
		page = (first + *last) / 2;
		LOG_INFO("first %d, last %d, page %d\n", first, *last, page);
		if (!readHdr(disk, disk->page_array[page], &hdr))
			return false;
		LOG_INFO("inode read: %d\n", hdr.inode);
		fcs = computeFcs(&hdr);
		if (hdr.fcs == fcs && hdr.inode == inode)
		{
			*last = page - hdr.pgoff;
			LOG_INFO("Found: %d\n", *last);
			return true;
		}
		else if (hdr.fcs == fcs && hdr.inode < inode)
			first = page + 1;
		else
			*last = page;
	}
	LOG_INFO("Not found: last %d\n", *last);
	return false;
}

/**
 * \return true if file \a inode exists on \a disk, false otherwise.
 */
bool battfs_fileExists(BattFsSuper *disk, inode_t inode)
{
	pgcnt_t dummy;
	return findFile(disk, inode, &dummy);
}

/**
 * Count size of file \a inode on \a disk, starting at pointer \a start
 * in disk->page_array. Size is written in \a size.
 * \return true if all s ok, false on disk read errors.
 */
static file_size_t countFileSize(BattFsSuper *disk, pgcnt_t *start, inode_t inode)
{
	file_size_t size = 0;
	BattFsPageHeader hdr;

	while (start < &disk->page_array[disk->free_page_start])
	{
		if (!readHdr(disk, *start++, &hdr))
			return EOF;
		if (hdr.fcs == computeFcs(&hdr) && hdr.inode == inode)
			size += hdr.fill;
		else
			break;
	}
	return size;
}

static int battfs_error(struct KFile *fd)
{
	BattFs *fdb = BATTFS_CAST(fd);
	return fdb->errors;
}


static void battfs_clearerr(struct KFile *fd)
{
	BattFs *fdb = BATTFS_CAST(fd);
	fdb->errors = 0;
}

/**
 * Open file \a inode from \a disk in \a mode.
 * File context is stored in \a fd.
 * \return true if ok, false otherwise.
 */
bool battfs_fileopen(BattFsSuper *disk, BattFs *fd, inode_t inode, filemode_t mode)
{
	Node *n;

	memset(fd, 0, sizeof(*fd));

	/* Search file start point in disk page array */
	pgcnt_t start_pos;
	if (!findFile(disk, inode, &start_pos))
	{
		LOG_INFO("file %d not found\n", inode);
		if (!(mode & BATTFS_CREATE))
		{
			fd->errors |= BATTFS_FILE_NOT_FOUND_ERR;
			return false;
		}
		/* Create the file */
		BattFsPageHeader hdr;

		if (allocateNewPage(disk, start_pos, inode) == NO_SPACE)
		{
			fd->errors |= BATTFS_DISK_SPACEOVER_ERR;
			return false;
		}

		hdr.inode = inode;
		hdr.pgoff = 0;
		hdr.fill = 0;
		hdr.seq = 0;
		if (!writeHdr(disk, disk->page_array[start_pos], &hdr))
		{
			fd->errors |= BATTFS_DISK_WRITE_ERR;
			return false;
		}
	}
	fd->start = &disk->page_array[start_pos];
	LOG_INFO("Start pos %d\n", start_pos);

	/* Fill file size */
	if ((fd->fd.size = countFileSize(disk, fd->start, inode)) == EOF)
	{
		fd->errors |= BATTFS_DISK_READ_ERR;
		return false;
	}
	fd->max_off = fd->fd.size / disk->data_size;

	/* Reset seek position */
	fd->fd.seek_pos = 0;

	/* Insert file handle in list, ordered by inode, ascending. */
	FOREACH_NODE(n, &disk->file_opened_list)
	{
		BattFs *file = containerof(n, BattFs, link);
		if (file->inode >= inode)
			break;
	}
	INSERT_BEFORE(&fd->link, n);

	/* Fill in data */
	fd->inode = inode;
	fd->mode = mode;
	fd->disk = disk;

	fd->fd.close = battfs_fileclose;
	fd->fd.flush = battfs_flush;
	fd->fd.read = battfs_read;
	fd->fd.reopen = kfile_genericReopen;
	fd->fd.seek = kfile_genericSeek;
	fd->fd.write = battfs_write;

	fd->fd.error = battfs_error;
	fd->fd.clearerr = battfs_clearerr;

	DB(fd->fd._type = KFT_BATTFS);

	return true;
}


/**
 * Umount \a disk.
 */
bool battfs_umount(struct BattFsSuper *disk)
{
	Node *n;
	int res = 0;

	/* Close all open files */
	FOREACH_NODE(n, &disk->file_opened_list)
	{
		BattFs *file = containerof(n, BattFs, link);
		res += battfs_fileclose(&file->fd);
	}

	/* Close disk */
	return (kblock_flush(disk->dev) == 0) && (kblock_close(disk->dev) == 0) && (res == 0);
}

#if UNIT_TEST

void battfs_writeTestBlock(KBlock *dev, pgcnt_t page, inode_t inode, seq_t seq, fill_t fill, pgoff_t pgoff)
{
	uint8_t buf[BATTFS_HEADER_LEN];
	battfs_eraseBlock(dev, page);

	BattFsPageHeader hdr;
	hdr.inode = inode;
	hdr.fill = fill;
	hdr.pgoff = pgoff;
	hdr.seq = seq;
	hdr.fcs = computeFcs(&hdr);

	battfs_to_disk(&hdr, buf);

	ASSERT(kblock_write(dev, page, buf, dev->blk_size - BATTFS_HEADER_LEN, BATTFS_HEADER_LEN) == BATTFS_HEADER_LEN);
	ASSERT(kblock_flush(dev) == 0);
}

void battfs_eraseBlock(KBlock *dev, pgcnt_t page)
{
	/* Reset page to all 0xff */
	uint8_t buf[dev->blk_size];
	memset(buf, 0xFF, dev->blk_size);
	ASSERT(kblock_write(dev, page, buf, 0, dev->blk_size) == dev->blk_size);
	ASSERT(kblock_flush(dev) == 0);
}

#endif
