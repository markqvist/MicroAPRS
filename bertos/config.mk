#
# $Id: config.mk 4643 2010-12-20 12:10:41Z lottaviano $
# Copyright 2002,2003, 2004, 2006 Develer S.r.l. (http://www.develer.com/)
# All rights reserved.
#
# Author: Bernie Innocenti <bernie@codewiz.org>
# Based on: GCC-AVR standard Makefile part 1, Volker Oth 1/2000
#

#
# Programmer type
# see local pgm_config.mk for programmer customization.

OPTCFLAGS = -ffunction-sections -fdata-sections
#OPTCFLAGS = -funsafe-loop-optimizations

# For AVRStudio
#DEBUGCFLAGS = -gdwarf-2

# For GDB
DEBUGCFLAGS = -ggdb

#
# define some variables based on the AVR base path in $(AVR)
#
CC      = gcc
CXX     = g++
AR      = ar
AS      = $(CC) -x assembler-with-cpp
LD      = $(CC)
LDXX	= $(CXX)
OBJCOPY = objcopy
STRIP   = strip
INSTALL = cp -a
RM      = rm -f
RM_R    = rm -rf
RN      = mv
MKDIR_P = mkdir -p
SHELL   = /bin/sh
CHECKER = sparse
DOXYGEN = doxygen
AVRDUDE = avrdude
FLEXCAT = $(top_srcdir)/tools/flexcat/flexcat

# For conversion from ELF to COFF for use in debugging / simulating in AVR Studio or VMLAB.
COFFCONVERT=$(OBJCOPY) \
	--debugging \
	--change-section-address .data-0x800000 \
	--change-section-address .bss-0x800000 \
	--change-section-address .noinit-0x800000 \
	--change-section-address .eeprom-0x810000

INCDIR  = -I. -Ibertos -Ibertos/net/lwip/src/include -Ibertos/net/lwip/src/include/ipv4
LIBDIR  = lib
OBJDIR  = obj
OUTDIR  = images

# output format can be srec, ihex (avrobj is always created)
FORMAT = srec
#FORMAT = ihex

# Compiler flags for generating dependencies
DEP_FLAGS = -MMD -MP

# Compiler flags for generating source listings
LIST_FLAGS = -Wa,-anhlmsd=$(@:.o=.lst) -dp

# Linker flags for generating map files
# Only in embedded related projects generate map files
MAP_FLAGS_EMB = -Wl,-Map=$(@:%.elf=%.map),--cref
MAP_FLAGS_HOST =

# Compiler warning flags for both C and C++
WARNFLAGS = \
	-W -Wformat -Wall -Wundef -Wpointer-arith -Wcast-qual \
	-Wcast-align -Wwrite-strings -Wsign-compare \
	-Wmissing-noreturn \
	-Wextra -Wstrict-aliasing=2 \
#	-Wunsafe-loop-optimizations

# Compiler warning flags for C only
C_WARNFLAGS = \
	-Wmissing-prototypes -Wstrict-prototypes

C_COMPILER_STD = -std=gnu99

# Default C preprocessor flags (for C, C++ and cpp+as)
CPPFLAGS = $(INCDIR)

# Default C compiler flags
CFLAGS = $(OPTCFLAGS) $(DEBUGCFLAGS) $(WARNFLAGS) $(C_WARNFLAGS) \
	$(DEP_FLAGS) $(LIST_FLAGS) $(C_COMPILER_STD)

# Default C++ compiler flags
CXXFLAGS = $(OPTCFLAGS) $(DEBUGCFLAGS) $(WARNFLAGS) \
	$(DEP_FLAGS) $(LIST_FLAGS)

# Default compiler assembly flags
CPPAFLAGS = $(DEBUGCFLAGS) -MMD

# Default assembler flags
ASFLAGS	= $(DEBUGCFLAGS)

# Default linker flags
#LDFLAGS = $(MAP_FLAGS)

#bernie: does not complain for missing symbols!
LDFLAGS = -Wl,--gc-sections

# Flags for avrdude
AVRDUDEFLAGS = $(DPROG)

# additional libs
LIB = -lm

# Archiver flags
ARFLAGS = rcs
