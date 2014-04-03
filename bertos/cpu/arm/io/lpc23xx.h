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
 * \author Francesco Sacchi <batt@develer.com>
 *
 * LPC23xx I/O registers.
 */

#ifndef LPC23XX_H
#define LPC23XX_H

#include <cfg/compiler.h>

#include <cpu/detect.h>
/* Vectored Interrupt Controller (VIC) */
#define VIC_BASE_ADDR	0xFFFFF000
#define VICIRQStatus   (*(reg32_t *)(VIC_BASE_ADDR + 0x000))
#define VICFIQStatus   (*(reg32_t *)(VIC_BASE_ADDR + 0x004))
#define VICRawIntr     (*(reg32_t *)(VIC_BASE_ADDR + 0x008))
#define VICIntSelect   (*(reg32_t *)(VIC_BASE_ADDR + 0x00C))
#define VICIntEnable   (*(reg32_t *)(VIC_BASE_ADDR + 0x010))
#define VICIntEnClr    (*(reg32_t *)(VIC_BASE_ADDR + 0x014))
#define VICSoftInt     (*(reg32_t *)(VIC_BASE_ADDR + 0x018))
#define VICSoftIntClr  (*(reg32_t *)(VIC_BASE_ADDR + 0x01C))
#define VICProtection  (*(reg32_t *)(VIC_BASE_ADDR + 0x020))
#define VICSWPrioMask  (*(reg32_t *)(VIC_BASE_ADDR + 0x024))

#define VICVectAddr0   (*(reg32_t *)(VIC_BASE_ADDR + 0x100))
#define VICVectAddr1   (*(reg32_t *)(VIC_BASE_ADDR + 0x104))
#define VICVectAddr2   (*(reg32_t *)(VIC_BASE_ADDR + 0x108))
#define VICVectAddr3   (*(reg32_t *)(VIC_BASE_ADDR + 0x10C))
#define VICVectAddr4   (*(reg32_t *)(VIC_BASE_ADDR + 0x110))
#define VICVectAddr5   (*(reg32_t *)(VIC_BASE_ADDR + 0x114))
#define VICVectAddr6   (*(reg32_t *)(VIC_BASE_ADDR + 0x118))
#define VICVectAddr7   (*(reg32_t *)(VIC_BASE_ADDR + 0x11C))
#define VICVectAddr8   (*(reg32_t *)(VIC_BASE_ADDR + 0x120))
#define VICVectAddr9   (*(reg32_t *)(VIC_BASE_ADDR + 0x124))
#define VICVectAddr10  (*(reg32_t *)(VIC_BASE_ADDR + 0x128))
#define VICVectAddr11  (*(reg32_t *)(VIC_BASE_ADDR + 0x12C))
#define VICVectAddr12  (*(reg32_t *)(VIC_BASE_ADDR + 0x130))
#define VICVectAddr13  (*(reg32_t *)(VIC_BASE_ADDR + 0x134))
#define VICVectAddr14  (*(reg32_t *)(VIC_BASE_ADDR + 0x138))
#define VICVectAddr15  (*(reg32_t *)(VIC_BASE_ADDR + 0x13C))
#define VICVectAddr16  (*(reg32_t *)(VIC_BASE_ADDR + 0x140))
#define VICVectAddr17  (*(reg32_t *)(VIC_BASE_ADDR + 0x144))
#define VICVectAddr18  (*(reg32_t *)(VIC_BASE_ADDR + 0x148))
#define VICVectAddr19  (*(reg32_t *)(VIC_BASE_ADDR + 0x14C))
#define VICVectAddr20  (*(reg32_t *)(VIC_BASE_ADDR + 0x150))
#define VICVectAddr21  (*(reg32_t *)(VIC_BASE_ADDR + 0x154))
#define VICVectAddr22  (*(reg32_t *)(VIC_BASE_ADDR + 0x158))
#define VICVectAddr23  (*(reg32_t *)(VIC_BASE_ADDR + 0x15C))
#define VICVectAddr24  (*(reg32_t *)(VIC_BASE_ADDR + 0x160))
#define VICVectAddr25  (*(reg32_t *)(VIC_BASE_ADDR + 0x164))
#define VICVectAddr26  (*(reg32_t *)(VIC_BASE_ADDR + 0x168))
#define VICVectAddr27  (*(reg32_t *)(VIC_BASE_ADDR + 0x16C))
#define VICVectAddr28  (*(reg32_t *)(VIC_BASE_ADDR + 0x170))
#define VICVectAddr29  (*(reg32_t *)(VIC_BASE_ADDR + 0x174))
#define VICVectAddr30  (*(reg32_t *)(VIC_BASE_ADDR + 0x178))
#define VICVectAddr31  (*(reg32_t *)(VIC_BASE_ADDR + 0x17C))

/* The name convention below is from previous LPC2000 family MCUs, in LPC23xx/24xx,
these registers are known as "VICVectPriority(x)". */
#define VICVectCntl0   (*(reg32_t *)(VIC_BASE_ADDR + 0x200))
#define VICVectCntl1   (*(reg32_t *)(VIC_BASE_ADDR + 0x204))
#define VICVectCntl2   (*(reg32_t *)(VIC_BASE_ADDR + 0x208))
#define VICVectCntl3   (*(reg32_t *)(VIC_BASE_ADDR + 0x20C))
#define VICVectCntl4   (*(reg32_t *)(VIC_BASE_ADDR + 0x210))
#define VICVectCntl5   (*(reg32_t *)(VIC_BASE_ADDR + 0x214))
#define VICVectCntl6   (*(reg32_t *)(VIC_BASE_ADDR + 0x218))
#define VICVectCntl7   (*(reg32_t *)(VIC_BASE_ADDR + 0x21C))
#define VICVectCntl8   (*(reg32_t *)(VIC_BASE_ADDR + 0x220))
#define VICVectCntl9   (*(reg32_t *)(VIC_BASE_ADDR + 0x224))
#define VICVectCntl10  (*(reg32_t *)(VIC_BASE_ADDR + 0x228))
#define VICVectCntl11  (*(reg32_t *)(VIC_BASE_ADDR + 0x22C))
#define VICVectCntl12  (*(reg32_t *)(VIC_BASE_ADDR + 0x230))
#define VICVectCntl13  (*(reg32_t *)(VIC_BASE_ADDR + 0x234))
#define VICVectCntl14  (*(reg32_t *)(VIC_BASE_ADDR + 0x238))
#define VICVectCntl15  (*(reg32_t *)(VIC_BASE_ADDR + 0x23C))
#define VICVectCntl16  (*(reg32_t *)(VIC_BASE_ADDR + 0x240))
#define VICVectCntl17  (*(reg32_t *)(VIC_BASE_ADDR + 0x244))
#define VICVectCntl18  (*(reg32_t *)(VIC_BASE_ADDR + 0x248))
#define VICVectCntl19  (*(reg32_t *)(VIC_BASE_ADDR + 0x24C))
#define VICVectCntl20  (*(reg32_t *)(VIC_BASE_ADDR + 0x250))
#define VICVectCntl21  (*(reg32_t *)(VIC_BASE_ADDR + 0x254))
#define VICVectCntl22  (*(reg32_t *)(VIC_BASE_ADDR + 0x258))
#define VICVectCntl23  (*(reg32_t *)(VIC_BASE_ADDR + 0x25C))
#define VICVectCntl24  (*(reg32_t *)(VIC_BASE_ADDR + 0x260))
#define VICVectCntl25  (*(reg32_t *)(VIC_BASE_ADDR + 0x264))
#define VICVectCntl26  (*(reg32_t *)(VIC_BASE_ADDR + 0x268))
#define VICVectCntl27  (*(reg32_t *)(VIC_BASE_ADDR + 0x26C))
#define VICVectCntl28  (*(reg32_t *)(VIC_BASE_ADDR + 0x270))
#define VICVectCntl29  (*(reg32_t *)(VIC_BASE_ADDR + 0x274))
#define VICVectCntl30  (*(reg32_t *)(VIC_BASE_ADDR + 0x278))
#define VICVectCntl31  (*(reg32_t *)(VIC_BASE_ADDR + 0x27C))

#define VICVectAddr    (*(reg32_t *)(VIC_BASE_ADDR + 0xF00))


/* Pin Connect Block */
#define PINSEL_BASE_ADDR 0xE002C000
#define PINSEL0        (*(reg32_t *)(PINSEL_BASE_ADDR + 0x00))
#define PINSEL1        (*(reg32_t *)(PINSEL_BASE_ADDR + 0x04))
#define PINSEL2        (*(reg32_t *)(PINSEL_BASE_ADDR + 0x08))
#define PINSEL3        (*(reg32_t *)(PINSEL_BASE_ADDR + 0x0C))
#define PINSEL4        (*(reg32_t *)(PINSEL_BASE_ADDR + 0x10))
#define PINSEL5        (*(reg32_t *)(PINSEL_BASE_ADDR + 0x14))
#define PINSEL6        (*(reg32_t *)(PINSEL_BASE_ADDR + 0x18))
#define PINSEL7        (*(reg32_t *)(PINSEL_BASE_ADDR + 0x1C))
#define PINSEL8        (*(reg32_t *)(PINSEL_BASE_ADDR + 0x20))
#define PINSEL9        (*(reg32_t *)(PINSEL_BASE_ADDR + 0x24))
#define PINSEL10       (*(reg32_t *)(PINSEL_BASE_ADDR + 0x28))

#define PINSEL0_OFF        0x00
#define PINSEL1_OFF        0x04
#define PINSEL2_OFF        0x08
#define PINSEL3_OFF        0x0C
#define PINSEL4_OFF        0x10
#define PINSEL5_OFF        0x14
#define PINSEL6_OFF        0x18
#define PINSEL7_OFF        0x1C
#define PINSEL8_OFF        0x20
#define PINSEL9_OFF        0x24
#define PINSEL10_OFF       0x28

#define PINMODE0        (*(reg32_t *)(PINSEL_BASE_ADDR + 0x40))
#define PINMODE1        (*(reg32_t *)(PINSEL_BASE_ADDR + 0x44))
#define PINMODE2        (*(reg32_t *)(PINSEL_BASE_ADDR + 0x48))
#define PINMODE3        (*(reg32_t *)(PINSEL_BASE_ADDR + 0x4C))
#define PINMODE4        (*(reg32_t *)(PINSEL_BASE_ADDR + 0x50))
#define PINMODE5        (*(reg32_t *)(PINSEL_BASE_ADDR + 0x54))
#define PINMODE6        (*(reg32_t *)(PINSEL_BASE_ADDR + 0x58))
#define PINMODE7        (*(reg32_t *)(PINSEL_BASE_ADDR + 0x5C))
#define PINMODE8        (*(reg32_t *)(PINSEL_BASE_ADDR + 0x60))
#define PINMODE9        (*(reg32_t *)(PINSEL_BASE_ADDR + 0x64))

/* General Purpose Input/Output (GPIO) */
#define GPIO_BASE_ADDR		0xE0028000
#define IOPIN0         (*(reg32_t *)(GPIO_BASE_ADDR + 0x00))
#define IOSET0         (*(reg32_t *)(GPIO_BASE_ADDR + 0x04))
#define IODIR0         (*(reg32_t *)(GPIO_BASE_ADDR + 0x08))
#define IOCLR0         (*(reg32_t *)(GPIO_BASE_ADDR + 0x0C))
#define IOPIN1         (*(reg32_t *)(GPIO_BASE_ADDR + 0x10))
#define IOSET1         (*(reg32_t *)(GPIO_BASE_ADDR + 0x14))
#define IODIR1         (*(reg32_t *)(GPIO_BASE_ADDR + 0x18))
#define IOCLR1         (*(reg32_t *)(GPIO_BASE_ADDR + 0x1C))

/* GPIO Interrupt Registers */
#define IO0_INT_EN_R    (*(reg32_t *)(GPIO_BASE_ADDR + 0x90))
#define IO0_INT_EN_F    (*(reg32_t *)(GPIO_BASE_ADDR + 0x94))
#define IO0_INT_STAT_R  (*(reg32_t *)(GPIO_BASE_ADDR + 0x84))
#define IO0_INT_STAT_F  (*(reg32_t *)(GPIO_BASE_ADDR + 0x88))
#define IO0_INT_CLR     (*(reg32_t *)(GPIO_BASE_ADDR + 0x8C))

