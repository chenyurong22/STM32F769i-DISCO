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
#include "customTrace.h"

entry_t logEntry;
entry_t readEntry;
entry_t writeEntry;

extern xQueueHandle logDataQueue;

/* USER CODE BEGIN Header_StartmountSDMMCTask */
void StartmountSDMMCTask(void *argument)
{
	uint16_t loopCount = 0;

	for (;;)
	{
		writeFormatData(&huart1, "Running [%s] \r\n", __func__);
		if (SDMMC2_mount() == FR_OK)
		{
			writetoSerial(&huart1, "SDMMC card mount Succeed ! \r\n");
			loopCount++;

			if (loopCount > 20)
			{
				writeFormatData(&huart1, "Deleting [%s] task ... \r\n",
						__func__);
				vTaskDelete(NULL);
			}
		}
		vTaskDelay(pdMS_TO_TICKS(500));
	}
}

/* USER CODE BEGIN Header_StartreadSDCardTask */
void StartreadSDCardTask(void *argument)
{

	for (;;)
	{
		vTaskDelay(pdMS_TO_TICKS(250));
	}
}

/* USER CODE BEGIN Header_StartshowSDCardTask */
extern xQueueHandle logPspQueue;
void StartshowSDCardTask(void *argument)
{
	traceLog_t readLog;
	BaseType_t pxHigherPriorityTaskWoken;

	char *name;
	UBaseType_t taskPriority;

	for (;;)
	{
		if (xQueueReceiveFromISR(logPspQueue, &readLog,
				&pxHigherPriorityTaskWoken) == pdPASS)
		{

			name = pcTaskGetTaskName((TaskHandle_t) readLog.currTaskHandle);
			taskPriority = uxTaskPriorityGet(
					(TaskHandle_t) readLog.currTaskHandle);

			writeFormatData(&huart1,
					"[%-15s:%2d] [R0:0x%lX] [R1:0x%lX] [R2:0x%lX]"
							"[LR:0x%lX] [PC:0x%lX] [xPSR:0x%lX] \r\n",
							name,
							taskPriority,
							readLog.R0,
							readLog.R1,
							readLog.R2,
							readLog.LR,
							readLog.PC,
							readLog.xPSR);
		}

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

		if (xQueueSend(logDataQueue, &logEntry,portMAX_DELAY) != pdPASS)
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
		if (xQueueReceive(logDataQueue, &recvEntry, portMAX_DELAY) == pdPASS)
		{
			if (recvEntry.DiskOp == WRITE_LOG)
			{
//				SDMMC2_WriteFile("Output.txt", &recvEntry, &byteCount);
//				writeFormatData(&huart1, "FileS size: %d bytes\r\n", byteCount);
//				indx++;
			}
		}

		vTaskDelay(pdMS_TO_TICKS(100));
	}
}

