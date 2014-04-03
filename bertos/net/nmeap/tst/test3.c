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
EXAMPLE : SETUP FOR GGA AND RMC SENTENCES + A CUSTOM SENTENCE PARSER WITH CHARACTER BY CHARACTER IO
=======================================================================================================
*/   


/*
$PGRMF

GARMIN PROPRIETARY GPS Position Fix Data

$PGRMF,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15*HH
1 = GPS week number
2 = GPS seconds in current week
3 = UTC date, ddmmyy format
4 = UTC time, hhmmss format
5 = GPS leap second count
6 = Latitude, dddmm.mmmm format
7 = Latitude hemisphere, N or S
8 = Longitude, dddmm.mmmm format
9 = Longitude hemisphere, E or W
10 = Mode (M=Manual, A=Automatic)
11 = Fix type (0=No fix, 1=2D fix, 2=3D fix)
12 = Speed over ground, kilometres / hour
13 = Course over ground, degrees true
14 = PDOP (Position dilution of precision), rounded to nearest integer
15 = TDOP (Time dilution of precision), rounded to nearest integer
HH = Checksum 
*/



#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "nmeap.h"

nmeap_gga_t g_gga;

char test_vector[] = {
"$GPGGA,123519,3929.946667,N,11946.086667,E,1,08,0.9,545.4,M,46.9,M,,*4A\r\n"
"$GPRMC,225446,A,4916.45,N,12311.12,W,000.5,054.7,191194,020.3,E*68\r\n"
"$PGRMF,1,100,191105,123519,13,3929.946667,N,12311.12,W,A,2,100.1,181.2,3,8*35\r\n"
};

char *pvec = test_vector;

/** simulate character by character IO */
int readchar() 
{
    int ch;
    if (*pvec == 0) {
        ch = -1;
    }
    else {
        ch = *pvec++;
    }
    return ch;
}
/* --------------------------------------------------------------*/
/*STEP 1a : define a data structure to contain the sentence data */                                                
/* ------------------------------------------------------------- */
struct garmin_rmf {
    /* field                            position in sentence */
    int            week;             /*    1 = GPS week number                                                          */
    int            seconds_of_week;  /*    2 = GPS seconds in current week                                              */
    unsigned long  date;             /*    3 = UTC date, ddmmyy format                                                  */
    unsigned long  time;             /*    4 = UTC time, hhmmss format                                                  */
    int            leap;             /*    5 = GPS leap second count                                                    */
    double         lat;              /*  6,7 = Latitude, dddmm.mmmm format (north positive)                             */
    double         lon;              /*  8,9 = Longitude, dddmm.mmmm format (east positive)                             */
    int            mode;             /*   10 = Mode (M=Manual, A=Automatic)                                             */
    int            fix;              /*   11 = Fix type (0=No fix, 1=2D fix, 2=3D fix)                                  */
    double         speed;            /*   12 = Speed over ground, kilometres / hour                                     */
    double         course;           /*   13 = Course over ground, degrees true                                         */
    int            pdop;             /*   14 = PDOP (Position dilution of precision), rounded to nearest integer        */
    int            tdop;             /*   15 = TDOP (Time dilution of precision), rounded to nearest integer            */
};
typedef struct garmin_rmf garmin_rmf_t;

/* --------------------------------------------------------------*/
/*STEP 1b : define an id value for the message                   */                                                
/* ------------------------------------------------------------- */
#define GARMIN_PGRMF (NMEAP_USER + 0)


/* --------------------------------------------------------------*/
/* STEP 1c : write the sentence parser                            */                                                
/* ------------------------------------------------------------- */
int custom_pgrmf(nmeap_context_t *context,nmeap_sentence_t *sentence)
{
#ifndef NDEBUG	
    int i;
#endif
    
	/* get pointer to sentence data */
	garmin_rmf_t *rmf = (garmin_rmf_t *)sentence->data;
	
	if (rmf != 0) {
		/* if the sentence has a data storage element, use it */
		
		
		/* extract data from the tokens */
		rmf->week            = atoi(context->token[1]);
		rmf->seconds_of_week = atoi(context->token[2]);
		rmf->date            = (unsigned long)atol(context->token[3]);
		rmf->time            = (unsigned long)atol(context->token[4]); 
		rmf->leap            = atoi(context->token[5]);
		rmf->lat             = nmeap_latitude(context->token[6],context->token[7]);
		rmf->lon             = nmeap_longitude(context->token[8],context->token[9]);
		rmf->mode            = atoi(context->token[10]);
		rmf->fix             = atoi(context->token[11]);
		rmf->speed           = atof(context->token[12]);
		rmf->course          = atof(context->token[13]);
		rmf->pdop            = atoi(context->token[14]); 
		rmf->tdop            = atoi(context->token[15]);
	}
	/* else there was no data element to store into */

#ifndef NDEBUG    
    /* print raw input string */
    printf("%s",context->debug_input);
    
    /* print some validation data */
    printf("%s==%s %02x==%02x\n",context->input_name,sentence->name,context->icks,context->ccks);
    
    /* print the tokens */
    for(i=0;i<context->tokens;i++) {
        printf("%d:%s\n",i,context->token[i]);
    }
#endif   

    /* if the sentence has a callout, call it */
    if (sentence->callout != 0) {
        (*sentence->callout)(context,rmf,context->user_data);
    }
    
    return GARMIN_PGRMF;
}


