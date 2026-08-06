/*
 * custom_tasks.c
 *
 *  Created on: Jul 31, 2026
 *      Author: Debasish Das
 */

#include "custom_tasks.h"
#include "stm32f769xx.h"
#include "custom.h"
#include "microSD.h"
#include "myCrypto.h"
#include "Uart_NTP.h"

#define CA_CERT_LEN 555U


extern uint8_t signBuff[80];
extern uint8_t certBuff[800];
extern uint8_t firmwareBuff[200];
extern uint8_t firmwareHash[SHA256_LEN];
extern size_t byteRead;
extern FIL pFile;

extern uint8_t NTPRxBuff[NTP_BUFF_SIZE];
extern UART_HandleTypeDef huart1;
extern const uint8_t CA_certificate_der[CA_CERT_LEN];

extern TaskHandle_t SyncNTPTaskHandle;
extern TaskHandle_t VerifySignTaskHandle;

void StartSyncNTPTask(void *argument)
{
	BaseType_t xHighPriorityTaskWoken = pdFALSE;

	for (;;)
	{
		ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
		writeFormatData(&huart1, "[DEBUG] %s \r\n", __func__);

		/* NTP Synchronisation */
		NTP_ProcessReceivedTime(NTPRxBuff, NTP_FRAME_LEN);

		//xTaskNotifyGive(VerifySignTaskHandle);
		vTaskDelay(pdMS_TO_TICKS(500));
	}
}


void StartVerifySignTask(void *argument)
{
	BaseType_t xHighPriorityTaskWoken = pdFALSE;
	writeFormatData(&huart1, "[DEBUG] %s \r\n", __func__);

	for (;;)
	{
		ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
		writeFormatData(&huart1, "[DEBUG] %s \r\n", __func__);

		/* Verifying Signature starts */
		size_t SignLen = 0;
		size_t CertLen = 0;
		size_t FirmLen = 0;
		int writeStatus = 0;
		int status = 0;

		TurnGreenLED_ON();
		/* Storing  CA Self signed Certificate in Flash if already not there */
		Write_Certificate_Flash((uint8_t*) CA_CERT_ADDR,
				CA_certificate_der,
				sizeof(CA_certificate_der),
				true); 						/* Note ✅  */

		if (CryptoData_ReadFromSdCard(&SignLen, &CertLen, &FirmLen) != FR_OK)
		{
			writetoSerial(&huart1, "[❌] Error Reading Crypto Files \r\n");
			break;
		}
		writetoSerial(&huart1, "[✔] Success Reading Crypto Files \r\n");

		/*******************Verify Developer Certificate uisng CA CErtificate ********************/
		mbedtls_x509_crt devCert;
		mbedtls_x509_crt_init(&devCert);
		uint32_t ErrCode = 0;

		size_t ca_cert_len = CA_CERT_LEN;
		status = Verify_Certificate((uint8_t*) CA_CERT_ADDR, /* From Flash */
		ca_cert_len, certBuff, CertLen, &ErrCode, &devCert);

		if (status != HAL_OK)
		{
			writetoSerial(&huart1,
					"[❌] Developer Certificate Verification Failed \r\n");
			break;
		}
		writetoSerial(&huart1,
				"[✔] Developer Certificate Verification Succeed \r\n");

		/* Extract Developer Public key */
		mbedtls_pk_context *pk = &devCert.pk;

		/* Return Key Type */
		switch (mbedtls_pk_get_type(pk) )
		{

		case MBEDTLS_PK_ECKEY:
			writetoSerial(&huart1, "[ℹ]  Key Type: EC\r\n");
			break;

		case MBEDTLS_PK_RSA:
			writetoSerial(&huart1, "[ℹ]  Key Type: RSA \r\n");
			break;

		case MBEDTLS_PK_ECDSA:
			writetoSerial(&huart1, "[ℹ]  Key Type: ECDSA \r\n");
			break;

		default:
			writetoSerial(&huart1, "[ℹ]  Key Type: Default \r\n");
			break;
		}

		/* Getting Public Key in uncompressed format (for Debug purspose) */
		uint8_t PubKeyUncomp[100];
		size_t oKeyLenUnComp = 0;

		mbedtls_ecp_keypair *ec = mbedtls_pk_ec(*pk);

		/* Export Uncompressed SEC1 Format */
		int ret = mbedtls_ecp_point_write_binary(&ec->grp, &ec->Q,
		MBEDTLS_ECP_PF_UNCOMPRESSED, &oKeyLenUnComp, PubKeyUncomp, /* For Debug purpose. Developer Publik key is passed as &devCert.pk below */
		sizeof(PubKeyUncomp));

		if (ret != 0)
		{
			writetoSerial(&huart1,
					"[❌] Error parsing Developer Public Key \r\n");
			break;
		}
		writetoSerial(&huart1, "[✔] Success parsing Developer Public Key \r\n");

		/* Verifying signed Firmware hash using Developers public key passsed as &devCert.pk below */
		ret = mbedtls_pk_verify(&devCert.pk,
				MBEDTLS_MD_SHA256,
				firmwareHash, /* Calculated Firmware Hash */
				SHA256_LEN,
				signBuff, /* Firmware hash signed by Developer private key */
				SignLen);

		if (ret != 0)
		{
			writetoSerial(&huart1, "[❌] Firmware Verification Failed \r\n"); /* U+274C */
			break;
		}
		writetoSerial(&huart1, "[✔] Firmware Verification Succeed \r\n"); /* U+2705 */
		TurnGreenLED_OFF();

		vTaskDelay(pdMS_TO_TICKS(500));
	}
}


/* Reset STM32 Board */
void StartResetBoardTask(void *argument)
{
	BaseType_t xHighPriorityTaskWoken = pdFALSE;
	for (;;)
	{
		ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
		writeFormatData(&huart1, "[DEBUG] %s \r\n", __func__);
		writetoSerial(&huart1, "[✔] Software Reboot STM32 Board \r\n"); /* U+2757 */

		TurnGreenLED_ON();
		HAL_Delay(500);
		TurnGreenLED_OFF();

		NVIC_SystemReset();

		vTaskDelay(pdMS_TO_TICKS(500));
	}
}
