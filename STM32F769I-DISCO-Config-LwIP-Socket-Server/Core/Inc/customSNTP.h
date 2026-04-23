/*
 * customSNTP.h
 *
 *  Created on: Apr 22, 2026
 *      Author: debasish
 */

#ifndef INC_CUSTOMSNTP_H_
#define INC_CUSTOMSNTP_H_

#include "stdint.h"



void sntp_set_system_time(uint32_t sec);
void SNTP_Init(void);
void verifyRTCTime();

#endif /* INC_CUSTOMSNTP_H_ */
