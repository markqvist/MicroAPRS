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
 *
 * -->
 *
 * \brief LM3S generic hardware macros.
 */

#ifndef LM3S_TYPES_H
#define LM3S_TYPES_H

#include <cfg/compiler.h>
#include <cpu/types.h>

/**
 * Helper Macros for determining the particular hardware revision.
 */
/*\{*/
#ifndef CLASS_IS_SANDSTORM
#define CLASS_IS_SANDSTORM                                                    \
        (((HWREG(SYSCTL_DID0) & SYSCTL_DID0_VER_M) == SYSCTL_DID0_VER_0) ||   \
         ((HWREG(SYSCTL_DID0) & (SYSCTL_DID0_VER_M | SYSCTL_DID0_CLASS_M)) == \
          (SYSCTL_DID0_VER_1 | SYSCTL_DID0_CLASS_SANDSTORM)))
#endif

#ifndef CLASS_IS_FURY
#define CLASS_IS_FURY                                                        \
        ((HWREG(SYSCTL_DID0) & (SYSCTL_DID0_VER_M | SYSCTL_DID0_CLASS_M)) == \
         (SYSCTL_DID0_VER_1 | SYSCTL_DID0_CLASS_FURY))
#endif

#ifndef CLASS_IS_DUSTDEVIL
#define CLASS_IS_DUSTDEVIL                                                   \
        ((HWREG(SYSCTL_DID0) & (SYSCTL_DID0_VER_M | SYSCTL_DID0_CLASS_M)) == \
         (SYSCTL_DID0_VER_1 | SYSCTL_DID0_CLASS_DUSTDEVIL))
#endif

#ifndef CLASS_IS_TEMPEST
#define CLASS_IS_TEMPEST                                                     \
        ((HWREG(SYSCTL_DID0) & (SYSCTL_DID0_VER_M | SYSCTL_DID0_CLASS_M)) == \
         (SYSCTL_DID0_VER_1 | SYSCTL_DID0_CLASS_TEMPEST))
#endif

#ifndef REVISION_IS_A0
#define REVISION_IS_A0                                                     \
        ((HWREG(SYSCTL_DID0) & (SYSCTL_DID0_MAJ_M | SYSCTL_DID0_MIN_M)) == \
         (SYSCTL_DID0_MAJ_REVA | SYSCTL_DID0_MIN_0))
#endif

#ifndef REVISION_IS_A1
#define REVISION_IS_A1                                                     \
        ((HWREG(SYSCTL_DID0) & (SYSCTL_DID0_MAJ_M | SYSCTL_DID0_MIN_M)) == \
         (SYSCTL_DID0_MAJ_REVA | SYSCTL_DID0_MIN_0))
#endif

#ifndef REVISION_IS_A2
#define REVISION_IS_A2                                                     \
        ((HWREG(SYSCTL_DID0) & (SYSCTL_DID0_MAJ_M | SYSCTL_DID0_MIN_M)) == \
         (SYSCTL_DID0_MAJ_REVA | SYSCTL_DID0_MIN_2))
#endif

#ifndef REVISION_IS_B0
#define REVISION_IS_B0                                                     \
        ((HWREG(SYSCTL_DID0) & (SYSCTL_DID0_MAJ_M | SYSCTL_DID0_MIN_M)) == \
         (SYSCTL_DID0_MAJ_REVB | SYSCTL_DID0_MIN_0))
#endif

#ifndef REVISION_IS_B1
#define REVISION_IS_B1                                                     \
        ((HWREG(SYSCTL_DID0) & (SYSCTL_DID0_MAJ_M | SYSCTL_DID0_MIN_M)) == \
         (SYSCTL_DID0_MAJ_REVB | SYSCTL_DID0_MIN_1))
#endif

#ifndef REVISION_IS_C0
#define REVISION_IS_C0                                                     \
        ((HWREG(SYSCTL_DID0) & (SYSCTL_DID0_MAJ_M | SYSCTL_DID0_MIN_M)) == \
         (SYSCTL_DID0_MAJ_REVC | SYSCTL_DID0_MIN_0))
#endif

#ifndef REVISION_IS_C1
#define REVISION_IS_C1                                                     \
        ((HWREG(SYSCTL_DID0) & (SYSCTL_DID0_MAJ_M | SYSCTL_DID0_MIN_M)) == \
         (SYSCTL_DID0_MAJ_REVC | SYSCTL_DID0_MIN_1))
#endif

#ifndef REVISION_IS_C2
#define REVISION_IS_C2                                                     \
        ((HWREG(SYSCTL_DID0) & (SYSCTL_DID0_MAJ_M | SYSCTL_DID0_MIN_M)) == \
         (SYSCTL_DID0_MAJ_REVC | SYSCTL_DID0_MIN_2))
#endif
/*\}*/

#endif /* LM3S_TYPES_H */
