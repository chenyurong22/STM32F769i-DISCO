/*
 * customSocket.c
 *
 *  Created on: Apr 19, 2026
 *      Author: Debasish Das
 */

#include "customSocket.h"
#include "custom.h"


void Init_LwIP_Socket()
{
	int sockID;
	struct sockaddr_in serverAddr;

	uint8_t connetStatus = 0;

	uint16_t servPort = 5000;

	/* Cresting a socket of Address Family ( AF_INET), SOCKET_STREAM and IPPROTO_TCP
	 * @return  : Positive Socket descriptor  *
	 : Negative for failure */

	sockID = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (sockID < 0)
	{
		writetoSerial(&huart1, "Socket creation failed \r\n");
		return;
	}

	/* Configuring server address */
	/* Clearing the socket address field */
	memset((struct sockaddr_in*) &serverAddr, 0, sizeof(struct sockaddr_in));

	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(servPort);
	serverAddr.sin_addr.s_addr = inet_addr("192.168.0.8");

	/* Connecting to the server */
	connetStatus = connect(sockID, (struct sockaddr* )&serverAddr,
			sizeof(struct sockaddr));

	if (connetStatus < 0)
	{
		writetoSerial(&huart1, "Connection to the server Failed! \r\n");
		return;
	}

	writetoSerial(&huart1, "Connection to the server succeed :) \r\n");
}