#define IO2_INT_EN_R    (*(reg32_t *)(GPIO_BASE_ADDR + 0xB0))
#define IO2_INT_EN_F    (*(reg32_t *)(GPIO_BASE_ADDR + 0xB4))
#define IO2_INT_STAT_R  (*(reg32_t *)(GPIO_BASE_ADDR + 0xA4))
#define IO2_INT_STAT_F  (*(reg32_t *)(GPIO_BASE_ADDR + 0xA8))
#define IO2_INT_CLR     (*(reg32_t *)(GPIO_BASE_ADDR + 0xAC))

#define IO_INT_STAT     (*(reg32_t *)(GPIO_BASE_ADDR + 0x80))

#define PARTCFG_BASE_ADDR		0x3FFF8000
#define PARTCFG        (*(reg32_t *)(PARTCFG_BASE_ADDR + 0x00))

/* Fast I/O setup */
#define FIO_BASE_ADDR		0x3FFFC000
#define FIO0DIR        (*(reg32_t *)(FIO_BASE_ADDR + 0x00))
#define FIO0MASK       (*(reg32_t *)(FIO_BASE_ADDR + 0x10))
#define FIO0PIN        (*(reg32_t *)(FIO_BASE_ADDR + 0x14))
#define FIO0SET        (*(reg32_t *)(FIO_BASE_ADDR + 0x18))
#define FIO0CLR        (*(reg32_t *)(FIO_BASE_ADDR + 0x1C))

#define FIO1DIR        (*(reg32_t *)(FIO_BASE_ADDR + 0x20))
#define FIO1MASK       (*(reg32_t *)(FIO_BASE_ADDR + 0x30))
#define FIO1PIN        (*(reg32_t *)(FIO_BASE_ADDR + 0x34))
#define FIO1SET        (*(reg32_t *)(FIO_BASE_ADDR + 0x38))
#define FIO1CLR        (*(reg32_t *)(FIO_BASE_ADDR + 0x3C))

#define FIO2DIR        (*(reg32_t *)(FIO_BASE_ADDR + 0x40))
#define FIO2MASK       (*(reg32_t *)(FIO_BASE_ADDR + 0x50))
#define FIO2PIN        (*(reg32_t *)(FIO_BASE_ADDR + 0x54))
#define FIO2SET        (*(reg32_t *)(FIO_BASE_ADDR + 0x58))
#define FIO2CLR        (*(reg32_t *)(FIO_BASE_ADDR + 0x5C))

#define FIO3DIR        (*(reg32_t *)(FIO_BASE_ADDR + 0x60))
#define FIO3MASK       (*(reg32_t *)(FIO_BASE_ADDR + 0x70))
#define FIO3PIN        (*(reg32_t *)(FIO_BASE_ADDR + 0x74))
#define FIO3SET        (*(reg32_t *)(FIO_BASE_ADDR + 0x78))
#define FIO3CLR        (*(reg32_t *)(FIO_BASE_ADDR + 0x7C))

#define FIO4DIR        (*(reg32_t *)(FIO_BASE_ADDR + 0x80))
#define FIO4MASK       (*(reg32_t *)(FIO_BASE_ADDR + 0x90))
#define FIO4PIN        (*(reg32_t *)(FIO_BASE_ADDR + 0x94))
#define FIO4SET        (*(reg32_t *)(FIO_BASE_ADDR + 0x98))
#define FIO4CLR        (*(reg32_t *)(FIO_BASE_ADDR + 0x9C))

/* FIOs can be accessed through WORD, HALF-WORD or BYTE. */
#define FIO0DIR0       (*(reg8_t *)(FIO_BASE_ADDR + 0x00))
#define FIO1DIR0       (*(reg8_t *)(FIO_BASE_ADDR + 0x20))
#define FIO2DIR0       (*(reg8_t *)(FIO_BASE_ADDR + 0x40))
#define FIO3DIR0       (*(reg8_t *)(FIO_BASE_ADDR + 0x60))
#define FIO4DIR0       (*(reg8_t *)(FIO_BASE_ADDR + 0x80))

#define FIO0DIR1       (*(reg8_t *)(FIO_BASE_ADDR + 0x01))
#define FIO1DIR1       (*(reg8_t *)(FIO_BASE_ADDR + 0x21))
#define FIO2DIR1       (*(reg8_t *)(FIO_BASE_ADDR + 0x41))
#define FIO3DIR1       (*(reg8_t *)(FIO_BASE_ADDR + 0x61))
#define FIO4DIR1       (*(reg8_t *)(FIO_BASE_ADDR + 0x81))

#define FIO0DIR2       (*(reg8_t *)(FIO_BASE_ADDR + 0x02))
#define FIO1DIR2       (*(reg8_t *)(FIO_BASE_ADDR + 0x22))
#define FIO2DIR2       (*(reg8_t *)(FIO_BASE_ADDR + 0x42))
#define FIO3DIR2       (*(reg8_t *)(FIO_BASE_ADDR + 0x62))
#define FIO4DIR2       (*(reg8_t *)(FIO_BASE_ADDR + 0x82))

#define FIO0DIR3       (*(reg8_t *)(FIO_BASE_ADDR + 0x03))
#define FIO1DIR3       (*(reg8_t *)(FIO_BASE_ADDR + 0x23))
#define FIO2DIR3       (*(reg8_t *)(FIO_BASE_ADDR + 0x43))
#define FIO3DIR3       (*(reg8_t *)(FIO_BASE_ADDR + 0x63))
#define FIO4DIR3       (*(reg8_t *)(FIO_BASE_ADDR + 0x83))

#define FIO0DIRL       (*(reg16_t *)(FIO_BASE_ADDR + 0x00))
#define FIO1DIRL       (*(reg16_t *)(FIO_BASE_ADDR + 0x20))
#define FIO2DIRL       (*(reg16_t *)(FIO_BASE_ADDR + 0x40))
#define FIO3DIRL       (*(reg16_t *)(FIO_BASE_ADDR + 0x60))
#define FIO4DIRL       (*(reg16_t *)(FIO_BASE_ADDR + 0x80))

#define FIO0DIRU       (*(reg16_t *)(FIO_BASE_ADDR + 0x02))
#define FIO1DIRU       (*(reg16_t *)(FIO_BASE_ADDR + 0x22))
#define FIO2DIRU       (*(reg16_t *)(FIO_BASE_ADDR + 0x42))
#define FIO3DIRU       (*(reg16_t *)(FIO_BASE_ADDR + 0x62))
#define FIO4DIRU       (*(reg16_t *)(FIO_BASE_ADDR + 0x82))

#define FIO0MASK0      (*(reg8_t *)(FIO_BASE_ADDR + 0x10))
#define FIO1MASK0      (*(reg8_t *)(FIO_BASE_ADDR + 0x30))
#define FIO2MASK0      (*(reg8_t *)(FIO_BASE_ADDR + 0x50))
#define FIO3MASK0      (*(reg8_t *)(FIO_BASE_ADDR + 0x70))
#define FIO4MASK0      (*(reg8_t *)(FIO_BASE_ADDR + 0x90))

#define FIO0MASK1      (*(reg8_t *)(FIO_BASE_ADDR + 0x11))
#define FIO1MASK1      (*(reg8_t *)(FIO_BASE_ADDR + 0x21))
#define FIO2MASK1      (*(reg8_t *)(FIO_BASE_ADDR + 0x51))
#define FIO3MASK1      (*(reg8_t *)(FIO_BASE_ADDR + 0x71))
#define FIO4MASK1      (*(reg8_t *)(FIO_BASE_ADDR + 0x91))

#define FIO0MASK2      (*(reg8_t *)(FIO_BASE_ADDR + 0x12))
#define FIO1MASK2      (*(reg8_t *)(FIO_BASE_ADDR + 0x32))
#define FIO2MASK2      (*(reg8_t *)(FIO_BASE_ADDR + 0x52))
#define FIO3MASK2      (*(reg8_t *)(FIO_BASE_ADDR + 0x72))
#define FIO4MASK2      (*(reg8_t *)(FIO_BASE_ADDR + 0x92))

#define FIO0MASK3      (*(reg8_t *)(FIO_BASE_ADDR + 0x13))
#define FIO1MASK3      (*(reg8_t *)(FIO_BASE_ADDR + 0x33))
#define FIO2MASK3      (*(reg8_t *)(FIO_BASE_ADDR + 0x53))
#define FIO3MASK3      (*(reg8_t *)(FIO_BASE_ADDR + 0x73))
#define FIO4MASK3      (*(reg8_t *)(FIO_BASE_ADDR + 0x93))

#define FIO0MASKL      (*(reg16_t *)(FIO_BASE_ADDR + 0x10))
#define FIO1MASKL      (*(reg16_t *)(FIO_BASE_ADDR + 0x30))
#define FIO2MASKL      (*(reg16_t *)(FIO_BASE_ADDR + 0x50))
#define FIO3MASKL      (*(reg16_t *)(FIO_BASE_ADDR + 0x70))
#define FIO4MASKL      (*(reg16_t *)(FIO_BASE_ADDR + 0x90))

#define FIO0MASKU      (*(reg16_t *)(FIO_BASE_ADDR + 0x12))
#define FIO1MASKU      (*(reg16_t *)(FIO_BASE_ADDR + 0x32))
#define FIO2MASKU      (*(reg16_t *)(FIO_BASE_ADDR + 0x52))
#define FIO3MASKU      (*(reg16_t *)(FIO_BASE_ADDR + 0x72))
#define FIO4MASKU      (*(reg16_t *)(FIO_BASE_ADDR + 0x92))

#define FIO0PIN0       (*(reg8_t *)(FIO_BASE_ADDR + 0x14))
#define FIO1PIN0       (*(reg8_t *)(FIO_BASE_ADDR + 0x34))
#define FIO2PIN0       (*(reg8_t *)(FIO_BASE_ADDR + 0x54))
#define FIO3PIN0       (*(reg8_t *)(FIO_BASE_ADDR + 0x74))
#define FIO4PIN0       (*(reg8_t *)(FIO_BASE_ADDR + 0x94))

#define FIO0PIN1       (*(reg8_t *)(FIO_BASE_ADDR + 0x15))
#define FIO1PIN1       (*(reg8_t *)(FIO_BASE_ADDR + 0x25))
#define FIO2PIN1       (*(reg8_t *)(FIO_BASE_ADDR + 0x55))
#define FIO3PIN1       (*(reg8_t *)(FIO_BASE_ADDR + 0x75))
#define FIO4PIN1       (*(reg8_t *)(FIO_BASE_ADDR + 0x95))

#define FIO0PIN2       (*(reg8_t *)(FIO_BASE_ADDR + 0x16))
#define FIO1PIN2       (*(reg8_t *)(FIO_BASE_ADDR + 0x36))
#define FIO2PIN2       (*(reg8_t *)(FIO_BASE_ADDR + 0x56))
#define FIO3PIN2       (*(reg8_t *)(FIO_BASE_ADDR + 0x76))
#define FIO4PIN2       (*(reg8_t *)(FIO_BASE_ADDR + 0x96))

#define FIO0PIN3       (*(reg8_t *)(FIO_BASE_ADDR + 0x17))
#define FIO1PIN3       (*(reg8_t *)(FIO_BASE_ADDR + 0x37))
#define FIO2PIN3       (*(reg8_t *)(FIO_BASE_ADDR + 0x57))
#define FIO3PIN3       (*(reg8_t *)(FIO_BASE_ADDR + 0x77))
#define FIO4PIN3       (*(reg8_t *)(FIO_BASE_ADDR + 0x97))

#define FIO0PINL       (*(reg16_t *)(FIO_BASE_ADDR + 0x14))
#define FIO1PINL       (*(reg16_t *)(FIO_BASE_ADDR + 0x34))
#define FIO2PINL       (*(reg16_t *)(FIO_BASE_ADDR + 0x54))
#define FIO3PINL       (*(reg16_t *)(FIO_BASE_ADDR + 0x74))
#define FIO4PINL       (*(reg16_t *)(FIO_BASE_ADDR + 0x94))

#define FIO0PINU       (*(reg16_t *)(FIO_BASE_ADDR + 0x16))
#define FIO1PINU       (*(reg16_t *)(FIO_BASE_ADDR + 0x36))
#define FIO2PINU       (*(reg16_t *)(FIO_BASE_ADDR + 0x56))
#define FIO3PINU       (*(reg16_t *)(FIO_BASE_ADDR + 0x76))
#define FIO4PINU       (*(reg16_t *)(FIO_BASE_ADDR + 0x96))

#define FIO0SET0       (*(reg8_t *)(FIO_BASE_ADDR + 0x18))
#define FIO1SET0       (*(reg8_t *)(FIO_BASE_ADDR + 0x38))
#define FIO2SET0       (*(reg8_t *)(FIO_BASE_ADDR + 0x58))
#define FIO3SET0       (*(reg8_t *)(FIO_BASE_ADDR + 0x78))
#define FIO4SET0       (*(reg8_t *)(FIO_BASE_ADDR + 0x98))

