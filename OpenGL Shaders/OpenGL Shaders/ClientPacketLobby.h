#pragma once

#include "Header.h"

#include "playerData.h"

typedef struct {
	// Packet Type, type of the packet
	packetTypes packet_type;

	// Timer to start the game
	float startTimer;
	// Actions from the server
	actionTypes action_types[MAX_ACTIONS];

	// All players in the lobby
	playerData players[MAX_LOBBYSIZE];

	void serialize(char * data) {
		memcpy(data, this, sizeof(ClientReceivePacketLobby));
	}
	void deserialize(char * data) {
		memcpy(this, data, sizeof(ClientReceivePacketLobby));
	}
} ClientReceivePacketLobby;

typedef struct {
	// Packet Type, type of the packet
	packetTypes packet_type;

	// My player data
	playerData player;
	// My actions
	actionTypes action_types[MAX_ACTIONS];

	void serialize(char * data) {
		memcpy(data, this, sizeof(ClientSendPacketLobby));
	}
	void deserialize(char * data) {
		memcpy(this, data, sizeof(ClientSendPacketLobby));
	}
} ClientSendPacketLobby;