#pragma once
class NetworkServices
{
public:

	// Send packet to socket
	static int sendMessage(SOCKET curSocket, char * message, int messageSize)
	{
		return send(curSocket, message, messageSize, 0);
	}

	// Receive packet from socket
	static int receiveMessage(SOCKET curSocket, char * buffer, int bufferSize)
	{
		return recv(curSocket, buffer, bufSize, 0);
	}
};

