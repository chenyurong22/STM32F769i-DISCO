/*
 * customTasks.c
 *
 *  Created on: Apr 15, 2026
 *      Author: Debasish Das
 */
#include "customTasks.h"
#include "microSD.h"

/* USER CODE BEGIN Header_StartmountSDMMCTask */
void StartmountSDMMCTask(void *argument)
{
	/* USER CODE BEGIN StartmountSDMMCTask */
	/* Infinite loop */
	for (;;)
	{
		if (SDMMC2_mount() == FR_OK)
		{
			writetoSerial(&huart1, "SDMMC card mount Succeed ! \r\n");
			vTaskDelete(NULL);
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
		osDelay(1);
	}
	/* USER CODE END StartreadSDCardTask */
}

/* USER CODE BEGIN Header_StartwriteSDCardTask */
void StartwriteSDCardTask(void *argument)
{
	/* USER CODE BEGIN StartwriteSDCardTask */
	/* Infinite loop */
	for (;;)
	{
		osDelay(1);
	}
	/* USER CODE END StartwriteSDCardTask */
}

/* USER CODE BEGIN Header_StartSDFileOperstionTask */
void StartSDFileOperationTask(void *argument)
{
	/* USER CODE BEGIN StartSDFileOperstionTask */
	/* Infinite loop */
	for (;;)
	{
		osDelay(1);
	}
	/* USER CODE END StartSDFileOperstionTask */
}
