#ifndef _ENEMY_
#define _ENEMY_

#include "Model.h"

class Enemy : Model
{
public:
	Enemy();
	~Enemy();

	void init(glm::vec3 position, glm::vec3 rotation, glm::vec3 scale, float _maxHealth, float _attackStrength);
	void update();

	void setActive();
	void setActive(glm::vec3 position, glm::vec3 rotation);
	void setActive(glm::vec3 position);

	bool active;

	float health, maxHealth;
	glm::vec3 speed;
	float attackStrength;
};

#endif