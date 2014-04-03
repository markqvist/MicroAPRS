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
 * Copyright 2004 Giovanni Bajo
 * All Rights Reserved.
 * -->
 *
 * \brief Line editing support with history
 *
 * Rationale for basic implementation choices:
 *
 * \li The history is implemented storing consecutive ASCIIZ strings within an array of memory. When
 * the history is full, the first (oldest) line is cancelled and the whole buffer is memmoved to
 * overwrite it and make room. while this is is obviously not the fastest algorithm (which would
 * require the use of a circular buffer) it is surely good enough for this module, which does not
 * aim at fast performances (line editing does not require to be blazingly fast).
 *
 * \li The first character in the history is always \c \\0, and it is used as a guard. By 'wasting' it
 * in this way, the code actually gets much simpler in that we remove many checks when moving
 * backward (\c i>0 and similar).
 *
 * \li While editing, the current index points to the position of the buffer which contains the
 * last character typed in (exactly like a stack pointer). This also allows to simplify calculations
 * and to make easier using the last byte of history.
 *
 * \li While editing, the current line is always kept null-terminated. This is important because
 * if the user press ENTER, we must have room to add a \c \\0 to terminate the line. If the line
 * is as long as the whole history buffer, there would not be space for it. By always keeping the
 * \c \\0 at the end, we properly ensure this without making index checks harder.
 *
 * \li When removing a line from the history (see \c pop_history()), instead of updating all the
 * indices we have around, we move backward the pointer to the history we use. This way, we don't
 * have to update anything. This means that we keep two pointers to the history: \c real_history
 * always points to the physical start, while \c history is the adjusted pointer (that is
 * dereference to read/write to it).
 *
 * \todo Use up/down to move through history  The history line will be copied to the current line,
 * making sure there is room for it.
 *
 * \author Giovanni Bajo <rasky@develer.com>
 */


#include "readline.h"

#include <cfg/compiler.h>
#include <cfg/debug.h>

#include <stdio.h>

/// Enable compilation of the unit test code
#define DEBUG_UNIT_TEST       0

/// Enable dump of the history after each line
#define DEBUG_DUMP_HISTORY    0


/** Special keys (escape sequences converted to a single code) */
enum RL_KEYS {
	SPECIAL_KEYS = 0x1000,

	/*
	 * Three byte keys:
	 * #################
	 * UpArrow:     0x1B 0x5B 0X41
	 * DownArrow:   0x1B 0x5B 0X42
	 * RightArrow:  0x1B 0x5B 0x43
	 * LeftArrow:   0x1b 0x5B 0x44
	 * Beak(Pause): 0x1b 0x5B 0x50
	*/
	KEY_UP_ARROW,
	KEY_DOWN_ARROW,
	KEY_LEFT_ARROW,
	KEY_RIGHT_ARROW,
	KEY_PAUSE,

	/*
	 * Four byte keys:
	 * ################
	 * F1:          0x1b 0x5B 0x5B 0x41
	 * F2:          0x1b 0x5B 0x5B 0x42
	 * F3:          0x1b 0x5B 0x5B 0x43
	 * F4:          0x1b 0x5B 0x5B 0x44
	 * F5:          0x1b 0x5B 0x5B 0x45
	 * Ins:         0x1b 0x5B 0x32 0x7E
	 * Home:        0x1b 0x5B 0x31 0x7E
	 * PgUp:        0x1b 0x5B 0x35 0x7E
	 * Del:         0x1b 0x5B 0x33 0x7E
	 * End:         0x1b 0x5B 0x34 0x7E
	 * PgDn:        0x1b 0x5B 0x36 0x7E
	 */
	KEY_F1, KEY_F2, KEY_F3, KEY_F4, KEY_F5,
	KEY_INS, KEY_HOME, KEY_PGUP, KEY_DEL, KEY_END, KEY_PGDN,

