/*
 * customTasks.c
 *
 *  Created on: Apr 15, 2026
 *      Author: Debasish Das
 */
#include "customTasks.h"
#include "microSD.h"
#include "stdint.h"
#include "custom.h"
#include "customTrace.h"
#include "customSNTP.h"
#include "lwip.h"
#include "customLwIP.h"

#include "lwip/sockets.h"

entry_t logEntry;
entry_t readEntry;
entry_t writeEntry;

extern xQueueHandle logDataQueue;

/* USER CODE BEGIN Header_StartmountSDMMCTask */
void StartmountSDMMCTask(void *argument)
{
	uint16_t loopCount = 0;

	for (;;)
	{
		if (SDMMC2_mount() == FR_OK)
		{
			loopCount++;

			if (loopCount == 5)
			{
				writetoSerial(&huart1, "SD card mount Successful \r\n");
				vTaskDelete(NULL);
			}
		}
		else
		{
			writetoSerial(&huart1, "Mount Failed ! \r\n");
		}

		vTaskDelay(pdMS_TO_TICKS(500));
	}
}

/* USER CODE BEGIN StartLwIPInitHandle */
extern struct netif gnetif;
extern TaskHandle_t SNTP_LinkHandle;

void StartLwIPLinkHandle(void *argument)
{
	uint16_t loopCount = 0;

	/* Resume after Receiving  notification from HAL_UART_RxCpltCallback */
	ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

	for (;;)
	{
		if (netif_is_link_up(&gnetif))
		{
			if (netif_is_up(&gnetif))
			{
				if (!ip4_addr_isany_val(*netif_ip4_addr(&gnetif)))
				{
					writeFormatData(&huart1, "IP address: %s\r\n",
							ip4addr_ntoa(netif_ip4_addr(&gnetif)));

					loopCount++;
					if (loopCount > 10)
					{
						/* Block StartLwIPSntpHandle task till LwIP is up */
						xTaskNotifyGive(SNTP_LinkHandle);

						vTaskDelete(NULL);
					}
				}
				else
				{
					writetoSerial(&huart1, "Waiting for IP...\r\n");
				}
			}
			else
			{
				writetoSerial(&huart1, "Interface not up yet\r\n");
			}
		}
		else
		{
			writetoSerial(&huart1, "Cable disconnected\r\n");
		}
		vTaskDelay(pdMS_TO_TICKS(250));
	}
}

/* USER CODE BEGIN StartLwIPSntptHandle */

void StartSNTP_LinkHandle(void *argument)
{

	/* Resume after Receiving  notification from StartLwIPLinkHandle */
	ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

	/* Wait for network to become UP */
	while (!netif_is_up(&gnetif))
	{
		vTaskDelay(pdMS_TO_TICKS(500));
	}
	SNTP_Init();

	for (;;)
	{
		vTaskDelay(pdMS_TO_TICKS(500));
	}
}

/* USER CODE BEGIN StartLwIPSntptHandle */
extern struct netif gnetif;
void StartUARTLinkHandle(void *argument)
{

	/* Resume after Receiving  notification from StartLwIPLinkHandle */
	ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

	/* Wait for network to become UP */
	while (!netif_is_up(&gnetif))
	{
		vTaskDelay(pdMS_TO_TICKS(500));
	}
	SNTP_Init();

	for (;;)
	{
		vTaskDelay(pdMS_TO_TICKS(500));
	}
}

/* USER CODE BEGIN StartLwIPClientHandle */

