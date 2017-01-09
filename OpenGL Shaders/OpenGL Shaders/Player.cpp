

#include "Player.h"

//Constructor
Player::Player()
{
}
//Destructor
Player::~Player()
{
}

//Initialise the player with its position, rotation and scale. 
//Also set the maxHealth of the player.
//This sets the attack strength to 100. This is knive kill damage. 
//To set the attack strength, use setAttackStrenght(float dmg).
void Player::init(glm::vec3 position, glm::vec3 rotation, glm::vec3 scale, float _maxHealth)
{
	Initialise(position, rotation, scale);
	maxHealth = _maxHealth;
	health = maxHealth;

	speed = glm::vec3(0.0f);

	//Set attackStrength to knife damage
	attackStrength = 100.0f;

	gun.Init(position, rotation);
}

//Set the amount of damage the player does.
void Player::setAttackStrength(float dmg)
{
	attackStrength = dmg;
}

//Update the player
void Player::update()
{
	gun.Update(getPosition(), getRotation());
	//TODO: player update
	
}