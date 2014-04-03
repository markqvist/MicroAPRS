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
 * \brief STM32F103xx Embedded flash definition.
 */

#ifndef STM32_FLASH_H
#define STM32_FLASH_H

#include <cfg/compiler.h>

#include <cpu/types.h>

/** Return the embedded flash size in kB */
#define F_SIZE                   ((*(reg32_t *) 0x1FFFF7E0) & 0xFFFF)


/* Flash Access Control Register bits */
#define ACR_LATENCY_MASK         ((uint32_t)0x00000038)
#define ACR_HLFCYA_MASK          ((uint32_t)0xFFFFFFF7)
#define ACR_PRFTBE_MASK          ((uint32_t)0xFFFFFFEF)

/* Flash Access Control Register bits */
#define ACR_PRFTBS_MASK          ((uint32_t)0x00000020)

/* Flash Control Register bits */
#define CR_PG_SET                ((uint32_t)0x00000001)
#define CR_PG_RESET              ((uint32_t)0x00001FFE)

#define CR_PER_SET               ((uint32_t)0x00000002)
#define CR_PER_RESET             ((uint32_t)0x00001FFD)

#define CR_MER_SET               ((uint32_t)0x00000004)
#define CR_MER_RESET             ((uint32_t)0x00001FFB)

#define CR_OPTPG_SET             ((uint32_t)0x00000010)
#define CR_OPTPG_RESET           ((uint32_t)0x00001FEF)

#define CR_OPTER_SET             ((uint32_t)0x00000020)
#define CR_OPTER_RESET           ((uint32_t)0x00001FDF)

#define CR_STRT_SET              ((uint32_t)0x00000040)

#define CR_LOCK_SET              ((uint32_t)0x00000080)

/* FLASH Mask */
#define RDPRT_MASK               ((uint32_t)0x00000002)
#define WRP0_MASK                ((uint32_t)0x000000FF)
#define WRP1_MASK                ((uint32_t)0x0000FF00)
#define WRP2_MASK                ((uint32_t)0x00FF0000)
#define WRP3_MASK                ((uint32_t)0xFF000000)

/* FLASH Keys */
#define RDP_KEY                  ((uint16_t)0x00A5)
#define FLASH_KEY1               ((uint32_t)0x45670123)
#define FLASH_KEY2               ((uint32_t)0xCDEF89AB)

/* Flash Latency */
#define FLASH_LATENCY_0                ((uint32_t)0x00000000)  /* FLASH Zero Latency cycle */
#define FLASH_LATENCY_1                ((uint32_t)0x00000001)  /* FLASH One Latency cycle */
#define FLASH_LATENCY_2                ((uint32_t)0x00000002)  /* FLASH Two Latency cycles */

/* Half Cycle Enable/Disable */
#define FLASH_HALFCYCLEACCESS_ENABLE   ((uint32_t)0x00000008)  /* FLASH Half Cycle Enable */
#define FLASH_HALFCYCLEACCESS_DISABLE  ((uint32_t)0x00000000)  /* FLASH Half Cycle Disable */

/* Prefetch Buffer Enable/Disable */
#define FLASH_PREFETCHBUFFER_ENABLE    ((uint32_t)0x00000010)  /* FLASH Prefetch Buffer Enable */
#define FLASH_PREFETCHBUFFER_DISABLE   ((uint32_t)0x00000000)  /* FLASH Prefetch Buffer Disable */

