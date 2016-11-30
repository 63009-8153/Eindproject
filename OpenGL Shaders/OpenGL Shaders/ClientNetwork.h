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

	//Get the server socket the client has connected to.
	SOCKET getSocket();

	//Get last errors
	//Removes all errors afterward..
	std::vector<networkingErrors> getErrors();

private:
	int iResult;
	std::vector<networkingErrors> errors;

	char ipAddress[39];
	char port[5];

	SOCKET ConnectSocket;
};

