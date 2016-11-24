#include "Header.h"

#include <winsock2.h>
#include <Windows.h>
#include "NetworkServices.h"
#include <ws2tcpip.h>
#include "networkData.h"

#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")

#include "ClientNetwork.h"

//Constructor
//Initialise the clientNetwork and connect to server with ipAddress and port.
ClientNetwork::ClientNetwork(char _ipAddress[39], char _port[5])
{
	// Create WSADATA object.
	WSADATA wsaData;

	// Intialise socket.
	ConnectSocket = INVALID_SOCKET;

	// Holds address info for socket to connect to.
	struct addrinfo *result = NULL,
		*ptr = NULL,
		hints;

	// Initialize Winsock
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);

	//Check if it didn't create an error.
	if (iResult != 0) {
		printf("WSAStartup failed with error: %d\n", iResult);
		error = -1;
	}

	// Set address info
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;  //Use TCP connection!

	// Resolve ipaddress and port.
	iResult = getaddrinfo(_ipAddress, _port, &hints, &result);

	//Check if resolving didn't create an error.
	if (iResult != 0)
	{
		printf("getaddrinfo failed with error: %d\n", iResult);
		WSACleanup();
		error = -2;
	}

	// Attempt to connect to an address until one succeeds.
	for (ptr = result; ptr != NULL; ptr = ptr->ai_next) {

		// Create a SOCKET for connecting to server
		ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);

		//Check if the socket is not an INVALID_SOCKET.
		if (ConnectSocket == INVALID_SOCKET) {
			printf("socket failed with error: %ld\n", WSAGetLastError());
			WSACleanup();
			error = -3;
		}

		// Connect to the server.
		iResult = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);

		//Check if connecting to the server didn't create an error.
		if (iResult == SOCKET_ERROR)
		{
			closesocket(ConnectSocket);
			ConnectSocket = INVALID_SOCKET;
			printf("The server is down... did not connect\n");
			error = -4;
		}
	}

	// No longer need address info for server.
	freeaddrinfo(result);

	// Check if creating the socket didn't fail.
	if (ConnectSocket == INVALID_SOCKET)
	{
		printf("Unable to connect to server!\n");
		WSACleanup();
		error = -5;
	}

	// Set the socket I/O mode: In this case FIONBIO
	// enables or disables the blocking mode for the 
	// socket based on the numerical value of iMode.
	// If iMode = 0, blocking is enabled; 
	// If iMode != 0, non-blocking mode is enabled.

	u_long iMode = 1;
	iResult = ioctlsocket(ConnectSocket, FIONBIO, &iMode);

	// Check if the socket didnt create an error.
	if (iResult == SOCKET_ERROR)
	{
		printf("ioctlsocket failed with error: %d\n", WSAGetLastError());
		closesocket(ConnectSocket);
		WSACleanup();
		error = -6;
	}

	// Disable nagle.
	char value = 1;
	setsockopt(ConnectSocket, IPPROTO_TCP, TCP_NODELAY, &value, sizeof(value));
}
//Destructor
ClientNetwork::~ClientNetwork()
{

}

//Get packets from server and put data in buffer
int ClientNetwork::receivePackets(char * recvbuf)
{
	//Receive message from the server and put the data in recvbuf
	iResult = NetworkServices::receiveMessage(ConnectSocket, recvbuf, MAX_PACKET_SIZE);

	//If the connection was closed.
	if (iResult == 0)
	{
		printf("Connection closed\n");

		closesocket(ConnectSocket);
		WSACleanup();

		error = -10;
	}

	return iResult;
}

//Get last error
//Sets error back to 0 afterward.
int ClientNetwork::getError()
{
	int tmpError = error;
	error = 0;
	return tmpError;
}