#define FIO0SET1       (*(reg8_t *)(FIO_BASE_ADDR + 0x19))
#define FIO1SET1       (*(reg8_t *)(FIO_BASE_ADDR + 0x29))
#define FIO2SET1       (*(reg8_t *)(FIO_BASE_ADDR + 0x59))
#define FIO3SET1       (*(reg8_t *)(FIO_BASE_ADDR + 0x79))
#define FIO4SET1       (*(reg8_t *)(FIO_BASE_ADDR + 0x99))

#define FIO0SET2       (*(reg8_t *)(FIO_BASE_ADDR + 0x1A))
#define FIO1SET2       (*(reg8_t *)(FIO_BASE_ADDR + 0x3A))
#define FIO2SET2       (*(reg8_t *)(FIO_BASE_ADDR + 0x5A))
#define FIO3SET2       (*(reg8_t *)(FIO_BASE_ADDR + 0x7A))
#define FIO4SET2       (*(reg8_t *)(FIO_BASE_ADDR + 0x9A))

#define FIO0SET3       (*(reg8_t *)(FIO_BASE_ADDR + 0x1B))
#define FIO1SET3       (*(reg8_t *)(FIO_BASE_ADDR + 0x3B))
#define FIO2SET3       (*(reg8_t *)(FIO_BASE_ADDR + 0x5B))
#define FIO3SET3       (*(reg8_t *)(FIO_BASE_ADDR + 0x7B))
#define FIO4SET3       (*(reg8_t *)(FIO_BASE_ADDR + 0x9B))

#define FIO0SETL       (*(reg16_t *)(FIO_BASE_ADDR + 0x18))
#define FIO1SETL       (*(reg16_t *)(FIO_BASE_ADDR + 0x38))
#define FIO2SETL       (*(reg16_t *)(FIO_BASE_ADDR + 0x58))
#define FIO3SETL       (*(reg16_t *)(FIO_BASE_ADDR + 0x78))
#define FIO4SETL       (*(reg16_t *)(FIO_BASE_ADDR + 0x98))

#define FIO0SETU       (*(reg16_t *)(FIO_BASE_ADDR + 0x1A))
#define FIO1SETU       (*(reg16_t *)(FIO_BASE_ADDR + 0x3A))
#define FIO2SETU       (*(reg16_t *)(FIO_BASE_ADDR + 0x5A))
#define FIO3SETU       (*(reg16_t *)(FIO_BASE_ADDR + 0x7A))
#define FIO4SETU       (*(reg16_t *)(FIO_BASE_ADDR + 0x9A))

#define FIO0CLR0       (*(reg8_t *)(FIO_BASE_ADDR + 0x1C))
#define FIO1CLR0       (*(reg8_t *)(FIO_BASE_ADDR + 0x3C))
#define FIO2CLR0       (*(reg8_t *)(FIO_BASE_ADDR + 0x5C))
#define FIO3CLR0       (*(reg8_t *)(FIO_BASE_ADDR + 0x7C))
#define FIO4CLR0       (*(reg8_t *)(FIO_BASE_ADDR + 0x9C))

#define FIO0CLR1       (*(reg8_t *)(FIO_BASE_ADDR + 0x1D))
#define FIO1CLR1       (*(reg8_t *)(FIO_BASE_ADDR + 0x2D))
#define FIO2CLR1       (*(reg8_t *)(FIO_BASE_ADDR + 0x5D))
#define FIO3CLR1       (*(reg8_t *)(FIO_BASE_ADDR + 0x7D))
#define FIO4CLR1       (*(reg8_t *)(FIO_BASE_ADDR + 0x9D))

#define FIO0CLR2       (*(reg8_t *)(FIO_BASE_ADDR + 0x1E))
#define FIO1CLR2       (*(reg8_t *)(FIO_BASE_ADDR + 0x3E))
#define FIO2CLR2       (*(reg8_t *)(FIO_BASE_ADDR + 0x5E))
#define FIO3CLR2       (*(reg8_t *)(FIO_BASE_ADDR + 0x7E))
#define FIO4CLR2       (*(reg8_t *)(FIO_BASE_ADDR + 0x9E))

#define FIO0CLR3       (*(reg8_t *)(FIO_BASE_ADDR + 0x1F))
#define FIO1CLR3       (*(reg8_t *)(FIO_BASE_ADDR + 0x3F))
#define FIO2CLR3       (*(reg8_t *)(FIO_BASE_ADDR + 0x5F))
#define FIO3CLR3       (*(reg8_t *)(FIO_BASE_ADDR + 0x7F))
#define FIO4CLR3       (*(reg8_t *)(FIO_BASE_ADDR + 0x9F))

#define FIO0CLRL       (*(reg16_t *)(FIO_BASE_ADDR + 0x1C))
#define FIO1CLRL       (*(reg16_t *)(FIO_BASE_ADDR + 0x3C))
#define FIO2CLRL       (*(reg16_t *)(FIO_BASE_ADDR + 0x5C))
#define FIO3CLRL       (*(reg16_t *)(FIO_BASE_ADDR + 0x7C))
#define FIO4CLRL       (*(reg16_t *)(FIO_BASE_ADDR + 0x9C))

#define FIO0CLRU       (*(reg16_t *)(FIO_BASE_ADDR + 0x1E))
#define FIO1CLRU       (*(reg16_t *)(FIO_BASE_ADDR + 0x3E))
#define FIO2CLRU       (*(reg16_t *)(FIO_BASE_ADDR + 0x5E))
#define FIO3CLRU       (*(reg16_t *)(FIO_BASE_ADDR + 0x7E))
#define FIO4CLRU       (*(reg16_t *)(FIO_BASE_ADDR + 0x9E))


/* System Control Block(SCB) modules include Memory Accelerator Module,
Phase Locked Loop, VPB divider, Power Control, External Interrupt,
Reset, and Code Security/Debugging */
#define SCB_BASE_ADDR	0xE01FC000

/* Memory Accelerator Module (MAM) */
#define MAMCR          (*(reg32_t *)(SCB_BASE_ADDR + 0x000))
#define MAMTIM         (*(reg32_t *)(SCB_BASE_ADDR + 0x004))
#define MEMMAP         (*(reg32_t *)(SCB_BASE_ADDR + 0x040))

/* Phase Locked Loop (PLL) */
#define PLLCON         (*(reg32_t *)(SCB_BASE_ADDR + 0x080))
#define PLLCFG         (*(reg32_t *)(SCB_BASE_ADDR + 0x084))
#define PLLSTAT        (*(reg32_t *)(SCB_BASE_ADDR + 0x088))
#define PLLFEED        (*(reg32_t *)(SCB_BASE_ADDR + 0x08C))

/* Power Control */
#define PCON           (*(reg32_t *)(SCB_BASE_ADDR + 0x0C0))
#define PCONP          (*(reg32_t *)(SCB_BASE_ADDR + 0x0C4))
#define PCONP_PCI2C0                                      7
#define PCONP_PCI2C1                                     19
#define PCONP_PCI2C2                                     26


/* Clock Divider */
// #define APBDIV         (*(reg32_t *)(SCB_BASE_ADDR + 0x100))
#define CCLKCFG        (*(reg32_t *)(SCB_BASE_ADDR + 0x104))
#define USBCLKCFG      (*(reg32_t *)(SCB_BASE_ADDR + 0x108))
#define CLKSRCSEL      (*(reg32_t *)(SCB_BASE_ADDR + 0x10C))
#define PCLKSEL0       (*(reg32_t *)(SCB_BASE_ADDR + 0x1A8))
#define PCLKSEL1       (*(reg32_t *)(SCB_BASE_ADDR + 0x1AC))

#define CCLKCFG_OFF        0x104
#define USBCLKCFG_OFF      0x108
#define CLKSRCSEL_OFF      0x10C
#define PCLKSEL0_OFF       0x1A8
#define PCLKSEL1_OFF       0x1AC


/* External Interrupts */
#define EXTINT         (*(reg32_t *)(SCB_BASE_ADDR + 0x140))
#define INTWAKE        (*(reg32_t *)(SCB_BASE_ADDR + 0x144))
#define EXTMODE        (*(reg32_t *)(SCB_BASE_ADDR + 0x148))
#define EXTPOLAR       (*(reg32_t *)(SCB_BASE_ADDR + 0x14C))

/* Reset, reset source identification */
#define RSIR           (*(reg32_t *)(SCB_BASE_ADDR + 0x180))

/* RSID, code security protection */
#define CSPR           (*(reg32_t *)(SCB_BASE_ADDR + 0x184))

/* AHB configuration */
#define AHBCFG1        (*(reg32_t *)(SCB_BASE_ADDR + 0x188))
#define AHBCFG2        (*(reg32_t *)(SCB_BASE_ADDR + 0x18C))

/* System Controls and Status */
#define SCS            (*(reg32_t *)(SCB_BASE_ADDR + 0x1A0))

/* MPMC(EMC) registers, note: all the external memory controller(EMC) registers
are for LPC24xx only. */
#define STATIC_MEM0_BASE		0x80000000
#define STATIC_MEM1_BASE		0x81000000
#define STATIC_MEM2_BASE		0x82000000
#define STATIC_MEM3_BASE		0x83000000

#define DYNAMIC_MEM0_BASE		0xA0000000
#define DYNAMIC_MEM1_BASE		0xB0000000
#define DYNAMIC_MEM2_BASE		0xC0000000
#define DYNAMIC_MEM3_BASE		0xD0000000

/* External Memory Controller (EMC) */
#define EMC_BASE_ADDR		0xFFE08000
#define EMC_CTRL       (*(reg32_t *)(EMC_BASE_ADDR + 0x000))
#define EMC_STAT       (*(reg32_t *)(EMC_BASE_ADDR + 0x004))
#define EMC_CONFIG     (*(reg32_t *)(EMC_BASE_ADDR + 0x008))

/* Dynamic RAM access registers */
#define EMC_DYN_CTRL     (*(reg32_t *)(EMC_BASE_ADDR + 0x020))
#define EMC_DYN_RFSH     (*(reg32_t *)(EMC_BASE_ADDR + 0x024))
#define EMC_DYN_RD_CFG   (*(reg32_t *)(EMC_BASE_ADDR + 0x028))
#define EMC_DYN_RP       (*(reg32_t *)(EMC_BASE_ADDR + 0x030))
#define EMC_DYN_RAS      (*(reg32_t *)(EMC_BASE_ADDR + 0x034))
#define EMC_DYN_SREX     (*(reg32_t *)(EMC_BASE_ADDR + 0x038))
#define EMC_DYN_APR      (*(reg32_t *)(EMC_BASE_ADDR + 0x03C))
#define EMC_DYN_DAL      (*(reg32_t *)(EMC_BASE_ADDR + 0x040))
#define EMC_DYN_WR       (*(reg32_t *)(EMC_BASE_ADDR + 0x044))
#define EMC_DYN_RC       (*(reg32_t *)(EMC_BASE_ADDR + 0x048))
#define EMC_DYN_RFC      (*(reg32_t *)(EMC_BASE_ADDR + 0x04C))
#define EMC_DYN_XSR      (*(reg32_t *)(EMC_BASE_ADDR + 0x050))
#define EMC_DYN_RRD      (*(reg32_t *)(EMC_BASE_ADDR + 0x054))
#define EMC_DYN_MRD      (*(reg32_t *)(EMC_BASE_ADDR + 0x058))

#define EMC_DYN_CFG0     (*(reg32_t *)(EMC_BASE_ADDR + 0x100))
#define EMC_DYN_RASCAS0  (*(reg32_t *)(EMC_BASE_ADDR + 0x104))
#define EMC_DYN_CFG1     (*(reg32_t *)(EMC_BASE_ADDR + 0x140))
#define EMC_DYN_RASCAS1  (*(reg32_t *)(EMC_BASE_ADDR + 0x144))
#define EMC_DYN_CFG2     (*(reg32_t *)(EMC_BASE_ADDR + 0x160))
#define EMC_DYN_RASCAS2  (*(reg32_t *)(EMC_BASE_ADDR + 0x164))
#define EMC_DYN_CFG3     (*(reg32_t *)(EMC_BASE_ADDR + 0x180))
#define EMC_DYN_RASCAS3  (*(reg32_t *)(EMC_BASE_ADDR + 0x184))

