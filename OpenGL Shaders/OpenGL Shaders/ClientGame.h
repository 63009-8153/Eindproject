#pragma once

#include "NetworkHeader.h"

#include "Header.h"

#include "networkData.h"
#include "ClientNetwork.h"


class ClientGame
{
public:
	//Constructor
	ClientGame();
	ClientGame(char ipAddress[39], char port[5]);
	//Destructor
	~ClientGame();

	//Update the client with server data.
	void updateClient();
	//Disconnect from the server.
	void disconnect();

	//Send playerData to the server.
	void sendPlayerData(playerData &player, packetTypes type);

	//Add an actionType to the next packet send to the server.
	void addActionType(actionTypes type);

	//Get the network error.
	//Resets to 0 after call
	std::vector<networkingErrors> getErrors();

	//ClientNetwork that controls the connection to the server.
	ClientNetwork* network;

private:
	std::vector<networkingErrors> errors;

	char network_data[MAX_PACKET_SIZE];

	std::vector<actionTypes> nextActionTypes;
};

