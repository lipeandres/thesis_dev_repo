#include "ch.h"
#include "hal.h"
#include "chprintf.h"
#include <stdio.h>
#include <string.h>
#include <serial.h>
#include <stdbool.h>
#include <time.h>
#include <stdlib.h>

#include "chrtclib.h"

/*struct comInfo{
int tm_sec;     //  seconds after minute [0-61] (61 allows for 2 leap-seconds)
int tm_min;     //  minutes after hour [0-59]
int tm_hour;    //  hours after midnight [0-23]
int tm_mday;    //  day of the month [1-31]
int tm_mon;    //  month of year [0-11]
int tm_year;     // current year-1900
int tm_wday;     // days since Sunday [0-6]
int tm_yday;    // days since January 1st [0-365]
int tm_isdst;    // daylight savings indicator (1 = yes, 0 = no, -1 = unknown)
};*/

typedef struct tm ComInfo;

static void imxRead	(ComInfo*);
static void imxWrite	(ComInfo*);
static void rtcSet(ComInfo*);
static void rtcGet(ComInfo*);
SerialConfig config;

int main(void) {
 	static ComInfo rtcInitTime;
	static ComInfo *rtcInitTimePtr = &rtcInitTime;
	static ComInfo rtcTime;
	static ComInfo *rtcTimePtr = &rtcTime;
	
	//Setting up the date for the first time
	rtcInitTimePtr->tm_sec=0;
	rtcInitTimePtr->tm_min=10;
	rtcInitTimePtr->tm_hour=20;	
	rtcInitTimePtr->tm_mday=29;
	rtcInitTimePtr->tm_mon=9;
	rtcInitTimePtr->tm_year=2012-1900;
	rtcInitTimePtr->tm_isdst=0;
	rtcInitTimePtr->tm_wday=1;
	//rtcInitTimePtr->tm_yday=302;
	

	
	halInit();
   	chSysInit();
//
//---USART Configuration
//---USART 1, talks to the IMX233. Set by default to 115200 Bps
	palSetPadMode(GPIOA, 9, PAL_MODE_ALTERNATE(7));//uart1 init
    	palSetPadMode(GPIOA, 10, PAL_MODE_ALTERNATE(7));
   	sdStart(&SD1, NULL);	
	//Shutdown wakeup
	rtcSet(rtcInitTimePtr);
	rtcSetPeriodicWakeup_v2(&RTCD1, NULL);
	while(true)
	{
	rtcGet(rtcTimePtr);	
	imxWrite(rtcTimePtr);
	chThdSleepMilliseconds(300);	
	}

return 0;
}

static void imxRead	(ComInfo *comObjectPtr){
	unsigned char inBuffer[sizeof(*comObjectPtr)];
	chSequentialStreamRead((BaseSequentialStream *)&SD1, 
	inBuffer, sizeof(*comObjectPtr));
	memcpy(comObjectPtr,inBuffer,sizeof(*comObjectPtr));
	return;
}

static void imxWrite	(ComInfo *comObjectPtr){
	unsigned int m;	
	unsigned char outBuffer[sizeof(*comObjectPtr)];	
	for(m=0;m<sizeof(comObjectPtr);m++)
	outBuffer[m]=0;

	memcpy(outBuffer,comObjectPtr,sizeof(*comObjectPtr));	
	chSequentialStreamWrite((BaseSequentialStream *)&SD1, outBuffer, sizeof(*comObjectPtr));	
		return;
}


static void rtcSet(ComInfo* rtcInitialTime)
{
      rtcSetTimeTm(&RTCD1, rtcInitialTime);
      return;
}

static void rtcGet(ComInfo* rtcTime)
{
    rtcGetTimeTm(&RTCD1, rtcTime);
    return;
}