/* -------------------------------------------------------------*/
/*STEP 2 : write a function to do something with the data       */                                                
/* ------------------------------------------------------------ */
static void print_pgrmf(garmin_rmf_t *rmf)
{
	assert(rmf != 0);         
	
	printf("                    w sec date   time   lp lat       lon         m f spd    crs    p t\n");
    printf("found PGRMF message %d %d %lu %lu %d %.6f %.6f %d %d %.2f %.2f %d %d\n",
            rmf->week,                   
            rmf->seconds_of_week, 
            rmf->date,            
            rmf->time,            
            rmf->leap,            
            rmf->lat,             
            rmf->lon,             
            rmf->mode,            
            rmf->fix,             
            rmf->speed,           
            rmf->course,          
            rmf->pdop,            
            rmf->tdop            
            );
}

/* -------------------------------------------------------------*/
/*STEP 3 : if using the callout method, write the callout       */                                                
/* ------------------------------------------------------------ */
static void pgrmf_callout(nmeap_context_t *context,void *data,void *user_data)
{
    garmin_rmf_t *rmf = (garmin_rmf_t *)data;
    
    printf("-------------callout\n");
    print_pgrmf(rmf);
}


/* ---------------------------------------------------------------------------------------*/
/* STEP 4 : allocate the data structures. be careful if you put them on the stack because */
/*          they need to be live for the duration of the parser                           */
/* ---------------------------------------------------------------------------------------*/
static nmeap_context_t nmea;       /* parser context */
static nmeap_gga_t     gga;        /* this is where the data from GGA messages will show up */
static nmeap_rmc_t     rmc;        /* this is where the data from RMC messages will show up */
static garmin_rmf_t    rmf;        /* this is where the data from RMF messages will show up */
static int             user_data; /* user can pass in anything. typically it will be a pointer to some user data */

int main(int argc,char *argv[])
{
    int             status;
    char            ch;
    
    /* ---------------------------------------*/
    /*STEP 5 : initialize the nmea context    */                                                
    /* ---------------------------------------*/
    status = nmeap_init(&nmea,(void *)&user_data);
    if (status != 0) {
        printf("nmeap_init %d\n",status);
        exit(1);
    }
    
    /* ---------------------------------------*/
    /*STEP 6 : add standard GPGGA parser      */
	/*         (no callout this time)         */
    /* -------------------------------------- */
    status = nmeap_addParser(&nmea,"GPGGA",nmeap_gpgga,0,&gga);
    if (status != 0) {
        printf("nmeap_add %d\n",status);
        exit(1);
    }

    /* ---------------------------------------*/
    /*STEP 7 : add standard GPRMC parser      */                                                
	/*         (no callout this time)         */
    /* -------------------------------------- */
    status = nmeap_addParser(&nmea,"GPRMC",nmeap_gprmc,0,&rmc);
    if (status != 0) {
        printf("nmeap_add %d\n",status);
        exit(1);
    }
    
    /* ---------------------------------------*/
    /*STEP 8 : ADD THE CUSTOM PARSER          */                                                
	/*         with callout         )         */
    /* -------------------------------------- */
    status = nmeap_addParser(&nmea,"PGRMF",custom_pgrmf,pgrmf_callout,&rmf);
    if (status != 0) {
        printf("nmeap_add %d\n",status);
        exit(1);
    }
    /* ---------------------------------------*/
    /*STEP 9 : process input until done       */                                                
    /* -------------------------------------- */
    for(;;) {
        /* ---------------------------------------*/
        /*STEP 10: get a byte at a time           */                                                
        /* -------------------------------------- */
        ch = readchar();
        if (ch <= 0) {
            break;
        }
        
        /* --------------------------------------- */
        /*STEP 11: pass it to the parser          */
        /* status indicates whether a complete msg */
        /* arrived for this byte                   */
        /* NOTE : in addition to the return status */
        /* the message callout will be fired when  */
        /* a complete message is processed         */
        /* --------------------------------------- */
        status = nmeap_parse(&nmea,ch);
        
        /* ---------------------------------------*/
        /*STEP 12 : process the return code       */                                                
        /* -------------------------------------- */
        switch(status) {
        case NMEAP_GPGGA:
            /* GOT A GPGGA MESSAGE */
            printf("-------------switch\n");
            printf("GPGGA\n");
            printf("-------------\n");
            break;
        case NMEAP_GPRMC:
            /* GOT A GPRMC MESSAGE */
            printf("-------------switch\n");
            printf("GPRMC\n");
            printf("-------------\n");
            break;
        case GARMIN_PGRMF:
            /* GOT A PGRMF MESSAGE */
            printf("-------------switch\n");
            print_pgrmf(&rmf);
            printf("-------------\n");
            break;
        default:
            break;
        }
    }
    
    return 0;
}

