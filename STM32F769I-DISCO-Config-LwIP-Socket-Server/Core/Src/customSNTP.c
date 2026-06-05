/*
 * customSNTP.c
 *
 *  Created on: Apr 22, 2026
 *      Author: Debasish Das
 */

#include "customSNTP.h"
#include "custom.h"
#include "lwip/apps/sntp.h"
#include "dns.h"

extern RTC_HandleTypeDef hrtc;

void sntp_set_system_time(uint32_t sec)
{
	RTC_TimeTypeDef sTime;
	RTC_DateTypeDef sDate;

	char currTime[50];
	char currDate[50];

	time_t epoch = (time_t) sec;
	epoch += (5 * 3600 + 30 * 60);

	struct tm *timeInfo = gmtime(&epoch);

	/* Saving received Time in structure */
	sTime.Hours = timeInfo->tm_hour;
	sTime.Minutes = timeInfo->tm_min;
	sTime.Seconds = timeInfo->tm_sec;
	sTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
	sTime.StoreOperation = RTC_STOREOPERATION_RESET;

	/* Saving received Day in structure */
	sDate.Date = timeInfo->tm_mday;
	sDate.Month = timeInfo->tm_mon + 1;
	sDate.Year = timeInfo->tm_year - 100;
	sDate.WeekDay = timeInfo->tm_wday + 1;

	/* Setting RTC time */
	HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
	HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BIN);
	HAL_Delay(100);

#if 1
	sprintf(currTime, "%02d:%02d:%02d",
			timeInfo->tm_hour,
			timeInfo->tm_min,
			timeInfo->tm_sec);

	sprintf(currDate, "%02d:%02d:%02d",
			timeInfo->tm_mday,
			timeInfo->tm_mon + 1,
			timeInfo->tm_year + 1900);

	writeFormatData(&huart1, "Received SNTP Date : [%s] \r\n", currDate);
	writeFormatData(&huart1, "Received SNTP Time : [%s] \r\n", currTime);
#endif

	/* Verify RTC Time */
	verifyRTCTime();

}

void SNTP_Init(void)
{
	sntp_setoperatingmode(SNTP_OPMODE_POLL);
	sntp_setservername(0, "pool.ntp.org");
	sntp_setservername(1, "time.google.com");

	sntp_init();
}

void verifyRTCTime()
{

	char currTime[50];

	RTC_TimeTypeDef gTime;
	RTC_DateTypeDef gDate;

	HAL_RTC_GetTime(&hrtc, &gTime, RTC_FORMAT_BIN);
	HAL_RTC_GetDate(&hrtc, &gDate, RTC_FORMAT_BIN);

	sprintf(currTime, "%02d:%02d:%02d", gTime.Hours, gTime.Minutes,
			gTime.Seconds);

	writeFormatData(&huart1, "Set Time : [%s] \r\n", currTime);
}


void getHostName()
{
	ip_addr_t hostAddr;
	dns_gethostbyname("www.google.com", &hostAddr, 0, NULL);
}
