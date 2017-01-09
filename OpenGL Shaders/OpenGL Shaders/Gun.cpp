#include "Gun.h"

Gun::Gun()
{
}

Gun::~Gun()
{
}

void Gun::Init(glm::vec3 position, glm::vec3 rotation)
{
	gun_model = GUN_WALTER;
	gun_model.setPosition(position);
	gun_model.setRotation(rotation);
}

void Gun::Update(glm::vec3 position, glm::vec3 rotation)
{
	gun_model.setPosition(position);
	gun_model.setRotation(rotation);
}
