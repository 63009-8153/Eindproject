#ifndef _PLAYER_
#define _PLAYER_

#include "Model.h"
#include "Gun.h"

class Player : public Model
{
public:
	Player();
	~Player();

	Gun gun;

	void init(glm::vec3 position, glm::vec3 rotation, glm::vec3 scale, float _maxHealth);
	void update();

	void setAttackStrength(float dmg);

	float health, maxHealth;

	glm::vec3 speed;

	float attackStrength;
};

#endif