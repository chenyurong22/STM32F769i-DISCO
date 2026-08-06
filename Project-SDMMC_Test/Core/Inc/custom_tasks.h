/*
 * custom_task.h
 *
 *  Created on: Jul 31, 2026
 *      Author: debasish
 */

#ifndef INC_CUSTOM_TASKS_H_
#define INC_CUSTOM_TASKS_H_

#include "FreeRTOS.h"
#include "task.h"

void StartSyncNTPTask(void *argument);
void StartVerifySignTask(void *argument);
void StartResetBoardTask(void *argument);

#define WRITE_ERR(taskHandle)\
		writeFormatData(&huart1,"[%s] : Taks creation Failed\r\n", pcTaskGetName(taskHandle))

#define ASSERT_TASK(status, taskHandle)\
							do								\
								{							\
								if((status) != pdPASS)		\
									{						\
										WRITE_ERR(taskHandle);\
										\
									} \
								} while(0);


#endif /* INC_CUSTOM_TASKS_H_ */
