#pragma once

#include "NetworkHeader.h"
#include "Header.h"

typedef struct {
	glm::vec3 position, rotation, scale;
	glm::vec3 velocity;

	float health, maxHealth;
	
	unsigned int modelType;
} enemyData;