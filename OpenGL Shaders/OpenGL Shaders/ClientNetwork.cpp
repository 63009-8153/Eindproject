#include "NetworkHeader.h"

#include "Header.h"

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

	// Check if it didn't create an error.
	if (iResult != 0) {
		printf("ERROR: -- WSAStartup failed with error: %d\n", iResult);
		errors.push_back(WSA_STARTUP_ERROR);
	}

	// Set address info
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;  //Use TCP connection!

	// Resolve ipaddress and port.
	iResult = getaddrinfo(_ipAddress, _port, &hints, &result);

	// Check if resolving didn't create an error.
	if (iResult != 0)
	{
		printf("ERROR: -- getaddrinfo failed with error: %d\n", iResult);
		WSACleanup();
		errors.push_back(GET_ADDR_INFO_ERROR);
	}

	// Attempt to connect to an address until one succeeds.
	for (ptr = result; ptr != NULL; ptr = ptr->ai_next) {

		// Create a SOCKET for connecting to server
		ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);

		//Check if the socket is not an INVALID_SOCKET.
		if (ConnectSocket == INVALID_SOCKET) {
			printf("ERROR: -- socket failed with error: %ld\n", WSAGetLastError());
			WSACleanup();
			errors.push_back(CREATE_SOCKET_ERROR);
		}

		// Connect to the server.
		iResult = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);

		// Check if connecting to the server didn't create an error.
		if (iResult == SOCKET_ERROR)
		{
			closesocket(ConnectSocket);
			ConnectSocket = INVALID_SOCKET;
			printf("ERROR: -- The server is down or was unable to connect\n");
			errors.push_back(CONNECT_SOCKET_ERROR);
		}
	}

	// No longer need address info for server.
	freeaddrinfo(result);

	// Check if creating the socket didn't fail.
	if (ConnectSocket == INVALID_SOCKET)
	{
		printf("ERROR: -- Unable to connect to server!\n");
		WSACleanup();
		errors.push_back(ALL_CONNECTING_SOCKETS_ERROR);
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
		printf("ERROR: -- ioctlsocket failed with error: %d\n", WSAGetLastError());
		closesocket(ConnectSocket);
		WSACleanup();
		errors.push_back(SET_NONBLOCKING_ERROR);
	}

	// Disable nagle.
	char value = 1;
	setsockopt(ConnectSocket, IPPROTO_TCP, TCP_NODELAY, &value, sizeof(value));
}
// Destructor
ClientNetwork::~ClientNetwork()
{
	printf("INFO:  -- Connection closed by destructor\n");
	closesocket(ConnectSocket);
	WSACleanup();
}

// Get packets from server and put data in buffer
int ClientNetwork::receivePackets(char * recvbuf)
{
	// Receive message from the server and put the data in recvbuf
	iResult = NetworkServices::receiveMessage(ConnectSocket, recvbuf, MAX_PACKET_SIZE);

	// If the connection was closed or we got another error.
	if (iResult == 0)
	{
		printf("INFO:  -- Connection error, Socket was possibly closed!\n");

		closesocket(ConnectSocket);
		WSACleanup();

		errors.push_back(CONNECT_SOCKET_ERROR);
	}

	return iResult;
}

// Get the server socket the client has connected to.
SOCKET ClientNetwork::getSocket()
{
	return ConnectSocket;
}

// Get last errors
// Removes all errors afterward.
std::vector<networkingErrors> ClientNetwork::getErrors()
{
	std::vector<networkingErrors> tmpErrors = errors;
	errors.clear();

	return tmpErrors;
}