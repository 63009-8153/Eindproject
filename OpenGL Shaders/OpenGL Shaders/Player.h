#ifndef _PLAYER_
#define _PLAYER_

#include "Model.h"

class Player : public Model
{
public:
	Player();
	~Player();

	void init(glm::vec3 position, glm::vec3 rotation, glm::vec3 scale, float _maxHealth);
	void update();

	void setAttackStrength(float dmg);

	float health, maxHealth;

	glm::vec3 speed;

	float attackStrength;
};

#endif