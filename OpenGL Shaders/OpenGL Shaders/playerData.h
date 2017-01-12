#pragma once

#include "NetworkHeader.h"
#include "Header.h"

struct playerData {
	glm::vec3 position, rotation, scale;
	glm::vec3 velocity;
	int playerID;
	float health, maxHealth;
	int shooting;
	char playerName[MAX_NAMESIZE];
	unsigned int playerNameSize;
	bool activePlayer;
	int animType;

	playerData& operator =(const playerData& p)
	{
		position = p.position;
		rotation = p.rotation;
		scale = p.scale;
		playerID = p.playerID;
		health = p.health;
		maxHealth = p.maxHealth;
		shooting = p.shooting;
		animType = p.animType;

		memcpy(playerName, p.playerName, MAX_NAMESIZE);
		playerNameSize = p.playerNameSize;

		activePlayer = p.activePlayer;

		return *this;
	}

	playerData& operator ==(const playerData& p)
	{
		position = p.position;
		scale = p.scale;
		playerID = p.playerID;
		health = p.health;
		maxHealth = p.maxHealth;
		shooting = p.shooting;
		animType = p.animType;

		memcpy(playerName, p.playerName, MAX_NAMESIZE);
		playerNameSize = p.playerNameSize;

		activePlayer = p.activePlayer;

		return *this;
	}


};