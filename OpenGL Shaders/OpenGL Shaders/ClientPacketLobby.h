#pragma once

#include "Header.h"

#include "playerData.h"

typedef struct {
	unsigned int packet_type;

	float startTimer;
	unsigned int action_types[MAX_ACTIONS];

	playerData players[MAX_LOBBYSIZE];

	void serialize(char * data) {
		memcpy(data, this, sizeof(ClientReceivePacketLobby));
	}
	void deserialize(char * data) {
		memcpy(this, data, sizeof(ClientReceivePacketLobby));
	}
} ClientReceivePacketLobby;

typedef struct {
	unsigned int packet_type;

	playerData player;
	unsigned int action_types[MAX_ACTIONS];

	void serialize(char * data) {
		memcpy(data, this, sizeof(ClientSendPacketLobby));
	}
	void deserialize(char * data) {
		memcpy(this, data, sizeof(ClientSendPacketLobby));
	}
} ClientSendPacketLobby;