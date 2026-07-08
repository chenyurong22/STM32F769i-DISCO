/*
 * microSD.h
 *
 *  Created on: Feb 3, 2026
 *      Author: debasish
 */

#ifndef INC_MICROSD_H_
#define INC_MICROSD_H_

#include <stdbool.h>
#include <stdint.h>
#include "stm32f769xx.h"
#include "stm32f7xx_hal.h"
#include "fatfs.h"



typedef enum
{
	BINARY_FILE,
	ASCII_FILE,
	OTHER_FILE
} FiletypeDef_t;

typedef enum
{
	READ_LOG,
	WRITE_LOG,
	NOP
} DiskOperation_t;

typedef struct entry
{
	char data[50];
	size_t index;
	DiskOperation_t DiskOp;
} entry_t;

bool SD_IsDetected(void);
void showSDcardInfo(SD_HandleTypeDef *hsd);
uint8_t readFileSDCard(char *pBuffer, uint16_t wSize, int *bByteRead);
void SD_Read_Test(void);
void readSector0();

FRESULT SDMMC2_mount();
FRESULT SDMMC2_Unmount(FIL *pFIL);

FRESULT SDMMC2_WriteFileBin(const char *fname, entry_t *dataEntry,
		size_t *wByeCount);

FRESULT SDMMC2_WriteFileText(const char *fname, const char *data, size_t dataLen,
		size_t *wByeCount);

FRESULT SDMMC2_ReadFile(const char *fname, uint8_t *aBuffer, size_t buffLen,
		size_t *wByeCount);

FRESULT SDMMC2_ReadFileBin(const char *fname, entry_t *dataEntry, size_t nEntries,
		size_t *wByeCount);

FRESULT SDMMC2_GetLastIndexBin(const char *fname, size_t *curIndex);

FSIZE_t fileSize(const char *pFileName);
FRESULT SDMMCDelete(const char *pFilename);

#endif /* INC_MICROSD_H_ */
