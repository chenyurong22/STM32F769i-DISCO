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
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	HAL_GPIO_TogglePin(GPIOJ, GPIO_PIN_5);

	BaseType_t xHighPriorityTaskWoken = pdFALSE;
	if(huart->Instance == USART1)
    {
		rxBuffer[count] = rxData;
		count++;

		if(count == 8)
		{
			count = 0;

			vTaskNotifyGiveFromISR(NtpTimeReceiveHandle, &xHighPriorityTaskWoken);
			portYIELD_FROM_ISR(xHighPriorityTaskWoken);
		}

        HAL_UART_Receive_IT(&huart1, &rxData, 1);
    }
}

