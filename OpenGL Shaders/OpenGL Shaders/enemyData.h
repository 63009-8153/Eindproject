#pragma once

#include "Header.h"
#include "NetworkHeader.h"

typedef struct {
	glm::vec3 position, rotation, scale;
	glm::vec3 velocity;

	float health, maxHealth;
	
	unsigned int modelType;
} enemyData;