	/*
	 * Five byte keys:
	 * ################
	 * F6:          0x1b 0x5B 0x31 0x37 0x7E
	 * F7:          0x1b 0x5B 0x31 0x38 0x7E
	 * F8:          0x1b 0x5B 0x31 0x39 0x7E
	 * F9:          0x1b 0x5B 0x32 0x30 0x7E
	 * F10:         0x1b 0x5B 0x32 0x31 0x7E
	 * F11:         0x1b 0x5B 0x32 0x33 0x7E
	 * F12:         0x1b 0x5B 0x32 0x34 0x7E
	 */
	KEY_F6, KEY_F7, KEY_F8, KEY_F9,
	KEY_F10, KEY_F11, KEY_F12,
};

/** Check if \a c is a separator between words.
 *  \note Parameter \a c is evaluated multiple times
 */
#define IS_WORD_SEPARATOR(c) ((c) == ' ' || (c) == '\0')

/// Write the string \a txt to the IO output (without any kind of termination)
INLINE void rl_puts(const struct RLContext* ctx, const char* txt)
{
	if (!ctx->put)
		return;

	while (*txt)
		ctx->put(*txt++, ctx->put_param);
}

/// Write character \a ch to the IO output.
INLINE void rl_putc(const struct RLContext* ctx, char ch)
{
	if (ctx->put)
		ctx->put(ch, ctx->put_param);
}

/** Read a character from the IO into \a ch. This function also takes
 *  care of converting the ANSI escape sequences into one of the codes
 *  defined in \c RL_KEYS.
 */
static bool rl_getc(const struct RLContext* ctx, int* ch)
{
	int c = ctx->get(ctx->get_param);

	if (c == EOF)
	{
		if (ctx->clear)
			ctx->clear(ctx->clear_param);

		return false;
	}

	if (c == 0x1B)
	{
		// Unknown ESC sequence. Ignore it and read
		//  return next character.
		if (ctx->get(ctx->get_param) != 0x5B)
			return rl_getc(ctx, ch);

		/* To be added:
			* Home:        0x1b 0x5B 0x31 0x7E
			* F6:          0x1b 0x5B 0x31 0x37 0x7E
			* F7:          0x1b 0x5B 0x31 0x38 0x7E
			* F8:          0x1b 0x5B 0x31 0x39 0x7E
			* Ins:         0x1b 0x5B 0x32 0x7E
			* F9:          0x1b 0x5B 0x32 0x30 0x7E
			* F10:         0x1b 0x5B 0x32 0x31 0x7E
			* F11:         0x1b 0x5B 0x32 0x33 0x7E
			* F12:         0x1b 0x5B 0x32 0x34 0x7E
			* Del:         0x1b 0x5B 0x33 0x7E
			* End:         0x1b 0x5B 0x34 0x7E
			* PgUp:        0x1b 0x5B 0x35 0x7E
			* PgDn:        0x1b 0x5B 0x36 0x7E
		*/

		c = ctx->get(ctx->get_param);
		switch (c)
		{
		case 0x41: c = KEY_UP_ARROW; break;
		case 0x42: c = KEY_DOWN_ARROW; break;
		case 0x43: c = KEY_RIGHT_ARROW; break;
		case 0x44: c = KEY_LEFT_ARROW; break;
		case 0x50: c = KEY_PAUSE; break;
		case 0x5B:
			c = ctx->get(ctx->get_param);
			switch (c)
			{
			case 0x41: c = KEY_F1; break;
			case 0x42: c = KEY_F2; break;
			case 0x43: c = KEY_F3; break;
			case 0x44: c = KEY_F4; break;
			case 0x45: c = KEY_F5; break;
			default: return rl_getc(ctx, ch);
			}
			break;
		default: return rl_getc(ctx, ch);
		}
	}

	*ch = c;
	return true;
}

INLINE void beep(struct RLContext* ctx)
{
	rl_putc(ctx, '\a');
}

