/*
 * CustomTasks.c
 *
 *  Created on: Mar 28, 2026
 *      Author: Debasish Das
 *
 */

#include "CustomTasks.h"
#include "customLCD.h"
#include "custom.h"
#include "stm32f769i_discovery_ts.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "semphr.h"

extern volatile uint8_t TOUCH_ON;
extern volatile uint8_t touchCount;
extern volatile uint8_t pixelXY[2];
extern TS_StateTypeDef TS_State;
extern SemaphoreHandle_t touchSem;
extern QueueHandle_t touchQueue;

extern volatile uint8_t aCordinate[2];

volatile uint16_t debugCnt1 = 0;
volatile uint16_t debugCnt2 = 0;

void TaskDebug1(void *pvArg)
{
	while (1)
	{
		writetoSerial(&huart1, "Running TaskDebug1() \r\n");
		debugCnt1++;

		ITM_SendChar('A');
		for (volatile int i = 0; i < 1000000; i++);

		vTaskDelay(pdMS_TO_TICKS(250));
	}
}

void TaskDebug2(void *pvArg)
{
	while (1)
	{
		writetoSerial(&huart1, "Running TaskDebug2() \r\n");
		debugCnt2++;
		vTaskDelay(pdMS_TO_TICKS(250));

	}
}

uint8_t QueueSendError = 0;
void TouchTask(void *pvArg)
{
	TouchData_t data;
	uint8_t prevTouch = 0;

	while (1)
	{
		if (BSP_TS_GetState(&TS_State) == TS_OK)
		{
			if (TS_State.touchDetected)
			{
				/* Only send when new touch detected */
				if (prevTouch == 0)
				{
					data.x = TS_State.touchX[0];
					data.y = TS_State.touchY[0];

					if (xQueueSend(touchQueue, &data, portMAX_DELAY) != pdPASS)
					{
						{
							PrintUARTDebug("SendQueue Full", __FILE__,
									__LINE__);
							vTaskDelay(pdMS_TO_TICKS(100));
						}
					}
					prevTouch = 1;
				}
			}
			else
			{
				prevTouch = 0;
			}
		}

		/* Polling Delay */
		vTaskDelay(pdMS_TO_TICKS(100));
	}
}

void PrintTask(void *pvArg)
{
	TouchData_t dataRec;
	char msg[60];

	while (1)
	{
		if (xQueueReceive(touchQueue, &dataRec, portMAX_DELAY))
		{
			sprintf(msg, "Touch Cordinate: [%3d:%3d]", dataRec.x, dataRec.y);
			BSP_LCD_DisplayStringAtLine(5, (uint8_t*) msg);
			vTaskDelay(pdMS_TO_TICKS(100));
		}
		else
		{
			PrintUARTDebug("ReceiveQueue Empty", __FILE__, __LINE__);
			vTaskDelay(pdMS_TO_TICKS(100));
		}
	}
}


void InitSWV()
{
	/* Enable Trace system */
	CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;

	/*Unlock ITM */
	ITM->LAR = 0xC5ACCE55;

	/* Enable ITM */
	ITM->TCR =
	    ITM_TCR_ITMENA_Msk |     // Enable ITM
	    ITM_TCR_SYNCENA_Msk |    // Sync packets (optional)
	    ITM_TCR_TSENA_Msk;       // Timestamp (optional)


	/* Enable stimulus port 0 */
	ITM->TCR = ITM_TCR_ITMENA_Msk;
	ITM->TER = 1;

    /* Configure TPIU */
	TPI->SPPR = 2;  // NRZ
    TPI->ACPR = (SystemCoreClock / 2000000) - 1;
}

void SendtoSWV(char ch)
{

	ITM_SendChar(ch);
	ITM_SendChar('\n');
}

void EnableSWO()
{
	__HAL_RCC_GPIOB_CLK_ENABLE();

	GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.Pin = GPIO_PIN_3;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Alternate = GPIO_AF0_TRACE;

	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
}

