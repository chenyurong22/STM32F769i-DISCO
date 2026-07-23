/*
 * custom_callback.c
 *
 *  Created on: Jun 29, 2026
 *      Author: Debasish Das
 */
#include "custom_callback.h"
#include "stm32f7xx_hal.h"
#include  "custom.h"

#include "FreeRTOS.h"
#include "task.h"
#include "cmsis_os.h"
#include "custom_crypto.h"

extern UART_HandleTypeDef huart1;
extern volatile uint8_t rxData;
extern volatile uint8_t rxBuffer[UART_TX_LEN];
extern volatile uint8_t count;

extern TaskHandle_t DecryptTaskHandle;
extern CryptoRx_t CryptoRx;

uint8_t complete = 0;

#define FREEROS 1

extern osThreadId NtpTimeReceiveHandle;
extern TaskHandle_t LwIPLinkHandle;
extern TaskHandle_t SNTP_LinkHandle;
extern TaskHandle_t UART_NtpTimeReceiveHandle;

#define ORIGINAL 0

#if (ORIGINAL == 1)
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	BaseType_t xHighPriorityTaskWoken = pdFALSE;
	if (huart->Instance == USART1)
	{
		rxBuffer[count] = rxData;
		count++;

		if (count == UART_TX_LEN )
		{
			count = 0;
			vTaskNotifyGiveFromISR(DecryptTaskHandle, &xHighPriorityTaskWoken);

			portYIELD_FROM_ISR(xHighPriorityTaskWoken);
		}
		HAL_UART_Receive_IT(&huart1, &rxData, 1);
	}
}
#elif (ORIGINAL == 0)

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	BaseType_t xHighPriorityTaskWoken = pdFALSE;
	if (huart->Instance == USART1)
	{
		Process_CryptRx(&CryptoRx, rxData);

		if (CryptoRx.state == RX_PACKET_READY)
		{
			vTaskNotifyGiveFromISR(DecryptTaskHandle, &xHighPriorityTaskWoken);
			portYIELD_FROM_ISR(xHighPriorityTaskWoken);
		}

		if (CryptoRx.state != RX_PACKET_READY)
		{
			HAL_UART_Receive_IT(&huart1, &rxData, 1);
		}
	}
}
#endif

void process_UART_Receive(const uint8_t *rxBuffer,
		BaseType_t *xHighPriorityTaskWoken)
{
	uint8_t aCmd = rxBuffer[0];

	switch (aCmd )
	{

	case 0xAA:
		vTaskNotifyGiveFromISR(UART_NtpTimeReceiveHandle,
				xHighPriorityTaskWoken);
	break;

	case 0x0C:
		vTaskNotifyGiveFromISR(LwIPLinkHandle, xHighPriorityTaskWoken);
	break;

	case 0x01:
		vTaskNotifyGiveFromISR(MicroSD_mountHandle, xHighPriorityTaskWoken);
	break;

	case 0x02:
		vTaskNotifyGiveFromISR(MicroSD_unmountHandle, xHighPriorityTaskWoken);
	break;

	case 0x03:
		vTaskNotifyGiveFromISR(MicroSD_readHandle, xHighPriorityTaskWoken);
	break;

	case 0x04:
		vTaskNotifyGiveFromISR(MicroSD_writeHandle, xHighPriorityTaskWoken);
	break;

	case 0x05:
		vTaskNotifyGiveFromISR(MicroSD_FileDelHandle, xHighPriorityTaskWoken);
	break;

	default:
		writetoSerial(&huart1, "Default ..\r\n");
	break;

	}
}

void process_CBC_Receive(const uint8_t *rxBuffer, uint8_t *pIV,
		uint8_t *pEncData, uint8_t *pHashIV)
{
	if (rxBuffer == NULL)
	{
		return;
	}
	memcpy(pIV, rxBuffer, IV_LEN_CB);
	memcpy(pEncData, &rxBuffer[IV_LEN_CB], ENC_DATA_LEN);
	memcpy(pHashIV, &rxBuffer[IV_LEN_CB + ENC_DATA_LEN], HASH_IV_LEN);
}

/**
 * @brief  EXTI line detection callbacks.
 * @param  GPIO_Pin Specifies the pins connected EXTI line
 * @retval None
 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;

	if (GPIO_Pin == GPIO_PIN_0)
	{
		HAL_GPIO_TogglePin(GPIOJ, GPIO_PIN_5);

		vTaskNotifyGiveFromISR(DecryptTaskHandle, &xHigherPriorityTaskWoken);

		portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
	}
}