/* static RAM access registers */
#define EMC_STA_CFG0      (*(reg32_t *)(EMC_BASE_ADDR + 0x200))
#define EMC_STA_WAITWEN0  (*(reg32_t *)(EMC_BASE_ADDR + 0x204))
#define EMC_STA_WAITOEN0  (*(reg32_t *)(EMC_BASE_ADDR + 0x208))
#define EMC_STA_WAITRD0   (*(reg32_t *)(EMC_BASE_ADDR + 0x20C))
#define EMC_STA_WAITPAGE0 (*(reg32_t *)(EMC_BASE_ADDR + 0x210))
#define EMC_STA_WAITWR0   (*(reg32_t *)(EMC_BASE_ADDR + 0x214))
#define EMC_STA_WAITTURN0 (*(reg32_t *)(EMC_BASE_ADDR + 0x218))

#define EMC_STA_CFG1      (*(reg32_t *)(EMC_BASE_ADDR + 0x220))
#define EMC_STA_WAITWEN1  (*(reg32_t *)(EMC_BASE_ADDR + 0x224))
#define EMC_STA_WAITOEN1  (*(reg32_t *)(EMC_BASE_ADDR + 0x228))
#define EMC_STA_WAITRD1   (*(reg32_t *)(EMC_BASE_ADDR + 0x22C))
#define EMC_STA_WAITPAGE1 (*(reg32_t *)(EMC_BASE_ADDR + 0x230))
#define EMC_STA_WAITWR1   (*(reg32_t *)(EMC_BASE_ADDR + 0x234))
#define EMC_STA_WAITTURN1 (*(reg32_t *)(EMC_BASE_ADDR + 0x238))

#define EMC_STA_CFG2      (*(reg32_t *)(EMC_BASE_ADDR + 0x240))
#define EMC_STA_WAITWEN2  (*(reg32_t *)(EMC_BASE_ADDR + 0x244))
#define EMC_STA_WAITOEN2  (*(reg32_t *)(EMC_BASE_ADDR + 0x248))
#define EMC_STA_WAITRD2   (*(reg32_t *)(EMC_BASE_ADDR + 0x24C))
#define EMC_STA_WAITPAGE2 (*(reg32_t *)(EMC_BASE_ADDR + 0x250))
#define EMC_STA_WAITWR2   (*(reg32_t *)(EMC_BASE_ADDR + 0x254))
#define EMC_STA_WAITTURN2 (*(reg32_t *)(EMC_BASE_ADDR + 0x258))

#define EMC_STA_CFG3      (*(reg32_t *)(EMC_BASE_ADDR + 0x260))
#define EMC_STA_WAITWEN3  (*(reg32_t *)(EMC_BASE_ADDR + 0x264))
#define EMC_STA_WAITOEN3  (*(reg32_t *)(EMC_BASE_ADDR + 0x268))
#define EMC_STA_WAITRD3   (*(reg32_t *)(EMC_BASE_ADDR + 0x26C))
#define EMC_STA_WAITPAGE3 (*(reg32_t *)(EMC_BASE_ADDR + 0x270))
#define EMC_STA_WAITWR3   (*(reg32_t *)(EMC_BASE_ADDR + 0x274))
#define EMC_STA_WAITTURN3 (*(reg32_t *)(EMC_BASE_ADDR + 0x278))

#define EMC_STA_EXT_WAIT  (*(reg32_t *)(EMC_BASE_ADDR + 0x880))


/* Timer 0 */
#define TMR0_BASE_ADDR		0xE0004000
#define T0IR           (*(reg32_t *)(TMR0_BASE_ADDR + 0x00))
#define T0TCR          (*(reg32_t *)(TMR0_BASE_ADDR + 0x04))
#define T0TC           (*(reg32_t *)(TMR0_BASE_ADDR + 0x08))
#define T0PR           (*(reg32_t *)(TMR0_BASE_ADDR + 0x0C))
#define T0PC           (*(reg32_t *)(TMR0_BASE_ADDR + 0x10))
#define T0MCR          (*(reg32_t *)(TMR0_BASE_ADDR + 0x14))
#define T0MR0          (*(reg32_t *)(TMR0_BASE_ADDR + 0x18))
#define T0MR1          (*(reg32_t *)(TMR0_BASE_ADDR + 0x1C))
#define T0MR2          (*(reg32_t *)(TMR0_BASE_ADDR + 0x20))
#define T0MR3          (*(reg32_t *)(TMR0_BASE_ADDR + 0x24))
#define T0CCR          (*(reg32_t *)(TMR0_BASE_ADDR + 0x28))
#define T0CR0          (*(reg32_t *)(TMR0_BASE_ADDR + 0x2C))
#define T0CR1          (*(reg32_t *)(TMR0_BASE_ADDR + 0x30))
#define T0CR2          (*(reg32_t *)(TMR0_BASE_ADDR + 0x34))
#define T0CR3          (*(reg32_t *)(TMR0_BASE_ADDR + 0x38))
#define T0EMR          (*(reg32_t *)(TMR0_BASE_ADDR + 0x3C))
#define T0CTCR         (*(reg32_t *)(TMR0_BASE_ADDR + 0x70))

/* Timer 1 */
#define TMR1_BASE_ADDR		0xE0008000
#define T1IR           (*(reg32_t *)(TMR1_BASE_ADDR + 0x00))
#define T1TCR          (*(reg32_t *)(TMR1_BASE_ADDR + 0x04))
#define T1TC           (*(reg32_t *)(TMR1_BASE_ADDR + 0x08))
#define T1PR           (*(reg32_t *)(TMR1_BASE_ADDR + 0x0C))
#define T1PC           (*(reg32_t *)(TMR1_BASE_ADDR + 0x10))
#define T1MCR          (*(reg32_t *)(TMR1_BASE_ADDR + 0x14))
#define T1MR0          (*(reg32_t *)(TMR1_BASE_ADDR + 0x18))
#define T1MR1          (*(reg32_t *)(TMR1_BASE_ADDR + 0x1C))
#define T1MR2          (*(reg32_t *)(TMR1_BASE_ADDR + 0x20))
#define T1MR3          (*(reg32_t *)(TMR1_BASE_ADDR + 0x24))
#define T1CCR          (*(reg32_t *)(TMR1_BASE_ADDR + 0x28))
#define T1CR0          (*(reg32_t *)(TMR1_BASE_ADDR + 0x2C))
#define T1CR1          (*(reg32_t *)(TMR1_BASE_ADDR + 0x30))
#define T1CR2          (*(reg32_t *)(TMR1_BASE_ADDR + 0x34))
#define T1CR3          (*(reg32_t *)(TMR1_BASE_ADDR + 0x38))
#define T1EMR          (*(reg32_t *)(TMR1_BASE_ADDR + 0x3C))
#define T1CTCR         (*(reg32_t *)(TMR1_BASE_ADDR + 0x70))

/* Timer 2 */
#define TMR2_BASE_ADDR		0xE0070000
#define T2IR           (*(reg32_t *)(TMR2_BASE_ADDR + 0x00))
#define T2TCR          (*(reg32_t *)(TMR2_BASE_ADDR + 0x04))
#define T2TC           (*(reg32_t *)(TMR2_BASE_ADDR + 0x08))
#define T2PR           (*(reg32_t *)(TMR2_BASE_ADDR + 0x0C))
#define T2PC           (*(reg32_t *)(TMR2_BASE_ADDR + 0x10))
#define T2MCR          (*(reg32_t *)(TMR2_BASE_ADDR + 0x14))
#define T2MR0          (*(reg32_t *)(TMR2_BASE_ADDR + 0x18))
#define T2MR1          (*(reg32_t *)(TMR2_BASE_ADDR + 0x1C))
#define T2MR2          (*(reg32_t *)(TMR2_BASE_ADDR + 0x20))
#define T2MR3          (*(reg32_t *)(TMR2_BASE_ADDR + 0x24))
#define T2CCR          (*(reg32_t *)(TMR2_BASE_ADDR + 0x28))
#define T2CR0          (*(reg32_t *)(TMR2_BASE_ADDR + 0x2C))
#define T2CR1          (*(reg32_t *)(TMR2_BASE_ADDR + 0x30))
#define T2CR2          (*(reg32_t *)(TMR2_BASE_ADDR + 0x34))
#define T2CR3          (*(reg32_t *)(TMR2_BASE_ADDR + 0x38))
#define T2EMR          (*(reg32_t *)(TMR2_BASE_ADDR + 0x3C))
#define T2CTCR         (*(reg32_t *)(TMR2_BASE_ADDR + 0x70))

/* Timer 3 */
#define TMR3_BASE_ADDR		0xE0074000
#define T3IR           (*(reg32_t *)(TMR3_BASE_ADDR + 0x00))
#define T3TCR          (*(reg32_t *)(TMR3_BASE_ADDR + 0x04))
#define T3TC           (*(reg32_t *)(TMR3_BASE_ADDR + 0x08))
#define T3PR           (*(reg32_t *)(TMR3_BASE_ADDR + 0x0C))
#define T3PC           (*(reg32_t *)(TMR3_BASE_ADDR + 0x10))
#define T3MCR          (*(reg32_t *)(TMR3_BASE_ADDR + 0x14))
#define T3MR0          (*(reg32_t *)(TMR3_BASE_ADDR + 0x18))
#define T3MR1          (*(reg32_t *)(TMR3_BASE_ADDR + 0x1C))
#define T3MR2          (*(reg32_t *)(TMR3_BASE_ADDR + 0x20))
#define T3MR3          (*(reg32_t *)(TMR3_BASE_ADDR + 0x24))
#define T3CCR          (*(reg32_t *)(TMR3_BASE_ADDR + 0x28))
#define T3CR0          (*(reg32_t *)(TMR3_BASE_ADDR + 0x2C))
#define T3CR1          (*(reg32_t *)(TMR3_BASE_ADDR + 0x30))
#define T3CR2          (*(reg32_t *)(TMR3_BASE_ADDR + 0x34))
#define T3CR3          (*(reg32_t *)(TMR3_BASE_ADDR + 0x38))
#define T3EMR          (*(reg32_t *)(TMR3_BASE_ADDR + 0x3C))
#define T3CTCR         (*(reg32_t *)(TMR3_BASE_ADDR + 0x70))


/* Pulse Width Modulator (PWM) */
#define PWM0_BASE_ADDR		0xE0014000
#define PWM0IR          (*(reg32_t *)(PWM0_BASE_ADDR + 0x00))
#define PWM0TCR         (*(reg32_t *)(PWM0_BASE_ADDR + 0x04))
#define PWM0TC          (*(reg32_t *)(PWM0_BASE_ADDR + 0x08))
#define PWM0PR          (*(reg32_t *)(PWM0_BASE_ADDR + 0x0C))
#define PWM0PC          (*(reg32_t *)(PWM0_BASE_ADDR + 0x10))
#define PWM0MCR         (*(reg32_t *)(PWM0_BASE_ADDR + 0x14))
#define PWM0MR0         (*(reg32_t *)(PWM0_BASE_ADDR + 0x18))
#define PWM0MR1         (*(reg32_t *)(PWM0_BASE_ADDR + 0x1C))
#define PWM0MR2         (*(reg32_t *)(PWM0_BASE_ADDR + 0x20))
#define PWM0MR3         (*(reg32_t *)(PWM0_BASE_ADDR + 0x24))
#define PWM0CCR         (*(reg32_t *)(PWM0_BASE_ADDR + 0x28))
#define PWM0CR0         (*(reg32_t *)(PWM0_BASE_ADDR + 0x2C))
#define PWM0CR1         (*(reg32_t *)(PWM0_BASE_ADDR + 0x30))
#define PWM0CR2         (*(reg32_t *)(PWM0_BASE_ADDR + 0x34))
#define PWM0CR3         (*(reg32_t *)(PWM0_BASE_ADDR + 0x38))
#define PWM0EMR         (*(reg32_t *)(PWM0_BASE_ADDR + 0x3C))
#define PWM0MR4         (*(reg32_t *)(PWM0_BASE_ADDR + 0x40))
#define PWM0MR5         (*(reg32_t *)(PWM0_BASE_ADDR + 0x44))
#define PWM0MR6         (*(reg32_t *)(PWM0_BASE_ADDR + 0x48))
#define PWM0PCR         (*(reg32_t *)(PWM0_BASE_ADDR + 0x4C))
#define PWM0LER         (*(reg32_t *)(PWM0_BASE_ADDR + 0x50))
#define PWM0CTCR        (*(reg32_t *)(PWM0_BASE_ADDR + 0x70))

