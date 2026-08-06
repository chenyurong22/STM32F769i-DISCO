/*
 * Uart_NTP.c
 *
 *  Created on: Jul 30, 2026
 *      Author: Debasish Das
 */
#include "Uart_NTP.h"
#include "custom.h"

extern UART_HandleTypeDef huart1;
extern RTC_HandleTypeDef hrtc;

/* RTC Related functions */
#define NTP_UNIX_OFFSET 2208988800U
#define IST_OFFSET_SECONDS 19800U

uint32_t getIST(const uint8_t *ntpEpochArray)
{
	uint32_t ntpTime  =
            ((uint32_t)ntpEpochArray[4] << 24) |
              ((uint32_t)ntpEpochArray[5] << 16) |
              ((uint32_t)ntpEpochArray[6] << 8)  |
               (uint32_t)ntpEpochArray[7];

	uint32_t unixTime = ntpTime - NTP_UNIX_OFFSET;
	uint32_t uinxTime_IST = unixTime + IST_OFFSET_SECONDS;

	return uinxTime_IST;
}

void setRtcTime_IST(const uint32_t uinxTime_IST)
{
	time_t epoch = (time_t)uinxTime_IST;
	struct tm *tmInfo = gmtime((time_t*)&epoch);

	RTC_TimeTypeDef sTime = {0};
	RTC_DateTypeDef sDate = {0};

	sTime.Hours   = tmInfo->tm_hour;
	sTime.Minutes = tmInfo->tm_min;
	sTime.Seconds = tmInfo->tm_sec;

	sDate.Date    = tmInfo->tm_mday;
	sDate.Month   = tmInfo->tm_mon + 1;
	sDate.Year    = tmInfo->tm_year - 100;
	sDate.WeekDay = tmInfo->tm_wday + 1;

	HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
	HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BIN);
}

char currTime[50];
void dsplayRTCTime()
{
	//char currTime[50];

	RTC_TimeTypeDef gTime;
	RTC_DateTypeDef gDate;

	HAL_RTC_GetTime(&hrtc, &gTime, RTC_FORMAT_BIN);
	HAL_RTC_GetDate(&hrtc, &gDate, RTC_FORMAT_BIN);

	sprintf(currTime, "%02d:%02d:%02d", gTime.Hours, gTime.Minutes,
			gTime.Seconds);

	writeFormatData(&huart1, "[TIME] : Current Time (IST): [%s] \r\n", currTime);
}

void getCurrentTime(const char *pCurtime, const char *pCurDate)
{
	RTC_TimeTypeDef gTime;
	RTC_DateTypeDef gDate;

	HAL_RTC_GetTime(&hrtc, &gTime, RTC_FORMAT_BIN);
	HAL_RTC_GetDate(&hrtc, &gDate, RTC_FORMAT_BIN);

	sprintf(pCurtime, "%02d:%02d:%02d", gTime.Hours, gTime.Minutes,
			gTime.Seconds);
}

void EpochToRtcTime(uint32_t uinxTime_IST, RTC_TimeTypeDef *sTime)
{
    uint32_t secondsOfDay = uinxTime_IST % 86400;

    sTime->Hours   = secondsOfDay / 3600;
    sTime->Minutes = (secondsOfDay % 3600) / 60;
    sTime->Seconds = secondsOfDay % 60;

    sTime->TimeFormat = RTC_HOURFORMAT12_AM;
    sTime->DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
    sTime->StoreOperation = RTC_STOREOPERATION_RESET;
}

void DisplayIST(RTC_TimeTypeDef *sTime)
{
	char timeStr[50];

	snprintf(timeStr, sizeof(timeStr), "Time: [%02u:%02u:%02u] \r\n",
			sTime->Hours,
			sTime->Minutes,
			sTime->Seconds);

	writetoSerial(&huart1, timeStr);
}

void NTP_ProcessReceivedTime(const uint8_t *pNtpRxBuff, size_t NtpFrameLen)
{
	if (pNtpRxBuff[3] == 0xE3)
	{

		writetoSerial(&huart1, "[❗] NTP time synchronisation \r\n");	/*U+2757*/
		/* Process RTC Receive array to UNIX time IST */
		uint32_t uinxTime_IST = getIST(pNtpRxBuff);

		/* Set RTC Time */
		setRtcTime_IST(uinxTime_IST);
		Get_RtcDateTime(); /* From RTC */
	}
	else
	{
		writetoSerial(&huart1, "[❗] Invalid action \r\n");
	}
}

/* Get hh::mm:ss from  uint32_t uinxTime_IST */
void GetIstTime(uint32_t unixTimeIST)
{
    time_t epoch = (time_t)unixTimeIST;
    struct tm *tmInfo = gmtime(&epoch);

    if (tmInfo != NULL)
    {
        writeFormatData(&huart1,"[ℹ] Time: %02d:%02d:%02d\r\n",
               tmInfo->tm_hour,
               tmInfo->tm_min,
               tmInfo->tm_sec);

        writeFormatData(&huart1,
                        "[ℹ] Date: %02d-%02d-%04d\r\n",
                        tmInfo->tm_mday,
                        tmInfo->tm_mon + 1,
                        tmInfo->tm_year + 1900);


    }
}

void Get_RtcDateTime(void)
{
    RTC_TimeTypeDef sTime = {0};
    RTC_DateTypeDef sDate = {0};

    /* Get RTC time first */
    HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);

    /* Get RTC date second */
    HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN);

    uint16_t year = (uint16_t)(sDate.Year + 2000U);

    writeFormatData(&huart1,
                    "[ℹ] Date: %02d-%02d-%04d\r\n",
                    sDate.Date,
                    sDate.Month,
                    year);

    writeFormatData(&huart1,
                    "[ℹ] Time: %02d:%02d:%02d\r\n",
                    sTime.Hours,
                    sTime.Minutes,
                    sTime.Seconds);
}

