#pragma once
class ClientNetwork
{
public:
	//Constructor
	//Initialise the clientNetwork and connect to server with ipAddress and port.
	ClientNetwork(char _ipAddress[39], char _port[5]);
	//Destructor
	~ClientNetwork();

	//Get packets from server and put data in buffer
	int receivePackets(char * recvbuf);

	//Get last error
	//Sets error back to 0 afterward.
	int getError();

private:
	int iResult;
	int error;

	char ipAddress[39];
	char port[5];

	SOCKET ConnectSocket;
};