#define PWM1_BASE_ADDR		0xE0018000
#define PWM1IR          (*(reg32_t *)(PWM1_BASE_ADDR + 0x00))
#define PWM1TCR         (*(reg32_t *)(PWM1_BASE_ADDR + 0x04))
#define PWM1TC          (*(reg32_t *)(PWM1_BASE_ADDR + 0x08))
#define PWM1PR          (*(reg32_t *)(PWM1_BASE_ADDR + 0x0C))
#define PWM1PC          (*(reg32_t *)(PWM1_BASE_ADDR + 0x10))
#define PWM1MCR         (*(reg32_t *)(PWM1_BASE_ADDR + 0x14))
#define PWM1MR0         (*(reg32_t *)(PWM1_BASE_ADDR + 0x18))
#define PWM1MR1         (*(reg32_t *)(PWM1_BASE_ADDR + 0x1C))
#define PWM1MR2         (*(reg32_t *)(PWM1_BASE_ADDR + 0x20))
#define PWM1MR3         (*(reg32_t *)(PWM1_BASE_ADDR + 0x24))
#define PWM1CCR         (*(reg32_t *)(PWM1_BASE_ADDR + 0x28))
#define PWM1CR0         (*(reg32_t *)(PWM1_BASE_ADDR + 0x2C))
#define PWM1CR1         (*(reg32_t *)(PWM1_BASE_ADDR + 0x30))
#define PWM1CR2         (*(reg32_t *)(PWM1_BASE_ADDR + 0x34))
#define PWM1CR3         (*(reg32_t *)(PWM1_BASE_ADDR + 0x38))
#define PWM1EMR         (*(reg32_t *)(PWM1_BASE_ADDR + 0x3C))
#define PWM1MR4         (*(reg32_t *)(PWM1_BASE_ADDR + 0x40))
#define PWM1MR5         (*(reg32_t *)(PWM1_BASE_ADDR + 0x44))
#define PWM1MR6         (*(reg32_t *)(PWM1_BASE_ADDR + 0x48))
#define PWM1PCR         (*(reg32_t *)(PWM1_BASE_ADDR + 0x4C))
#define PWM1LER         (*(reg32_t *)(PWM1_BASE_ADDR + 0x50))
#define PWM1CTCR        (*(reg32_t *)(PWM1_BASE_ADDR + 0x70))


/* Universal Asynchronous Receiver Transmitter 0 (UART0) */
#define UART0_BASE_ADDR		0xE000C000
#define U0RBR          (*(reg32_t *)(UART0_BASE_ADDR + 0x00))
#define U0THR          (*(reg32_t *)(UART0_BASE_ADDR + 0x00))
#define U0DLL          (*(reg32_t *)(UART0_BASE_ADDR + 0x00))
#define U0DLM          (*(reg32_t *)(UART0_BASE_ADDR + 0x04))
#define U0IER          (*(reg32_t *)(UART0_BASE_ADDR + 0x04))
#define U0IIR          (*(reg32_t *)(UART0_BASE_ADDR + 0x08))
#define U0FCR          (*(reg32_t *)(UART0_BASE_ADDR + 0x08))
#define U0LCR          (*(reg32_t *)(UART0_BASE_ADDR + 0x0C))
#define U0LSR          (*(reg32_t *)(UART0_BASE_ADDR + 0x14))
#define U0SCR          (*(reg32_t *)(UART0_BASE_ADDR + 0x1C))
#define U0ACR          (*(reg32_t *)(UART0_BASE_ADDR + 0x20))
#define U0ICR          (*(reg32_t *)(UART0_BASE_ADDR + 0x24))
#define U0FDR          (*(reg32_t *)(UART0_BASE_ADDR + 0x28))
#define U0TER          (*(reg32_t *)(UART0_BASE_ADDR + 0x30))

/* Universal Asynchronous Receiver Transmitter 1 (UART1) */
#define UART1_BASE_ADDR		0xE0010000
#define U1RBR          (*(reg32_t *)(UART1_BASE_ADDR + 0x00))
#define U1THR          (*(reg32_t *)(UART1_BASE_ADDR + 0x00))
#define U1DLL          (*(reg32_t *)(UART1_BASE_ADDR + 0x00))
#define U1DLM          (*(reg32_t *)(UART1_BASE_ADDR + 0x04))
#define U1IER          (*(reg32_t *)(UART1_BASE_ADDR + 0x04))
#define U1IIR          (*(reg32_t *)(UART1_BASE_ADDR + 0x08))
#define U1FCR          (*(reg32_t *)(UART1_BASE_ADDR + 0x08))
#define U1LCR          (*(reg32_t *)(UART1_BASE_ADDR + 0x0C))
#define U1MCR          (*(reg32_t *)(UART1_BASE_ADDR + 0x10))
#define U1LSR          (*(reg32_t *)(UART1_BASE_ADDR + 0x14))
#define U1MSR          (*(reg32_t *)(UART1_BASE_ADDR + 0x18))
#define U1SCR          (*(reg32_t *)(UART1_BASE_ADDR + 0x1C))
#define U1ACR          (*(reg32_t *)(UART1_BASE_ADDR + 0x20))
#define U1FDR          (*(reg32_t *)(UART1_BASE_ADDR + 0x28))
#define U1TER          (*(reg32_t *)(UART1_BASE_ADDR + 0x30))

/* Universal Asynchronous Receiver Transmitter 2 (UART2) */
#define UART2_BASE_ADDR		0xE0078000
#define U2RBR          (*(reg32_t *)(UART2_BASE_ADDR + 0x00))
#define U2THR          (*(reg32_t *)(UART2_BASE_ADDR + 0x00))
#define U2DLL          (*(reg32_t *)(UART2_BASE_ADDR + 0x00))
#define U2DLM          (*(reg32_t *)(UART2_BASE_ADDR + 0x04))
#define U2IER          (*(reg32_t *)(UART2_BASE_ADDR + 0x04))
#define U2IIR          (*(reg32_t *)(UART2_BASE_ADDR + 0x08))
#define U2FCR          (*(reg32_t *)(UART2_BASE_ADDR + 0x08))
#define U2LCR          (*(reg32_t *)(UART2_BASE_ADDR + 0x0C))
#define U2LSR          (*(reg32_t *)(UART2_BASE_ADDR + 0x14))
#define U2SCR          (*(reg32_t *)(UART2_BASE_ADDR + 0x1C))
#define U2ACR          (*(reg32_t *)(UART2_BASE_ADDR + 0x20))
#define U2ICR          (*(reg32_t *)(UART2_BASE_ADDR + 0x24))
#define U2FDR          (*(reg32_t *)(UART2_BASE_ADDR + 0x28))
#define U2TER          (*(reg32_t *)(UART2_BASE_ADDR + 0x30))

/* Universal Asynchronous Receiver Transmitter 3 (UART3) */
#define UART3_BASE_ADDR		0xE007C000
#define U3RBR          (*(reg32_t *)(UART3_BASE_ADDR + 0x00))
#define U3THR          (*(reg32_t *)(UART3_BASE_ADDR + 0x00))
#define U3DLL          (*(reg32_t *)(UART3_BASE_ADDR + 0x00))
#define U3DLM          (*(reg32_t *)(UART3_BASE_ADDR + 0x04))
#define U3IER          (*(reg32_t *)(UART3_BASE_ADDR + 0x04))
#define U3IIR          (*(reg32_t *)(UART3_BASE_ADDR + 0x08))
#define U3FCR          (*(reg32_t *)(UART3_BASE_ADDR + 0x08))
#define U3LCR          (*(reg32_t *)(UART3_BASE_ADDR + 0x0C))
#define U3LSR          (*(reg32_t *)(UART3_BASE_ADDR + 0x14))
#define U3SCR          (*(reg32_t *)(UART3_BASE_ADDR + 0x1C))
#define U3ACR          (*(reg32_t *)(UART3_BASE_ADDR + 0x20))
#define U3ICR          (*(reg32_t *)(UART3_BASE_ADDR + 0x24))
#define U3FDR          (*(reg32_t *)(UART3_BASE_ADDR + 0x28))
#define U3TER          (*(reg32_t *)(UART3_BASE_ADDR + 0x30))

/* I2C Interface 0 */
#define I2C0_BASE_ADDR 0xE001C000
#define I20CONSET      (*(reg32_t *)(I2C0_BASE_ADDR + 0x00))
#define I20STAT        (*(reg32_t *)(I2C0_BASE_ADDR + 0x04))
#define I20DAT         (*(reg32_t *)(I2C0_BASE_ADDR + 0x08))
#define I20ADR         (*(reg32_t *)(I2C0_BASE_ADDR + 0x0C))
#define I20SCLH        (*(reg32_t *)(I2C0_BASE_ADDR + 0x10))
#define I20SCLL        (*(reg32_t *)(I2C0_BASE_ADDR + 0x14))
#define I20CONCLR      (*(reg32_t *)(I2C0_BASE_ADDR + 0x18))

/* I2C Interface 1 */
#define I2C1_BASE_ADDR 0xE005C000
#define I21CONSET      (*(reg32_t *)(I2C1_BASE_ADDR + 0x00))
#define I21STAT        (*(reg32_t *)(I2C1_BASE_ADDR + 0x04))
#define I21DAT         (*(reg32_t *)(I2C1_BASE_ADDR + 0x08))
#define I21ADR         (*(reg32_t *)(I2C1_BASE_ADDR + 0x0C))
#define I21SCLH        (*(reg32_t *)(I2C1_BASE_ADDR + 0x10))
#define I21SCLL        (*(reg32_t *)(I2C1_BASE_ADDR + 0x14))
#define I21CONCLR      (*(reg32_t *)(I2C1_BASE_ADDR + 0x18))

/* I2C Interface 2 */
#define I2C2_BASE_ADDR 0xE0080000
#define I22CONSET      (*(reg32_t *)(I2C2_BASE_ADDR + 0x00))
#define I22STAT        (*(reg32_t *)(I2C2_BASE_ADDR + 0x04))
#define I22DAT         (*(reg32_t *)(I2C2_BASE_ADDR + 0x08))
#define I22ADR         (*(reg32_t *)(I2C2_BASE_ADDR + 0x0C))
#define I22SCLH        (*(reg32_t *)(I2C2_BASE_ADDR + 0x10))
#define I22SCLL        (*(reg32_t *)(I2C2_BASE_ADDR + 0x14))
#define I22CONCLR      (*(reg32_t *)(I2C2_BASE_ADDR + 0x18))

/* I2C offesets */
#define I2C_CONSET_OFF      0x00
#define I2C_STAT_OFF        0x04
#define I2C_DAT_OFF         0x08
#define I2C_ADR_OFF         0x0C
#define I2C_SCLH_OFF        0x10
#define I2C_SCLL_OFF        0x14
#define I2C_CONCLR_OFF      0x18

/* I2C register definition Clear */
#define I2CON_I2ENC                                        6 // I2C interface Disable bit
#define I2CON_STAC                                         5 // START flag Clear bit
#define I2CON_SIC                                          3 // I2C interrupt Clear bit
#define I2CON_AAC                                          2 // Assert acknowledge Clear bit

/* I2C register definition Set */
#define I2CON_I2EN                                         6 // I2C interface enable
#define I2CON_STA                                          5 // START flag Clear bit
#define I2CON_STO                                          4 // STOP flag Clear bit
#define I2CON_SI                                           3 // I2C interrupt Clear bit
#define I2CON_AA                                           2 // Assert acknowledge Clear bit

/* I2C Status codes */
#define I2C_STAT_ERROR                                  0x00
#define I2C_STAT_UNKNOW                                 0xF8
#define I2C_STAT_SEND                                   0x08
#define I2C_STAT_RESEND                                 0x10
#define I2C_STAT_SLAW_ACK                               0x18
#define I2C_STAT_SLAW_NACK                              0x20
#define I2C_STAT_SLAR_ACK                               0x40
#define I2C_STAT_SLAR_NACK                              0x48
#define I2C_STAT_DATA_ACK                               0x28
#define I2C_STAT_DATA_NACK                              0x30
#define I2C_STAT_RDATA_ACK                              0x50
#define I2C_STAT_RDATA_NACK                             0x58
#define I2C_STAT_ARB_LOST                               0x38

#define I2C0_PCLK_MASK                                0xC000
#define I2C0_PCLK_DIV8                                0xC000
#define I2C0_PCLK_DIV4                                0x4000
#define I2C1_PCLK_MASK                                0x00C0
#define I2C1_PCLK_DIV8                                0x00C0
#define I2C1_PCLK_DIV4                                0x0040
#define I2C2_PCLK_MASK                              0x300000
#define I2C2_PCLK_DIV8                              0x300000
#define I2C2_PCLK_DIV4                              0x100000

