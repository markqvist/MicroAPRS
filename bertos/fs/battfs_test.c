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
 * Copyright 2007, 2008 Develer S.r.l. (http://www.develer.com/)
 * -->
 *
 * \brief BattFS Test.
 *
 * \author Francesco Sacchi <batt@develer.com>
 */

#include <fs/battfs.h>
#include <io/kblock_posix.h>

#include <cfg/debug.h>
#include <cfg/test.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define FILE_SIZE 32768
#define PAGE_SIZE 128

#define DATA_SIZE (PAGE_SIZE - BATTFS_HEADER_LEN)
#define PAGE_COUNT (FILE_SIZE / PAGE_SIZE)

#define HW_PAGEBUF true
#if UNIT_TEST

const char test_filename[]="battfs_disk.bin";

static uint8_t page_buffer[PAGE_SIZE];
static pgcnt_t page_array[PAGE_COUNT];

static void testCheck(BattFsSuper *disk, pgcnt_t *reference)
{
	ASSERT(battfs_fsck(disk));

	for (unsigned i = 0; i < disk->dev->blk_cnt; i++)
	{
		if (disk->page_array[i] != reference[i])
		{
			kprintf("Error at addr %d: page_array read", i);
			for (pgcnt_t i = 0; i < disk->dev->blk_cnt; i++)
			{
				if (!(i % 16))
					kputchar('\n');
				kprintf("%04d ", disk->page_array[i]);
			}
			kputchar('\n');
			kprintf("Expected:");
			for (pgcnt_t i = 0; i < disk->dev->blk_cnt; i++)
			{
				if (!(i % 16))
					kputchar('\n');
				kprintf("%04d ", reference[i]);
			}
			kputchar('\n');
			battfs_umount(disk);
			exit(2);
		}
	}

	ASSERT(battfs_fsck(disk));
	ASSERT(battfs_umount(disk));
}

static void diskNew(BattFsSuper *disk)
{
	pgcnt_t ref[PAGE_COUNT];

	TRACEMSG("1: disk new\n");

	FILE *fpt = fopen(test_filename, "w+");

	for (int i = 0; i < FILE_SIZE; i++)
		fputc(0xff, fpt);

	for (int i = 0; i < PAGE_COUNT; i++)
		ref[i] = i;

	KBlockPosix f;
	kblockposix_init(&f, fpt, HW_PAGEBUF, page_buffer, PAGE_SIZE, PAGE_COUNT);
	battfs_mount(disk, &f.b, page_array, sizeof(page_array));

	testCheck(disk, ref);
	TRACEMSG("1: passed\n");
}

static void disk1File(BattFsSuper *disk)
{
	pgcnt_t ref[PAGE_COUNT];
	TRACEMSG("2: disk full with 1 contiguos file\n");

	FILE *fp = fopen(test_filename, "w+");
	for (int i = 0; i < FILE_SIZE; i++)
		fputc(0xff, fp);

	KBlockPosix f;
	kblockposix_init(&f, fp, HW_PAGEBUF, page_buffer, PAGE_SIZE, PAGE_COUNT);

	for (int i = 0; i < PAGE_COUNT; i++)
	{
		battfs_writeTestBlock(&f.b, i, 0, 0, DATA_SIZE, i);
		ref[i] = i;
	}

	battfs_mount(disk, &f.b, page_array, sizeof(page_array));

	testCheck(disk, ref);
	TRACEMSG("2: passed\n");
}


static void diskHalfFile(BattFsSuper *disk)
{
	pgcnt_t ref[PAGE_COUNT];
	TRACEMSG("3: disk half full with 1 contiguos file, rest unformatted\n");


	FILE *fp = fopen(test_filename, "w+");
	for (int i = 0; i < FILE_SIZE; i++)
		fputc(0xff, fp);

	KBlockPosix f;
	kblockposix_init(&f, fp, HW_PAGEBUF, page_buffer, PAGE_SIZE, PAGE_COUNT);

	for (int i = 0; i < PAGE_COUNT / 2; i++)
	{
		battfs_writeTestBlock(&f.b, i, 0, 0, DATA_SIZE, i);
		ref[i] = i;
	}
	fseek(fp, FILE_SIZE / 2, SEEK_SET);
	for (int i = FILE_SIZE / 2; i < FILE_SIZE; i++)
		fputc(0xff, fp);

	for (int i = PAGE_COUNT / 2; i < PAGE_COUNT; i++)
	{
		ref[i] = i;
	}

	battfs_mount(disk, &f.b, page_array, sizeof(page_array));

	testCheck(disk, ref);
	TRACEMSG("3: passed\n");
}


static void oldSeq1(BattFsSuper *disk)
{
	pgcnt_t ref[4];
	TRACEMSG("6: 1 file with 1 old seq num, 1 free block\n");

	FILE *fp = fopen(test_filename, "w+");
	for (int i = 0; i < PAGE_SIZE * 4; i++)
		fputc(0xff, fp);
	KBlockPosix f;
	kblockposix_init(&f, fp, HW_PAGEBUF, page_buffer, PAGE_SIZE, 4);

	// page, inode, seq, fill, pgoff
	battfs_writeTestBlock(&f.b, 0, 0, 0, DATA_SIZE, 0);
	battfs_writeTestBlock(&f.b, 1, 0, 0, DATA_SIZE, 1);
	battfs_writeTestBlock(&f.b, 2, 0, 1, DATA_SIZE, 1);
	battfs_eraseBlock(&f.b, 3);

	ref[0] = 0;
	ref[1] = 2;
	ref[2] = 1;
	ref[3] = 3;

	battfs_mount(disk, &f.b, page_array, sizeof(page_array));

	testCheck(disk, ref);
	TRACEMSG("6: passed\n");
}

static void oldSeq2(BattFsSuper *disk)
{
	pgcnt_t ref[4];
	TRACEMSG("7: 1 file with 1 old seq num, 1 free block\n");

	FILE *fp = fopen(test_filename, "w+");
	for (int i = 0; i < PAGE_SIZE * 4; i++)
		fputc(0xff, fp);
	KBlockPosix f;
	kblockposix_init(&f, fp, HW_PAGEBUF, page_buffer, PAGE_SIZE, 4);

	// page, inode, seq, fill, pgoff
	battfs_writeTestBlock(&f.b, 0, 0, 0, DATA_SIZE, 0);
	battfs_writeTestBlock(&f.b, 1, 0, 1, DATA_SIZE, 1);
	battfs_writeTestBlock(&f.b, 2, 0, 0, DATA_SIZE, 1);
	battfs_eraseBlock(&f.b, 3);

	ref[0] = 0;
	ref[1] = 1;
	ref[2] = 2;
	ref[3] = 3;

	battfs_mount(disk, &f.b, page_array, sizeof(page_array));
	testCheck(disk, ref);
	TRACEMSG("7: passed\n");
}

static void oldSeq3(BattFsSuper *disk)
{
	pgcnt_t ref[4];
	TRACEMSG("8: 1 file with 1 old seq num, 1 free block\n");


	FILE *fp = fopen(test_filename, "w+");
	for (int i = 0; i < PAGE_SIZE * 4; i++)
		fputc(0xff, fp);
	KBlockPosix f;
	kblockposix_init(&f, fp, HW_PAGEBUF, page_buffer, PAGE_SIZE, 4);

	// page, inode, seq, fill, pgoff
	battfs_eraseBlock(&f.b, 0);
	battfs_writeTestBlock(&f.b, 1, 0, 0, DATA_SIZE, 0);
	battfs_writeTestBlock(&f.b, 2, 0, 1, DATA_SIZE, 1);
	battfs_writeTestBlock(&f.b, 3, 0, 0, DATA_SIZE, 1);

	ref[0] = 1;
	ref[1] = 2;
	ref[2] = 0;
	ref[3] = 3;

	battfs_mount(disk, &f.b, page_array, sizeof(page_array));
	testCheck(disk, ref);
	TRACEMSG("8: passed\n");
}

static void oldSeq2File(BattFsSuper *disk)
{
	pgcnt_t ref[8];
	TRACEMSG("9: 2 file with old seq num, 2 free block\n");


	FILE *fp = fopen(test_filename, "w+");
	for (int i = 0; i < PAGE_SIZE * 8; i++)
		fputc(0xff, fp);
	KBlockPosix f;
	kblockposix_init(&f, fp, HW_PAGEBUF, page_buffer, PAGE_SIZE, 8);

	// page, inode, seq, fill, pgoff
	battfs_eraseBlock(&f.b, 0);
	battfs_writeTestBlock(&f.b, 1, 0, 0, DATA_SIZE, 0);
	battfs_writeTestBlock(&f.b, 2, 0, 3, DATA_SIZE, 1);
	battfs_writeTestBlock(&f.b, 3, 0, 0, DATA_SIZE, 1);
	battfs_eraseBlock(&f.b, 4);
	battfs_writeTestBlock(&f.b, 5, 4, 0, DATA_SIZE, 0);
	battfs_writeTestBlock(&f.b, 6, 4, 1, DATA_SIZE, 1);
	battfs_writeTestBlock(&f.b, 7, 4, 0, DATA_SIZE, 1);


	ref[0] = 1;
	ref[1] = 2;
	ref[2] = 5;
	ref[3] = 6;
	ref[4] = 0;
	ref[5] = 3;
	ref[6] = 4;
	ref[7] = 7;

	battfs_mount(disk, &f.b, page_array, sizeof(page_array));
	testCheck(disk, ref);
	TRACEMSG("9: passed\n");
}

static void openFile(BattFsSuper *disk)
{
	BattFs fd1;
	BattFs fd2;
	TRACEMSG("10: open file test, inode 0 and inode 4\n");

	FILE *fp = fopen(test_filename, "w+");
	for (int i = 0; i < PAGE_SIZE * 8; i++)
		fputc(0xff, fp);
	KBlockPosix f;
	kblockposix_init(&f, fp, HW_PAGEBUF, page_buffer, PAGE_SIZE, 8);


	int PAGE_FILL = PAGE_SIZE - BATTFS_HEADER_LEN;
	inode_t INODE = 0;
	inode_t INODE2 = 4;
	inode_t INEXISTENT_INODE = 123;
	unsigned int MODE = 0;

	// page, inode, seq, fill, pgoff
	battfs_eraseBlock(&f.b, 0);
	battfs_writeTestBlock(&f.b, 1, INODE, 0, PAGE_FILL, 0);
	battfs_writeTestBlock(&f.b, 2, INODE, 3, PAGE_FILL, 1);
	battfs_writeTestBlock(&f.b, 3, INODE, 0, PAGE_FILL, 1);
	battfs_eraseBlock(&f.b, 4);
	battfs_writeTestBlock(&f.b, 5, INODE2, 0, PAGE_FILL, 0);
	battfs_writeTestBlock(&f.b, 6, INODE2, 1, PAGE_FILL, 1);
	battfs_writeTestBlock(&f.b, 7, INODE2, 0, PAGE_FILL, 1);

	ASSERT(battfs_mount(disk, &f.b, page_array, sizeof(page_array)));
	ASSERT(battfs_fsck(disk));
	ASSERT(!battfs_fileExists(disk, INEXISTENT_INODE));

	ASSERT(battfs_fileExists(disk, INODE));
	ASSERT(battfs_fileopen(disk, &fd1, INODE, MODE));
	ASSERT(fd1.fd.size == PAGE_FILL * 2);
	ASSERT(fd1.fd.seek_pos == 0);
	ASSERT(fd1.mode == MODE);
	ASSERT(fd1.inode == INODE);
	ASSERT(fd1.start == &disk->page_array[0]);
	ASSERT(fd1.disk == disk);
	ASSERT(LIST_HEAD(&disk->file_opened_list) == &fd1.link);

	ASSERT(kfile_reopen(&fd1.fd) == &fd1.fd);
	ASSERT(fd1.fd.size == PAGE_FILL * 2);
	ASSERT(fd1.fd.seek_pos == 0);
	ASSERT(fd1.mode == MODE);
	ASSERT(fd1.inode == INODE);
	ASSERT(fd1.start == &disk->page_array[0]);
	ASSERT(fd1.disk == disk);
	ASSERT(LIST_HEAD(&disk->file_opened_list) == &fd1.link);

	ASSERT(battfs_fileExists(disk, INODE2));
	ASSERT(battfs_fileopen(disk, &fd2, INODE2, MODE));
	ASSERT(fd2.fd.size == PAGE_FILL * 2);
	ASSERT(fd2.fd.seek_pos == 0);
	ASSERT(fd2.mode == MODE);
	ASSERT(fd2.inode == INODE2);
	ASSERT(fd2.start == &disk->page_array[2]);
	ASSERT(fd2.disk == disk);
	ASSERT(LIST_HEAD(&disk->file_opened_list)->succ == &fd2.link);

	ASSERT(kfile_close(&fd1.fd) == 0);
	ASSERT(kfile_error(&fd1.fd) == 0);
	ASSERT(kfile_close(&fd2.fd) == 0);
	ASSERT(kfile_error(&fd2.fd) == 0);
	ASSERT(LIST_EMPTY(&disk->file_opened_list));
	ASSERT(battfs_fsck(disk));
	ASSERT(battfs_umount(disk));

	TRACEMSG("10: passed\n");
}

static void readFile(BattFsSuper *disk)
{
	BattFs fd1;
	uint8_t buf[16];

	TRACEMSG("11: read file test\n");

	FILE *fp = fopen(test_filename, "w+");
	for (int i = 0; i < PAGE_SIZE * 8; i++)
		fputc(0xff, fp);
	KBlockPosix f;
	kblockposix_init(&f, fp, HW_PAGEBUF, page_buffer, PAGE_SIZE, 8);


	unsigned int PAGE_FILL = PAGE_SIZE - BATTFS_HEADER_LEN;
	inode_t INODE = 0;
	inode_t INODE2 = 4;
	unsigned int MODE = 0;

	battfs_eraseBlock(&f.b, 0);
	battfs_writeTestBlock(&f.b, 1, INODE, 0, PAGE_FILL, 0);
	battfs_writeTestBlock(&f.b, 2, INODE, 3, PAGE_FILL, 1);
	battfs_writeTestBlock(&f.b, 3, INODE, 0, PAGE_FILL, 1);
	battfs_eraseBlock(&f.b, 4);
	battfs_writeTestBlock(&f.b, 5, INODE2, 0, PAGE_FILL, 0);
	battfs_writeTestBlock(&f.b, 6, INODE2, 1, PAGE_FILL, 1);
	battfs_writeTestBlock(&f.b, 7, INODE2, 0, PAGE_FILL, 1);

	ASSERT(battfs_mount(disk, &f.b, page_array, sizeof(page_array)));
	ASSERT(battfs_fsck(disk));
	ASSERT(battfs_fileopen(disk, &fd1, INODE, MODE));
	ASSERT(kfile_read(&fd1.fd, buf, sizeof(buf)) == sizeof(buf));
	ASSERT(fd1.fd.seek_pos == sizeof(buf));
	for (size_t i = 0; i < sizeof(buf); i++)
		ASSERT(buf[i] == 0xff);

	ASSERT(kfile_close(&fd1.fd) == 0);
	ASSERT(kfile_error(&fd1.fd) == 0);
	ASSERT(battfs_fsck(disk));
	ASSERT(battfs_umount(disk));

	TRACEMSG("11: passed\n");
}

static void readAcross(BattFsSuper *disk)
{
	BattFs fd1;

	TRACEMSG("12: read file test across page boundary and seek test\n");

	FILE *fp = fopen(test_filename, "w+");
	for (int i = 0; i < PAGE_SIZE * 8; i++)
		fputc(0xff, fp);
	KBlockPosix f;
	kblockposix_init(&f, fp, HW_PAGEBUF, page_buffer, PAGE_SIZE, 8);

	const unsigned int PAGE_FILL = PAGE_SIZE - BATTFS_HEADER_LEN;
	inode_t INODE = 0;
	unsigned int MODE = 0;
	uint8_t buf[PAGE_FILL + BATTFS_HEADER_LEN / 2];

	battfs_eraseBlock(&f.b, 0);
	battfs_writeTestBlock(&f.b, 1, INODE, 0, PAGE_FILL, 0);
	battfs_writeTestBlock(&f.b, 2, INODE, 3, PAGE_FILL, 1);
	battfs_writeTestBlock(&f.b, 3, INODE, 0, PAGE_FILL, 1);
	battfs_eraseBlock(&f.b, 4);
	battfs_writeTestBlock(&f.b, 5, INODE, 0, PAGE_FILL, 2);
	battfs_writeTestBlock(&f.b, 6, INODE, 1, PAGE_FILL, 3);
	battfs_writeTestBlock(&f.b, 7, INODE, 0, PAGE_FILL, 3);

	ASSERT(battfs_mount(disk, &f.b, page_array, sizeof(page_array)));
	ASSERT(battfs_fsck(disk));
	ASSERT(battfs_fileopen(disk, &fd1, INODE, MODE));

	ASSERT(kfile_read(&fd1.fd, buf, sizeof(buf)) == sizeof(buf));
	ASSERT(fd1.fd.seek_pos == (kfile_off_t)sizeof(buf));
	for (size_t i = 0; i < sizeof(buf); i++)
		ASSERT(buf[i] == 0xff);

	ASSERT(kfile_read(&fd1.fd, buf, sizeof(buf)) == sizeof(buf));
	ASSERT(fd1.fd.seek_pos == (kfile_off_t)sizeof(buf) * 2);
	for (size_t i = 0; i < sizeof(buf); i++)
		ASSERT(buf[i] == 0xff);

	ASSERT(kfile_read(&fd1.fd, buf, sizeof(buf)) == sizeof(buf));
	ASSERT(fd1.fd.seek_pos == (kfile_off_t)sizeof(buf) * 3);
	for (size_t i = 0; i < sizeof(buf); i++)
		ASSERT(buf[i] == 0xff);

	ASSERT(kfile_read(&fd1.fd, buf, sizeof(buf)) == PAGE_FILL * 4 - sizeof(buf) * 3);
	ASSERT(fd1.fd.seek_pos == (kfile_off_t)fd1.fd.size);
	for (size_t i = 0; i < PAGE_FILL * 4 - sizeof(buf) * 3; i++)
		ASSERT(buf[i] == 0xff);

	ASSERT(kfile_seek(&fd1.fd, 0, KSM_SEEK_SET) == 0);
	ASSERT(fd1.fd.seek_pos == 0);

	ASSERT(kfile_seek(&fd1.fd, 0, KSM_SEEK_END) == (kfile_off_t)fd1.fd.size);
	ASSERT(fd1.fd.seek_pos = (kfile_off_t)fd1.fd.size);

	ASSERT(kfile_close(&fd1.fd) == 0);
	ASSERT(kfile_error(&fd1.fd) == 0);
	ASSERT(battfs_fsck(disk));
	ASSERT(battfs_umount(disk));

	TRACEMSG("12: passed\n");
}


static void writeFile(BattFsSuper *disk)
{
	BattFs fd1;
	uint8_t buf[PAGE_SIZE - BATTFS_HEADER_LEN];

	TRACEMSG("13: write file test\n");

	FILE *fp = fopen(test_filename, "w+");
	for (int i = 0; i < PAGE_SIZE * 8; i++)
		fputc(0xff, fp);
	KBlockPosix f;
	kblockposix_init(&f, fp, HW_PAGEBUF, page_buffer, PAGE_SIZE, 8);


	unsigned int PAGE_FILL = PAGE_SIZE - BATTFS_HEADER_LEN;
	inode_t INODE = 0;
	inode_t INODE2 = 4;
	unsigned int MODE = 0;

	battfs_eraseBlock(&f.b, 0);
	battfs_writeTestBlock(&f.b, 1, INODE, 0, PAGE_FILL, 0);
	battfs_writeTestBlock(&f.b, 2, INODE, 3, PAGE_FILL, 1);
	battfs_writeTestBlock(&f.b, 3, INODE, 0, PAGE_FILL, 1);
	battfs_eraseBlock(&f.b, 4);
	battfs_writeTestBlock(&f.b, 5, INODE2, 0, PAGE_FILL, 0);
	battfs_writeTestBlock(&f.b, 6, INODE2, 1, PAGE_FILL, 1);
	battfs_writeTestBlock(&f.b, 7, INODE2, 0, PAGE_FILL, 1);

	for (size_t i = 0; i < sizeof(buf); i++)
		buf[i] = i;

	ASSERT(battfs_mount(disk, &f.b, page_array, sizeof(page_array)));
	ASSERT(battfs_fsck(disk));
	ASSERT(battfs_fileopen(disk, &fd1, INODE, MODE));
	ASSERT(kfile_write(&fd1.fd, buf, sizeof(buf)) == sizeof(buf));
	ASSERT(fd1.fd.seek_pos == sizeof(buf));
	ASSERT(kfile_seek(&fd1.fd, 0, KSM_SEEK_SET) == 0);
	ASSERT(fd1.fd.seek_pos == 0);

	memset(buf, 0, sizeof(buf));
	ASSERT(kfile_read(&fd1.fd, buf, sizeof(buf)) == sizeof(buf));
	for (size_t i = 0; i < sizeof(buf); i++)
		ASSERT(buf[i] == i);

	ASSERT(kfile_close(&fd1.fd) == 0);
	ASSERT(kfile_error(&fd1.fd) == 0);
	ASSERT(battfs_fsck(disk));
	ASSERT(battfs_umount(disk));

	TRACEMSG("13: passed\n");
}

static void writeAcross(BattFsSuper *disk)
{
	BattFs fd1;

	TRACEMSG("14: write file test across page boundary and seek test\n");

	FILE *fp = fopen(test_filename, "w+");
	for (int i = 0; i < PAGE_SIZE * 8; i++)
		fputc(0xff, fp);
	KBlockPosix f;
	kblockposix_init(&f, fp, HW_PAGEBUF, page_buffer, PAGE_SIZE, 8);

	const unsigned int PAGE_FILL = PAGE_SIZE - BATTFS_HEADER_LEN;
	inode_t INODE = 0;
	unsigned int MODE = 0;
	uint8_t buf[PAGE_FILL + BATTFS_HEADER_LEN / 2];

	battfs_eraseBlock(&f.b, 0);
	battfs_writeTestBlock(&f.b, 1, INODE, 0, PAGE_FILL, 0);
	battfs_writeTestBlock(&f.b, 2, INODE, 3, PAGE_FILL, 1);
	battfs_writeTestBlock(&f.b, 3, INODE, 0, PAGE_FILL, 1);
	battfs_eraseBlock(&f.b, 4);
	battfs_writeTestBlock(&f.b, 5, INODE, 0, PAGE_FILL, 2);
	battfs_writeTestBlock(&f.b, 6, INODE, 1, PAGE_FILL, 3);
	battfs_writeTestBlock(&f.b, 7, INODE, 0, PAGE_FILL, 3);

	ASSERT(battfs_mount(disk, &f.b, page_array, sizeof(page_array)));
	ASSERT(battfs_fsck(disk));
	ASSERT(battfs_fileopen(disk, &fd1, INODE, MODE));

	uint8_t val = 0;
	for (size_t i = 0; i < sizeof(buf); i++)
		buf[i] = val++;
	ASSERT(kfile_write(&fd1.fd, buf, sizeof(buf)) == sizeof(buf));
	ASSERT(fd1.fd.seek_pos == (kfile_off_t)sizeof(buf));

	for (size_t i = 0; i < sizeof(buf); i++)
		buf[i] = val++;
	ASSERT(kfile_write(&fd1.fd, buf, sizeof(buf)) == sizeof(buf));
	ASSERT(fd1.fd.seek_pos == (kfile_off_t)sizeof(buf) * 2);

	for (size_t i = 0; i < sizeof(buf); i++)
		buf[i] = val++;
	ASSERT(kfile_write(&fd1.fd, buf, sizeof(buf)) == sizeof(buf));
	ASSERT(fd1.fd.seek_pos == (kfile_off_t)sizeof(buf) * 3);

	ASSERT(kfile_seek(&fd1.fd, 0, KSM_SEEK_SET) == 0);
	ASSERT(fd1.fd.seek_pos == 0);
	val = 0;

	memset(buf, 0, sizeof(buf));
	ASSERT(kfile_read(&fd1.fd, buf, sizeof(buf)) == sizeof(buf));
	for (size_t i = 0; i < sizeof(buf); i++)
		ASSERT(buf[i] == val++);

	memset(buf, 0, sizeof(buf));
	ASSERT(kfile_read(&fd1.fd, buf, sizeof(buf)) == sizeof(buf));
	for (size_t i = 0; i < sizeof(buf); i++)
		ASSERT(buf[i] == val++);

	memset(buf, 0, sizeof(buf));
	ASSERT(kfile_read(&fd1.fd, buf, sizeof(buf)) == sizeof(buf));
	for (size_t i = 0; i < sizeof(buf); i++)
		ASSERT(buf[i] == val++);

	ASSERT(fd1.fd.seek_pos == (kfile_off_t)sizeof(buf) * 3);

	ASSERT(kfile_close(&fd1.fd) == 0);
	ASSERT(kfile_error(&fd1.fd) == 0);
	ASSERT(battfs_fsck(disk));
	ASSERT(battfs_umount(disk));

	TRACEMSG("14: passed\n");
}

static void createFile(BattFsSuper *disk)
{
	TRACEMSG("15: file creation on new disk\n");

	FILE *fpt = fopen(test_filename, "w+");
	for (int i = 0; i < FILE_SIZE; i++)
		fputc(0xff, fpt);
	KBlockPosix f;
	kblockposix_init(&f, fpt, HW_PAGEBUF, page_buffer, PAGE_SIZE, PAGE_COUNT);

	BattFs fd1;
	inode_t INODE = 0;
	unsigned int MODE = BATTFS_CREATE;

	ASSERT(battfs_mount(disk, &f.b, page_array, sizeof(page_array)));
	ASSERT(battfs_fsck(disk));
	ASSERT(battfs_fileopen(disk, &fd1, INODE, MODE));
	for (int i = 0; i < FILE_SIZE / 2; i++)
		ASSERT(kfile_putc(i, &fd1.fd) != EOF);

	ASSERT(fd1.fd.seek_pos == FILE_SIZE / 2);
	ASSERT(fd1.fd.size == FILE_SIZE / 2);
	ASSERT(kfile_close(&fd1.fd) == 0);
	ASSERT(kfile_error(&fd1.fd) == 0);
	ASSERT(battfs_fsck(disk));
	ASSERT(battfs_umount(disk));

	fpt = fopen(test_filename, "r+");
	kblockposix_init(&f, fpt, HW_PAGEBUF, page_buffer, PAGE_SIZE, PAGE_COUNT);

	ASSERT(battfs_mount(disk, &f.b, page_array, sizeof(page_array)));
	ASSERT(battfs_fsck(disk));
	ASSERT(battfs_fileopen(disk, &fd1, INODE, 0));
	ASSERT(fd1.fd.size == FILE_SIZE / 2);
	ASSERT(fd1.fd.seek_pos == 0);

	uint8_t buf[FILE_SIZE / 2];
	memset(buf, 0, sizeof(buf));
	ASSERT(kfile_read(&fd1.fd, buf, FILE_SIZE / 2) == FILE_SIZE / 2);

	for (int i = 0; i < FILE_SIZE / 2; i++)
		ASSERT(buf[i] == (i & 0xff));

	ASSERT(fd1.fd.seek_pos == FILE_SIZE / 2);
	ASSERT(kfile_close(&fd1.fd) == 0);
	ASSERT(kfile_error(&fd1.fd) == 0);
	ASSERT(battfs_fsck(disk));
	ASSERT(battfs_umount(disk));


	TRACEMSG("15: passed\n");
}

static void multipleWrite(BattFsSuper *disk)
{
	TRACEMSG("16: multiple write on file\n");

	FILE *fpt = fopen(test_filename, "w+");
	for (int i = 0; i < FILE_SIZE; i++)
		fputc(0xff, fpt);
	KBlockPosix f;
	kblockposix_init(&f, fpt, HW_PAGEBUF, page_buffer, PAGE_SIZE, PAGE_COUNT);


	BattFs fd1;
	inode_t INODE = 0;
	unsigned int MODE = BATTFS_CREATE;
	uint8_t buf[1000];

	ASSERT(battfs_mount(disk, &f.b, page_array, sizeof(page_array)));
	ASSERT(battfs_fsck(disk));
	ASSERT(battfs_fileopen(disk, &fd1, INODE, MODE));

	int j;
	for (j = 1; j < 1013; j++)
	{
		for (unsigned i = 0; i < sizeof(buf); i++)
			buf[i] = j+i;

		ASSERT(kfile_write(&fd1.fd, buf, sizeof(buf)) == sizeof(buf));
		ASSERT(fd1.fd.seek_pos == sizeof(buf));
		ASSERT(fd1.fd.size == sizeof(buf));
		ASSERT(kfile_seek(&fd1.fd, 0, KSM_SEEK_SET) == 0);
		memset(buf, 0, sizeof(buf));
		ASSERT(kfile_read(&fd1.fd, buf, sizeof(buf)) == sizeof(buf));
		ASSERT(fd1.fd.seek_pos == sizeof(buf));
		for (unsigned i = 0; i < sizeof(buf); i++)
			ASSERT(buf[i] == ((j+i) & 0xff));
		ASSERT(kfile_seek(&fd1.fd, 0, KSM_SEEK_SET) == 0);
		ASSERT(disk->free_bytes == disk->disk_size - sizeof(buf));
	}
	ASSERT(kfile_close(&fd1.fd) == 0);
	ASSERT(kfile_error(&fd1.fd) == 0);
	ASSERT(battfs_fsck(disk));
	ASSERT(battfs_umount(disk));

	fpt = fopen(test_filename, "r+");
	kblockposix_init(&f, fpt, HW_PAGEBUF, page_buffer, PAGE_SIZE, PAGE_COUNT);

	ASSERT(battfs_mount(disk, &f.b, page_array, sizeof(page_array)));
	ASSERT(battfs_fsck(disk));
	ASSERT(disk->free_bytes == disk->disk_size - sizeof(buf));
	ASSERT(battfs_fileopen(disk, &fd1, INODE, 0));
	ASSERT(fd1.fd.size == sizeof(buf));
	memset(buf, 0, sizeof(buf));
	ASSERT(kfile_read(&fd1.fd, buf, sizeof(buf)) == sizeof(buf));
	for (unsigned i = 0; i < sizeof(buf); i++)
			ASSERT(buf[i] == ((j-1+i) & 0xff));
	ASSERT(kfile_close(&fd1.fd) == 0);
	ASSERT(kfile_error(&fd1.fd) == 0);
	ASSERT(battfs_fsck(disk));
	ASSERT(battfs_umount(disk));


	TRACEMSG("16: passed\n");
}

static void increaseFile(BattFsSuper *disk)
{
	TRACEMSG("17: increasing dimension of a file with multiple open files.\n");

	FILE *fpt = fopen(test_filename, "w+");
	for (int i = 0; i < FILE_SIZE / 10; i++)
		fputc(0xff, fpt);

	KBlockPosix f;
	kblockposix_init(&f, fpt, HW_PAGEBUF, page_buffer, PAGE_SIZE, PAGE_COUNT / 10);


	BattFs fd1,fd2;
	inode_t INODE1 = 1, INODE2 = 2;
	unsigned int MODE = BATTFS_CREATE;
	uint8_t buf[1000];

	ASSERT(battfs_mount(disk, &f.b, page_array, sizeof(page_array)));
	ASSERT(battfs_fsck(disk));
	ASSERT(battfs_fileopen(disk, &fd1, INODE1, MODE));
	ASSERT(battfs_fileopen(disk, &fd2, INODE2, MODE));
	for (unsigned i = 0; i < sizeof(buf); i++)
		ASSERT(kfile_putc(i, &fd2.fd) != EOF);
	ASSERT(kfile_seek(&fd2.fd, 0, KSM_SEEK_SET) == 0);
	memset(buf, 0, sizeof(buf));
	ASSERT(kfile_read(&fd2.fd, buf, sizeof(buf)) == sizeof(buf));

	for (unsigned i = 0; i < sizeof(buf); i++)
		ASSERT(buf[i] == (i & 0xff));
	ASSERT(kfile_seek(&fd2.fd, 0, KSM_SEEK_SET) == 0);

	for (unsigned i = 0; i < sizeof(buf); i++)
		ASSERT(kfile_putc(i, &fd1.fd) != EOF);

	memset(buf, 0, sizeof(buf));
	ASSERT(kfile_read(&fd2.fd, buf, sizeof(buf)) == sizeof(buf));

	for (unsigned i = 0; i < sizeof(buf); i++)
		ASSERT(buf[i] == (i & 0xff));

	ASSERT(kfile_close(&fd1.fd) == 0);
	ASSERT(kfile_error(&fd1.fd) == 0);
	ASSERT(kfile_close(&fd2.fd) == 0);
	ASSERT(kfile_error(&fd2.fd) == 0);
	ASSERT(battfs_fsck(disk));
	ASSERT(battfs_umount(disk));

	TRACEMSG("17: passed\n");
}

static void readEOF(BattFsSuper *disk)
{
	BattFs fd1;
	uint8_t buf[16];

	TRACEMSG("18: reading over EOF test\n");

	FILE *fp = fopen(test_filename, "w+");
	for (int i = 0; i < PAGE_SIZE * 8; i++)
		fputc(0xff, fp);
	KBlockPosix f;
	kblockposix_init(&f, fp, HW_PAGEBUF, page_buffer, PAGE_SIZE, 8);


	unsigned int PAGE_FILL = PAGE_SIZE - BATTFS_HEADER_LEN;
	inode_t INODE = 0;
	inode_t INODE2 = 4;
	unsigned int MODE = 0;

	battfs_eraseBlock(&f.b, 0);
	battfs_writeTestBlock(&f.b, 1, INODE, 0, PAGE_FILL, 0);
	battfs_writeTestBlock(&f.b, 2, INODE, 3, PAGE_FILL, 1);
	battfs_writeTestBlock(&f.b, 3, INODE, 0, PAGE_FILL, 1);
	battfs_eraseBlock(&f.b, 4);
	battfs_writeTestBlock(&f.b, 5, INODE2, 0, PAGE_FILL, 0);
	battfs_writeTestBlock(&f.b, 6, INODE2, 1, PAGE_FILL, 1);
	battfs_writeTestBlock(&f.b, 7, INODE2, 0, PAGE_FILL, 1);

	ASSERT(battfs_mount(disk, &f.b, page_array, sizeof(page_array)));
	ASSERT(battfs_fsck(disk));
	ASSERT(battfs_fileopen(disk, &fd1, INODE, MODE));
	ASSERT(kfile_seek(&fd1.fd, fd1.fd.size + 10, SEEK_SET) == fd1.fd.size + 10);
	ASSERT(fd1.fd.seek_pos == fd1.fd.size + 10);
	ASSERT(kfile_read(&fd1.fd, buf, sizeof(buf)) == 0);

	ASSERT(kfile_close(&fd1.fd) == 0);
	ASSERT(kfile_error(&fd1.fd) == 0);
	ASSERT(battfs_fsck(disk));
	ASSERT(battfs_umount(disk));

	TRACEMSG("18: passed\n");
}

static void writeEOF(BattFsSuper *disk)
{
	TRACEMSG("19: writing over EOF test\n");

	FILE *fpt = fopen(test_filename, "w+");
	for (int i = 0; i < FILE_SIZE / 5; i++)
		fputc(0xff, fpt);
	KBlockPosix f;
	kblockposix_init(&f, fpt, HW_PAGEBUF, page_buffer, PAGE_SIZE, PAGE_COUNT / 5);

	BattFs fd1;
	inode_t INODE = 0;
	unsigned int MODE = BATTFS_CREATE;
	uint8_t buf[FILE_SIZE / 13];

	for (int i = 0; i < 2; i++)
		buf[i] = i;

	ASSERT(battfs_mount(disk, &f.b, page_array, sizeof(page_array)));
	ASSERT(battfs_fsck(disk));
	disk_size_t prev_free = disk->free_bytes;
	ASSERT(battfs_fileopen(disk, &fd1, INODE, MODE));
	ASSERT(fd1.fd.size == 0);

	ASSERT(kfile_seek(&fd1.fd, 2, KSM_SEEK_END) == 2);
	ASSERT(kfile_write(&fd1.fd, buf, 2));
	ASSERT(fd1.fd.seek_pos == 4);
	ASSERT(fd1.fd.size == 4);
	ASSERT(disk->free_bytes == prev_free - 4);
	ASSERT(kfile_seek(&fd1.fd, 0, KSM_SEEK_SET) == 0);
	memset(buf, 0, 2);
	ASSERT(kfile_read(&fd1.fd, buf, 2) == 2);
	for (int i = 0; i < 2; i++)
		ASSERT(buf[i] == 0);

	memset(buf, 0, 2);
	ASSERT(kfile_read(&fd1.fd, buf, 2) == 2);
	for (int i = 0; i < 2; i++)
		ASSERT(buf[i] == (i & 0xff));

	ASSERT(kfile_seek(&fd1.fd, sizeof(buf), KSM_SEEK_END) == sizeof(buf) + 4);
	for (unsigned i = 0; i < sizeof(buf); i++)
		buf[i] = i;
	ASSERT(kfile_write(&fd1.fd, buf, sizeof(buf)));
	ASSERT(fd1.fd.seek_pos == sizeof(buf) * 2 + 4);
	ASSERT(fd1.fd.size == sizeof(buf) * 2 + 4);
	ASSERT(disk->free_bytes == prev_free - sizeof(buf) * 2 - 4);

	ASSERT(kfile_seek(&fd1.fd, 0, KSM_SEEK_SET) == 0);

	memset(buf, 0, 2);
	ASSERT(kfile_read(&fd1.fd, buf, 2) == 2);
	ASSERT(fd1.fd.seek_pos == 2);
	for (int i = 0; i < 2; i++)
		ASSERT(buf[i] == 0);

	memset(buf, 0, 2);
	ASSERT(kfile_read(&fd1.fd, buf, 2) == 2);
	ASSERT(fd1.fd.seek_pos == 4);
	for (int i = 0; i < 2; i++)
		ASSERT(buf[i] == (i & 0xff));

	memset(buf, 0, 4);
	ASSERT(kfile_read(&fd1.fd, buf, sizeof(buf)) == sizeof(buf));
	ASSERT(fd1.fd.seek_pos == sizeof(buf) + 4);
	for (unsigned i = 0; i < sizeof(buf); i++)
		ASSERT(buf[i] == 0);

	memset(buf, 0, sizeof(buf));
	ASSERT(kfile_read(&fd1.fd, buf, sizeof(buf)) == sizeof(buf));
	for (unsigned i = 0; i < sizeof(buf); i++)
		ASSERT(buf[i] == (i & 0xff));

	ASSERT(kfile_close(&fd1.fd) == 0);
	ASSERT(kfile_error(&fd1.fd) == 0);
	ASSERT(battfs_fsck(disk));
	ASSERT(battfs_umount(disk));

	TRACEMSG("19: passed\n");

}

static void endOfSpace(BattFsSuper *disk)
{
	TRACEMSG("20: what happens when disk space is over?\n");
	BattFs fd1;
	uint8_t buf[(PAGE_SIZE - BATTFS_HEADER_LEN) * 5];

	FILE *fp = fopen(test_filename, "w+");
	for (int i = 0; i < PAGE_SIZE * 4; i++)
		fputc(0xff, fp);
	KBlockPosix f;
	kblockposix_init(&f, fp, HW_PAGEBUF, page_buffer, PAGE_SIZE, 4);

	unsigned int PAGE_FILL = PAGE_SIZE - BATTFS_HEADER_LEN;
	inode_t INODE = 0;
	unsigned int MODE = BATTFS_CREATE;

	battfs_eraseBlock(&f.b, 0);
	battfs_eraseBlock(&f.b, 1);
	battfs_eraseBlock(&f.b, 2);
	battfs_eraseBlock(&f.b, 3);

	ASSERT(battfs_mount(disk, &f.b, page_array, sizeof(page_array)));
	ASSERT(battfs_fsck(disk));
	ASSERT(battfs_fileopen(disk, &fd1, INODE, MODE));
	ASSERT(kfile_write(&fd1.fd, buf, sizeof(buf)) == PAGE_FILL * 4);
	ASSERT(fd1.fd.size == (kfile_off_t)(PAGE_FILL * 4));
	ASSERT(fd1.fd.seek_pos == (kfile_off_t)(PAGE_FILL * 4));
	ASSERT(disk->free_bytes == 0);

	ASSERT(kfile_close(&fd1.fd) == 0);
	ASSERT(kfile_error(&fd1.fd) == BATTFS_DISK_SPACEOVER_ERR);
	ASSERT(battfs_fsck(disk));
	ASSERT(battfs_umount(disk));

	TRACEMSG("20: passed\n");
}


static void multipleFilesRW(BattFsSuper *disk)
{
	TRACEMSG("21: multiple files read/write test\n");

	FILE *fpt = fopen(test_filename, "w+");
	for (int i = 0; i < FILE_SIZE; i++)
		fputc(0xff, fpt);
	KBlockPosix f;
	kblockposix_init(&f, fpt, HW_PAGEBUF, page_buffer, PAGE_SIZE, PAGE_COUNT);

	#define N_FILES 10
	BattFs fd[N_FILES];
	unsigned int MODE = BATTFS_CREATE;
	uint32_t buf[FILE_SIZE / (4 * N_FILES * sizeof(uint32_t))];

	ASSERT(battfs_mount(disk, &f.b, page_array, sizeof(page_array)));
	ASSERT(battfs_fsck(disk));
	for (inode_t i = 0; i < N_FILES; i++)
		ASSERT(battfs_fileopen(disk, &fd[i], i, MODE));

	for (int i = N_FILES - 1; i >= 0; i--)
	{
		for (uint32_t j = 0; j < countof(buf); j++)
			buf[j] = j+i;

		ASSERT(kfile_write(&fd[i].fd, buf, sizeof(buf)) == sizeof(buf));
		ASSERT(fd[i].fd.size == sizeof(buf));
		ASSERT(fd[i].fd.seek_pos == sizeof(buf));
		ASSERT(kfile_seek(&fd[i].fd, 0, SEEK_SET) == 0);
	}

	for (inode_t i = 0; i < N_FILES; i++)
	{
		memset(buf, 0, sizeof(buf));
		ASSERT(kfile_read(&fd[i].fd, buf, sizeof(buf)) == sizeof(buf));

		for (uint32_t j = 0; j < countof(buf); j++)
			ASSERT(buf[j] == j+i);

		ASSERT(fd[i].fd.size == sizeof(buf));
		ASSERT(fd[i].fd.seek_pos == sizeof(buf));
		ASSERT(kfile_seek(&fd[i].fd, 0, SEEK_SET) == 0);
	}

	for (inode_t i = 0; i < N_FILES; i++)
	{
		ASSERT(kfile_close(&fd[i].fd) == 0);
		ASSERT(kfile_error(&fd[i].fd) == 0);
	}

	ASSERT(battfs_fsck(disk));
	ASSERT(battfs_umount(disk));

	fpt = fopen(test_filename, "r+");
	kblockposix_init(&f, fpt, HW_PAGEBUF, page_buffer, PAGE_SIZE, PAGE_COUNT);

	ASSERT(battfs_mount(disk, &f.b, page_array, sizeof(page_array)));
	ASSERT(battfs_fsck(disk));

	for (inode_t i = 0; i < N_FILES; i++)
		ASSERT(battfs_fileopen(disk, &fd[i], i, 0));

	for (inode_t i = 0; i < N_FILES; i++)
	{
		memset(buf, 0, sizeof(buf));
		ASSERT(kfile_read(&fd[i].fd, buf, sizeof(buf)) == sizeof(buf));

		for (uint32_t j = 0; j < countof(buf); j++)
			ASSERT(buf[j] == j+i);

		ASSERT(fd[i].fd.size == sizeof(buf));
		ASSERT(fd[i].fd.seek_pos == sizeof(buf));
		ASSERT(kfile_seek(&fd[i].fd, 0, SEEK_SET) == 0);
	}

	for (inode_t i = 0; i < N_FILES; i++)
	{
		ASSERT(kfile_close(&fd[i].fd) == 0);
		ASSERT(kfile_error(&fd[i].fd) == 0);
	}

	ASSERT(battfs_umount(disk));
	TRACEMSG("21: passed\n");
}


static void openAllFiles(BattFsSuper *disk)
{
	TRACEMSG("22: try to open a lot of files\n");

	FILE *fpt = fopen(test_filename, "w+");
	for (int i = 0; i < FILE_SIZE; i++)
		fputc(0xff, fpt);
	KBlockPosix f;
	kblockposix_init(&f, fpt, HW_PAGEBUF, page_buffer, PAGE_SIZE, PAGE_COUNT);

	BattFs fd[BATTFS_MAX_FILES];
	unsigned int MODE = BATTFS_CREATE;

	ASSERT(battfs_mount(disk, &f.b, page_array, sizeof(page_array)));
	ASSERT(battfs_fsck(disk));
	for (unsigned i = 0; i < countof(fd); i++)
		ASSERT(battfs_fileopen(disk, &fd[i], i, MODE));

	ASSERT(battfs_fsck(disk));

	for (unsigned i = 0; i < countof(fd); i++)
	{
		ASSERT(kfile_close(&fd[i].fd) == 0);
		ASSERT(kfile_error(&fd[i].fd) == 0);
	}

	ASSERT(battfs_fsck(disk));
	ASSERT(battfs_umount(disk));


	fpt = fopen(test_filename, "r+");
	kblockposix_init(&f, fpt, HW_PAGEBUF, page_buffer, PAGE_SIZE, PAGE_COUNT);

	ASSERT(battfs_mount(disk, &f.b, page_array, sizeof(page_array)));
	ASSERT(battfs_fsck(disk));


	for (unsigned i = 0; i < countof(fd); i++)
		ASSERT(battfs_fileopen(disk, &fd[i], i, MODE));

	ASSERT(battfs_fsck(disk));

	for (unsigned i = 0; i < countof(fd); i++)
	{
		ASSERT(kfile_close(&fd[i].fd) == 0);
		ASSERT(kfile_error(&fd[i].fd) == 0);
	}

	ASSERT(battfs_fsck(disk));
	ASSERT(battfs_umount(disk));
	TRACEMSG("22: passed\n");
}


int battfs_testRun(void)
{
	BattFsSuper disk;

	diskNew(&disk);
	disk1File(&disk);
	diskHalfFile(&disk);
	oldSeq1(&disk);
	oldSeq2(&disk);
	oldSeq3(&disk);
	oldSeq2File(&disk);
	openFile(&disk);
	readFile(&disk);
	readAcross(&disk);
	writeFile(&disk);
	writeAcross(&disk);
	createFile(&disk);
	multipleWrite(&disk);
	increaseFile(&disk);
	readEOF(&disk);
	writeEOF(&disk);
	endOfSpace(&disk);
	multipleFilesRW(&disk);
	openAllFiles(&disk);

	kprintf("All tests passed!\n");

	return 0;
}

int battfs_testSetup(void)
{
	return 0;
}

int battfs_testTearDown(void)
{
	return 0;
}

TEST_MAIN(battfs);

#endif // _TEST
