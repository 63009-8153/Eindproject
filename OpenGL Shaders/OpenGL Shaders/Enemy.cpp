#include "Header.h"

#include "Enemy.h"

//Constructor
Enemy::Enemy()
{
}
//Destructor
Enemy::~Enemy()
{
}

//Initialise the enemy and set its position, rotation and scale. 
//Also se the maxHealth of the enemy and its attack strength.
//Active is set to false here. Use setActive to set active to true and use the enemy.
void Enemy::init(glm::vec3 position, glm::vec3 rotation, glm::vec3 scale, float _maxHealth, float _attackStrength)
{
	Initialise(position, rotation, scale);

	maxHealth = _maxHealth;
	health = maxHealth;

	attackStrength = _attackStrength;
	speed = glm::vec3(0.0f);

	active = false;
}

//Set Active.
void Enemy::setActive()
{
	active = true;
}
//Set Active and set a new position and rotation.
void Enemy::setActive(glm::vec3 position, glm::vec3 rotation)
{
	setActive(position);
	setRotation(rotation);
}
//Set Active and set a new position.
void Enemy::setActive(glm::vec3 position)
{
	setActive();
	setPosition(position);
}

//Update the enemy.
void Enemy::update()
{
	//TODO: enemy update
}