/* I2C pins defines */
#define I2C0_PINSEL_MASK                           0x3C00000
#define I2C0_PINSEL                                0x1400000
#define I2C1_PINSEL_MASK                           0x000000F
#define I2C1_PINSEL                                0x000000F
#define I2C2_PINSEL_MASK                           0x0F00000
#define I2C2_PINSEL                                0x0A00000

/* SPI0 (Serial Peripheral Interface 0) */
#define SPI0_BASE_ADDR		0xE0020000
#define S0SPCR         (*(reg32_t *)(SPI0_BASE_ADDR + 0x00))
#define S0SPSR         (*(reg32_t *)(SPI0_BASE_ADDR + 0x04))
#define S0SPDR         (*(reg32_t *)(SPI0_BASE_ADDR + 0x08))
#define S0SPCCR        (*(reg32_t *)(SPI0_BASE_ADDR + 0x0C))
#define S0SPINT        (*(reg32_t *)(SPI0_BASE_ADDR + 0x1C))

/* SSP0 Controller */
#define SSP0_BASE_ADDR		0xE0068000
#define SSP0CR0        (*(reg32_t *)(SSP0_BASE_ADDR + 0x00))
#define SSP0CR1        (*(reg32_t *)(SSP0_BASE_ADDR + 0x04))
#define SSP0DR         (*(reg32_t *)(SSP0_BASE_ADDR + 0x08))
#define SSP0SR         (*(reg32_t *)(SSP0_BASE_ADDR + 0x0C))
#define SSP0CPSR       (*(reg32_t *)(SSP0_BASE_ADDR + 0x10))
#define SSP0IMSC       (*(reg32_t *)(SSP0_BASE_ADDR + 0x14))
#define SSP0RIS        (*(reg32_t *)(SSP0_BASE_ADDR + 0x18))
#define SSP0MIS        (*(reg32_t *)(SSP0_BASE_ADDR + 0x1C))
#define SSP0ICR        (*(reg32_t *)(SSP0_BASE_ADDR + 0x20))
#define SSP0DMACR      (*(reg32_t *)(SSP0_BASE_ADDR + 0x24))

/* SSP1 Controller */
#define SSP1_BASE_ADDR		0xE0030000
#define SSP1CR0        (*(reg32_t *)(SSP1_BASE_ADDR + 0x00))
#define SSP1CR1        (*(reg32_t *)(SSP1_BASE_ADDR + 0x04))
#define SSP1DR         (*(reg32_t *)(SSP1_BASE_ADDR + 0x08))
#define SSP1SR         (*(reg32_t *)(SSP1_BASE_ADDR + 0x0C))
#define SSP1CPSR       (*(reg32_t *)(SSP1_BASE_ADDR + 0x10))
#define SSP1IMSC       (*(reg32_t *)(SSP1_BASE_ADDR + 0x14))
#define SSP1RIS        (*(reg32_t *)(SSP1_BASE_ADDR + 0x18))
#define SSP1MIS        (*(reg32_t *)(SSP1_BASE_ADDR + 0x1C))
#define SSP1ICR        (*(reg32_t *)(SSP1_BASE_ADDR + 0x20))
#define SSP1DMACR      (*(reg32_t *)(SSP1_BASE_ADDR + 0x24))


/* Real Time Clock */
#define RTC_BASE_ADDR		0xE0024000
#define RTC_ILR         (*(reg32_t *)(RTC_BASE_ADDR + 0x00))
#define RTC_CTC         (*(reg32_t *)(RTC_BASE_ADDR + 0x04))
#define RTC_CCR         (*(reg32_t *)(RTC_BASE_ADDR + 0x08))
#define RTC_CIIR        (*(reg32_t *)(RTC_BASE_ADDR + 0x0C))
#define RTC_AMR         (*(reg32_t *)(RTC_BASE_ADDR + 0x10))
#define RTC_CTIME0      (*(reg32_t *)(RTC_BASE_ADDR + 0x14))
#define RTC_CTIME1      (*(reg32_t *)(RTC_BASE_ADDR + 0x18))
#define RTC_CTIME2      (*(reg32_t *)(RTC_BASE_ADDR + 0x1C))
#define RTC_SEC         (*(reg32_t *)(RTC_BASE_ADDR + 0x20))
#define RTC_MIN         (*(reg32_t *)(RTC_BASE_ADDR + 0x24))
#define RTC_HOUR        (*(reg32_t *)(RTC_BASE_ADDR + 0x28))
#define RTC_DOM         (*(reg32_t *)(RTC_BASE_ADDR + 0x2C))
#define RTC_DOW         (*(reg32_t *)(RTC_BASE_ADDR + 0x30))
#define RTC_DOY         (*(reg32_t *)(RTC_BASE_ADDR + 0x34))
#define RTC_MONTH       (*(reg32_t *)(RTC_BASE_ADDR + 0x38))
#define RTC_YEAR        (*(reg32_t *)(RTC_BASE_ADDR + 0x3C))
#define RTC_CISS        (*(reg32_t *)(RTC_BASE_ADDR + 0x40))
#define RTC_ALSEC       (*(reg32_t *)(RTC_BASE_ADDR + 0x60))
#define RTC_ALMIN       (*(reg32_t *)(RTC_BASE_ADDR + 0x64))
#define RTC_ALHOUR      (*(reg32_t *)(RTC_BASE_ADDR + 0x68))
#define RTC_ALDOM       (*(reg32_t *)(RTC_BASE_ADDR + 0x6C))
#define RTC_ALDOW       (*(reg32_t *)(RTC_BASE_ADDR + 0x70))
#define RTC_ALDOY       (*(reg32_t *)(RTC_BASE_ADDR + 0x74))
#define RTC_ALMON       (*(reg32_t *)(RTC_BASE_ADDR + 0x78))
#define RTC_ALYEAR      (*(reg32_t *)(RTC_BASE_ADDR + 0x7C))
#define RTC_PREINT      (*(reg32_t *)(RTC_BASE_ADDR + 0x80))
#define RTC_PREFRAC     (*(reg32_t *)(RTC_BASE_ADDR + 0x84))


/* A/D Converter 0 (AD0) */
#define AD0_BASE_ADDR		0xE0034000
#define AD0CR          (*(reg32_t *)(AD0_BASE_ADDR + 0x00))
#define AD0GDR         (*(reg32_t *)(AD0_BASE_ADDR + 0x04))
#define AD0INTEN       (*(reg32_t *)(AD0_BASE_ADDR + 0x0C))
#define AD0DR0         (*(reg32_t *)(AD0_BASE_ADDR + 0x10))
#define AD0DR1         (*(reg32_t *)(AD0_BASE_ADDR + 0x14))
#define AD0DR2         (*(reg32_t *)(AD0_BASE_ADDR + 0x18))
#define AD0DR3         (*(reg32_t *)(AD0_BASE_ADDR + 0x1C))
#define AD0DR4         (*(reg32_t *)(AD0_BASE_ADDR + 0x20))
#define AD0DR5         (*(reg32_t *)(AD0_BASE_ADDR + 0x24))
#define AD0DR6         (*(reg32_t *)(AD0_BASE_ADDR + 0x28))
#define AD0DR7         (*(reg32_t *)(AD0_BASE_ADDR + 0x2C))
#define AD0STAT        (*(reg32_t *)(AD0_BASE_ADDR + 0x30))


/* D/A Converter */
#define DAC_BASE_ADDR		0xE006C000
#define DACR           (*(reg32_t *)(DAC_BASE_ADDR + 0x00))


/* Watchdog */
#define WDG_BASE_ADDR		0xE0000000
#define WDMOD          (*(reg32_t *)(WDG_BASE_ADDR + 0x00))
#define WDTC           (*(reg32_t *)(WDG_BASE_ADDR + 0x04))
#define WDFEED         (*(reg32_t *)(WDG_BASE_ADDR + 0x08))
#define WDTV           (*(reg32_t *)(WDG_BASE_ADDR + 0x0C))
#define WDCLKSEL       (*(reg32_t *)(WDG_BASE_ADDR + 0x10))

/* CAN CONTROLLERS AND ACCEPTANCE FILTER */
#define CAN_ACCEPT_BASE_ADDR		0xE003C000
#define CAN_AFMR		(*(reg32_t *)(CAN_ACCEPT_BASE_ADDR + 0x00))
#define CAN_SFF_SA 		(*(reg32_t *)(CAN_ACCEPT_BASE_ADDR + 0x04))
#define CAN_SFF_GRP_SA 	(*(reg32_t *)(CAN_ACCEPT_BASE_ADDR + 0x08))
#define CAN_EFF_SA 		(*(reg32_t *)(CAN_ACCEPT_BASE_ADDR + 0x0C))
#define CAN_EFF_GRP_SA 	(*(reg32_t *)(CAN_ACCEPT_BASE_ADDR + 0x10))
#define CAN_EOT 		(*(reg32_t *)(CAN_ACCEPT_BASE_ADDR + 0x14))
#define CAN_LUT_ERR_ADR (*(reg32_t *)(CAN_ACCEPT_BASE_ADDR + 0x18))
#define CAN_LUT_ERR 	(*(reg32_t *)(CAN_ACCEPT_BASE_ADDR + 0x1C))

#define CAN_CENTRAL_BASE_ADDR		0xE0040000
#define CAN_TX_SR 	(*(reg32_t *)(CAN_CENTRAL_BASE_ADDR + 0x00))
#define CAN_RX_SR 	(*(reg32_t *)(CAN_CENTRAL_BASE_ADDR + 0x04))
#define CAN_MSR 	(*(reg32_t *)(CAN_CENTRAL_BASE_ADDR + 0x08))

#define CAN1_BASE_ADDR		0xE0044000
#define CAN1MOD 	(*(reg32_t *)(CAN1_BASE_ADDR + 0x00))
#define CAN1CMR 	(*(reg32_t *)(CAN1_BASE_ADDR + 0x04))
#define CAN1GSR 	(*(reg32_t *)(CAN1_BASE_ADDR + 0x08))
#define CAN1ICR 	(*(reg32_t *)(CAN1_BASE_ADDR + 0x0C))
#define CAN1IER 	(*(reg32_t *)(CAN1_BASE_ADDR + 0x10))
#define CAN1BTR 	(*(reg32_t *)(CAN1_BASE_ADDR + 0x14))
#define CAN1EWL 	(*(reg32_t *)(CAN1_BASE_ADDR + 0x18))
#define CAN1SR 		(*(reg32_t *)(CAN1_BASE_ADDR + 0x1C))
#define CAN1RFS 	(*(reg32_t *)(CAN1_BASE_ADDR + 0x20))
#define CAN1RID 	(*(reg32_t *)(CAN1_BASE_ADDR + 0x24))
#define CAN1RDA 	(*(reg32_t *)(CAN1_BASE_ADDR + 0x28))
#define CAN1RDB 	(*(reg32_t *)(CAN1_BASE_ADDR + 0x2C))

#define CAN1TFI1 	(*(reg32_t *)(CAN1_BASE_ADDR + 0x30))
#define CAN1TID1 	(*(reg32_t *)(CAN1_BASE_ADDR + 0x34))
#define CAN1TDA1 	(*(reg32_t *)(CAN1_BASE_ADDR + 0x38))
#define CAN1TDB1 	(*(reg32_t *)(CAN1_BASE_ADDR + 0x3C))
#define CAN1TFI2 	(*(reg32_t *)(CAN1_BASE_ADDR + 0x40))
#define CAN1TID2 	(*(reg32_t *)(CAN1_BASE_ADDR + 0x44))
#define CAN1TDA2 	(*(reg32_t *)(CAN1_BASE_ADDR + 0x48))
#define CAN1TDB2 	(*(reg32_t *)(CAN1_BASE_ADDR + 0x4C))
#define CAN1TFI3 	(*(reg32_t *)(CAN1_BASE_ADDR + 0x50))
#define CAN1TID3 	(*(reg32_t *)(CAN1_BASE_ADDR + 0x54))
#define CAN1TDA3 	(*(reg32_t *)(CAN1_BASE_ADDR + 0x58))
#define CAN1TDB3 	(*(reg32_t *)(CAN1_BASE_ADDR + 0x5C))

