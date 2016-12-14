#include "../header.h"

#include "Light.h"

Light::Light(){}
Light::Light(glm::vec3 pos, glm::vec3 col) 
{
	position = pos;
	colour = col;
	attenuation = glm::vec3(1, 0, 0);

	setLightCanCastShadow(false);
}
Light::Light(glm::vec3 pos, glm::vec3 col, glm::vec3 _attenuation) 
{
	position = pos;
	colour = col;
	attenuation = _attenuation;

	setLightCanCastShadow(false);
}
Light::~Light() {}

void Light::setPosition(glm::vec3 pos) 
{
	position = pos;
}
glm::vec3 Light::getPosition() 
{
	return position;
}

void Light::setColour(glm::vec3 col) 
{
	colour = col;
}
glm::vec3 Light::getColour() 
{
	return colour;
}

glm::vec3 Light::getAttenuation()
{
	return attenuation;
}

void Light::setLightCanCastShadow(bool c)
{
	castsShadow = c;
}
bool Light::getLightCanCastShadow()
{
	return castsShadow;
}