#define SERV_PORT 5050
void StartLwIPClientHandle(void *argument)
{
	int sockID;
	struct sockaddr_in serverAddr;
	static int clientPort = 0;

	char serverMsg[80];
	int timeStamp = 0;

	timeStruct_t recvTimeStruct;

	static int newConnect = 0;
	static int lastConnect = 0;

	writetoSerial(&huart1, "Waiting for network become high.. \r\n");

	/* Wait for network to become UP */
	while (!netif_is_up(&gnetif))
	{
		vTaskDelay(pdMS_TO_TICKS(500));
	}
	writetoSerial(&huart1, "Network is Up \r\n");

	for (;;)
	{
		/* Creating TCP_IP socket in */
		sockID = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

		if (sockID < 0)
		{
			writetoSerial(&huart1, "Socket creation failed \r\n");
			vTaskDelay(pdMS_TO_TICKS(1000));
			continue;
		}

		/* Configuring server address */
		/* Clearing the socket address field */
		memset((struct sockaddr_in*) &serverAddr, 0,
				sizeof(struct sockaddr_in));

		serverAddr.sin_family = AF_INET;
		serverAddr.sin_port = htons(SERV_PORT);
		serverAddr.sin_addr.s_addr = inet_addr("192.168.0.8");

		/* Connecting to the server */
		clientPort = connect(sockID, (struct sockaddr* )&serverAddr,
				sizeof(serverAddr));

		if (clientPort < 0 && newConnect == 0)
		{
			writetoSerial(&huart1, "Connection to the server Failed! \r\n");
			close(sockID);
			vTaskDelay(pdMS_TO_TICKS(3000));
			lastConnect = 0;
			continue;
		}

		newConnect = 1;
		if (lastConnect == 0 && newConnect == 1)
		{
			lastConnect = 1;
			writetoSerial(&huart1, "Connection to the server succeed :) \r\n");
		}

		/* Receive message from connected Server */
		int len = recv(clientPort, &recvTimeStruct, sizeof(recvTimeStruct), 0);

		if (len > 0)
		{
			len = snprintf(serverMsg, sizeof(serverMsg), "[%d] [%s]",
					recvTimeStruct.frameCount, recvTimeStruct.timeStr);
			serverMsg[len] = '\0';

			writetoSerial(&huart1, serverMsg);
			writetoSerial(&huart1, "\r\n");
		}

		vTaskDelay(pdMS_TO_TICKS(500));

		/* Close the socket descriptor for next connect */
		shutdown(sockID, SHUT_RDWR);
		close(sockID);

		/* Wait before reconnect */
		vTaskDelay(pdMS_TO_TICKS(2000));
	}
}

/* USER CODE BEGIN Header_StartreadSDCardTask */
void StartreadSDCardTask(void *argument)
{

	for (;;)
	{
		vTaskDelay(pdMS_TO_TICKS(500));
	}
}

/* USER CODE BEGIN Header_StartshowSDCardTask */
extern xQueueHandle logPspQueue;
void StartshowSDCardTask(void *argument)
{
	traceLog_t readLog;
	BaseType_t pxHigherPriorityTaskWoken;

	char *name;
	UBaseType_t taskPriority;

	for (;;)
	{
		if (xQueueReceiveFromISR(logPspQueue, &readLog,
				&pxHigherPriorityTaskWoken) == pdPASS)
		{

			name = pcTaskGetTaskName((TaskHandle_t) readLog.currTaskHandle);
			taskPriority = uxTaskPriorityGet(
					(TaskHandle_t) readLog.currTaskHandle);

			writeFormatData(&huart1,
					"[%-15s:%2d] [R0:0x%lX] [R1:0x%lX] [R2:0x%lX]"
							"[LR:0x%lX] [PC:0x%lX] [xPSR:0x%lX] \r\n", name,
					taskPriority, readLog.R0, readLog.R1, readLog.R2,
					readLog.LR, readLog.PC, readLog.xPSR);
		}

		vTaskDelay(pdMS_TO_TICKS(500));
	}
}

/* USER CODE BEGIN Header_StartwriteSDCardTask */
void StartwriteSDCardTask(void *argument)
{
	const char *msg = "Data sent to MicroSD card\r\n";
	static uint16_t logCount = 0;

	/* Infinite loop */
	for (;;)
	{
		strcpy(logEntry.data, msg);
		logEntry.index = logCount;
		logEntry.DiskOp = WRITE_LOG;

		if (xQueueSend(logDataQueue, &logEntry,portMAX_DELAY) != pdPASS)
		{
			writetoSerial(&huart1, "******** FULL **************** \r\n");
			vTaskDelay(pdMS_TO_TICKS(100));
		}
		logCount++;

		vTaskDelay(pdMS_TO_TICKS(10));
	}
}

/* USER CODE BEGIN Header_StartSDFileOperstionTask */
extern TaskHandle_t mountSDMMCTaskHandle;
void StartSDFileOperationTask(void *argument)
{
	entry_t recvEntry;
	size_t byteCount = 0;
	static uint16_t indx = 0;

	for (;;)
	{
		if (xQueueReceive(logDataQueue, &recvEntry, portMAX_DELAY) == pdPASS)
		{
			if (recvEntry.DiskOp == WRITE_LOG)
			{
//				SDMMC2_WriteFile("Output.txt", &recvEntry, &byteCount);
//				writeFormatData(&huart1, "FileS size: %d bytes\r\n", byteCount);
//				indx++;
			}
		}

		vTaskDelay(pdMS_TO_TICKS(100));
	}
}
/* Task from other source */

/* Global Queue for sharing Time information */

QueueHandle_t uinxTimeISTQueue;