#define CAN2_BASE_ADDR		0xE0048000
#define CAN2MOD 	(*(reg32_t *)(CAN2_BASE_ADDR + 0x00))
#define CAN2CMR 	(*(reg32_t *)(CAN2_BASE_ADDR + 0x04))
#define CAN2GSR 	(*(reg32_t *)(CAN2_BASE_ADDR + 0x08))
#define CAN2ICR 	(*(reg32_t *)(CAN2_BASE_ADDR + 0x0C))
#define CAN2IER 	(*(reg32_t *)(CAN2_BASE_ADDR + 0x10))
#define CAN2BTR 	(*(reg32_t *)(CAN2_BASE_ADDR + 0x14))
#define CAN2EWL 	(*(reg32_t *)(CAN2_BASE_ADDR + 0x18))
#define CAN2SR 		(*(reg32_t *)(CAN2_BASE_ADDR + 0x1C))
#define CAN2RFS 	(*(reg32_t *)(CAN2_BASE_ADDR + 0x20))
#define CAN2RID 	(*(reg32_t *)(CAN2_BASE_ADDR + 0x24))
#define CAN2RDA 	(*(reg32_t *)(CAN2_BASE_ADDR + 0x28))
#define CAN2RDB 	(*(reg32_t *)(CAN2_BASE_ADDR + 0x2C))

#define CAN2TFI1 	(*(reg32_t *)(CAN2_BASE_ADDR + 0x30))
#define CAN2TID1 	(*(reg32_t *)(CAN2_BASE_ADDR + 0x34))
#define CAN2TDA1 	(*(reg32_t *)(CAN2_BASE_ADDR + 0x38))
#define CAN2TDB1 	(*(reg32_t *)(CAN2_BASE_ADDR + 0x3C))
#define CAN2TFI2 	(*(reg32_t *)(CAN2_BASE_ADDR + 0x40))
#define CAN2TID2 	(*(reg32_t *)(CAN2_BASE_ADDR + 0x44))
#define CAN2TDA2 	(*(reg32_t *)(CAN2_BASE_ADDR + 0x48))
#define CAN2TDB2 	(*(reg32_t *)(CAN2_BASE_ADDR + 0x4C))
#define CAN2TFI3 	(*(reg32_t *)(CAN2_BASE_ADDR + 0x50))
#define CAN2TID3 	(*(reg32_t *)(CAN2_BASE_ADDR + 0x54))
#define CAN2TDA3 	(*(reg32_t *)(CAN2_BASE_ADDR + 0x58))
#define CAN2TDB3 	(*(reg32_t *)(CAN2_BASE_ADDR + 0x5C))


/* MultiMedia Card Interface(MCI) Controller */
#define MCI_BASE_ADDR		0xE008C000
#define MCI_POWER      (*(reg32_t *)(MCI_BASE_ADDR + 0x00))
#define MCI_CLOCK      (*(reg32_t *)(MCI_BASE_ADDR + 0x04))
#define MCI_ARGUMENT   (*(reg32_t *)(MCI_BASE_ADDR + 0x08))
#define MCI_COMMAND    (*(reg32_t *)(MCI_BASE_ADDR + 0x0C))
#define MCI_RESP_CMD   (*(reg32_t *)(MCI_BASE_ADDR + 0x10))
#define MCI_RESP0      (*(reg32_t *)(MCI_BASE_ADDR + 0x14))
#define MCI_RESP1      (*(reg32_t *)(MCI_BASE_ADDR + 0x18))
#define MCI_RESP2      (*(reg32_t *)(MCI_BASE_ADDR + 0x1C))
#define MCI_RESP3      (*(reg32_t *)(MCI_BASE_ADDR + 0x20))
#define MCI_DATA_TMR   (*(reg32_t *)(MCI_BASE_ADDR + 0x24))
#define MCI_DATA_LEN   (*(reg32_t *)(MCI_BASE_ADDR + 0x28))
#define MCI_DATA_CTRL  (*(reg32_t *)(MCI_BASE_ADDR + 0x2C))
#define MCI_DATA_CNT   (*(reg32_t *)(MCI_BASE_ADDR + 0x30))
#define MCI_STATUS     (*(reg32_t *)(MCI_BASE_ADDR + 0x34))
#define MCI_CLEAR      (*(reg32_t *)(MCI_BASE_ADDR + 0x38))
#define MCI_MASK0      (*(reg32_t *)(MCI_BASE_ADDR + 0x3C))
#define MCI_MASK1      (*(reg32_t *)(MCI_BASE_ADDR + 0x40))
#define MCI_FIFO_CNT   (*(reg32_t *)(MCI_BASE_ADDR + 0x48))
#define MCI_FIFO       (*(reg32_t *)(MCI_BASE_ADDR + 0x80))


/* I2S Interface Controller (I2S) */
#define I2S_BASE_ADDR		0xE0088000
#define I2S_DAO        (*(reg32_t *)(I2S_BASE_ADDR + 0x00))
#define I2S_DAI        (*(reg32_t *)(I2S_BASE_ADDR + 0x04))
#define I2S_TX_FIFO    (*(reg32_t *)(I2S_BASE_ADDR + 0x08))
#define I2S_RX_FIFO    (*(reg32_t *)(I2S_BASE_ADDR + 0x0C))
#define I2S_STATE      (*(reg32_t *)(I2S_BASE_ADDR + 0x10))
#define I2S_DMA1       (*(reg32_t *)(I2S_BASE_ADDR + 0x14))
#define I2S_DMA2       (*(reg32_t *)(I2S_BASE_ADDR + 0x18))
#define I2S_IRQ        (*(reg32_t *)(I2S_BASE_ADDR + 0x1C))
#define I2S_TXRATE     (*(reg32_t *)(I2S_BASE_ADDR + 0x20))
#define I2S_RXRATE     (*(reg32_t *)(I2S_BASE_ADDR + 0x24))


/* General-purpose DMA Controller */
#define DMA_BASE_ADDR		0xFFE04000
#define GPDMA_INT_STAT         (*(reg32_t *)(DMA_BASE_ADDR + 0x000))
#define GPDMA_INT_TCSTAT       (*(reg32_t *)(DMA_BASE_ADDR + 0x004))
#define GPDMA_INT_TCCLR        (*(reg32_t *)(DMA_BASE_ADDR + 0x008))
#define GPDMA_INT_ERR_STAT     (*(reg32_t *)(DMA_BASE_ADDR + 0x00C))
#define GPDMA_INT_ERR_CLR      (*(reg32_t *)(DMA_BASE_ADDR + 0x010))
#define GPDMA_RAW_INT_TCSTAT   (*(reg32_t *)(DMA_BASE_ADDR + 0x014))
#define GPDMA_RAW_INT_ERR_STAT (*(reg32_t *)(DMA_BASE_ADDR + 0x018))
#define GPDMA_ENABLED_CHNS     (*(reg32_t *)(DMA_BASE_ADDR + 0x01C))
#define GPDMA_SOFT_BREQ        (*(reg32_t *)(DMA_BASE_ADDR + 0x020))
#define GPDMA_SOFT_SREQ        (*(reg32_t *)(DMA_BASE_ADDR + 0x024))
#define GPDMA_SOFT_LBREQ       (*(reg32_t *)(DMA_BASE_ADDR + 0x028))
#define GPDMA_SOFT_LSREQ       (*(reg32_t *)(DMA_BASE_ADDR + 0x02C))
#define GPDMA_CONFIG           (*(reg32_t *)(DMA_BASE_ADDR + 0x030))
#define GPDMA_SYNC             (*(reg32_t *)(DMA_BASE_ADDR + 0x034))

/* DMA channel 0 registers */
#define GPDMA_CH0_SRC      (*(reg32_t *)(DMA_BASE_ADDR + 0x100))
#define GPDMA_CH0_DEST     (*(reg32_t *)(DMA_BASE_ADDR + 0x104))
#define GPDMA_CH0_LLI      (*(reg32_t *)(DMA_BASE_ADDR + 0x108))
#define GPDMA_CH0_CTRL     (*(reg32_t *)(DMA_BASE_ADDR + 0x10C))
#define GPDMA_CH0_CFG      (*(reg32_t *)(DMA_BASE_ADDR + 0x110))

/* DMA channel 1 registers */
#define GPDMA_CH1_SRC      (*(reg32_t *)(DMA_BASE_ADDR + 0x120))
#define GPDMA_CH1_DEST     (*(reg32_t *)(DMA_BASE_ADDR + 0x124))
#define GPDMA_CH1_LLI      (*(reg32_t *)(DMA_BASE_ADDR + 0x128))
#define GPDMA_CH1_CTRL     (*(reg32_t *)(DMA_BASE_ADDR + 0x12C))
#define GPDMA_CH1_CFG      (*(reg32_t *)(DMA_BASE_ADDR + 0x130))


/* USB Controller */
#define USB_INT_BASE_ADDR	0xE01FC1C0
#define USB_BASE_ADDR		0xFFE0C200		/* USB Base Address */

#define USB_INT_STAT    (*(reg32_t *)(USB_INT_BASE_ADDR + 0x00))

/* USB Device Interrupt Registers */
#define DEV_INT_STAT    (*(reg32_t *)(USB_BASE_ADDR + 0x00))
#define DEV_INT_EN      (*(reg32_t *)(USB_BASE_ADDR + 0x04))
#define DEV_INT_CLR     (*(reg32_t *)(USB_BASE_ADDR + 0x08))
#define DEV_INT_SET     (*(reg32_t *)(USB_BASE_ADDR + 0x0C))
#define DEV_INT_PRIO    (*(reg32_t *)(USB_BASE_ADDR + 0x2C))

/* USB Device Endpoint Interrupt Registers */
#define EP_INT_STAT     (*(reg32_t *)(USB_BASE_ADDR + 0x30))
#define EP_INT_EN       (*(reg32_t *)(USB_BASE_ADDR + 0x34))
#define EP_INT_CLR      (*(reg32_t *)(USB_BASE_ADDR + 0x38))
#define EP_INT_SET      (*(reg32_t *)(USB_BASE_ADDR + 0x3C))
#define EP_INT_PRIO     (*(reg32_t *)(USB_BASE_ADDR + 0x40))

/* USB Device Endpoint Realization Registers */
#define REALIZE_EP      (*(reg32_t *)(USB_BASE_ADDR + 0x44))
#define EP_INDEX        (*(reg32_t *)(USB_BASE_ADDR + 0x48))
#define MAXPACKET_SIZE  (*(reg32_t *)(USB_BASE_ADDR + 0x4C))

/* USB Device Command Reagisters */
#define CMD_CODE        (*(reg32_t *)(USB_BASE_ADDR + 0x10))
#define CMD_DATA        (*(reg32_t *)(USB_BASE_ADDR + 0x14))

/* USB Device Data Transfer Registers */
#define RX_DATA         (*(reg32_t *)(USB_BASE_ADDR + 0x18))
#define TX_DATA         (*(reg32_t *)(USB_BASE_ADDR + 0x1C))
#define RX_PLENGTH      (*(reg32_t *)(USB_BASE_ADDR + 0x20))
#define TX_PLENGTH      (*(reg32_t *)(USB_BASE_ADDR + 0x24))
#define USB_CTRL        (*(reg32_t *)(USB_BASE_ADDR + 0x28))

/* USB Device DMA Registers */
#define DMA_REQ_STAT        (*(reg32_t *)(USB_BASE_ADDR + 0x50))
#define DMA_REQ_CLR         (*(reg32_t *)(USB_BASE_ADDR + 0x54))
#define DMA_REQ_SET         (*(reg32_t *)(USB_BASE_ADDR + 0x58))
#define UDCA_HEAD           (*(reg32_t *)(USB_BASE_ADDR + 0x80))
#define EP_DMA_STAT         (*(reg32_t *)(USB_BASE_ADDR + 0x84))
#define EP_DMA_EN           (*(reg32_t *)(USB_BASE_ADDR + 0x88))
#define EP_DMA_DIS          (*(reg32_t *)(USB_BASE_ADDR + 0x8C))
#define DMA_INT_STAT        (*(reg32_t *)(USB_BASE_ADDR + 0x90))
#define DMA_INT_EN          (*(reg32_t *)(USB_BASE_ADDR + 0x94))
#define EOT_INT_STAT        (*(reg32_t *)(USB_BASE_ADDR + 0xA0))
#define EOT_INT_CLR         (*(reg32_t *)(USB_BASE_ADDR + 0xA4))
#define EOT_INT_SET         (*(reg32_t *)(USB_BASE_ADDR + 0xA8))
#define NDD_REQ_INT_STAT    (*(reg32_t *)(USB_BASE_ADDR + 0xAC))
#define NDD_REQ_INT_CLR     (*(reg32_t *)(USB_BASE_ADDR + 0xB0))
#define NDD_REQ_INT_SET     (*(reg32_t *)(USB_BASE_ADDR + 0xB4))
#define SYS_ERR_INT_STAT    (*(reg32_t *)(USB_BASE_ADDR + 0xB8))
#define SYS_ERR_INT_CLR     (*(reg32_t *)(USB_BASE_ADDR + 0xBC))
#define SYS_ERR_INT_SET     (*(reg32_t *)(USB_BASE_ADDR + 0xC0))

