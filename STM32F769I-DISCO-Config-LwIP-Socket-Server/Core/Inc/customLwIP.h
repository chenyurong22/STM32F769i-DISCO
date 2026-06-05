/*
 * customLwIP.h
 *
 *  Created on: Feb 7, 2026
 *      Author: Debasish Das
 */

#ifndef INC_CUSTOMLWIP_H_
#define INC_CUSTOMLWIP_H_

#include "lwip/netif.h"
#include "lwip/prot/ethernet.h"
#include "lwip/ip_addr.h"
#include "lwip/prot/ip4.h"
#include "lwip/prot/tcp.h"
#include "lwip/tcpip.h"

typedef struct timeStruct
{
	char timeStr[64];
	int frameCount;
} timeStruct_t;



extern UART_HandleTypeDef huart1;

int ethernet_link_up(void);
void getMacAddr(struct netif *pnetif);
void getGateway(struct netif *pnetif);
void getIPAddress(struct netif *pnetif);
void getSubnetMask(struct netif *pnetif);

/* Decoding Ethernet Packet */
void decode_ehernet_packet(struct pbuf *p);
const char* EthFrameType(struct eth_hdr *ethhdr);



#endif /* INC_CUSTOMLWIP_H_ */
