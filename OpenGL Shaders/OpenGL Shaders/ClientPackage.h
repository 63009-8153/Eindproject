#pragma once

#include "Header.h"

#include "playerData.h"
#include "enemyData.h"

typedef struct {
	unsigned int packet_type;

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
	unsigned int packet_type;

	unsigned int action_types[MAX_ACTIONS];

	glm::vec3 rotation;

	void serialize(char * data) {
		memcpy(data, this, sizeof(ClientSendPacket));
	}
	void deserialize(char * data) {
		memcpy(this, data, sizeof(ClientSendPacket));
	}
} ClientSendPacket;