/* USB Host and OTG registers are for LPC24xx only */
/* USB Host Controller */
#define USBHC_BASE_ADDR		0xFFE0C000
#define HC_REVISION         (*(reg32_t *)(USBHC_BASE_ADDR + 0x00))
#define HC_CONTROL          (*(reg32_t *)(USBHC_BASE_ADDR + 0x04))
#define HC_CMD_STAT         (*(reg32_t *)(USBHC_BASE_ADDR + 0x08))
#define HC_INT_STAT         (*(reg32_t *)(USBHC_BASE_ADDR + 0x0C))
#define HC_INT_EN           (*(reg32_t *)(USBHC_BASE_ADDR + 0x10))
#define HC_INT_DIS          (*(reg32_t *)(USBHC_BASE_ADDR + 0x14))
#define HC_HCCA             (*(reg32_t *)(USBHC_BASE_ADDR + 0x18))
#define HC_PERIOD_CUR_ED    (*(reg32_t *)(USBHC_BASE_ADDR + 0x1C))
#define HC_CTRL_HEAD_ED     (*(reg32_t *)(USBHC_BASE_ADDR + 0x20))
#define HC_CTRL_CUR_ED      (*(reg32_t *)(USBHC_BASE_ADDR + 0x24))
#define HC_BULK_HEAD_ED     (*(reg32_t *)(USBHC_BASE_ADDR + 0x28))
#define HC_BULK_CUR_ED      (*(reg32_t *)(USBHC_BASE_ADDR + 0x2C))
#define HC_DONE_HEAD        (*(reg32_t *)(USBHC_BASE_ADDR + 0x30))
#define HC_FM_INTERVAL      (*(reg32_t *)(USBHC_BASE_ADDR + 0x34))
#define HC_FM_REMAINING     (*(reg32_t *)(USBHC_BASE_ADDR + 0x38))
#define HC_FM_NUMBER        (*(reg32_t *)(USBHC_BASE_ADDR + 0x3C))
#define HC_PERIOD_START     (*(reg32_t *)(USBHC_BASE_ADDR + 0x40))
#define HC_LS_THRHLD        (*(reg32_t *)(USBHC_BASE_ADDR + 0x44))
#define HC_RH_DESCA         (*(reg32_t *)(USBHC_BASE_ADDR + 0x48))
#define HC_RH_DESCB         (*(reg32_t *)(USBHC_BASE_ADDR + 0x4C))
#define HC_RH_STAT          (*(reg32_t *)(USBHC_BASE_ADDR + 0x50))
#define HC_RH_PORT_STAT1    (*(reg32_t *)(USBHC_BASE_ADDR + 0x54))
#define HC_RH_PORT_STAT2    (*(reg32_t *)(USBHC_BASE_ADDR + 0x58))

/* USB OTG Controller */
#define USBOTG_BASE_ADDR	0xFFE0C100
#define OTG_INT_STAT        (*(reg32_t *)(USBOTG_BASE_ADDR + 0x00))
#define OTG_INT_EN          (*(reg32_t *)(USBOTG_BASE_ADDR + 0x04))
#define OTG_INT_SET         (*(reg32_t *)(USBOTG_BASE_ADDR + 0x08))
#define OTG_INT_CLR         (*(reg32_t *)(USBOTG_BASE_ADDR + 0x0C))
/* On LPC23xx, the name is USBPortSel, on LPC24xx, the name is OTG_STAT_CTRL */
#define OTG_STAT_CTRL       (*(reg32_t *)(USBOTG_BASE_ADDR + 0x10))
#define OTG_TIMER           (*(reg32_t *)(USBOTG_BASE_ADDR + 0x14))

#define USBOTG_I2C_BASE_ADDR	0xFFE0C300
#define OTG_I2C_RX          (*(reg32_t *)(USBOTG_I2C_BASE_ADDR + 0x00))
#define OTG_I2C_TX          (*(reg32_t *)(USBOTG_I2C_BASE_ADDR + 0x00))
#define OTG_I2C_STS         (*(reg32_t *)(USBOTG_I2C_BASE_ADDR + 0x04))
#define OTG_I2C_CTL         (*(reg32_t *)(USBOTG_I2C_BASE_ADDR + 0x08))
#define OTG_I2C_CLKHI       (*(reg32_t *)(USBOTG_I2C_BASE_ADDR + 0x0C))
#define OTG_I2C_CLKLO       (*(reg32_t *)(USBOTG_I2C_BASE_ADDR + 0x10))

/* On LPC23xx, the names are USBClkCtrl and USBClkSt; on LPC24xx, the names are
OTG_CLK_CTRL and OTG_CLK_STAT respectively. */
#define USBOTG_CLK_BASE_ADDR	0xFFE0CFF0
#define OTG_CLK_CTRL        (*(reg32_t *)(USBOTG_CLK_BASE_ADDR + 0x04))
#define OTG_CLK_STAT        (*(reg32_t *)(USBOTG_CLK_BASE_ADDR + 0x08))

/* Note: below three register name convention is for LPC23xx USB device only, match
with the spec. update in USB Device Section. */
#define USBPortSel          (*(reg32_t *)(USBOTG_BASE_ADDR + 0x10))
#define USBClkCtrl          (*(reg32_t *)(USBOTG_CLK_BASE_ADDR + 0x04))
#define USBClkSt            (*(reg32_t *)(USBOTG_CLK_BASE_ADDR + 0x08))

/* Ethernet MAC (32 bit data bus) -- all registers are RW unless indicated in parentheses */
#define MAC_BASE_ADDR		0xFFE00000 /* AHB Peripheral # 0 */
#define MAC_MAC1            (*(reg32_t *)(MAC_BASE_ADDR + 0x000)) /* MAC config reg 1 */
#define MAC_MAC2            (*(reg32_t *)(MAC_BASE_ADDR + 0x004)) /* MAC config reg 2 */
#define MAC_IPGT            (*(reg32_t *)(MAC_BASE_ADDR + 0x008)) /* b2b InterPacketGap reg */
#define MAC_IPGR            (*(reg32_t *)(MAC_BASE_ADDR + 0x00C)) /* non b2b InterPacketGap reg */
#define MAC_CLRT            (*(reg32_t *)(MAC_BASE_ADDR + 0x010)) /* CoLlision window/ReTry reg */
#define MAC_MAXF            (*(reg32_t *)(MAC_BASE_ADDR + 0x014)) /* MAXimum Frame reg */
#define MAC_SUPP            (*(reg32_t *)(MAC_BASE_ADDR + 0x018)) /* PHY SUPPort reg */
#define MAC_TEST            (*(reg32_t *)(MAC_BASE_ADDR + 0x01C)) /* TEST reg */
#define MAC_MCFG            (*(reg32_t *)(MAC_BASE_ADDR + 0x020)) /* MII Mgmt ConFiG reg */
#define MAC_MCMD            (*(reg32_t *)(MAC_BASE_ADDR + 0x024)) /* MII Mgmt CoMmanD reg */
#define MAC_MADR            (*(reg32_t *)(MAC_BASE_ADDR + 0x028)) /* MII Mgmt ADdRess reg */
#define MAC_MWTD            (*(reg32_t *)(MAC_BASE_ADDR + 0x02C)) /* MII Mgmt WriTe Data reg (WO) */
#define MAC_MRDD            (*(reg32_t *)(MAC_BASE_ADDR + 0x030)) /* MII Mgmt ReaD Data reg (RO) */
#define MAC_MIND            (*(reg32_t *)(MAC_BASE_ADDR + 0x034)) /* MII Mgmt INDicators reg (RO) */

#define MAC_SA0             (*(reg32_t *)(MAC_BASE_ADDR + 0x040)) /* Station Address 0 reg */
#define MAC_SA1             (*(reg32_t *)(MAC_BASE_ADDR + 0x044)) /* Station Address 1 reg */
#define MAC_SA2             (*(reg32_t *)(MAC_BASE_ADDR + 0x048)) /* Station Address 2 reg */

#define MAC_COMMAND         (*(reg32_t *)(MAC_BASE_ADDR + 0x100)) /* Command reg */
#define MAC_STATUS          (*(reg32_t *)(MAC_BASE_ADDR + 0x104)) /* Status reg (RO) */
#define MAC_RXDESCRIPTOR    (*(reg32_t *)(MAC_BASE_ADDR + 0x108)) /* Rx descriptor base address reg */
#define MAC_RXSTATUS        (*(reg32_t *)(MAC_BASE_ADDR + 0x10C)) /* Rx status base address reg */
#define MAC_RXDESCRIPTORNUM (*(reg32_t *)(MAC_BASE_ADDR + 0x110)) /* Rx number of descriptors reg */
#define MAC_RXPRODUCEINDEX  (*(reg32_t *)(MAC_BASE_ADDR + 0x114)) /* Rx produce index reg (RO) */
#define MAC_RXCONSUMEINDEX  (*(reg32_t *)(MAC_BASE_ADDR + 0x118)) /* Rx consume index reg */
#define MAC_TXDESCRIPTOR    (*(reg32_t *)(MAC_BASE_ADDR + 0x11C)) /* Tx descriptor base address reg */
#define MAC_TXSTATUS        (*(reg32_t *)(MAC_BASE_ADDR + 0x120)) /* Tx status base address reg */
#define MAC_TXDESCRIPTORNUM (*(reg32_t *)(MAC_BASE_ADDR + 0x124)) /* Tx number of descriptors reg */
#define MAC_TXPRODUCEINDEX  (*(reg32_t *)(MAC_BASE_ADDR + 0x128)) /* Tx produce index reg */
#define MAC_TXCONSUMEINDEX  (*(reg32_t *)(MAC_BASE_ADDR + 0x12C)) /* Tx consume index reg (RO) */

#define MAC_TSV0            (*(reg32_t *)(MAC_BASE_ADDR + 0x158)) /* Tx status vector 0 reg (RO) */
#define MAC_TSV1            (*(reg32_t *)(MAC_BASE_ADDR + 0x15C)) /* Tx status vector 1 reg (RO) */
#define MAC_RSV             (*(reg32_t *)(MAC_BASE_ADDR + 0x160)) /* Rx status vector reg (RO) */

#define MAC_FLOWCONTROLCNT  (*(reg32_t *)(MAC_BASE_ADDR + 0x170)) /* Flow control counter reg */
#define MAC_FLOWCONTROLSTS  (*(reg32_t *)(MAC_BASE_ADDR + 0x174)) /* Flow control status reg */

#define MAC_RXFILTERCTRL    (*(reg32_t *)(MAC_BASE_ADDR + 0x200)) /* Rx filter ctrl reg */
#define MAC_RXFILTERWOLSTS  (*(reg32_t *)(MAC_BASE_ADDR + 0x204)) /* Rx filter WoL status reg (RO) */
#define MAC_RXFILTERWOLCLR  (*(reg32_t *)(MAC_BASE_ADDR + 0x208)) /* Rx filter WoL clear reg (WO) */

#define MAC_HASHFILTERL     (*(reg32_t *)(MAC_BASE_ADDR + 0x210)) /* Hash filter LSBs reg */
#define MAC_HASHFILTERH     (*(reg32_t *)(MAC_BASE_ADDR + 0x214)) /* Hash filter MSBs reg */

#define MAC_INTSTATUS       (*(reg32_t *)(MAC_BASE_ADDR + 0xFE0)) /* Interrupt status reg (RO) */
#define MAC_INTENABLE       (*(reg32_t *)(MAC_BASE_ADDR + 0xFE4)) /* Interrupt enable reg  */
#define MAC_INTCLEAR        (*(reg32_t *)(MAC_BASE_ADDR + 0xFE8)) /* Interrupt clear reg (WO) */
#define MAC_INTSET          (*(reg32_t *)(MAC_BASE_ADDR + 0xFEC)) /* Interrupt set reg (WO) */

#define MAC_POWERDOWN       (*(reg32_t *)(MAC_BASE_ADDR + 0xFF4)) /* Power-down reg */
#define MAC_MODULEID        (*(reg32_t *)(MAC_BASE_ADDR + 0xFFC)) /* Module ID reg (RO) */


/* IRQ numbers */
#define INT_I2C0                     9
#define INT_I2C1                    19
#define INT_I2C2                    30
#define INT_UART0                    6
#define INT_UART1                    7
#define INT_UART2                 	28
#define INT_UART3                 	29

#endif /* LPC23XX_H */
