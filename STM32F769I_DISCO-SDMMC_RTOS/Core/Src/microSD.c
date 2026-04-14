/*
 * microSD.c
 *
 *  Created on: Feb 3, 2026
 *      Author: Debasish Das
 */

#include "microSD.h"
#include "stm32f7xx_hal.h"
#include "fatfs.h"
#include "custom.h"
#include <stdio.h>

extern FATFS SDFatFS; /* Make it extern  */

extern SD_HandleTypeDef hsd2;
extern UART_HandleTypeDef huart1;

void showSDcardInfo(SD_HandleTypeDef *hsd)
{
	HAL_SD_CardInfoTypeDef cardInfo;

	if (HAL_SD_GetCardInfo(&hsd2, &cardInfo) != HAL_OK)
	{
		writetoSerial(&huart1, "SD Card information failed \r\n");
	}
	else
	{
		writetoSerial(&huart1, "SD Card information succeed \r\n");
	}
}

FIL file1;

__attribute__((aligned(32)))
          static uint8_t buffer[512];

void SD_Read_Test(void)
{
	FRESULT res;
	UINT bytesRead;

	res = f_mount(&SDFatFS, "0:", 1);
	if (res != FR_OK)
	{
		writetoSerial(&huart1, "File mount Failed \r\n");
		return;
	}

	writetoSerial(&huart1, "Waiting for reading the file ..\r\n");

	res = f_open(&file1, "Readme.txt", FA_READ);
	//res = f_open(&file1, "STM32Key.bin", FA_READ);

	if (res != FR_OK)
	{
		writetoSerial(&huart1, "Error reading binary file !\r\n");
		return;
	}

	res = f_read(&file1, buffer, sizeof(buffer), &bytesRead);

	writeASCIItoSerial(&huart1, ASCII, buffer, bytesRead, "Received Key");

	f_close(&file1);
	f_mount(NULL, "", 0);
}

void readSector0()
{
	uint8_t sector0[512];
	DRESULT dres;
	UNUSED(dres);
	uint8_t bs[512] __attribute__((aligned(32)));

	dres = disk_read(0, sector0, 0, 1);

	DWORD start_lba = sector0[0x1BE + 8] | (sector0[0x1BE + 9] << 8)
			| (sector0[0x1BE + 10] << 16) | (sector0[0x1BE + 11] << 24);

	disk_read(0, bs, start_lba, 1);

}

FRESULT SDMMC2_mount(FIL pFile)
{
	FRESULT res;
	res = f_mount(&SDFatFS, "0:", 1);
	return res;
}

FRESULT SDMMC2_Unmount(FIL *pFIL)
{
	FRESULT res;
	f_close(pFIL);

	res = f_mount(NULL, "0:", 1);

	if (res != FR_OK)
	{
		writetoSerial(&huart1, "SD Card Un-mounting failed \r\n");
		return res;
	}

	return res;
}

FRESULT SDMMC2_ReadFile(const char *fname, uint8_t bType, uint8_t *aBuffer,
		size_t *wByeCount)
{

	FIL fp;
	FRESULT status;
	UINT bytesRead;

	status = f_open(&fp, fname, FA_READ);

	if (status != FR_OK)
	{
		writetoSerial(&huart1, "Error reading binary file !\r\n");
		return status;
	}

	/* Reading from File */
	status = f_read(&fp, aBuffer, sizeof(buffer), &bytesRead);
	f_sync(&fp);
	f_close(&fp);

	if (status != FR_OK)
	{
		writetoSerial(&huart1, "Error reading from file !\r\n");
		return status;
	}

	*wByeCount = bytesRead;

	return status;

}

FRESULT SDMMC2_WriteFile(const char *fname, entry_t *dataEntry,
		size_t *wByeCount)
{

	FIL fp;
	FRESULT status;
	UINT bytesWritten = 0;
	static uint8_t indx = 0;
	char writeBuffer[80];

	status = f_open(&fp, fname, FA_WRITE | FA_OPEN_ALWAYS);

	if (status != FR_OK)
	{
		writetoSerial(&huart1, "Error reading READING/CREATING file !\r\n");
		return FR_DISK_ERR;
	}

	/* Moving to the end of the file for append mode */
	if (f_lseek(&fp, f_size(&fp)) != FR_OK)
	{
		writetoSerial(&huart1, "Error seekink file end! \r\n");
		return FR_INT_ERR;
	}

	/* Writing to File */

	snprintf(writeBuffer, sizeof(writeBuffer), "[Index: %d] [%s] \r\n",
			f_size(&fp) / 64, dataEntry->data);

	status = f_write(&fp, writeBuffer, strlen(writeBuffer), &bytesWritten);

	if (status != FR_OK || bytesWritten == 0)
	{
		writetoSerial(&huart1, "Error writing to file !\r\n");
		f_close(&fp);
		return FR_DISK_ERR;
	}

	/* Ensure Data is flused into SDMMC */
	f_sync(&fp);
	f_close(&fp);

	/* Returning number of byte written */
	*wByeCount = bytesWritten;

	return status;
}

FSIZE_t fileSize(const char *pFileName)
{
	FIL fp;
	FSIZE_t fileSize = 0;

	if (f_open(&fp, pFileName, FA_READ) != FR_OK)
	{
		writetoSerial(&huart1, "Error opening file !\r\n");
		return FR_DISK_ERR;
	}

	fileSize = f_size(&fp);

	f_close(&fp);
	return fileSize;
}

FRESULT SDMMCDelete(const char *pFilename)
{
	FRESULT result = 0;

	result = f_unlink(pFilename);
	if (result != FR_OK)
	{
		writetoSerial(&huart1, "Error deleting file !\r\n");
		return FR_DISK_ERR;
	}

	return result;
}

