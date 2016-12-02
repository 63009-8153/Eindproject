#pragma once

#include "NetworkHeader.h"

#include "Header.h"

#include "networkData.h"
#include "ClientNetwork.h"

extern void(*networkUpdateFunction)(void);

class ClientGame
{
public:
	//Constructor
	ClientGame();
	// Constructor
	// Connect to the server on ipAddress and port
	ClientGame(char ipAddress[39], char port[5]);
	//Destructor
	~ClientGame();

	//Update the client with server data.
	void updateClient();
	//Disconnect from the server.
	void disconnect();

	//Send playerData to the server.
	void sendPlayerData(playerData &player, packetTypes type);

	// Send an heartbeat response packet to the server
	void sendHeartbeatPacket();

	//Add an actionType to the next packet send to the server.
	void addActionType(actionTypes type);

	//Get the network error.
	//Resets to 0 after call
	std::vector<networkingErrors> getErrors();

	//ClientNetwork that controls the connection to the server.
	ClientNetwork* network;

private:
	// Error checking
	std::vector<networkingErrors> errors;

	// Network buffer
	char network_data[MAX_PACKET_SIZE];

	// Action types to be send in next packet
	std::vector<actionTypes> nextActionTypes;

	// All clients
	playerData allClients[MAX_LOBBYSIZE];
	unsigned int myClientID;
};

