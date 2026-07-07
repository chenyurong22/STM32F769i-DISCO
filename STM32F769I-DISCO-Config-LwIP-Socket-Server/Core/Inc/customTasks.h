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

extern volatile uint8_t rxBuffer[8];


/* Private function prototypes */
void StartmountSDMMCTask(void *argument);
void StartreadSDCardTask(void *argument);
void StartwriteSDCardTask(void *argument);
void StartshowSDCardTask(void *argument);
void StartSDFileOperationTask(void *argument);
void StartLwIPLinkHandle(void *argument);
void StartLwIPClientHandle(void *argument);
void StartSNTP_LinkHandle(void *argument);
void StartUARTLinkHandle(void *argument);

void StartUART_NtpTimeReceive(void *argument);
void StartUART_NtpTimeSet(void *argument);
void StartDisplay_DeviceTime(void *argument);
void StartReset_Device(void *argument);


/* MicroCD task declaration */
void StartMicroSD_mount(void *argument);
void StartMicroSD_unmount(void *argument);
void StartMicroSD_read(void *argument);
void StartMicroSD_write(void *argument);
void StartMicroSD_FileDel(void *atgument);


/* External Task Handler declaration */
extern TaskHandle_t UART_NtpTimeReceiveHandle;
extern TaskHandle_t UART_NtpTimeSetHandle;
extern TaskHandle_t Display_DeviceTimeHandle;

#endif /* INC_CUSTOMTASKS_H_ */
