/*
 * customLwIP.c
 *
 *  Created on: Feb 7, 2026
 *      Author: Debasish Das
 */

#include "customLwIP.h"
#include "custom.h"
#include "lan8742.h"

extern lan8742_Object_t LAN8742;
char aMsgBuff[MAX_ARR_SIZE];

int ethernet_link_up(void)
{
	int32_t status;
	status = LAN8742_GetLinkState(&LAN8742);
	return (status != LAN8742_STATUS_LINK_DOWN);
}

void getMacAddr(struct netif *pnetif)
{
	char MACStr[50];
	int wMsgLen = 0;

	wMsgLen = snprintf(MACStr, MAX_ARR_SIZE,
			"%02X:%02X:%02X:%02X:%02X:%02X", pnetif->hwaddr[0],
			pnetif->hwaddr[1], pnetif->hwaddr[2], pnetif->hwaddr[3],
			pnetif->hwaddr[4], pnetif->hwaddr[5]);

	writeASCIItoSerial(&huart1, ASCII, MACStr, strlen(MACStr), "MAC address");
}

void getGateway(struct netif *pnetif)
{
	char GWStr[50];
	u32_t GW_u32 = pnetif->gw.addr;
	ip4addr_ntoa_r((const ip4_addr_t*) &GW_u32, GWStr, sizeof(GWStr));

	writeASCIItoSerial(&huart1, ASCII, GWStr, strlen(GWStr), "Gateway");
}

void getSubnetMask(struct netif *pnetif)
{
	char SNetStr[50];
	u32_t SNet_u32 = pnetif->netmask.addr;
	ip4addr_ntoa_r((const ip4_addr_t*) &SNet_u32, SNetStr, sizeof(SNetStr));

	writeASCIItoSerial(&huart1, ASCII, SNetStr, strlen(SNetStr), "Subnet Mask");
}

void getIPAddress(struct netif *pnetif)
{
	char IPStr[50];
	u32_t IP_u32 = pnetif->ip_addr.addr;
	ip4addr_ntoa_r((const ip4_addr_t*) &IP_u32, IPStr, sizeof(IPStr));

	writeASCIItoSerial(&huart1, ASCII, IPStr, strlen(IPStr), "IP address");
}

void decode_ehernet_packet(struct pbuf *p)
{
	struct eth_hdr *ethhdr;
	struct ip_hdr *ip4hdr;
	struct tcp_hdr *tcphdr;
	char srcMAC[30];
	char dstMAC[30];
	int len;
	char msgIP[64];
	char msgTYPE[20];
	uint16_t ethTYPE;
	char msgMAC[120];
	char msgSEQ_ACK[50];

	uint32_t wSeqCount;
	uint32_t wAcknowCount;

	uint16_t srcPort, dstPort;

	if ((p == NULL) || (p->payload == NULL))
	{
		return;
	}

	if (p->len < SIZEOF_ETH_HDR)
	{
		return;
	}

	ethhdr = (struct eth_hdr*) p->payload;
	ip4hdr = (struct ip_hdr*) ((uint8_t*) p->payload + SIZEOF_ETH_HDR);
	tcphdr = (struct tcp_hdr *)((uint8_t*)ip4hdr + IPH_HL_BYTES(ip4hdr));

	if (lwip_ntohs(ethhdr->type) == ETHTYPE_IP)
	{
		srcPort = lwip_ntohs(tcphdr->src);
		dstPort = lwip_ntohs(tcphdr->dest);

		wSeqCount = lwip_ntohl(tcphdr->seqno);
		wAcknowCount = lwip_ntohl(tcphdr->ackno);

		len = snprintf(msgSEQ_ACK, sizeof(msgSEQ_ACK), "[SEQ:%lu] [ACK:%lu] \r\n",
				(unsigned long)wSeqCount,
				(unsigned long)wAcknowCount);

		msgSEQ_ACK[len] = '\0';

		snprintf(msgIP, sizeof(msgIP),
		         "[IP] [%s]\r\n",
		         ip4addr_ntoa((const ip4_addr_t *)&ip4hdr->src));

		snprintf(srcMAC, sizeof(srcMAC), "MAC: %02X:%02X:%02X:%02X:%02X:%02X",
				ethhdr->src.addr[0],
				ethhdr->src.addr[1],
				ethhdr->src.addr[2],
				ethhdr->src.addr[3],
				ethhdr->src.addr[4],
				ethhdr->src.addr[5]);

		len = snprintf(msgMAC, sizeof(msgMAC), "[%s] [%s] [%s] [srcPort: %d] [dstPort: %d] [SEQ:%lu] [ACK:%lu] \r\n",
				EthFrameType(ethhdr),
				srcMAC,
				ip4addr_ntoa((const ip4_addr_t *)&ip4hdr->src),
				srcPort,
				dstPort,
				wSeqCount,
				wAcknowCount);
		msgMAC[len] = '\0';

		//writetoSerial(&huart1, (char*)msgMAC); /* Skip displaying IP stack messages */
	}
}

const char* EthFrameType(struct eth_hdr *ethhdr)
{
	uint16_t addrType;
	addrType = lwip_htons(ethhdr->type);

	struct ip_hdr *iphdr;

	switch (addrType )
	{
	case ETHTYPE_ARP:
		return "ARP";

	case ETHTYPE_IP:
		return "IP4";

	case ETHTYPE_IPV6:
		return "IP6";

	default:
		return "UKN";
	}
}

