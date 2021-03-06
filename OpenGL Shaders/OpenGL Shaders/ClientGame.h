#pragma once

#include "NetworkHeader.h"

#include "Header.h"

#include "networkData.h"
#include "ClientNetwork.h"

#include "Player.h"
#include "Enemy.h"

extern void(*networkUpdateFunction)(void);
extern void SendInitData();
extern void SendLobbyData();
extern void SendGameData();

// My Client
extern playerData myPlayerData;
// All clients
extern playerData allClients[MAX_LOBBYSIZE];
// All enemies
extern enemyData allEnemies[MAX_ENEMIES];

class ClientGame
{
public:
	// Constructor
	ClientGame();
	// Constructor
	// Connect to the server on ipAddress and port
	ClientGame(const char ipAddress[39], char port[5]);
	// Destructor
	~ClientGame();

	// Update the client with server data.
	void updateClient();
	// Disconnect from the server.
	void disconnect();

	// Update the playerdata of own player
	void setPlayerData(Player &player);

	// Update the player with specified playerData
	void getPlayerData(Player &player, int index);
	// Update the player of own playerData
	void getPlayerData(Player &player);

	// Update the enemy with specified enemyData index
	void getEnemyData(Enemy &enem, int index);

	// Send playerData to the server.
	// This function is only to be send during the game
	void sendPlayerData();

	// Send a lobbyUpdate packet to the server
	// This function is only to be send during the lobby
	void sendLobbyUpdate();

	// Send an heartbeat response packet to the server
	void sendHeartbeatPacket();

	// Add an actionType to the next packet send to the server.
	void addActionType(actionTypes type);
	bool hasActionType();

	// Get the network error.
	// Resets to 0 after call
	std::vector<networkingErrors> getErrors();

	// ClientNetwork that controls the connection to the server.
	ClientNetwork* network;

	bool gameStarting,
		 gameStarted;

	bool networkConnected;

	// The startTimer
	float lobbyTimer;

	// My Client
	playerData myPlayerData;
	// All clients
	playerData allClients[MAX_LOBBYSIZE];
	// All enemies
	enemyData  allEnemies[MAX_ENEMIES];

	// The actual lobbysize
	unsigned int actualLobbySize;

private:
	// Error checking
	std::vector<networkingErrors> errors;

	// Network buffer
	char network_data[MAX_PACKET_SIZE];

	// Action types to be send in next packet
	std::vector<actionTypes> nextActionTypes;
	
	// My ClientID
	unsigned int myClientID;
};

