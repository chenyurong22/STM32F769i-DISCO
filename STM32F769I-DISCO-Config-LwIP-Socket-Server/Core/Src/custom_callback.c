/*
 * custom_callback.c
 *
 *  Created on: Jun 29, 2026
 *      Author: Debasish Das
 */
#include "custom_callback.h"
#include "stm32f7xx_hal.h"
#include  "custom.h"

#include "FreeRTOS.h"
#include "task.h"
#include "cmsis_os.h"


extern UART_HandleTypeDef huart1;
extern volatile uint8_t rxData;
extern volatile uint8_t rxBuffer[8];
extern volatile uint8_t count;

extern osThreadId NtpTimeEpochTasHandle;
uint8_t complete = 0;

#define FREEROS 1


extern osThreadId NtpTimeReceiveHandle;
extern TaskHandle_t LwIPLinkHandle;

#if 1
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	BaseType_t xHighPriorityTaskWoken = pdFALSE;
	if(huart->Instance == USART1)
    {
		rxBuffer[count] = rxData;
		count++;

		if(count == 8)
		{
			count = 0;
			process_UART_Receive(rxBuffer);

			portYIELD_FROM_ISR(xHighPriorityTaskWoken);
		}

        HAL_UART_Receive_IT(&huart1, &rxData, 1);
    }
}

extern TaskHandle_t SNTP_LinkHandle;
extern TaskHandle_t UART_NtpTimeReceiveHandle;
void process_UART_Receive(const uint8_t *rxBuffer)
{
	BaseType_t xHighPriorityTaskWoken = pdFALSE;

	if (rxBuffer[0] == 0xAA)
	{
		vTaskNotifyGiveFromISR(UART_NtpTimeReceiveHandle, &xHighPriorityTaskWoken);
	}
	else if (rxBuffer[0] == 0x0C)
	{
		vTaskNotifyGiveFromISR(LwIPLinkHandle, &xHighPriorityTaskWoken);
	}

	else
	{
		return;
	}
}


#endif
