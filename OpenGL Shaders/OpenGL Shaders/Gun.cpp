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

// -4 & 20

float calculated_rotation(float rot)
{
	//Limit rotation between 20 and -4
	float r = rot;
	if (r > 20) r = 20;
	if (r < -4) r = -4;
	return r;
};

void Gun::Update(glm::vec3 position, glm::vec3 rotation)
{
	glm::vec3 rot = rotation;
	rot = glm::radians(rot);

	position.x += cos(rot.y) * 0.7f;
	position.y = 4.7f;
	position.z += sin(rot.y) * 0.7f;

	gun_model.setPosition(glm::vec3(position.x, position.y, position.z));
	gun_model.setRotation(glm::radians(glm::vec3(rotation.z, -rotation.y - 90.0f, rotation.x)));
}
