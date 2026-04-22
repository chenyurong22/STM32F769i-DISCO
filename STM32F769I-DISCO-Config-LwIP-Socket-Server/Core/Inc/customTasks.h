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


/* Private function prototypes */
void StartmountSDMMCTask(void *argument);
void StartreadSDCardTask(void *argument);
void StartwriteSDCardTask(void *argument);
void StartshowSDCardTask(void *argument);
void StartSDFileOperationTask(void *argument);
void StartLwIPLinkHandle(void *argument);
void StartLwIPClientHandle(void *argument);



#endif /* INC_CUSTOMTASKS_H_ */
