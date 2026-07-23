/*
 * custom_crypto.h
 *
 *  Created on: Jul 17, 2026
 *      Author: Debasish Das
 */

#ifndef INC_CUSTOM_CRYPTO_H_
#define INC_CUSTOM_CRYPTO_H_
#include "stdint.h"
#include "stdio.h"
#include "stdbool.h"


#define IV_LEN 			16U
#define MAX_CIPHER_LEN 	200U
#define HASH_LEN 		32U

#define PKT_SYNC1      0x55U
#define PKT_SYNC2      0xAAU

#define ALGO_AES256    0xBBU
#define MODE_CBC       0xDDU

typedef enum
{
	RX_WAIT_SYNC1 = 0,
	RX_WAIT_SYNC2,
	RX_WAIT_ALGO,
	RX_WAIT_MODE,
	RX_WAIT_LEN1,
	RX_WAIT_LEN2,
	RX_WAIT_IV,
	RX_WAIT_CIPHER,
	RX_WAIT_HASH,
	RX_PACKET_READY,
	RX_ERROR
} CRYPTO_RX_STATE;

typedef struct
{
	CRYPTO_RX_STATE state;
	uint8_t IV[IV_LEN];
	uint8_t Cipher[MAX_CIPHER_LEN];
	uint8_t Hash[HASH_LEN];
	uint8_t index;
	uint16_t cipherLen;
	uint8_t algorithm;
	uint8_t mode;
} CryptoRx_t;

void decryptMsgCBC(const uint8_t ciperText[16], size_t ciperTextLen,
                    uint8_t decryptText[16], const uint8_t key[16], uint8_t iv[16]);

void hashParameter(uint8_t *pKey, size_t keyLen,  uint8_t *pHash256);
void Init_CryptoRx_Process(CryptoRx_t *rx);
void Process_CryptRx(CryptoRx_t *rx, uint8_t data);
bool CryptoEngineGetPlaintextLength(uint8_t *pDecPad,
		size_t MsgLen,
		size_t *pPlainMsgLen);

#endif /* INC_CUSTOM_CRYPTO_H_ */