/* Option Bytes Write Protection */
#define FLASH_WRPROT_PAGES0TO3      ((uint32_t)0x00000001) /* Write protection of page 0 to 3 */
#define FLASH_WRPROT_PAGES4TO7      ((uint32_t)0x00000002) /* Write protection of page 4 to 7 */
#define FLASH_WRPROT_PAGES8TO11     ((uint32_t)0x00000004) /* Write protection of page 8 to 11 */
#define FLASH_WRPROT_PAGES12TO15    ((uint32_t)0x00000008) /* Write protection of page 12 to 15 */
#define FLASH_WRPROT_PAGES16TO19    ((uint32_t)0x00000010) /* Write protection of page 16 to 19 */
#define FLASH_WRPROT_PAGES20TO23    ((uint32_t)0x00000020) /* Write protection of page 20 to 23 */
#define FLASH_WRPROT_PAGES24TO27    ((uint32_t)0x00000040) /* Write protection of page 24 to 27 */
#define FLASH_WRPROT_PAGES28TO31    ((uint32_t)0x00000080) /* Write protection of page 28 to 31 */
#define FLASH_WRPROT_PAGES32TO35    ((uint32_t)0x00000100) /* Write protection of page 32 to 35 */
#define FLASH_WRPROT_PAGES36TO39    ((uint32_t)0x00000200) /* Write protection of page 36 to 39 */
#define FLASH_WRPROT_PAGES40TO43    ((uint32_t)0x00000400) /* Write protection of page 40 to 43 */
#define FLASH_WRPROT_PAGES44TO47    ((uint32_t)0x00000800) /* Write protection of page 44 to 47 */
#define FLASH_WRPROT_PAGES48TO51    ((uint32_t)0x00001000) /* Write protection of page 48 to 51 */
#define FLASH_WRPROT_PAGES52TO55    ((uint32_t)0x00002000) /* Write protection of page 52 to 55 */
#define FLASH_WRPROT_PAGES56TO59    ((uint32_t)0x00004000) /* Write protection of page 56 to 59 */
#define FLASH_WRPROT_PAGES60TO63    ((uint32_t)0x00008000) /* Write protection of page 60 to 63 */
#define FLASH_WRPROT_PAGES64TO67    ((uint32_t)0x00010000) /* Write protection of page 64 to 67 */
#define FLASH_WRPROT_PAGES68TO71    ((uint32_t)0x00020000) /* Write protection of page 68 to 71 */
#define FLASH_WRPROT_PAGES72TO75    ((uint32_t)0x00040000) /* Write protection of page 72 to 75 */
#define FLASH_WRPROT_PAGES76TO79    ((uint32_t)0x00080000) /* Write protection of page 76 to 79 */
#define FLASH_WRPROT_PAGES80TO83    ((uint32_t)0x00100000) /* Write protection of page 80 to 83 */
#define FLASH_WRPROT_PAGES84TO87    ((uint32_t)0x00200000) /* Write protection of page 84 to 87 */
#define FLASH_WRPROT_PAGES88TO91    ((uint32_t)0x00400000) /* Write protection of page 88 to 91 */
#define FLASH_WRPROT_PAGES92TO95    ((uint32_t)0x00800000) /* Write protection of page 92 to 95 */
#define FLASH_WRPROT_PAGES96TO99    ((uint32_t)0x01000000) /* Write protection of page 96 to 99 */
#define FLASH_WRPROT_PAGES100TO103  ((uint32_t)0x02000000) /* Write protection of page 100 to 103 */
#define FLASH_WRPROT_PAGES104TO107  ((uint32_t)0x04000000) /* Write protection of page 104 to 107 */
#define FLASH_WRPROT_PAGES108TO111  ((uint32_t)0x08000000) /* Write protection of page 108 to 111 */
#define FLASH_WRPROT_PAGES112TO115  ((uint32_t)0x10000000) /* Write protection of page 112 to 115 */
#define FLASH_WRPROT_PAGES116TO119  ((uint32_t)0x20000000) /* Write protection of page 115 to 119 */
#define FLASH_WRPROT_PAGES120TO123  ((uint32_t)0x40000000) /* Write protection of page 120 to 123 */
#define FLASH_WRPROT_PAGES124TO127  ((uint32_t)0x80000000) /* Write protection of page 124 to 127 */
#define FLASH_WRPROT_ALLPAGES       ((uint32_t)0xFFFFFFFF) /* Write protection All Pages */

/* Option Bytes IWatchdog */
#define OB_IWDG_SW                  ((uint16_t)0x0001)  /* Software IWDG selected */
#define OB_IWDG_HW                  ((uint16_t)0x0000)  /* Hardware IWDG selected */

/* Option Bytes nRST_STOP */
#define OB_STOP_NORST               ((uint16_t)0x0002) /* No reset generated when entering in STOP */
#define OB_STOP_RST                 ((uint16_t)0x0000) /* Reset generated when entering in STOP */

/* Option Bytes nRST_STDBY */
#define OB_STDBY_NORST              ((uint16_t)0x0004) /* No reset generated when entering in STANDBY */
#define OB_STDBY_RST                ((uint16_t)0x0000) /* Reset generated when entering in STANDBY */

/* FLASH Interrupts */
#define FLASH_IT_ERROR           ((uint32_t)0x00000400)  /* FPEC error interrupt source */
#define FLASH_IT_EOP             ((uint32_t)0x00001000)  /* End of FLASH Operation Interrupt source */

/* FLASH Flags */
#define FLASH_FLAG_BSY           ((uint32_t)0x00000001)  /* FLASH Busy flag */
#define FLASH_FLAG_EOP           ((uint32_t)0x00000020)  /* FLASH End of Operation flag */
#define FLASH_FLAG_PGERR         ((uint32_t)0x00000004)  /* FLASH Program error flag */
#define FLASH_FLAG_WRPRTERR      ((uint32_t)0x00000010)  /* FLASH Write protected error flag */
#define FLASH_FLAG_OPTERR        ((uint32_t)0x00000001)  /* FLASH Option Byte error flag */



/**
 * Embbeded flash configuration registers structure
 */
struct stm32_flash
{
  reg32_t ACR;
  reg32_t KEYR;
  reg32_t OPTKEYR;
  reg32_t SR;
  reg32_t CR;
  reg32_t AR;
  reg32_t RESERVED;
  reg32_t OBR;
  reg32_t WRPR;
};

#endif /* STM32_FLASH_H */