static bool pop_history(struct RLContext* ctx, int total_len)
{
	// Compute the length of the first command (including terminator).
	int len = strlen(ctx->real_history+1)+1;

	// (the first byte of the history should always be 0)
	ASSERT(ctx->real_history[0] == '\0');

	// If it is the only one in the history, do nothing
	if (len == total_len)
		return false;

	// Overwrite the first command with the second one
	memmove(ctx->real_history, ctx->real_history+len, HISTORY_SIZE-len);

	// Move back the ctx->buffer pointer so that all the indices are still valid
	ctx->history -= len;

	return true;
}

/// Check if index \a i points to the begin of the history.
INLINE bool is_history_begin(struct RLContext* ctx, int i)
{ return ctx->history + i == ctx->real_history; }

/// Check if index \a i points to the (exclusive) end of history
INLINE bool is_history_end(struct RLContext* ctx, int i)
{ return ctx->history + i == ctx->real_history + HISTORY_SIZE; }

/// Check if index \a i points to the (exclusive) end of history, or somewhere past the end.
INLINE bool is_history_past_end(struct RLContext* ctx, int i)
{ return ctx->history + i >= ctx->real_history + HISTORY_SIZE; }

/** Insert \a num_chars characters from \a ch into the history buffer at the
 *  position indicated by \a curpos. If needed, remove old history to make room.
 *  Returns true if everything was successful, false if there was no room to
 *  add the characters.
 *  \note \a num_chars can be 0, in which case we just make sure the line is
 *  correctly zero-terminated (ASCIIZ format).
 */
static bool insert_chars(struct RLContext* ctx, size_t *curpos, const char* ch, int num_chars)
{
	ASSERT(!is_history_past_end(ctx, *curpos));

	while (is_history_past_end(ctx, *curpos+num_chars+1))
	{
		if (!pop_history(ctx, *curpos))
			return false;
	}

	while (num_chars--)
		ctx->history[++(*curpos)] = *ch++;

	ASSERT(!is_history_past_end(ctx, *curpos + 1));
	ctx->history[*curpos+1] = '\0';
	return true;
}

/// Insert a single character \a ch into the buffer (with the same semantic of \c insert_chars())
static bool insert_char(struct RLContext* ctx, size_t *curpos, char ch)
{
	return insert_chars(ctx, curpos, &ch, 1);
}

#if DEBUG_DUMP_HISTORY
/// Dump the internal history of a context (used only for debug purposes)
static void dump_history(struct RLContext* ctx)
{
	int k;
	char buf[8];
	ASSERT(ctx->real_history[0] == '\0');
	rl_puts(ctx, "History dump:");
	rl_puts(ctx, "\r\n");
	for (k = 1;
	     ctx->real_history + k != ctx->history + ctx->history_pos + 1;
	     k += strlen(&ctx->real_history[k]) + 1)
	{
		rl_puts(ctx, &ctx->real_history[k]);
		rl_puts(ctx, "\r\n");
	}

	sprintf(buf, "%d\r\n", ctx->history_pos + (ctx->history - ctx->real_history));
	rl_puts(ctx, buf);
}
#endif /* DEBUG_DUMP_HISTORY */

/// Complete the current word. Return false if no unambiguous completion was found
static bool complete_word(struct RLContext *ctx, size_t *curpos)
{
	const char* completed_word;
	size_t wstart;

	// If the current character is a separator,
	//  there is nothing to complete
	wstart = *curpos;
	if (IS_WORD_SEPARATOR(ctx->history[wstart]))
	{
		beep(ctx);
		return false;
	}

	// Find the separator before the current word
	do
		--wstart;
	while (!IS_WORD_SEPARATOR(ctx->history[wstart]));

	// Complete the word through the hook
	completed_word = ctx->match(ctx->match_param, ctx->history + wstart + 1, *curpos - wstart);
	if (!completed_word)
		return false;

	// Move back the terminal cursor to the separator
	while (*curpos != wstart)
	{
		rl_putc(ctx, '\b');
		--*curpos;
	}

	// Insert the completed command
	insert_chars(ctx, curpos, completed_word, strlen(completed_word));
	rl_puts(ctx, completed_word);
	insert_char(ctx, curpos, ' ');
	rl_putc(ctx, ' ');

	return true;
}

