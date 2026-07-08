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

extern FATFS SDFatFS; /* Make it external variable  */

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

/* Global File pointer */
FIL fp;

FRESULT SDMMC2_mount()
{
	FRESULT res;
	res = f_mount(&SDFatFS, "0:", 1);
	return res;
}

extern const char *pFileName;

FRESULT SDMMC2_Unmount(FIL *pFIL)
{
	FRESULT res;

	res = f_mount(NULL, "0:", 1);

	if (res != FR_OK)
	{
		writetoSerial(&huart1, "[STATUS] : SD Card Un-mounting failed \r\n");
	}
	else
	{
		writetoSerial(&huart1, "[STATUS] : SD Card Un-mounting succeed \r\n");
	}

	return res;
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
	FRESULT result;

	if(pFilename == NULL)
	{
		return FR_INVALID_NAME;
	}

	result = f_unlink(pFilename);

	if (result == FR_NO_FILE)
	{
		writetoSerial(&huart1, "[STATUS] : File Does not exists \r\n");
		return result;
	}

	if (result != FR_OK)
	{
		writetoSerial(&huart1, "[STATUS] : Error deleting file\r\n");
		return result;
	}

	return result;
}

FRESULT SDMMC2_WriteFileBin(const char *fname, entry_t *dataEntry,
		size_t *wByeCount)
{
	FIL fp;
	FRESULT status;
	UINT bytesWritten = 0;

	status = f_open(&fp, fname, FA_WRITE | FA_OPEN_ALWAYS);

	if (status != FR_OK)
	{
		writetoSerial(&huart1, "[STATUS] : Error WRITING/CREATING binary file !\r\n");
		return FR_DISK_ERR;
	}

	/* Moving to the end of the file for append mode */
	if (f_lseek(&fp, f_size(&fp)) != FR_OK)
	{
		writetoSerial(&huart1, "[STATUS] : Error seek file end! \r\n");
		return FR_INT_ERR;
	}

	/* Writing to Binary File */
	status = f_write(&fp, dataEntry, sizeof(entry_t), &bytesWritten);

	if (status != FR_OK || bytesWritten == 0)
	{
		writetoSerial(&huart1, "[STATUS] : Error writing to binary file !\r\n");
		f_close(&fp);
		return FR_DISK_ERR;
	}

	/* Ensure Data is flushed into SDMMC */
	f_sync(&fp);
	f_close(&fp);

	/* Returning number of byte written */
	*wByeCount = bytesWritten;

	return status;
}


FRESULT SDMMC2_WriteFileText(const char *fname, const char *data, size_t dataLen,
		size_t *wByeCount)
{
	FIL fp;
	FRESULT status;
	UINT bytesWritten = 0;

	status = f_open(&fp, fname, FA_WRITE | FA_OPEN_ALWAYS);

	if (status != FR_OK)
	{
		writetoSerial(&huart1, "[STATUS] : Error WRITING/CREATING text file !\r\n");
		return FR_DISK_ERR;
	}

	/* Moving to the end of the file for append mode */
	if (f_lseek(&fp, f_size(&fp)) != FR_OK)
	{
		writetoSerial(&huart1, "Error seek file end! \r\n");
		return FR_INT_ERR;
	}

	/* Writing to File */
	status = f_write(&fp, data, dataLen, &bytesWritten);

	if (status != FR_OK || bytesWritten == 0)
	{
		writetoSerial(&huart1, "[STATUS] : Error writing to file !\r\n");
		f_close(&fp);
		return FR_DISK_ERR;
	}

	/* Ensure Data is flushed into SDMMC */
	f_sync(&fp);
	f_close(&fp);

	/* Returning number of byte written */
	*wByeCount = bytesWritten;

	return status;
}

FRESULT SDMMC2_ReadFile(const char *fname, uint8_t *aBuffer, size_t buffLen,
		size_t *wByeCount)
{
	FIL fp;
	FRESULT status;
	UINT bytesRead;
	FSIZE_t fileSize = 0;
	FSIZE_t blockSize = 27;

	size_t offset = 0;
	status = f_open(&fp, fname, FA_READ);

	if (status == FR_OK)
	{
		fileSize = f_size(&fp);

		while(offset != fileSize)
		{

		/* Reading from File */
		status = f_read(&fp, aBuffer, blockSize, &bytesRead); /* Read Entire file */
		aBuffer[bytesRead] = '\0';

		writeFormatData(&huart1, "%s \r", aBuffer);
		offset += bytesRead;
		}

		if ((status == FR_OK) && (offset == fileSize))
		{
			f_sync(&fp);
			f_close(&fp);
			writetoSerial(&huart1, "[STATUS] : Log file read Complete !\r\n");
		}
	}

	*wByeCount = offset;
	return status;
}

/* Reading from Binary File */
FRESULT SDMMC2_ReadFileBin(const char *fname, entry_t *dataEntry,
		size_t nEntries, size_t *wByeCount)
{
	FIL fp;
	FRESULT status;
	UINT bytesRead;
	FSIZE_t fileSize = 0;

	size_t offset = 0;
	status = f_open(&fp, fname, FA_READ);

	if(status == FR_NO_FILE)
	{
		writetoSerial(&huart1, "[STATUS] : File Does not exists \r\n");
		*wByeCount = 0;
		return status;
	}

	if (status == FR_OK)
	{
		fileSize = f_size(&fp);

		while (offset != fileSize)
		{
			/* Reading from File */
			status = f_read(&fp, dataEntry, sizeof(entry_t), &bytesRead); /* Read Entire file */
			writeFormatData(&huart1, "Index: %d %s  %d\r",
					dataEntry->index,
					dataEntry->data,
					sizeof(entry_t));

			offset += sizeof(entry_t);
		}

		writetoSerial(&huart1, "\r\n");

		if ((status == FR_OK) && (offset == fileSize))
		{
			f_sync(&fp);
			f_close(&fp);
			writetoSerial(&huart1, "[STATUS] : Binary Log file read Complete\r\n");
		}
	}

	*wByeCount = offset;
	return status;
}

/* Get last Index in File */

FRESULT SDMMC2_GetLastIndexBin(const char *fname, size_t *curIndex)
{
	FIL fp;
	FRESULT status;
	FSIZE_t fileSize;
	size_t wByteRead = 0;
	entry_t dataEntry;


	status = f_open(&fp, fname, FA_READ);
	if(status == FR_NO_FILE)
	{
		writeFormatData(&huart1, "[STATUS] : File %s does not exists \r\n", fname);
		*curIndex = 0;
		return status;
	}

	if(status != FR_OK)
	{
		writeFormatData(&huart1, "[STATUS] : Unable to open file %s in %s() \r\n", fname, __func__);
		f_close(&fp);
		return status;
	}

	/* Get File size */
	fileSize = f_size(&fp);

	if(fileSize < sizeof(entry_t))
	{
		f_close(&fp);
		writetoSerial(&huart1, "Invalid Object \r\n");

		return FR_INVALID_OBJECT;
	}

	if(fileSize >= sizeof(entry_t))
	{
		f_lseek(&fp, fileSize - sizeof(entry_t));
	}

	status = f_read(&fp, &dataEntry, sizeof(entry_t), &wByteRead);

	if((status != FR_OK) || (wByteRead != sizeof(entry_t)))
	{
		f_close(&fp);
		writetoSerial(&huart1, "FR_INT_ERR \r\n");

		return (status != FR_OK) ? status : FR_INT_ERR;
	}

	f_close(&fp);
	*curIndex = dataEntry.index;
}



