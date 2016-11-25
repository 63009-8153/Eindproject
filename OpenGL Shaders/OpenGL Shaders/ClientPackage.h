#pragma once

#include "Header.h"

#include "playerData.h"
#include "enemyData.h"

typedef struct {
	packetTypes packet_type;

	unsigned int lobbySize;
	playerData players[MAX_LOBBYSIZE];

	enemyData enemies[MAX_ENEMIES];

	void serialize(char * data) {
		memcpy(data, this, sizeof(ClientReceivePacket));
	}
	void deserialize(char * data) {
		memcpy(this, data, sizeof(ClientReceivePacket));
	}
} ClientReceivePacket;

typedef struct {
	packetTypes packet_type;

	playerData player;
	actionTypes action_types[MAX_ACTIONS];

	void serialize(char * data) {
		memcpy(data, this, sizeof(ClientSendPacket));
	}
	void deserialize(char * data) {
		memcpy(this, data, sizeof(ClientSendPacket));
	}
} ClientSendPacket;