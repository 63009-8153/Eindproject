#pragma once

#include "Header.h"

#include "playerData.h"
#include "enemyData.h"

typedef struct {
	// Packet Type, type of the packet
	packetTypes packet_type;

	// Lobby size
	unsigned int lobbySize;
	// All player data
	playerData players[MAX_LOBBYSIZE];

	// All enemy data
	enemyData enemies[MAX_ENEMIES];

	long float checkSum;

	void serialize(char * data) {
		memcpy(data, this, sizeof(ClientReceivePacket));
	}
	void deserialize(char * data) {
		memcpy(this, data, sizeof(ClientReceivePacket));
	}
} ClientReceivePacket;

typedef struct {
	// Packet Type, type of the packet
	packetTypes packet_type;

	// My player data
	playerData player;
	// My actions
	actionTypes action_types[MAX_ACTIONS];

	long float checkSum;

	void serialize(char * data) {
		memcpy(data, this, sizeof(ClientSendPacket));
	}
	void deserialize(char * data) {
		memcpy(this, data, sizeof(ClientSendPacket));
	}
} ClientSendPacket;