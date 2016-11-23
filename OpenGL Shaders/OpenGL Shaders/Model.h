#pragma once



class Model
{

public:
	Model();
	~Model();

	// Get the models position
	glm::vec3 getPosition();
	// Set the models position
	void setPosition(glm::vec3 pos);

	// Get the models rotation
	glm::vec3 getRotation();
	// Set the models rotation
	void setRotation(glm::vec3 rot);

	// Get the models scale
	glm::vec3 getScale();

	// Get the models velocity
	glm::vec3 getVelocity();
	// Set the models velocity
	void setVelocity(glm::vec3 vel);

	// Draw the model. 
	// Please do not use this unless nessesary. 
	// You can also get the gameobject with getModel(). Use this instead!
	void draw();

	// Initialise the model
	void Initialise(glm::vec3 pos, glm::vec3 rot, glm::vec3 scal);

	// Set the models gameobject
	void setModel(gameobject * object);
	// Get the pointer to the gameobject
	gameobject* getModel();

private:
	glm::vec3 position, rotation, scale;
	glm::vec3 velocity;

	gameobject model;
};

