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
 * \brief Function library for Accessing FT245RL USB interface.
 *
 *
 * \author Francesco Sacchi <batt@develer.com>
 *
 * $WIZ$ module_name = "ft245rl"
 * $WIZ$ module_depends = "kfile", "timer"
 * $WIZ$ module_hw = "bertos/hw/hw_ft245rl.h"
 */


#ifndef DRV_FT245RL_H
#define DRV_FT245RL_H

#include <io/kfile.h>
#include <cfg/compiler.h>

/**
 * ID for FT245RL KFiles.
 */
#define KFT_FT245RL MAKE_ID('F', '2', '4', '5')

/**
 * Ft245rl KFile context structure.
 */
typedef struct Ft245rl
{
	KFile fd; ///< File descriptor.
} Ft245rl;


/**
 * Convert + ASSERT from generic KFile to KFileFt245rl.
 */
INLINE Ft245rl * FT245RL_CAST(KFile *fd)
{
	ASSERT(fd->_type == KFT_FT245RL);
	return (Ft245rl *)fd;
}

void ft245rl_init(Ft245rl *fd);

#endif /* DRV_FT245RL_H */
