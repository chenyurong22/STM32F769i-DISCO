/*
 * customTrace.h
 *
 *  Created on: Apr 15, 2026
 *      Author: Debasish Das
 */

#ifndef INC_CUSTOMTRACE_H_
#define INC_CUSTOMTRACE_H_

typedef struct traceLog
{
	uint32_t R4;
	uint32_t R5;
	uint32_t R6;
	uint32_t R7;
	uint32_t R8;
	uint32_t R9;
	uint32_t R10;
	uint32_t R11;
	uint32_t R0;
	uint32_t R1;
	uint32_t R2;
	uint32_t R3;
	uint32_t R12;
	uint32_t LR;
	uint32_t PC;
	uint32_t xPSR;
	void *currTaskHandle;
} traceLog_t;


/* Private function prototypes starts */
void logTaskPSPMicroSD(void *pxCurrentTCB);
void sendPspLogfromISR(traceLog_t *log);


/* Private function prototypes ends */


#endif /* INC_CUSTOMTRACE_H_ */
