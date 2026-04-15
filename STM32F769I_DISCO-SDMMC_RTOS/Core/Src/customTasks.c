/*
 * customTasks.c
 *
 *  Created on: Apr 15, 2026
 *      Author: Debasish Das
 */
#include "customTasks.h"
#include "microSD.h"
#include "stdint.h"
#include "custom.h"

entry_t logEntry;
entry_t readEntry;
entry_t writeEntry;

extern xQueueHandle logDataQueue;

/* USER CODE BEGIN Header_StartmountSDMMCTask */
void StartmountSDMMCTask(void *argument)
{
	/* USER CODE BEGIN StartmountSDMMCTask */

	/* Infinite loop */
	for (;;)
	{
		if (SDMMC2_mount() == FR_OK)
		{
			writetoSerial(&huart1, "SDMMC card mount Succeed.! \r\n");
			vTaskSuspend(NULL);
		}

		vTaskDelay(pdMS_TO_TICKS(250));
	}
	/* USER CODE END StartmountSDMMCTask */
}

/* USER CODE BEGIN Header_StartreadSDCardTask */

void StartreadSDCardTask(void *argument)
{
	/* USER CODE BEGIN StartreadSDCardTask */

	/* Infinite loop */
	for (;;)
	{
		//writeFormatData(&huart1, "[%s] \r\n", pcTaskGetName(NULL));
		vTaskDelay(pdMS_TO_TICKS(500));
	}
}

/* USER CODE BEGIN Header_StartwriteSDCardTask */
void StartwriteSDCardTask(void *argument)
{
	const char *msg = "Data sent to MicroSD card [Startwrite]";
	static uint16_t logCount = 0;

	/* Infinite loop */
	for (;;)
	{
		logEntry.data = msg;
		logEntry.index = logCount;
		logEntry.DiskOp = WRITE_LOG;

		//writeFormatData(&huart1, "[%s] \r\n", pcTaskGetName(NULL));
		if (xQueueSend(logDataQueue, &logEntry,200) != pdPASS)
		{
			writetoSerial(&huart1, "******** FULL **************** \r\n");
			vTaskDelay(pdMS_TO_TICKS(100));
		}
		logCount++;

		vTaskDelay(pdMS_TO_TICKS(10));
	}
}

/* USER CODE BEGIN Header_StartSDFileOperstionTask */
extern TaskHandle_t mountSDMMCTaskHandle;
void StartSDFileOperationTask(void *argument)
{
	entry_t recvEntry;
	size_t byteCount = 0;
	static uint16_t indx = 0;

	for (;;)
	{
		if (xQueueReceive(logDataQueue, &recvEntry, 200) == pdPASS)
		{
			if (recvEntry.DiskOp == WRITE_LOG)
			{
				SDMMC2_WriteFile("Output.txt", &recvEntry, &byteCount);
				writeFormatData(&huart1, "FileS size: %d bytes\r\n", byteCount);
				indx++;
			}
		}
		else
		{
			writetoSerial(&huart1, "******** EMPTY **************** \r\n");
		}

		vTaskDelay(pdMS_TO_TICKS(100));
	}
}
