#include "../header.h"
#include "../Utils/loader.h"

#include "WaterTile.h"

//Constructor
WaterTile::WaterTile()
{}
WaterTile::WaterTile(glm::vec3 position, glm::vec3 rotation, glm::vec3 scale)
{
	waterTile = new gameobject();

	waterTile.setPosition(position);
	waterTile.setRotation(rotation);
	waterTile.setScale(scale);
}

//Destructor
WaterTile::~WaterTile()
{
}

//Return the gameobject so it can be rendered.
gameobject* WaterTile::getWaterTile()
{
	return &waterTile;
}

//Getters and setters
void WaterTile::setReflectionTexture(GLuint text)
{
	reflectionText = text;
}
void WaterTile::setRefractionTexture(GLuint text)
{
	refractionText = text;
}
void WaterTile::setRefractionDepthTexture(GLuint text)
{
	refractionDepthText = text;
}
void WaterTile::setDuDvTexture(GLuint text)
{
	DuDvMap = text;
}
void WaterTile::setNormalMapTexture(GLuint text)
{
	NormalMap = text;
}


GLuint WaterTile::getReflectionTexture()
{
	return reflectionText;
}
GLuint WaterTile::getRefractionTexture()
{
	return refractionText;
}
GLuint WaterTile::getRefractionDepthTexture()
{
	return refractionDepthText;
}
GLuint WaterTile::getDuDvTexture()
{
	return DuDvMap;
}
GLuint WaterTile::getNormalMapTexture()
{
	return NormalMap;
}