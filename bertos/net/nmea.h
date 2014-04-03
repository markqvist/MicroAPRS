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
 * \brief NMEA Parser.
 *
 * \author Daniele Basile <asterix@develer.com>
 *
 * $WIZ$ module_name = "nmea"
 * $WIZ$ module_configuration = "bertos/cfg/cfg_nmea.h"
 * $WIZ$ module_depends = "kfile", "nmeap01"
 */

#ifndef NET_NMEA_H
#define NET_NMEA_H

#include "cfg/cfg_nmea.h"

#include <net/nmeap/inc/nmeap.h>

#include <io/kfile.h>

#include <time.h>

/*
 * Implemented NMEA parser strings.
 */
#define NMEA_GPGGA 1   // GGA MESSAGE ID
#define NMEA_GPRMC 2   // RMC MESSAGE ID
#define NMEA_GPVTG 3   // VTG MESSAGE ID
#define NMEA_GPGSV 4   // GSV MESSAGE ID

// Standart type to rappresent fiels.
typedef int32_t udegree_t;    // Micro degrees
typedef int32_t mdegree_t;    // Milli degrees
typedef int16_t degree_t;     // Degrees


/**
 * Global Positioning System Fix Data.
 * Extracted data from a GGA message
 *
 * Note: time member contains the seconds elapsed from 00:00:00 1/1/1970,
 * because from nmea sentence we read only the time of UTC position, we
 * have not any reference of date (day, month and year) so time is referred to
 * the start of unix time.
 */
typedef struct NmeaGga
{
	udegree_t     latitude;   /* Latitude (micro degree) */
	udegree_t     longitude;  /* Longitude (micro degree) */
	int32_t       altitude;   /* Altitude (Meter) */
	time_t        time;       /* UTC of position  (Unix time) */
	uint16_t      satellites; /* Satellites are in view */
	uint16_t      quality;    /* Fix Quality: 0 = Invalid; 1 = GPS fix; 2 = DGPS fix; */
	uint16_t      hdop;       /* Relative accuracy of horizontal position (hdop * 10) */
	int16_t       geoid;      /* Height of geoid above WGS84 ellipsoid (Meter) */
} NmeaGga;

/**
 * Recommended minimum specific GPS/Transit data.
 * Extracted data from an RMC message
 *
 * Note: RMC sentences contain also date stamp so, time contains real seconds elapsed
 * from 0:00:00 1/1/1970.
 */
typedef struct NmeaRmc
{
	time_t        time;       /* UTC of position  (Unix time) */
	char          warn;       /* Navigation receiver warning A = OK, V = warning */
	udegree_t     latitude;   /* Latitude (micro degree) */
	udegree_t     longitude;  /* Longitude (micro degree) */
	uint16_t      speed;      /* Speed over ground (knots) */
	degree_t      course;     /* Track made good in degrees True (degree) */
	degree_t      mag_var;    /* Magnetic variation degrees (degree) */
} NmeaRmc;

/**
 * Extracted data from an vtg message
 */
typedef struct NmeaVtg
{
	degree_t     track_good;  /* True track made good (degree) */
	uint16_t     knot_speed;  /* Speed over ground (knots) */
	uint16_t     km_speed;    /* Speed over ground in kilometers/hour */
} NmeaVtg;

/**
 * Extracted data from an gsv message
 */
struct SvInfo
{
	uint16_t    sv_prn;       /* SV PRN number */
	degree_t    elevation;    /* Elevation in degrees, 90 maximum */
	degree_t    azimut;       /* Azimuth, degrees from true north, 000 to 359 */
	uint16_t    snr;          /* SNR, 00-99 dB (null when not tracking) */
};

typedef struct NmeaGsv
{
	uint16_t    tot_message;  /* Total number of messages of this type in this cycle */
	uint16_t    message_num;  /* Message number */
	uint16_t    tot_svv;      /* Total number of SVs in view */
	struct SvInfo info[4];    /* Stanrd gsv nmea report up to 4 sv info */
} NmeaGsv;

void nmea_poll(nmeap_context_t *context, KFile *channel);

int nmea_gpgsv(nmeap_context_t *context, nmeap_sentence_t *sentence);
int nmea_gpvtg(nmeap_context_t *context, nmeap_sentence_t *sentence);
int nmea_gprmc(nmeap_context_t *context, nmeap_sentence_t *sentence);
int nmea_gpgga(nmeap_context_t *context, nmeap_sentence_t *sentence);

// Example of callout
void gpgga_callout(nmeap_context_t *context, void *data, void *user_data);
void gprmc_callout(nmeap_context_t *context, void *data, void *user_data);
void gpgsv_callout(nmeap_context_t *context, void *data, void *user_data);
void gpvtg_callout(nmeap_context_t *context, void *data, void *user_data);

int nmea_testSetup(void);
int nmea_testTearDown(void);
int nmea_testRun(void);

#endif /* NET_NMEA_H */