/* USER CODE END Header_StartNtpTimeReceive */
void StartUART_NtpTimeReceive(void *argument)
{
	/* USER CODE BEGIN StartNtpTimeReceive */
	RTC_TimeTypeDef timeInfo;

	/* Crate a Queue to share Time information to StartSetRTCTime task */
	uinxTimeISTQueue = xQueueCreate(5, sizeof(uint32_t));

	/* Infinite loop */
	for (;;)
	{
		/* 🔥 DO NOT include any blocking API before ulTaskNotifyTake() ⛔ */
		ulTaskNotifyTake(pdTRUE, portMAX_DELAY); /* WAIT for the notification ✍. Holds*/

		/* Make StartSetRTCTime READY */
		vTaskResume(UART_NtpTimeSetHandle);

		/* Convert to IST epoch */
		uint32_t IST_epoch = getIST(rxBuffer);

		/* Encapsulate is time structure */
		EpochToRtcTime(IST_epoch, &timeInfo);

		/* Sending Time information to Queue */
		xQueueSend(uinxTimeISTQueue, &IST_epoch, portMAX_DELAY);

		/* ✍ Notify StartSetRTCTime task to unblock */
		xTaskNotifyGive(UART_NtpTimeSetHandle);

		vTaskDelay(pdMS_TO_TICKS(500));
	}
	/* USER CODE END StartNtpTimeReceive */
}

/* USER CODE BEGIN Header_StartSetRTCTime */
/**
 * @brief Function implementing the SetRTCTime thread.
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_StartSetRTCTime */
void StartUART_NtpTimeSet(void *argument)
{
	/* USER CODE BEGIN StartSetRTCTime */

	uint32_t receivedTime;
	/* Infinite loop */
	for (;;)
	{
		/* Wait for StartNtpTimeReceive task to Notify */
		ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

		/* Receiving Time information */
		if (xQueueReceive(uinxTimeISTQueue, &receivedTime,
		portMAX_DELAY) == pdPASS)
		{
			setRtcTime_IST(receivedTime);
		}

		//writeFormatData(&huart1, "Running [%s] \r\n", __func__);
		xTaskNotifyGive(Display_DeviceTimeHandle);

		/* Suspend current task after one execution */
		vTaskSuspend(NULL);
	}
	/* USER CODE END StartSetRTCTime */
}

/* USER CODE BEGIN Header_StartDisplay_DeviceTime */
/**
 * @brief Function implementing the VerifyRtcTime thread.
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_StartDisplay_DeviceTime */
void StartDisplay_DeviceTime(void *argument)
{
	/* USER CODE BEGIN StartDisplay_DeviceTime */
	ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

	/* Infinite loop */
	for (;;)
	{
		/* Verify RTC time has been Set */
		//verifyRTCTime();

		vTaskDelay(pdMS_TO_TICKS(500));
	}
	/* USER CODE END StartDisplay_DeviceTime */
}

/* USER CODE BEGIN Header_StartReset_Device */
/**
 * @brief Function implementing the StartReset_Device thread.
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_StartReset_Device */
void StartReset_Device(void *argument)
{
	/* USER CODE BEGIN StartDisplay_DeviceTime */

	/* Infinite loop */
	for (;;)
	{
		ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
		writetoSerial(&huart1, "Resetting STM32 device ..\r\n");

		NVIC_SystemReset();
		vTaskDelay(pdMS_TO_TICKS(500));
	}
	/* USER CODE END StartDisplay_DeviceTime */
}

/* USER CODE BEGIN StartMicroSD_mount */
/**
 * @brief Function implementing the StartMicroSD_mount thread.
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END StartMicroSD_mount */
void StartMicroSD_mount(void *argument)
{

	for (;;)
	{
		ulTaskNotifyTake(pdTRUE, portMAX_DELAY); /* WAIT for the notification ✍. Holds*/

		if (SDMMC2_mount() == FR_OK)
		{
			writetoSerial(&huart1, "[STATUS] : SD card mount Successful \r\n");
		}
		else
		{
			writetoSerial(&huart1, "[STATUS] : Mount Failed ! \r\n");
		}

		vTaskDelay(pdMS_TO_TICKS(500));
	}
}

/* USER CODE BEGIN StartMicroSD_unmount */
/**
 * @brief Function implementing the StartMicroSD_unmount thread.
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END StartMicroSD_unmount */

extern FIL fp;
void StartMicroSD_unmount(void *argument)
{
	for (;;)
	{
		ulTaskNotifyTake(pdTRUE, portMAX_DELAY); /* WAIT for the notification ✍. Holds*/

		SDMMC2_Unmount(&fp);
		vTaskDelay(pdMS_TO_TICKS(500));
	}
}

/* USER CODE BEGIN StartMicroSD_read */
/**
 * @brief Function implementing the StartMicroSD_read thread.
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END StartMicroSD_read */

