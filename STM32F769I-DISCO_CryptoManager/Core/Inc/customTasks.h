/*
 * customTasks.h
 *
 *  Created on: Apr 15, 2026
 *      Author: Debasish Das
 */

#ifndef INC_CUSTOMTASKS_H_
#define INC_CUSTOMTASKS_H_

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "queue.h"
#include "stdio.h"
#include "custom.h"

extern volatile uint8_t rxBuffer[UART_TX_LEN];

/* Private function prototypes */
void StartDecryptTask(void *argument);

/* External Task Handler declaration */
extern TaskHandle_t DecryptTaskHandle;

#endif /* INC_CUSTOMTASKS_H_ */
