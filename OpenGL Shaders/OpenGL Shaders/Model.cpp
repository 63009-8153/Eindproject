#include "Header.h"

#include "Model.h"

//Constructor
Model::Model()
{
}
//Destructor
Model::~Model()
{
}

// Get the models position
glm::vec3 Model::getPosition()
{
	return model.getPosition();
}

// Set the models position
void Model::setPosition(glm::vec3 pos)
{
	model.setPosition(pos);
}

// Get the models rotation
glm::vec3 Model::getRotation()
{
	return model.getRotation();
}

// Set the models rotation
void Model::setRotation(glm::vec3 rot)
{
	model.setRotation(rot);
}

// Get the models scale
glm::vec3 Model::getScale()
{
	return model.getScale();
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

// Initialise the model
void Model::Initialise(glm::vec3 pos, glm::vec3 rot, glm::vec3 scal)
{
	model.setPosition(pos);
	model.setRotation(rot);
	model.setScale(scal);

	velocity = glm::vec3(0.0f);
}

// Set the models gameobject
void Model::setModel(gameobject * object)
{
	model = gameobject(object);
	model.init();
}
// Get the pointer to the models gameobject
gameobject* Model::getModel()
{
	return &model;
}

// Draw the model
void Model::draw()
{
	
}
