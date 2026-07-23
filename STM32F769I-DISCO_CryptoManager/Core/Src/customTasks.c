/*
 * customTasks.c
 *
 *  Created on: Apr 15, 2026
 *      Author: Debasish Das
 */
#include "customTasks.h"
#include "stdint.h"
#include "custom.h"
#include "lwip.h"
#include "lwip/sockets.h"
#include "custom_callback.h"
#include "custom_crypto.h"

extern CryptoRx_t CryptoRx;
extern volatile uint8_t rxData;

uint8_t aIV[16];
uint8_t aEnc[32];
uint8_t aDec[32];
uint8_t aHash256Recv[HASH_IV_LEN];
uint8_t aHash256Comp[HASH_IV_LEN];

uint8_t key[16] = { 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88,
                    0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F, 0xAA, 0xBB
				  };

BaseType_t xHighPriorityTaskWoken;

void StartDecryptTask(void *argument)
{
	for (;;)
	{
		CryptoRx_t *pCrypto = (CryptoRx_t *)argument;

		/* Wait for Notification */
		ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

		/* Generating the hash of the key */
		hashParameter(pCrypto->IV, IV_LEN, aHash256Comp);

		if (memcmp(pCrypto->Hash, aHash256Comp, HASH_IV_LEN) != 0)
		{
			writetoSerial(&huart1, "IV has been compromised \r\n");
			continue;
		}

		writetoSerial(&huart1, "IV verified Successfully ! \r\n");

		/* Decrypt the massage */
		decryptMsgCBC(pCrypto->Cipher, pCrypto->cipherLen, aDec, key, pCrypto->IV);

		/* Display the Decrypted message */
		writeHextoSerial(&huart1, "Decrypted Message: ", aDec, pCrypto->cipherLen);

		/* Filter unpadded message */

		size_t MsgLen = 0U;
		if(CryptoEngineGetPlaintextLength(aDec, pCrypto->cipherLen, &MsgLen) == false)
		{
			continue;
		}
		writeHextoSerial(&huart1, "Original Message:  ", aDec, MsgLen );

		/* Re-Enable UART Interrupt */
		HAL_UART_Receive_IT(&huart1, &rxData, 1);

		/* Re-Initialize Crypto Engine */
		Init_CryptoRx_Process(pCrypto);

		vTaskDelay(pdMS_TO_TICKS(500));
	}
}

