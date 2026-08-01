/*
 * custom_callback.c
 *
 *  Created on: Jul 30, 2026
 *      Author: Debasish Das
 */

#include "custom_callback.h"
#include "stm32f7xx_hal.h"
#include "custom.h"
#include "myCrypto.h"
#include "Uart_NTP.h"



extern volatile uint8_t NtpBuff[7];
extern volatile uint8_t rxData;
extern volatile uint8_t count;
extern NtpBuffRx_t NtpRx;
extern uint8_t NTPRxBuff[NTP_FRAME_LEN];

extern UART_HandleTypeDef huart1;
extern TaskHandle_t SyncNTPTaskHandle;
extern TaskHandle_t ResetBoardTaskHandle;
extern TaskHandle_t VerifySignTaskHandle;

uint8_t recBuffer[NTP_BUFF_SIZE];

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	static uint8_t index = 0;

	BaseType_t xHighPriorityTaskWoken = pdFALSE;

	if(huart->Instance == USART1)
    {
		NTP_ReceiveStateMachine(&NtpRx, rxData);
		if(NtpRx.isComplete)
		{
				memcpy((void*)NTPRxBuff, (void*)NtpRx.NTPBuff, NTP_FRAME_LEN);
				NtpRx.isComplete = false;
				NtpRx.index = 0;

				if(NtpRx.NTPBuff[3] == 0xE3)
				{

					vTaskNotifyGiveFromISR(SyncNTPTaskHandle, &xHighPriorityTaskWoken);
				}

				else if(NtpRx.NTPBuff[3] == 0xEE)
				{
					vTaskNotifyGiveFromISR(ResetBoardTaskHandle, &xHighPriorityTaskWoken);
				}
				else if(NtpRx.NTPBuff[3] == 0xDE)
				{
					vTaskNotifyGiveFromISR(VerifySignTaskHandle, &xHighPriorityTaskWoken);
				}
				else
				{

				}

				portYIELD_FROM_ISR(xHighPriorityTaskWoken);
		}

		HAL_UART_Receive_IT(&huart1, &rxData, 1);
    }
}
