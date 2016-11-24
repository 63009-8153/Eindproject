#pragma once

#include "Header.h"
#include "NetworkHeader.h"

typedef struct {
	glm::vec3 position, rotation, scale;
	glm::vec3 velocity;
	int playerID;
	float health, maxHealth;
	int shooting;
	char playerName[MAX_NAMESIZE];
} playerData;