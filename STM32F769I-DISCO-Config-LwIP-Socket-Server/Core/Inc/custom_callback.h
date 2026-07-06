/*
 * custom_callback.h
 *
 *  Created on: Jun 29, 2026
 *      Author: debasish
 */

#ifndef INC_CUSTOM_CALLBACK_H_
#define INC_CUSTOM_CALLBACK_H_

#include <stdint.h>
#include <stdbool.h>
#include "FreeRTOS.h"
#include "task.h"

/* MicroSD card task handles External */
extern TaskHandle_t MicroSD_mountHandle;
extern TaskHandle_t MicroSD_unmountHandle;
extern TaskHandle_t MicroSD_readHandle;
extern TaskHandle_t MicroSD_writeHandle;


/* Function Prototypes */
void process_UART_Receive(const uint8_t *rxBuffer, BaseType_t *xHighPriorityTaskWoken);

#endif /* INC_CUSTOM_CALLBACK_H_ */
