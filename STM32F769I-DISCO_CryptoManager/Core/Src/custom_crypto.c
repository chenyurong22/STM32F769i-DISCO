/*
 * custom_crypto.c
 *
 *  Created on: Jul 17, 2026
 *      Author: Debasish Das
 */

#include "custom_crypto.h"
#include "stdint.h"
#include "stdio.h"
#include "mbedtls/aes.h"
#include "mbedtls/entropy.h"
#include "mbedtls/sha256.h"

#include "custom.h"

extern UART_HandleTypeDef huart1;
extern CryptoRx_t CryptoRx;

void decryptMsgCBC(const uint8_t ciperText[16], size_t ciperTextLen,
		uint8_t decryptText[16], const uint8_t key[16], uint8_t iv[16])
{
	mbedtls_aes_context ctx;
	mbedtls_aes_init(&ctx);

	if (mbedtls_aes_setkey_dec(&ctx, key, 128) != 0)
	{
		mbedtls_aes_free(&ctx);
		return;
	}

	mbedtls_aes_crypt_cbc(&ctx,
	MBEDTLS_AES_DECRYPT, ciperTextLen, iv, ciperText, decryptText);
	mbedtls_aes_free(&ctx);
}

void hashParameter(uint8_t *pParam, size_t bParamLen, uint8_t *pHash256)
{
	writetoSerial(&huart1, "Generating Hash256 ..\r\n");
	mbedtls_sha256(pParam, bParamLen, pHash256, 0);
}

void Init_CryptoRx_Process(CryptoRx_t *rx)
{
	memset(rx, 0, sizeof(CryptoRx_t));
	rx->state = RX_WAIT_SYNC1;
}

#define MAX_CIPHER_LEN		200U
#define CIPHER_BLOCK_SIZE 	16U
#define HASH_SIZE 			32U

void Process_CryptRx(CryptoRx_t *rx, uint8_t data)
{
	switch (rx->state )
	{
	case RX_WAIT_SYNC1:
			if (data == PKT_SYNC1)
			{
				rx->state = RX_WAIT_SYNC2;
			}
			break;

	case RX_WAIT_SYNC2:
			if (data == PKT_SYNC2)
			{
				rx->state = RX_WAIT_ALGO;
			}
			else
			{
				rx->state = RX_WAIT_SYNC1;
			}
			break;

	case RX_WAIT_ALGO:
			if (data == (uint8_t)ALGO_AES256)
			{
				rx->state = RX_WAIT_MODE;
			}
			else
			{
				rx->state = RX_ERROR;
			}
			break;

	case RX_WAIT_MODE:
			if (data == (uint8_t)MODE_CBC)
			{
				rx->state = RX_WAIT_LEN1;
			}
			else
			{
				rx->state = RX_ERROR;
			}
			break;

	case RX_WAIT_LEN1:
			rx->cipherLen = ((uint16_t)data << 8);
			rx->state = RX_WAIT_LEN2;
			break;

	case RX_WAIT_LEN2:
			rx->cipherLen |= (uint16_t)data;

			if ((rx->cipherLen == 0) || (rx->cipherLen > MAX_CIPHER_LEN)
					|| (rx->cipherLen % CIPHER_BLOCK_SIZE))
			{
				rx->state = RX_ERROR;
			}
			else
			{
				rx->state = RX_WAIT_IV;
			}
			break;

	case RX_WAIT_IV:
			rx->IV[rx->index++] = (uint8_t) data;
			if (rx->index >= IV_LEN)
			{
				rx->index = 0;
				rx->state = RX_WAIT_CIPHER;
			}
			break;

	case RX_WAIT_CIPHER:
			rx->Cipher[rx->index++] = (uint8_t) data;
			if (rx->index >= rx->cipherLen)
			{
				rx->index = 0;
				rx->state = RX_WAIT_HASH;
			}
			break;

	case RX_WAIT_HASH:
			rx->Hash[rx->index++] = (uint8_t) data;
			if (rx->index >= HASH_SIZE)
			{
				rx->index = 0;
				rx->state = RX_PACKET_READY;
			}
			break;

	case RX_PACKET_READY:
			break;

	case RX_ERROR:
			Init_CryptoRx_Process(rx);
			break;

	default:
			Init_CryptoRx_Process(rx);
			break;
	}
}

bool CryptoEngineGetPlaintextLength(uint8_t *pDecPad,
		size_t MsgLen,
		size_t *pPlainMsgLen)
{
	size_t padByte = pDecPad[MsgLen-1];

	if( (pDecPad == NULL) || (MsgLen == 0U) || (pPlainMsgLen == NULL))
	{
		return false;
	}

	for(uint8_t i = MsgLen - padByte; i <= MsgLen -1; i++)
	{
		if(pDecPad[i] != padByte)
		{
			return false;
		}
	}

	*pPlainMsgLen = MsgLen - padByte;

	return true;
}

