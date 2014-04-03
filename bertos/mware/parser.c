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
 * Copyright 2003, 2006 Develer S.r.l. (http://www.develer.com/)
 * All Rights Reserved.
 * -->
 *
 * \brief Channel protocol parser and commands.
 *
 * This file contains the channel protocol parser and
 * the definition of the protocol commands. Commands are defined
 * in a "CmdTemplate" type array, containing:
 * - the name of the command,
 * - the arguments it expects to receive,
 * - the output values,
 * - the name of the function implementing the command.
 *
 * The arguments and results are passed to command function
 * using an union: the element of the union to use for each
 * argument is determined by format strings present in the
 * CmdTemplate table.
 *
 * \author Bernie Innocenti <bernie@codewiz.org>
 * \author Stefano Fedrigo <aleph@develer.com>
 * \author Giovanni Bajo <rasky@develer.com>
 *
 *
 */


#include "parser.h"

#include "cfg/cfg_parser.h"

#include <io/kfile.h>
#include <struct/hashtable.h>

#include <stdlib.h> // atol(), NULL
#include <string.h> // strchr(), strcmp()

/// Hashtable hook to extract the key from a command
static const void* get_key_from_command(const void* cmd, uint8_t* length);

/// Hashtable that handles the commands that can be executed
DECLARE_HASHTABLE_STATIC(commands, CONFIG_MAX_COMMANDS_NUMBER, get_key_from_command);


/**
 * \brief Tokenize one word at a time from a text.
 *
 * This function is similar to strtok, but does not use any implicit
 * context, nor it does modify the input buffer in any form.
 * The word is returned as a STL-like [begin,end) range.
 *
 * To extract the first word, make both begin and end point at the
 * start of the text, and call the function. Then, subsequent
 * calls will return the following words (assuming the begin/end
 * variable are not modified between calls).
 *
 * \param begin Will contain the index of the first character of the word
 * \param end Will contain the index of the character after the last
 *     character of the word
 *
 * \return True if a word was extracted, false if we got to the end
 * of the string without extracting any word.
 */
static bool get_word(const char **begin, const char **end)
{
	const char *cur = *end;

	while ((*cur == ' ' || *cur == '\t') && *cur)
		++cur;

	*begin = cur;

	while ((*cur != ' ' && *cur != '\t') && *cur)
		++cur;

	*end = cur;

	return (*end != *begin);
}


/**
 * \brief Command arguments parser.
 *
 * Using the format pointed by the argument fmt
 * parses the input string filling the array argv
 * with input parameters of the correct type.
 *
 * \param fmt   Parameters format string.
 * \param input Input string.
 * \param argv  Array filled with parameters.
 *
 * \return False in case of errors, otherwise true.
 */
static bool parseArgs(const char *fmt, const char *input, parms argv[])
{
	const char *begin = input, *end = input;

	while (*fmt)
	{
		// Extract the argument
		if (!get_word(&begin, &end))
			return false;

		switch (*fmt)
		{
			case 'd':
				(*argv++).l = atol(begin);
				break;

			case 's':
				(*argv++).s = begin;
				break;

			default:
				ASSERT2(0, "Unknown format for argument");
				return false;
		}

		++fmt;
	}

	/* check if there are remaining args */
	if (get_word(&begin, &end))
		return false;

	return true;
}

/// Hook provided by the parser for matching of command names (TAB completion) for readline
const char* parser_rl_match(UNUSED_ARG(void *,dummy), const char *word, int word_len)
{
	HashIterator cur;
	HashIterator end = ht_iter_end(&commands);
	const char *found = NULL;

	for (cur = ht_iter_begin(&commands);
	     !ht_iter_cmp(cur, end);
	     cur = ht_iter_next(cur))
	{
		const struct CmdTemplate* cmdp = (const struct CmdTemplate*)ht_iter_get(cur);
		if (strncmp(cmdp->name, word, word_len) == 0)
		{
			// If there was another matching word, it means that we have a multiple
			//  match: then return NULL.
			if (found)
				return NULL;

			found = cmdp->name;
		}
	}

	return found;
}

#if CONFIG_ENABLE_COMPAT_BEHAVIOUR
bool parser_get_cmd_id(const char* line, unsigned long* ID)
{
	const char *begin = line, *end = line;
	char *end2;

	// The first word is the ID
	if (!get_word(&begin, &end))
		return false;

	*ID = strtoul(begin, &end2, 10);
	if (end2 != end)
		return false;

	return true;
}
#endif

/**
 * Find the template for the command contained in the text line.
 * The template can be used to tokenize the command and interpret
 * it.
 *
 * This function can be used to find out which command is contained
 * in a given text line without parsing all the parameters and
 * executing it.
 *
 * \param input Text line to be processed (ASCIIZ)
 *
 * \return The command template associated with the command contained
 * in the line, or NULL if the command is invalid.
 */
const struct CmdTemplate* parser_get_cmd_template(const char *input)
{
	const char *begin = input, *end = input;

#if CONFIG_ENABLE_COMPAT_BEHAVIOUR
	// Skip the ID, and get the command
	if (!get_word(&begin, &end))
		return NULL;
#endif
	if (!get_word(&begin, &end))
		return NULL;

	return (const struct CmdTemplate*)ht_find(&commands, begin, end-begin);
}

static const char *skip_to_params(const char *input, const struct CmdTemplate *cmdp)
{
	const char *begin = input, *end = input;

#if CONFIG_ENABLE_COMPAT_BEHAVIOUR
	// Skip the ID, and get the command
	if (!get_word(&begin, &end))
		return NULL;
#endif
	if (!get_word(&begin, &end))
		return NULL;

	ASSERT2(strlen(cmdp->name) == (size_t)(end-begin), "Invalid command template specified");
	ASSERT2(!strncmp(begin, cmdp->name, end-begin), "Invalid command template specified");

	return end;
}

/**
 * Extract the arguments for the command contained in the text line.
 *
 * The first argument will always be the command name, so the actual arguments
 * will start at index 1.
 *
 * \param input Text line to be processed (ASCIIZ)
 * \param cmdp Command template for this line
 * \param args Will contain the extracted parameters
 *
 * \return True if everything OK, false in case of parsing error.
 */
bool parser_get_cmd_arguments(const char* input, const struct CmdTemplate* cmdp, parms args[CONFIG_PARSER_MAX_ARGS])
{
	input = skip_to_params(input, cmdp);
	if (!input)
		return false;

	args[0].s = cmdp->name;
	if (!parseArgs(cmdp->arg_fmt, input, args + 1))
		return false;

	return true;
}

static const void* get_key_from_command(const void* cmd, uint8_t* length)
{
	const struct CmdTemplate* c = cmd;
	*length = strlen(c->name);
	return c->name;
}

/**
 * \brief Command input handler.
 *
 * Process the input, calling the requested command (if found).
 *
 * \param input Text line to be processed (ASCIIZ)
 *
 * \return true if everything is OK, false in case of errors
 */
bool parser_process_line(const char* input)
{
	const struct CmdTemplate *cmdp;
	parms args[CONFIG_PARSER_MAX_ARGS];

	cmdp = parser_get_cmd_template(input);
	if (!cmdp)
		return false;

	if (!parser_get_cmd_arguments(input, cmdp, args))
		return false;

	if (!parser_execute_cmd(cmdp, args))
		return false;

	return true;
}

/**
 * Register a new command into the parser
 *
 * \param cmd Command template describing the command
 * \return true if registration was successful, false otherwise
 */
bool parser_register_cmd(const struct CmdTemplate* cmd)
{
	return ht_insert(&commands, cmd);
}

void parser_init(void)
{
	// Initialize the hashtable used to store the command description
	ht_init(&commands);
}
