#include "Header.h"

#include "Model.h"

Model::Model()
{
}
Model::~Model()
{
}

// Get the models position
glm::vec3 Model::getPosition()
{
	return position;
}

// Set the models position
void Model::setPosition(glm::vec3 pos)
{
	position = pos;
}

// Get the models rotation
glm::vec3 Model::getRotation()
{
	return rotation;
}

// Set the models rotation
void Model::setRotation(glm::vec3 rot)
{
	rotation = rot;
}

// get the models scale
glm::vec3 Model::getScale()
{
	return scale;
}

// Get the models velocity
glm::vec3 Model::getVelocity()
{
	return velocity;
}

// Set the models velocity
void Model::setVelocity(glm::vec3 vel)
{
	velocity = vel;
}

gameobject* Model::getModel()
{
	return &model;
}

// Initialise the model
void Model::Init(glm::vec3 pos, glm::vec3 rot, glm::vec3 scal)
{
	position = pos;
	rotation = rot;
	scale = scal;
	velocity = glm::vec3(0.0f);
}

// Set the models gameobject
void Model::setModel(gameobject * object)
{
	model = object;
}

//Draw the model
void Model::draw()
{
}
