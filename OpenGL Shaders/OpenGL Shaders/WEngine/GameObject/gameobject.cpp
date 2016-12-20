#include "../header.h"

#include "gameobject.h"

//Gameobject constructor
gameobject::gameobject(){}
gameobject::gameobject(gameobject *object){
	//Set Vao ID and vertexCount
	vaoID = object->vaoID;
	vertexCount = object->vertexCount;

	shineDamper = 1;
	reflectivity = 0;

	numberOfRows = 1;
	textureIndex = 0;

	hasSpecularMap = false;
	hasNormalMap = false;
	hasShadowMap = false;
	hasReflectionCubeMap = false;
	hasRefractionCubeMap = false;

	tiledAmount = 1.0f;
}
gameobject::gameobject(int _vaoID, int _vertexCount)
{
	//Set Vao ID and vertexCount
	vaoID = _vaoID;
	vertexCount = _vertexCount;

	shineDamper = 1;
	reflectivity = 0;

	numberOfRows = 1;
	textureIndex = 0;

	hasSpecularMap = false;
	hasNormalMap = false;
	hasShadowMap = false;
	hasReflectionCubeMap = false;
	hasRefractionCubeMap = false;

	tiledAmount = 1.0f;
}

//Gameobject destructor
gameobject::~gameobject()
{
}

//Get the Vao ID
GLuint gameobject::getVaoID() 
{
	return vaoID;
}

//Get the Vertex Count
int gameobject::getVertexCount() 
{
	return vertexCount;
}

//Set Texture ID
void gameobject::addTexture(GLuint textID)
{
	textureID.push_back(textID);
}

void gameobject::setNormalMap(GLuint textID)
{
	normalMapID = textID;
	hasNormalMap = true;
}
GLuint gameobject::getNormalMapID()
{
	return normalMapID;
}

void gameobject::setShadowMap(GLuint textID)
{
	shadowMapID = textID;
	hasShadowMap = true;
}
GLuint gameobject::getShadowMapID()
{
	return shadowMapID;
}

void gameobject::setSpecularMap(GLuint textID)
{
	specularMapID = textID;
	hasSpecularMap = true;
}
GLuint gameobject::getSpecularMapID()
{
	return specularMapID;
}

void gameobject::setEnviromentCubeMapID(GLuint textID)
{
	enviromentCubeMap = textID;
}
GLuint gameobject::getEnviromentCubeMapID()
{
	return enviromentCubeMap;
}
void gameobject::setReflectionRatio(float value)
{
	reflectionRatio = value;
}
void gameobject::setReflectionRefractionRatio(float value)
{
	reflectionRefractionRatio = value;
}

void gameobject::setTextureAtlasTexture(GLuint textID, int nrRows, int textIndex) 
{
	textureID.push_back(textID);

	numberOfRows = nrRows;
	textureIndex = textIndex;
}

float gameobject::getTextureXOffset()
{
	int column = textureIndex % numberOfRows;
	return (float)column / (float)numberOfRows;
}
float gameobject::getTextureYOffset()
{
	int row = textureIndex % numberOfRows;
	return (float)row / (float)numberOfRows;
}

//Get Texture ID
GLuint gameobject::getTextureID(int position)
{
	if (position >= textureID.size()) return -1;
	return textureID[position];
}

int gameobject::getTextureAmount()
{
	return textureID.size();
}

void gameobject::init(glm::vec3 pos, glm::vec3 rot, glm::vec3 scal)
{
	position = pos;
	rotation = rot;
	scale = scal;

	shineDamper = -1;
	reflectivity = -1;

	useFakeLighting = false;
	cullFaces = true;

	ambientLight = 0.1f;
}
void gameobject::init()
{
	shineDamper = -1;
	reflectivity = -1;

	useFakeLighting = false;
	cullFaces = true;

	ambientLight = 0.1f;
}

//Move the object
void gameobject::Move(glm::vec3 _movement)
{
	position += _movement;
}
//Rotate the object
void gameobject::Rotate(glm::vec3 _rotation)
{
	rotation += _rotation;
}

//Set the ambient light of the model
void gameobject::setAmbientLight(float ambient) {
	ambientLight = ambient;
}

//Set the position
void gameobject::setPosition(glm::vec3 pos)
{
	position = pos;
}
//Set the rotation
void gameobject::setRotation(glm::vec3 rot)
{
	rotation = rot;
}
//Set the scale
void gameobject::setScale(glm::vec3 scal)
{
	scale = scal;
}
//Set the shineDamper value
void gameobject::setShineDamper(float val)
{
	shineDamper = val;
}
//Set the reflectivityness
void gameobject::setReflectivity(float val)
{
	reflectivity = val;
}

float gameobject::getAmbientLight() {
	return ambientLight;
}
//Get the position of the object
glm::vec3 gameobject::getPosition()
{
	return position;
}
//Get the rotation of the object
glm::vec3 gameobject::getRotation()
{
	return rotation;
}
//Get the scale of the object
glm::vec3 gameobject::getScale()
{
	return scale;
}
//get the shineDamper
float gameobject::getShineDamper()
{
	return shineDamper;
}
//Ge the reflectivityness
float gameobject::getReflectivity()
{
	return reflectivity;
}

void gameobject::setTiledAmount(float amount)
{
	tiledAmount = amount;
}

float gameobject::getTiledAmount()
{
	return tiledAmount;
}
