#include "../header.h"

#include "../Utils/loader.h"
#include "skybox.h"

//Skybox constructor
Skybox::Skybox()
{
}
//Skybox destructor
Skybox::~Skybox()
{
}

void Skybox::load(char vertexfile[], char fragmentfile[], Camera *camera, std::string *filenames)
{
	shader.load(vertexfile, fragmentfile);
	object = loader.loadToVAO(VERTICES, 108, 3);

	cubeMap.loadCubeMap(filenames);

	shader.start();
	shader.loadProjectionMatrix(camera->getProjectionMatrix());
	shader.loadSkyColour(clearColor);
	shader.stop();
}

void Skybox::render(Camera *camera)
{
	shader.start();
	glm::mat4 viewMatrix = Maths::createViewMatrix(camera->position, camera->rotation);
	viewMatrix[3][0] = 0.0f;
	viewMatrix[3][1] = 0.0f;
	viewMatrix[3][2] = 0.0f;
	shader.loadViewMatrix(viewMatrix);

	glBindVertexArray(object.getVaoID());
	glEnableVertexAttribArray(0);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMap.textureid);

	glDisable(GL_DEPTH_TEST);

	glDrawArrays(GL_TRIANGLES, 0, object.getVertexCount());

	glEnable(GL_DEPTH_TEST);

	glDisableVertexAttribArray(0);
	glBindVertexArray(0);

	shader.stop();
}

//Actually render all the gameobjects but update more uniform variables
void Skybox::renderUpdated(Camera *camera, float fov, glm::vec3 newCameraPosition, glm::vec3 newCameraRotation)
{
	shader.start();

	glm::mat4 viewMatrix = Maths::createViewMatrix(newCameraPosition, newCameraRotation);
	viewMatrix[3][0] = 0.0f;
	viewMatrix[3][1] = 0.0f;
	viewMatrix[3][2] = 0.0f;
	shader.loadViewMatrix(viewMatrix);

	shader.loadProjectionMatrix(camera->getDifferentProjectionMatrix(fov));

	glBindVertexArray(object.getVaoID());
	glEnableVertexAttribArray(0);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMap.textureid);

	glDisable(GL_DEPTH_TEST);

	glDrawArrays(GL_TRIANGLES, 0, object.getVertexCount());

	glEnable(GL_DEPTH_TEST);

	//Reset projectionmatrix back to normal
	shader.loadProjectionMatrix(camera->getProjectionMatrix());

	shader.stop();
	glDisableVertexAttribArray(0);
	glBindVertexArray(0);
}
