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
* Copyright 2011 Develer S.r.l. (http://www.develer.com/)
* -->
*
* \brief ONFI 1.0 compliant NAND kblock driver
*
* Defective blocks are remapped in a reserved area of configurable size
* at the bottom of the NAND.
* At the moment there is no wear-leveling block translation: kblock's blocks
* are mapped directly on NAND erase blocks: when a (k)block is written the
* corresponding erase block is erased and all pages within are rewritten.
* Partial write is not possible: it's recommended to use buffered mode.
*
* The driver needs to format the NAND before use. If the initialization code
* detects a fresh memory it does a bad block scan and a formatting.
* Format info isn't stored in NAND in a global structure: each block has its
* info written in the spare area of its first page.  These info contais a tag
* to detect formatted blocks and an index for bad block remapping (struct
* RemapInfo).
*
* The ECC for each page is written in the spare area too.
*
* Works only in 8 bit data mode and NAND parameters are not
* detected at run-time, but hand-configured in cfg_nand.h.
*
* Heap is needed to allocate the tipically large buffer necessary
* to erase and write a block.
*
* \author Stefano Fedrigo <aleph@develer.com>
*
* notest: avr
*/

#include "nand.h"
#include <cfg/log.h>
#include <struct/heap.h>
#include <string.h> // memset


/*
 * Remap info written in the first page of each block.
 *
 * This structure is used in blocks of the reserved area to store
 * which block the block containing the structure is remapping.
 * It's stored in all other blocks too to mark a formatted block.
 * In this case the member mapped_blk has non meaning.
 */
struct RemapInfo
{
	uint32_t tag;         // Magic number to detect valid info
	uint16_t mapped_blk;  // Bad block the block containing this info is remapping
};

// Where RemapInfo is stored in the spare area
#define NAND_REMAP_TAG_OFFSET  (CONFIG_NAND_SPARE_SIZE - sizeof(struct RemapInfo))

// Fixed tag to detect RemapInfo
#define NAND_REMAP_TAG         0x3e10c8ed

/*
 * Number of ECC words computed for a page.
 *
 * For 2048 bytes pages and 1 ECC word each 256 bytes,
 * 24 bytes of ECC data are stored.
 */
#define NAND_ECC_NWORDS        (CONFIG_NAND_DATA_SIZE / 256)

// Total page size (user data + spare) in bytes
#define NAND_PAGE_SIZE         (CONFIG_NAND_DATA_SIZE + CONFIG_NAND_SPARE_SIZE)

// Erase block size in bytes
#define NAND_BLOCK_SIZE        (CONFIG_NAND_DATA_SIZE * CONFIG_NAND_PAGES_PER_BLOCK)

// Number of usable blocks, and index of first remapping block
#define NAND_NUM_USER_BLOCKS   (CONFIG_NAND_NUM_BLOCK - CONFIG_NAND_NUM_REMAP_BLOCKS)

// ONFI NAND status codes
#define NAND_STATUS_READY  BV(6)
#define NAND_STATUS_ERROR  BV(0)


// Get block from page
#define PAGE(blk)            ((blk) * CONFIG_NAND_PAGES_PER_BLOCK)

// Page from block and page in block
#define BLOCK(page)          ((uint16_t)((page) / CONFIG_NAND_PAGES_PER_BLOCK))
#define PAGE_IN_BLOCK(page)  ((uint16_t)((page) % CONFIG_NAND_PAGES_PER_BLOCK))


/*
 * Translate page index plus a byte offset
 * in the five address cycles format needed by NAND.
 *
 * Cycles in x8 mode.
 * CA = column addr, PA = page addr, BA = block addr
 *
 * Cycle    I/O7  I/O6  I/O5  I/O4  I/O3  I/O2  I/O1  I/O0
 * -------------------------------------------------------
 * First    CA7   CA6   CA5   CA4   CA3   CA2   CA1   CA0
 * Second   LOW   LOW   LOW   LOW   CA11  CA10  CA9   CA8
 * Third    BA7   BA6   PA5   PA4   PA3   PA2   PA1   PA0
 * Fourth   BA15  BA14  BA13  BA12  BA11  BA10  BA9   BA8
 * Fifth    LOW   LOW   LOW   LOW   LOW   LOW   LOW   BA16
 */
static void getAddrCycles(uint32_t page, uint16_t offset, uint32_t *cycle0, uint32_t *cycle1234)
{
	ASSERT(offset < NAND_PAGE_SIZE);

	*cycle0 = offset & 0xff;
	*cycle1234 = (page << 8) | ((offset >> 8) & 0xf);
}


static void chipReset(Nand *chip)
{
	nand_sendCommand(chip, NAND_CMD_RESET, 0, 0, 0, 0);
	nand_waitReadyBusy(chip, CONFIG_NAND_TMOUT);
}


static bool isOperationComplete(Nand *chip)
{
	uint8_t status;

	nand_sendCommand(chip, NAND_CMD_STATUS, 0, 0, 0, 0);

	status = nand_getChipStatus(chip);
	return (status & NAND_STATUS_READY) && !(status & NAND_STATUS_ERROR);
}


/**
 * Erase the whole block.
 */
int nand_blockErase(Nand *chip, uint16_t block)
{
	uint32_t cycle0;
	uint32_t cycle1234;

	uint16_t remapped_block = chip->block_map[block];
	if (block != remapped_block)
	{
		LOG_INFO("nand_blockErase: remapped block: blk %d->%d\n", block, remapped_block);
		block = remapped_block;
	}

	getAddrCycles(PAGE(block), 0, &cycle0, &cycle1234);

	nand_sendCommand(chip, NAND_CMD_ERASE_1, NAND_CMD_ERASE_2, 3, 0, cycle1234 >> 8);

	nand_waitReadyBusy(chip, CONFIG_NAND_TMOUT);

	if (!isOperationComplete(chip))
	{
		LOG_ERR("nand: error erasing block\n");
		chip->status |= NAND_ERR_ERASE;
		return -1;
	}

	return 0;
}


/**
 * Read Device ID and configuration codes.
 */
bool nand_getDevId(Nand *chip, uint8_t dev_id[5])
{
	nand_sendCommand(chip, NAND_CMD_READID, 0, 1, 0, 0);

	nand_waitReadyBusy(chip, CONFIG_NAND_TMOUT);
	if (!nand_waitTransferComplete(chip, CONFIG_NAND_TMOUT))
	{
		LOG_ERR("nand: getDevId timeout\n");
		chip->status |= NAND_ERR_RD_TMOUT;
		return false;
	}

	memcpy(dev_id, nand_dataBuffer(chip), sizeof(dev_id));
	return true;
}


static bool nand_readPage(Nand *chip, uint32_t page, uint16_t offset)
{
	uint32_t cycle0;
	uint32_t cycle1234;

	//LOG_INFO("nand_readPage: page 0x%lx off 0x%x\n", page, offset);

	getAddrCycles(page, offset, &cycle0, &cycle1234);

	nand_sendCommand(chip, NAND_CMD_READ_1, NAND_CMD_READ_2, 5, cycle0, cycle1234);

	nand_waitReadyBusy(chip, CONFIG_NAND_TMOUT);
	if (!nand_waitTransferComplete(chip, CONFIG_NAND_TMOUT))
	{
		LOG_ERR("nand: read timeout\n");
		chip->status |= NAND_ERR_RD_TMOUT;
		return false;
	}

	return true;
}


/*
 * Read page data and ECC, checking for errors.
 * TODO: fix errors with ECC when possible.
 */
static bool nand_read(Nand *chip, uint32_t page, void *buf, uint16_t offset, uint16_t size)
{
	struct RemapInfo remap_info;
	uint32_t remapped_page = PAGE(chip->block_map[BLOCK(page)]) + PAGE_IN_BLOCK(page);

	//LOG_INFO("nand_read: page=%ld, offset=%d, size=%d\n", page, offset, size);

	if (page != remapped_page)
	{
		LOG_INFO("nand_read: remapped block: blk %d->%d, pg %ld->%ld\n",
				BLOCK(page), chip->block_map[BLOCK(page)], page, remapped_page);
		page = remapped_page;
	}

	if (!nand_readPage(chip, page, 0))
		return false;

	memcpy(buf, (char *)nand_dataBuffer(chip) + offset, size);

	/*
	 * Check for ECC hardware status only if a valid RemapInfo structure is found.
	 * That guarantees the page is written by us and a valid ECC is present.
	 */
	memcpy(&remap_info, (char *)buf + NAND_REMAP_TAG_OFFSET, sizeof(remap_info));
	if (remap_info.tag == NAND_REMAP_TAG && !nand_checkEcc(chip))
	{
		chip->status |= NAND_ERR_ECC;
		return false;
	}
	else
		return true;
}


/*
 * Write data stored in nand_dataBuffer() to a NAND page, starting at a given offset.
 * Usually offset will be 0 to write data or CONFIG_NAND_DATA_SIZE to write the spare
 * area.
 */
static bool nand_writePage(Nand *chip, uint32_t page, uint16_t offset)
{
	uint32_t cycle0;
	uint32_t cycle1234;

	//LOG_INFO("nand_writePage: page 0x%lx off 0x%x\n", page, offset);

	getAddrCycles(page, offset, &cycle0, &cycle1234);

	nand_sendCommand(chip, NAND_CMD_WRITE_1, 0, 5, cycle0, cycle1234);

	if (!nand_waitTransferComplete(chip, CONFIG_NAND_TMOUT))
	{
		LOG_ERR("nand: write timeout\n");
		chip->status |= NAND_ERR_WR_TMOUT;
		return false;
	}

	nand_sendCommand(chip, NAND_CMD_WRITE_2, 0, 0, 0, 0);

	nand_waitReadyBusy(chip, CONFIG_NAND_TMOUT);

	if (!isOperationComplete(chip))
	{
		LOG_ERR("nand: error writing page\n");
		chip->status |= NAND_ERR_WRITE;
		return false;
	}

	return true;
}


/*
 * Write data, ECC and remap block info.
 *
 * \param page           the page to be written
 * \parma original_page  if different from page, it's the page that's being remapped
 *
 * Implementation note for SAM3 NFC controller:
 * according to datasheet to get ECC computed by hardware is sufficient
 * to write the main area.  But it seems that in that way the last ECC_PR
 * register is not generated.  The workaround is to write data and dummy (ff)
 * spare data in one write, at this point the last ECC_PR is correct and
 * ECC data can be written in the spare area with a second program operation.
 */
static bool nand_write(Nand *chip, uint32_t page, const void *buf, size_t size)
{
	struct RemapInfo remap_info;
	uint32_t *nand_buf = (uint32_t *)nand_dataBuffer(chip);
	uint32_t remapped_page = PAGE(chip->block_map[BLOCK(page)]) + PAGE_IN_BLOCK(page);

	ASSERT(size <= CONFIG_NAND_DATA_SIZE);

	if (page != remapped_page)
		LOG_INFO("nand_write: remapped block: blk %d->%d, pg %ld->%ld\n",
				BLOCK(page), chip->block_map[BLOCK(page)], page, remapped_page);

	// Data
	memset(nand_buf, 0xff, NAND_PAGE_SIZE);
	memcpy(nand_buf, buf, size);
	if (!nand_writePage(chip, remapped_page, 0))
		return false;

	// ECC
	memset(nand_buf, 0xff, CONFIG_NAND_SPARE_SIZE);
	nand_computeEcc(chip, buf, size, nand_buf, NAND_ECC_NWORDS);

	// Remap info
	remap_info.tag = NAND_REMAP_TAG;
	remap_info.mapped_blk = BLOCK(page);
	memcpy((char *)nand_buf + NAND_REMAP_TAG_OFFSET, &remap_info, sizeof(remap_info));

	return nand_writePage(chip, remapped_page, CONFIG_NAND_DATA_SIZE);
}


/*
 * Check if the given block is marked bad: ONFI standard mandates
 * that bad block are marked with "00" bytes on the spare area of the
 * first page in block.
 */
static bool blockIsGood(Nand *chip, uint16_t blk)
{
	uint8_t *first_byte = (uint8_t *)nand_dataBuffer(chip);
	bool good;

	// Check first byte in spare area of first page in block
	nand_readPage(chip, PAGE(blk), CONFIG_NAND_DATA_SIZE);
	good = *first_byte != 0;

	if (!good)
		LOG_INFO("nand: bad block %d\n", blk);

	return good;
}


/*
 * Return the main partition block remapped on given block in the remap
 * partition (dest_blk).
 */
static int getBadBlockFromRemapBlock(Nand *chip, uint16_t dest_blk)
{
	struct RemapInfo *remap_info = (struct RemapInfo *)nand_dataBuffer(chip);

	if (!nand_readPage(chip, PAGE(dest_blk), CONFIG_NAND_DATA_SIZE + NAND_REMAP_TAG_OFFSET))
		return -1;

	if (remap_info->tag == NAND_REMAP_TAG)
		return remap_info->mapped_blk;
	else
		return -1;
}


/*
 * Set a block remapping: src_blk (a block in main data partition) is remapped
 * on dest_blk (block in reserved remapped blocks partition).
 */
static bool setMapping(Nand *chip, uint32_t src_blk, uint32_t dest_blk)
{
	struct RemapInfo *remap_info = (struct RemapInfo *)nand_dataBuffer(chip);

	LOG_INFO("nand, setMapping(): src=%ld dst=%ld\n", src_blk, dest_blk);

	if (!nand_readPage(chip, PAGE(dest_blk), CONFIG_NAND_DATA_SIZE + NAND_REMAP_TAG_OFFSET))
		return false;

	remap_info->tag = NAND_REMAP_TAG;
	remap_info->mapped_blk = src_blk;

	return nand_writePage(chip, PAGE(dest_blk), CONFIG_NAND_DATA_SIZE + NAND_REMAP_TAG_OFFSET);
}


/*
 * Get a new block from the remap partition to use as a substitute
 * for a bad block.
 */
static uint16_t getFreeRemapBlock(Nand *chip)
{
	int blk;

	for (blk = chip->remap_start; blk < CONFIG_NAND_NUM_BLOCK; blk++)
	{
		if (blockIsGood(chip, blk))
		{
			chip->remap_start = blk + 1;
			return blk;
		}
	}

	LOG_ERR("nand: reserved blocks for bad block remapping exhausted!\n");
	return 0;
}


/*
 * Check if NAND is initialized.
 */
static bool chipIsMarked(Nand *chip)
{
	return getBadBlockFromRemapBlock(chip, NAND_NUM_USER_BLOCKS) != -1;
}


/*
 * Initialize NAND (format). Scan NAND for factory marked bad blocks.
 * All found bad blocks are remapped to the remap partition: each
 * block in the remap partition used to remap bad blocks is marked.
 */
static void initBlockMap(Nand *chip)
{
	int b, last;

	// Default is for each block to not be remapped
	for (b = 0; b < CONFIG_NAND_NUM_BLOCK; b++)
		chip->block_map[b] = b;
	chip->remap_start = NAND_NUM_USER_BLOCKS;

	if (chipIsMarked(chip))
	{
		LOG_INFO("nand: found initialized NAND, searching for remapped blocks\n");

		// Scan for assigned blocks in remap area
		for (b = last = NAND_NUM_USER_BLOCKS; b < CONFIG_NAND_NUM_BLOCK; b++)
		{
			int remapped_blk = getBadBlockFromRemapBlock(chip, b);
			if (remapped_blk != -1 && remapped_blk != b)
			{
				LOG_INFO("nand: found remapped block %d->%d\n", remapped_blk, b);
				chip->block_map[remapped_blk] = b;
				last = b + 1;
			}
		}
		chip->remap_start = last;
	}
	else
	{
		bool remapped_anything = false;

		LOG_INFO("nand: found new NAND, searching for bad blocks\n");

		for (b = 0; b < NAND_NUM_USER_BLOCKS; b++)
		{
			if (!blockIsGood(chip, b))
			{
				chip->block_map[b] = getFreeRemapBlock(chip);
				setMapping(chip, b, chip->block_map[b]);
				remapped_anything = true;
				LOG_WARN("nand: found new bad block %d, remapped to %d\n", b, chip->block_map[b]);
			}
		}

		/*
	     * If no bad blocks are found (we're lucky!) write anyway a dummy
		 * remap to mark NAND and detect we already scanned it next time.
		 */
		if (!remapped_anything)
		{
			setMapping(chip, NAND_NUM_USER_BLOCKS, NAND_NUM_USER_BLOCKS);
			LOG_INFO("nand: no bad block founds, marked NAND\n");
		}
	}
}


/**
 * Reset bad blocks map and erase all blocks.
 *
 * \note DON'T USE on production chips: this function will try to erase
 *       factory marked bad blocks too.
 */
void nand_format(Nand *chip)
{
	int b;

	for (b = 0; b < CONFIG_NAND_NUM_BLOCK; b++)
	{
		LOG_INFO("nand: erasing block %d\n", b);
		chip->block_map[b] = b;
		nand_blockErase(chip, b);
	}
	chip->remap_start = NAND_NUM_USER_BLOCKS;
}

#ifdef _DEBUG

/*
 * Create some bad blocks, erasing them and writing the bad block mark.
 */
void nand_ruinSomeBlocks(Nand *chip)
{
	int bads[] = { 7, 99, 555, 1003, 1004, 1432 };
	unsigned i;

	LOG_INFO("nand: erasing mark\n");
	nand_blockErase(chip, NAND_NUM_USER_BLOCKS);

	for (i = 0; i < countof(bads); i++)
	{
		LOG_INFO("nand: erasing block %d\n", bads[i]);
		nand_blockErase(chip, bads[i]);

		LOG_INFO("nand: marking page %d as bad\n", PAGE(bads[i]));
		memset(nand_dataBuffer(chip), 0, CONFIG_NAND_SPARE_SIZE);
		nand_writePage(chip, PAGE(bads[i]), CONFIG_NAND_DATA_SIZE);
	}
}

#endif

static bool commonInit(Nand *chip, struct Heap *heap, unsigned chip_select)
{
	memset(chip, 0, sizeof(Nand));

	DB(chip->fd.priv.type = KBT_NAND);
	chip->fd.blk_size = NAND_BLOCK_SIZE;
	chip->fd.blk_cnt  = NAND_NUM_USER_BLOCKS;

	chip->chip_select = chip_select;
	chip->block_map = heap_allocmem(heap, CONFIG_NAND_NUM_BLOCK * sizeof(*chip->block_map));
	if (!chip->block_map)
	{
		LOG_ERR("nand: error allocating block map\n");
		return false;
	}

	nand_hwInit(chip);
	chipReset(chip);
	initBlockMap(chip);

	return true;
}


/**************** Kblock interface ****************/


static size_t nand_writeDirect(struct KBlock *kblk, block_idx_t idx, const void *buf, size_t offset, size_t size)
{
	ASSERT(offset <= NAND_BLOCK_SIZE);
	ASSERT(offset % CONFIG_NAND_DATA_SIZE == 0);
	ASSERT(size <= NAND_BLOCK_SIZE);
	ASSERT(size % CONFIG_NAND_DATA_SIZE == 0);

	LOG_INFO("nand_writeDirect: idx=%ld offset=%d size=%d\n", idx, offset, size);

	nand_blockErase(NAND_CAST(kblk), idx);

	while (offset < size)
	{
		uint32_t page = PAGE(idx) + (offset / CONFIG_NAND_DATA_SIZE);

		if (!nand_write(NAND_CAST(kblk), page, buf, CONFIG_NAND_DATA_SIZE))
			break;

		offset += CONFIG_NAND_DATA_SIZE;
		buf = (const char *)buf + CONFIG_NAND_DATA_SIZE;
	}

	return offset;
}


static size_t nand_readDirect(struct KBlock *kblk, block_idx_t idx, void *buf, size_t offset, size_t size)
{
	uint32_t page;
	size_t   read_size;
	size_t   read_offset;
	size_t   nread = 0;

	ASSERT(offset < NAND_BLOCK_SIZE);
	ASSERT(size <= NAND_BLOCK_SIZE);

	LOG_INFO("nand_readDirect: idx=%ld offset=%d size=%d\n", idx, offset, size);

	while (nread < size)
	{
		page        = PAGE(idx) + (offset / CONFIG_NAND_DATA_SIZE);
		read_offset = offset % CONFIG_NAND_DATA_SIZE;
		read_size   = MIN(size, CONFIG_NAND_DATA_SIZE - read_offset);

		if (!nand_read(NAND_CAST(kblk), page, (char *)buf + nread, read_offset, read_size))
			break;

		offset += read_size;
		nread  += read_size;
	}

	return nread;
}


static int nand_error(struct KBlock *kblk)
{
	Nand *chip = NAND_CAST(kblk);
	return chip->status;
}


static void nand_clearError(struct KBlock *kblk)
{
	Nand *chip = NAND_CAST(kblk);
	chip->status = 0;
}


static const KBlockVTable nand_buffered_vt =
{
	.readDirect = nand_readDirect,
	.writeDirect = nand_writeDirect,

	.readBuf = kblock_swReadBuf,
	.writeBuf = kblock_swWriteBuf,
	.load = kblock_swLoad,
	.store = kblock_swStore,

	.error = nand_error,
	.clearerr = nand_clearError,
};

static const KBlockVTable nand_unbuffered_vt =
{
	.readDirect = nand_readDirect,
	.writeDirect = nand_writeDirect,

	.error = nand_error,
	.clearerr = nand_clearError,
};


/**
 * Initialize NAND kblock driver in buffered mode.
 */
bool nand_init(Nand *chip, struct Heap *heap, unsigned chip_select)
{
	if (!commonInit(chip, heap, chip_select))
		return false;

	chip->fd.priv.vt = &nand_buffered_vt;
	chip->fd.priv.flags |= KB_BUFFERED;

	chip->fd.priv.buf = heap_allocmem(heap, NAND_BLOCK_SIZE);
	if (!chip->fd.priv.buf)
	{
		LOG_ERR("nand: error allocating block buffer\n");
		return false;
	}

	// Load the first block in the cache
	return nand_readDirect(&chip->fd, 0, chip->fd.priv.buf, 0, chip->fd.blk_size);
}


/**
 * Initialize NAND kblock driver in unbuffered mode.
 */
bool nand_initUnbuffered(Nand *chip, struct Heap *heap, unsigned chip_select)
{
	if (!commonInit(chip, heap, chip_select))
		return false;

	chip->fd.priv.vt = &nand_unbuffered_vt;
	return true;
}

