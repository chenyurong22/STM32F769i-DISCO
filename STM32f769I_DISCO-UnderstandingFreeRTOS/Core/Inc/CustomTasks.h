/*
 * CustomTasks.h
 *
 *  Created on: Mar 28, 2026
 *      Author: debasish
 */

#ifndef INC_CUSTOMTASKS_H_
#define INC_CUSTOMTASKS_H_


#include "stdint.h"

typedef struct
{
	uint16_t x;
	uint16_t y;
}TouchData_t ;

void TaskDebug1(void *pvArg);
void TaskDebug2(void *pvArg);
void TouchTask(void *pvArg);
void PrintTask(void *pvArg);

void SendtoSWV(char ch);
void InitSWV();
void EnableSWO();




#endif /* INC_CUSTOMTASKS_H_ */
