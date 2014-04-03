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
 * Copyright (C) 2004 Develer S.r.l. (http://www.develer.com/)
 * All Rights Reserved.
 * -->
 *
 * \author Stefano Fedrigo <aleph@develer.com>
 *
 * \brief Conversion tool from image TXT format to LCD bitmap
 */

/*
 * $Log$
 * Revision 1.1  2006/09/19 17:48:00  bernie
 * Add temporary logo.
 *
 * Revision 1.3  2004/09/24 17:25:32  powersoft
 * Fix for new version of ImageMagick.
 *
 * Revision 1.2  2004/03/13 22:52:54  aleph
 * documentation fixes
 *
 * Revision 1.1  2004/01/26 15:31:17  aleph
 * Add boot images and tools for importing them
 *
 */

#include <iostream>
#include <sstream>
#include <iomanip>

extern "C" {
#include <string.h>
#include <stdio.h>
}

#define RASTER_WIDTH 82
#define RASTER_HEIGHT 64

// Minimum color level to consider a pixel on
#define COLOR_THRESHOLD 128


using namespace std;


int main(void)
{
	char buf[64];
	int n;
	unsigned char mask;
	unsigned char line[RASTER_WIDTH];

	for (int l = 0; l < RASTER_HEIGHT/8; l++)
	{
		mask = 1;
		memset(line, 0, sizeof(line));

		do
		{
			for (int x = 0; x < RASTER_WIDTH; x++)
			{
				if (!cin.getline(buf, sizeof(buf)))
					goto print_line;

				istringstream ss(buf);
				string s;
				char c;

				ss
					>> s  // skip glob coord spec
					>> c  // skip open paren
					>> n; // this is the red level

				//DEBUG
				//cout << "s='" << s << "' n='" << n << "'" << endl;

				if (n < COLOR_THRESHOLD)
					line[x] |= mask;
			}
		}
		while (mask <<= 1);

print_line:
		cout << "\n\t";
		for (int x = 0; x < RASTER_WIDTH; x++)
		{
			printf("0x%02X", line[x]);
			if (x % 8 == 7)
				cout << ",\n\t";
			else
				cout << ", ";
		}
		cout << endl;
	}

	return 0;
}
