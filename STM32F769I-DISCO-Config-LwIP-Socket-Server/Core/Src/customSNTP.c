/*
 * customSNTP.c
 *
 *  Created on: Apr 22, 2026
 *      Author: Debasish Das
 */

#include "customSNTP.h"
#include "custom.h"


void sntp_set_system_time(uint32_t sec)
{
	writeFormatData(&huart1, "Executing [%s] \r\n", __func__);

//    time_t epoch = (time_t)sec;
//
//    struct tm *tm_info = gmtime(&epoch);
//
//    RTC_TimeTypeDef sTime;
//    RTC_DateTypeDef sDate;
//
//    sTime.Hours   = tm_info->tm_hour;
//    sTime.Minutes = tm_info->tm_min;
//    sTime.Seconds = tm_info->tm_sec;
//
//    sDate.Date    = tm_info->tm_mday;
//    sDate.Month   = tm_info->tm_mon + 1;
//    sDate.Year    = tm_info->tm_year - 100; // STM32 counts from 2000
//
//    HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
//    HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BIN);
}