#define LOG_FORMAT_BIN 1
#define LOG_FORMAT_ASCII 0
#define LOG_FORMAT LOG_FORMAT_BIN

const char *pFileName = "TimeLog.txt";
const char *pFileNameBin = "TimeLog.bin";

void StartMicroSD_read(void *argument)
{
	uint8_t aFileRead[28];
	size_t textLen = 27;
	size_t bByeteCount = 0;
	FRESULT status;

#if LOG_FORMAT == LOG_FORMAT_BIN

	size_t logIndex = 0;
	entry_t dataEntry;
	size_t nEntries = 0;
#endif

	for (;;)
	{
		ulTaskNotifyTake(pdTRUE, portMAX_DELAY); /* WAIT for the notification ✍. Holds*/

		/* Get last entry from the file */
		SDMMC2_GetLastIndexBin(pFileNameBin, &logIndex); /* ✅ */

#if defined LOG_FORMAT == LOG_FORMAT_ASCII

		writetoSerial(&huart1, "Reading text Log file ..\r\n");
		SDMMC2_ReadFile(pFileName, aFileRead, textLen, &bByeteCount);

#elif defined LOG_FORMAT == LOG_FORMAT_BIN

		writetoSerial(&huart1, "[STATUS] : Reading binary Log file ..\r\n");
		status = SDMMC2_ReadFileBin(pFileNameBin, &dataEntry, nEntries, &bByeteCount);

		if(status == FR_OK)
		{
			writeFormatData(&huart1, "[STATUS] : Total Bytes: %d \r\n", bByeteCount);
		}

#endif
		vTaskDelay(pdMS_TO_TICKS(500));
	}
}

/* USER CODE BEGIN StartMicroSD_write */
/**
 * @brief Function implementing the StartMicroSD_write thread.
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END StartMicroSD_write */

void StartMicroSD_write(void *argument)
{
	const char aCurTime[50];
	const char aCurDate[50];
	const char aLogTime[100];
	size_t dataLen = 0;
	size_t wByteWritten = 0;

#ifdef LOG_FORMAT_BIN
	entry_t logEntry =
	{ 0 };
	size_t logIndex = 0;
	DiskOperation_t DiskOper = NOP;

#endif

	for (;;)
	{
		ulTaskNotifyTake(pdTRUE, portMAX_DELAY); /* WAIT for the notification ✍. Holds*/

		/* Get last log index */
		SDMMC2_GetLastIndexBin(pFileNameBin, &logIndex); /* ✅ */

		/* Retrieve Current Time */
		getCurrentTime(aCurTime, aCurDate); /*  ✅ */

#if LOG_FORMAT == LOG_FORMAT_BIN
		strcpy(logEntry.data, aCurTime);
		logEntry.index = logIndex + 1;
		logEntry.DiskOp = WRITE_LOG;

		SDMMC2_WriteFileBin(pFileNameBin, &logEntry, &wByteWritten);
		writeFormatData(&huart1, "[STATUS] : Data written to [%s] \r\n", pFileNameBin);

#elif LOG_FORMAT == LOG_FORMAT_ASCII

		dataLen = snprintf(aLogTime, sizeof(aLogTime), "Recorded Time:[%s] \r\n", aCurTime);
		SDMMC2_WriteFileText(pFileName, aLogTime, dataLen, &wByteWritten);
		writeFormatData(&huart1, "Recorded Time:[%s]  Byte written: [%d] Size: [%d] \r\n",
				aCurTime,
				wByteWritten,
				fileSize(pFileName));
#else
#error "Invalid LOG FORMAT"
#endif
		vTaskDelay(pdMS_TO_TICKS(500));
	}
}

/* Deleting Log File */
void StartMicroSD_FileDel(void *argument)
{
	const char *msg = "ফাইলটি মুছে ফেলা হয়েছে।\r\n";

#if LOG_FORMAT == LOG_FORMAT_ASCII
	pFileName = "TimeLog.txt";
#elif LOG_FORMAT == LOG_FORMAT_BIN
	pFileName = "TimeLog.bin";
#endif

	for (;;)
	{
		ulTaskNotifyTake(pdTRUE, portMAX_DELAY); /* WAIT for the notification ✍. Holds*/

		writetoSerial(&huart1, "[STATUS] : Deleting log file ..✍ \r\n");
		if (SDMMCDelete(pFileName) == FR_OK)
		{
			writeFormatData(&huart1, "[STATUS] : Log File %s deleted \r\n", pFileName);
		}
		vTaskDelay(pdMS_TO_TICKS(500));
	}
}
