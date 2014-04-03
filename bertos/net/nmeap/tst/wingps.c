/*
Copyright (c) 2005, David M Howard (daveh at dmh2000.com)
All rights reserved.

This product is licensed for use and distribution under the BSD Open Source License.
see the file COPYING for more details.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE 
ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE 
LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, 
OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT 
OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; 
OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE 
OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, 
EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. 

*/

/*
========================================================================================================
EXAMPLE : SETUP FOR GGA AND RMC SENTENCES WITH SERIAL IO FOR WIN32
=======================================================================================================
*/   
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "nmeap.h"

/** open a serial port */
static HANDLE openPort(const char *port,int baud)
{
	HANDLE       h;
	DCB          dcb;
	COMMTIMEOUTS tmo;
	int          status;

	/* open the port */
	h = CreateFile( port,  
                    GENERIC_READ | GENERIC_WRITE, 
                    0, 
                    0, 
                    OPEN_EXISTING,
                    0,
                    0);
	if (h == INVALID_HANDLE_VALUE) {
		/* quit on error */
		return h;
	}
   

	/* read current configuration */
   status = GetCommState(h,&dcb);
   if (status == 0) {
	   CloseHandle(h);
	   return INVALID_HANDLE_VALUE;
   }

   /* set the baud rate and other parameters */
   dcb.BaudRate = baud;
   dcb.ByteSize = 8;
   dcb.Parity   = NOPARITY; 
   dcb.StopBits = ONESTOPBIT;

   /* set configuration */
   status = SetCommState(h, &dcb);
   if (status == 0) {
	   CloseHandle(h);
	   return INVALID_HANDLE_VALUE;
   }

   /* read timeout configuration */
   status = GetCommTimeouts(h,&tmo);
   if (status == 0) {
	   CloseHandle(h);
	   return INVALID_HANDLE_VALUE;
   }

   /* set to indefinite blocking */
   tmo.ReadIntervalTimeout        = 0;
   tmo.ReadTotalTimeoutConstant   = 0;
   tmo.ReadTotalTimeoutMultiplier = 0;
   status = SetCommTimeouts(h,&tmo);
   if (status == 0) {
	   CloseHandle(h);
	   return INVALID_HANDLE_VALUE;
   }

	return h;
}

/** read a byte (blocking) */
static int readPort(HANDLE h)
{
	BOOL  status;
	char  ch;
	DWORD count;
	status =  ReadFile(h,&ch,1,&count,0);
	if (status == 0) {
		return -1;
	}

	return (int)ch;
}


static void closePort(HANDLE h)
{
	CloseHandle(h);
}


/** print current data */
static void printGps(nmeap_gga_t *gga,nmeap_rmc_t *rmc)
{
	printf("%lu %lu %.6f %.6f %.0f %f %f %d %d\n",
			gga->time,
			rmc->date,
            gga->latitude  ,
            gga->longitude, 
            gga->altitude , 
			rmc->course,
			rmc->speed,
            gga->satellites,
            gga->quality   
            );
}

/* ---------------------------------------------------------------------------------------*/
/* STEP 1 : allocate the data structures. be careful if you put them on the stack because */
/*          they need to be live for the duration of the parser                           */
/* ---------------------------------------------------------------------------------------*/
static nmeap_context_t nmea;	   /* parser context */
static nmeap_gga_t     gga;		   /* this is where the data from GGA messages will show up */
static nmeap_rmc_t     rmc;		   /* this is where the data from RMC messages will show up */
static int             user_data; /* user can pass in anything. typically it will be a pointer to some user data */

int main(int argc,char *argv[])
{
    int             status;
    char            ch;
	const char     *port;
	int             baud;
	HANDLE          h;

	/* require both arguments */
	if (argc != 3) {
		printf("%s <comport> <baud>\n",argv[0]);
		return 1;
	}

	/* serial port argument */
	port = argv[1];

	/* baud rate argument */
	status = sscanf(argv[2],"%d",&baud);
	if (status != 1) {
		printf("%s <comport> <baud>\n",argv[0]);
		printf("invalid <baud> : %s\n",argv[2]);
		return 1;
	}

	/** open the serial port */
	h = openPort(port,baud);
	if (h == INVALID_HANDLE_VALUE) {
		printf("can't open port : %s\n",port);
		return 1;
	}
    
	/* ---------------------------------------*/
	/*STEP 2 : initialize the nmea context    */                                                
	/* ---------------------------------------*/
    status = nmeap_init(&nmea,(void *)&user_data);
    if (status != 0) {
        printf("nmeap_init %d\n",status);
        exit(1);
    }
    
	/* ---------------------------------------*/
	/*STEP 3 : add standard GPGGA parser      */                                                
	/* -------------------------------------- */
    status = nmeap_addParser(&nmea,"GPGGA",nmeap_gpgga,0,&gga);
    if (status != 0) {
        printf("nmeap_add %d\n",status);
        exit(1);
    }

	/* ---------------------------------------*/
	/*STEP 4 : add standard GPRMC parser      */                                                
	/* -------------------------------------- */
    status = nmeap_addParser(&nmea,"GPRMC",nmeap_gprmc,0,&rmc);
    if (status != 0) {
        printf("nmeap_add %d\n",status);
        exit(1);
    }

	/* ---------------------------------------*/
	/*STEP 5 : process input until done       */                                                
	/* -------------------------------------- */
    for(;;) {
		/* ---------------------------------------*/
		/*STEP 6 : get a byte at a time           */                                                
		/* -------------------------------------- */
        ch = readPort(h);
        if (ch <= 0) {
            break;
        }
        
		/* --------------------------------------- */
		/*STEP 7 : pass it to the parser           */
		/* status indicates whether a complete msg */
		/* arrived for this byte                   */
		/* NOTE : in addition to the return status */
		/* the message callout will be fired when  */
		/* a complete message is processed         */
		/* --------------------------------------- */
        status = nmeap_parse(&nmea,ch);
        
		/* ---------------------------------------*/
		/*STEP 8 : process the return code        */                                                
		/* -------------------------------------- */
        switch(status) {
        case NMEAP_GPGGA:
			/* GOT A GPGGA MESSAGE */
            printGps(&gga,&rmc);
            break;
        case NMEAP_GPRMC:
			/* GOT A GPRMC MESSAGE */
            printGps(&gga,&rmc);
            break;
        default:
            break;
        }
    }

	/* close and quit */
	closePort(h);
    
    return 0;
}
