#include "Gun.h"

Gun::Gun()
{
}

Gun::~Gun()
{
}

void Gun::Init(glm::vec3 position, glm::vec3 rotation)
{
	setPosition(position);
	setRotation(rotation);
}

void Gun::Update(glm::vec3 position, glm::vec3 rotation)
{
	setPosition(position);
	setRotation(rotation);
}
