#
# $Id: Makefile 18234 2007-10-08 13:39:48Z rasky $
# Copyright 2004, 2006 Develer S.r.l. (http://www.develer.com/)
# All rights reserved.
#
# Author: Stefano Fedrigo <aleph@develer.com>
#
# $Log$
# Revision 1.2  2006/09/20 14:18:01  marco
# Use only icons neede by demo.
#
# Revision 1.1  2006/09/19 17:48:00  bernie
# Add temporary logo.
#
#

# Set to 1 for debug builds
DEBUG = 1

RASTERS = logo.raster

FLIPIMAGE = flipimage
flipimage_SRC = flipimage.cpp
flipimage_OBJ = $(flipimage_SRC:%.cpp=%.o)

.PHONY: all
all: $(RASTERS)

# Link rule
$(FLIPIMAGE): $(flipimage_OBJ)
	$(CXX) $^ $(flipimage_LIBS) $(LDFLAGS) -o $@

.PHONY: clean
clean:
	rm -f $(TRG) $(flipimage_OBJ)
	rm -f $(RASTERS)

.PHONY: rasters
rasters: $(FLIPIMAGE) $(RASTERS)

%.raster: %.png $(FLIPIMAGE)
	convert $< txt:- | ./flipimage >$@

# Include dependencies
-include *.d
