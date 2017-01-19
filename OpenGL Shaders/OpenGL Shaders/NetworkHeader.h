#ifndef _NETWORK_HEADER_
#define _NETWORK_HEADER_

#include <winsock2.h>
#include <Windows.h>

#include <ws2tcpip.h>

#include <SDKDDKVer.h>

#include "NetworkServices.h"
#include "networkingErrors.h"

#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")

#define MAX_NAMESIZE 32
#define MAX_LOBBYSIZE 5
#define MAX_ENEMIES 100
#define MAX_ACTIONS 10

#define MAX_PACKET_SIZE 1000000

#define MAX_STARTTIME 30.0f

#endif