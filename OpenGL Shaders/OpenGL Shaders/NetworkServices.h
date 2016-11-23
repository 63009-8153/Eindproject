#pragma once

#include "Header.h"

#include "NetworkHeader.h"

class NetworkServices
{
public:

	// Send packet to a socket
	static int sendMessage(SOCKET curSocket, char * message, int messageSize)
	{
		return send(curSocket, message, messageSize, 0);
	}

	// Receive packet from a socket
	static int receiveMessage(SOCKET curSocket, char * buffer, int bufferSize)
	{
		return recv(curSocket, buffer, bufferSize, 0);
	}
};

