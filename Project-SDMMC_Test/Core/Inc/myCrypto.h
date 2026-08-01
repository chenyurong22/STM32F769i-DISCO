/*
 * myCrypto.h
 *
 *  Created on: Jul 28, 2026
 *      Author: debasish
 */

#ifndef INC_MYCRYPTO_H_
#define INC_MYCRYPTO_H_

#include "microSD.h"
#include "stm32f7xx_hal.h"
#include "fatfs.h"
#include "custom.h"
#include <stdio.h>


#include <mbedtls/certs.h>
#include <mbedtls/x509.h>
#include <mbedtls/x509_crt.h>
#include <mbedtls/pk.h>
#include <mbedtls/sha256.h>

#define CA_CERT_ADDR  	0x08180000UL
#define SHA256_LEN 		32U
#define NTP_FRAME_LEN 	9U

/* NTP Data Receive State machine */

typedef enum
{
	WAIT_LI_VM_MODE,
	WAIT_STRATUM,
	WAIT_POLL_INT,
	WAIT_PRECISION,
	WAIT_DATA,
	WAIT_END_BYTE,
	RX_PACKET_READY,
	RX_ERROR,
	WAIT_COMPLETE
} NTPState;

#define NTP_BUFF_SIZE 		9U
#define MAX_ERR_MSG_LEN		80U

typedef struct
{
	NTPState state;
	size_t index;
	uint8_t NTPBuff[NTP_BUFF_SIZE];
	uint8_t errMsg[MAX_ERR_MSG_LEN];
	bool isComplete;
	uint32_t flag;
} NtpBuffRx_t;

#define LI_VM_MODE	 	0x1C
#define STRATUM  		0x01
#define POLL_INT		0x0D
#define PRECION			0xE3
#define STM32_RST		0xEE
#define STM32_SIG		0xDE
#define END_BYTE		0xFF

#define ASSERT_FRESULT_OK(status)	\
							do {	\
								if ( (status) != FR_OK)	\
								 {						\
								writetoSerial(&huart1, "Error Reading File !\r\n");\
								return (status);	\
								 }					\
							}while(0);


FRESULT CryptoData_ReadFromSdCard(size_t *pSignLen,
								size_t *pCertLen,
								size_t *pFirmLen);

/* Taken from Eclispe Project */
#define MAX_SIG_LEN 100
#define MAX_HASH_LEN 32

/* Function Prototypes */
int Verify_Certificate (const uint8_t *pCACertBuff,
						size_t CACertBuffLen,
						const uint8_t *pDevCertBuff,
						size_t DevCertBuffLen,
						 uint32_t *ErrCode,
						 mbedtls_x509_crt *devCert);

void Read_File_Signature(const char *pSigFilePath, uint8_t *SigBuff, size_t *SigLen);
void Read_File_Hash(const char *pHashFilePath, uint8_t *HashBuff, size_t *HashLen);

int Calculate_File_SHA256(const char *filename,
                          uint8_t hash[32]);

void WritetoFlash(uint8_t *pAddr, uint8_t *pData, size_t dataLen);
uint8_t EraseFlash(uint32_t SectorNum);
void Write_Certificate_Flash(uint8_t *pAddr, uint8_t *pData, size_t dataLen, bool checkByte);

static int Verify_Callback(void *ctx,
                           mbedtls_x509_crt *crt,
                           int depth,
                           uint32_t *flags);

void Calculate_SHA256(const uint8_t *data,
                      size_t data_len,
                      uint8_t hash[SHA256_LEN]);

void Init_NTPByte_Receive(NtpBuffRx_t *rx);
void NTP_ReceiveStateMachine(NtpBuffRx_t *rx, uint8_t data);


#endif /* INC_MYCRYPTO_H_ */
