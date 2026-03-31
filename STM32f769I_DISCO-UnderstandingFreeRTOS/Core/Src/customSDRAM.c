/*
 * customSDRAM.c
 *
 *  Created on: Mar 25, 2026
 *      Author: Debasish Das
 */

#include "customSDRAM.h"
#include "main.h"

void MPU_SDRAM_Config()
	{
		MPU_Region_InitTypeDef MPU_InitStruct = { 0 };

		HAL_MPU_Disable();

		/* SDRAM: 0xC0000000, 8MB */
		MPU_InitStruct.Enable = MPU_REGION_ENABLE;
		MPU_InitStruct.BaseAddress = 0xC0000000;
		MPU_InitStruct.Size = MPU_REGION_SIZE_8MB;
		MPU_InitStruct.AccessPermission = MPU_REGION_FULL_ACCESS;
		MPU_InitStruct.IsBufferable = MPU_ACCESS_NOT_BUFFERABLE;
		MPU_InitStruct.IsCacheable = MPU_ACCESS_CACHEABLE;
		MPU_InitStruct.IsShareable = MPU_ACCESS_NOT_SHAREABLE;
		MPU_InitStruct.Number = MPU_REGION_NUMBER0;
		MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL1;
		MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_ENABLE;

		HAL_MPU_ConfigRegion(&MPU_InitStruct);

		HAL_MPU_Enable(MPU_PRIVILEGED_DEFAULT);
	}
