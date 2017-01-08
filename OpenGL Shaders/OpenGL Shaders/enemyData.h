#pragma once

#include "NetworkHeader.h"
#include "Header.h"

struct enemyData {
	glm::vec3 position, rotation, scale;
	glm::vec3 velocity;

	float health, maxHealth;
	
	unsigned int modelType;

	enemyData& operator =(const enemyData& p)
	{
		position = p.position;
		rotation = p.rotation;
		scale = p.scale;

		health = p.health;
		maxHealth = p.maxHealth;

		modelType = p.modelType;

		return *this;
	}
};