void rl_refresh(struct RLContext* ctx)
{
	rl_puts(ctx, "\r\n");
	if (ctx->prompt)
		rl_puts(ctx, ctx->prompt);
	rl_puts(ctx, ctx->history + ctx->history_pos + 1);
}

const char* rl_readline(struct RLContext* ctx)
{
	while (1)
	{
		char ch;
		int c;

		ASSERT(ctx->history - ctx->real_history + ctx->line_pos < HISTORY_SIZE);

		if (!rl_getc(ctx, &c))
			return NULL;

		// Just ignore special keys for now
		if (c > SPECIAL_KEYS)
			continue;

		if (c == '\t')
		{
			// Ask the match hook if available
			if (!ctx->match)
				return NULL;

			complete_word(ctx, &ctx->line_pos);
			continue;
		}

		// Backspace cancels a character, or it is ignored if at
		//  the start of the line
		if (c == '\b')
		{
			if (ctx->history[ctx->line_pos] != '\0')
			{
				--ctx->line_pos;
				rl_puts(ctx, "\b \b");
			}
			continue;
		}

		if (c == '\r' || c == '\n')
		{
			rl_puts(ctx, "\r\n");
			break;
		}


		// Add a character to the buffer, if possible
		ch = (char)c;
		ASSERT2(ch == c, "a special key was not properly handled");
		if (insert_chars(ctx, &ctx->line_pos, &ch, 1))
			rl_putc(ctx, ch);
		else
			beep(ctx);
	}

	ctx->history_pos = ctx->line_pos + 1;
	while (ctx->history[ctx->line_pos] != '\0')
		--ctx->line_pos;

	// Do not store empty lines in the history
	if (ctx->line_pos == ctx->history_pos - 1)
		ctx->history_pos -= 1;

#if DEBUG_DUMP_HISTORY
	dump_history(ctx);
#endif

	const char *buf = &ctx->history[ctx->line_pos + 1];

	ctx->line_pos = ctx->history_pos;

	if (ctx->prompt)
		rl_puts(ctx, ctx->prompt);

	insert_chars(ctx, &ctx->line_pos, NULL, 0);

	// Since the current pointer now points to the separator, we need
	//  to return the first character
	return buf;
}


#if DEBUG_UNIT_TEST

/** Perform the unit test for the readline library */
void rl_test(void);

#if HISTORY_SIZE != 32
	#error This test needs HISTORY_SIZE to be set at 32
#endif

static struct RLContext test_ctx;

static char* test_getc_ptr;
static int test_getc(void* data)
{
	return *test_getc_ptr++;
}

/** Perform a readline test. The function pipes the characters from \a input_buffer
 *  through the I/O to \c rl_readline(). After the whole string is sent, \c do_test()
 *  checks if the current history within the context match \a expected_history.
 */
static bool do_test(char* input_buffer, char* expected_history)
{
	rl_init_ctx(&test_ctx);
	rl_sethook_get(&test_ctx, test_getc, NULL);

	test_getc_ptr = input_buffer;
	while (*test_getc_ptr)
		rl_readline(&test_ctx);

	if (memcmp(test_ctx.real_history, expected_history, HISTORY_SIZE) != 0)
	{
		ASSERT2(0, "history compare failed");
		return false;
	}

	return true;
}

void rl_test(void)
{
	char* test1_in = "a\nb\nc\nd\ne\nf\ng\nh\ni\nj\nk\nl\nm\nn\no\np\nq\nr\ns\nt\nu\nv\nw\nx\ny\nz\n";
	char test1_hist[HISTORY_SIZE] = "\0l\0m\0n\0o\0p\0q\0r\0s\0t\0u\0v\0w\0x\0y\0z";

	if (!do_test(test1_in, test1_hist))
		return;

	kprintf("rl_test successful\n");
}

#endif /* DEBUG_UNIT_TEST */

