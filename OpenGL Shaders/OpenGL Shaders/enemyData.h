#pragma once

#include "NetworkHeader.h"
#include "Header.h"

struct enemyData {
	glm::vec3 position, rotation, scale;
	glm::vec3 velocity;

	float health, maxHealth;

	unsigned int modelType;

	int animType;

	bool active = false;

	enemyData& operator =(const enemyData& e)
	{
		position = e.position;
		rotation = e.rotation;
		scale = e.scale;

		health = e.health;
		maxHealth = e.maxHealth;

		modelType = e.modelType;

		animType = e.animType;

		active = e.active;

		return *this;
	}
};