/*
 * Uart_NTP.h
 *
 *  Created on: Jul 30, 2026
 *      Author: debasish
 */

#ifndef INC_UART_NTP_H_
#define INC_UART_NTP_H_

#include <stdint.h>
#include <stdio.h>
#include <time.h>
#include "stm32f769xx.h"
#include "stm32f7xx_hal.h"

uint32_t getIST(const uint8_t *ntpEpochArray);
void setRtcTime_IST(const uint32_t uinxTime_IST);
void dsplayRTCTime();
void getCurrentTime(const char *pCurtime, const char *pCurDate);
void EpochToRtcTime(uint32_t uinxTime_IST, RTC_TimeTypeDef *sTime);
void DisplayIST(RTC_TimeTypeDef *sTime);
void NTP_ProcessReceivedTime(const uint8_t *pNtpRxBuff, size_t NtpFrameLen);
void GetIstTime(uint32_t unixTimeIST);
void Get_RtcDateTime(void);
#endif /* INC_UART_NTP_H_ */
