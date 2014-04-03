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
 * This file is part of DevLib - See devlib/README for information.
 * -->
 *
 * \author Bernie Innocenti <bernie@codewiz.org>
 *
 * \brief i386 context switch for WIN32
 */

!!!!!! THIS FILE HAS NOT BEEN REVISED FOR THE NEW SCHEDULER API !!!!!!

__declspec(naked) void AsmSwitchContext(void * new_sp, void ** save_sp)
{
	__asm
	{
		push	eax
		push	ebx
		push	ecx
		push	edx
		push	esi
		push	edi
		push	ebp
		mov		ebp,dword ptr [esp+24h]	; ebp <- save_sp
		mov		dword ptr [ebp],esp		; *save_sp = esp
		mov		esp,dword ptr [esp+20h]	; new_sp
		pop		ebp
		pop		edi
		pop		esi
		pop		edx
		pop		ecx
		pop		ebx
		pop		eax
		ret
	}
}

__declspec(naked) void AsmReplaceContext(void * new_sp, void ** dummy)
{
	__asm
	{
		mov		esp,dword ptr [esp + 4]	; new_sp
		pop		ebp
		pop		edi
		pop		esi
		pop		edx
		pop		ecx
		pop		ebx
		pop		eax
		ret
	}
}

