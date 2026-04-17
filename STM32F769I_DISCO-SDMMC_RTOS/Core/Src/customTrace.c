/*
 * customTrace.c
 *
 *  Created on: Apr 15, 2026
 *      Author: Debasish Das
 */

#include "FreeRTOS.h"
#include "task.h"
#include "customTrace.h"
#include "stdint.h"
#include "customTasks.h"

#include "custom.h"

extern xQueueHandle logPspQueue;

void logTaskPSPMicroSD(void *pxCurrentTCB)
{
	traceLog_t pspLog;
	uint32_t *psp = *(uint32_t**) pxCurrentTCB;

	memcpy((traceLog_t*) &pspLog, psp, sizeof(traceLog_t)-sizeof(void*));
	pspLog.currTaskHandle = (TaskHandle_t*)pxCurrentTCB;

	const char *name = pcTaskGetName((TaskHandle_t)pxCurrentTCB);

	/* Sending the Log data over the Queque */
	sendPspLogfromISR(&pspLog);
}


void sendPspLogfromISR(traceLog_t *log)
{

	BaseType_t xHighPriorityTaskWoken = pdFALSE;

	if(xQueueSendFromISR(logPspQueue, log, &xHighPriorityTaskWoken) == pdPASS)
	{

	}
	portYIELD_FROM_ISR(xHighPriorityTaskWoken);

}

