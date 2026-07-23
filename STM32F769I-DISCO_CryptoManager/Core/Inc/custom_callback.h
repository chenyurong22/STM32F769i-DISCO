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

#define IV_LEN_CB 16
#define ENC_DATA_LEN 32
#define HASH_IV_LEN 32

/* MicroSD card task handles External */
extern TaskHandle_t MicroSD_mountHandle;
extern TaskHandle_t MicroSD_unmountHandle;
extern TaskHandle_t MicroSD_readHandle;
extern TaskHandle_t MicroSD_writeHandle;
extern TaskHandle_t MicroSD_FileDelHandle;


/* Function Prototypes */
void process_UART_Receive(const uint8_t *rxBuffer, BaseType_t *xHighPriorityTaskWoken);
void process_CBC_Receive(const uint8_t *rxBuffer, uint8_t *pIV, uint8_t *pEncData, uint8_t *pHashIV);

#endif /* INC_CUSTOM_CALLBACK_H_ */
