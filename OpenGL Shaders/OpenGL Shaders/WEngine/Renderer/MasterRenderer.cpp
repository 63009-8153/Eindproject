#include "../header.h"

#include "MasterRenderer.h"

//Constructor
MasterRenderer::MasterRenderer(){}
//Destructor and cleanup
MasterRenderer::~MasterRenderer()
{
	cleanUp();
}
void MasterRenderer::cleanUp()
{
	shader.cleanUp();
}

void MasterRenderer::load(char vertexFile[], char fragmentFile[], Camera *camera)
{
	shader.load(vertexFile, fragmentFile);
	renderer = Renderer(&shader);

	//Set the projection matrix since this will not be changed a lot.
	shader.start();
	shader.loadProjectionMatrix(camera->getProjectionMatrix());
	shader.connectTextureUnits();

	shader.stop();

	MasterRenderer::EnableCulling();
	
	wireframeRender = false;
}

void MasterRenderer::DisableCulling() {
	//Only draw the triangles which face the camera
	glDisable(GL_CULL_FACE);
}
void MasterRenderer::EnableCulling() {
	//Only draw the triangles which face the camera
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
}

//Static function that prepares the rendering
void MasterRenderer::prepare()
{
	//Enable Depth Test in OpenGL
	glEnable(GL_DEPTH_TEST);

	//Clear the screen
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//Default background color
	glClearColor(clearColor.x, clearColor.y, clearColor.z, 1.0f);
}

//Add gameobject to the rendering list.
//This way all gameobject that are the same, get rendered with the same texture and shader.
//This has great performance increse! So good practise :)
void MasterRenderer::addToRenderList(gameobject *object)
{
	GLuint textID = object->getTextureID(0);

	if (gameobjects.find(textID) != gameobjects.end()) {
		gameobjects.find(textID)->second.push_back(*object);
	}
	else {
		gameobjects.insert(std::pair<GLuint, std::vector<gameobject>>(textID, std::vector<gameobject>()));
		gameobjects.find(textID)->second.push_back(*object);
	}
}

void MasterRenderer::renderWireframe()
{
	wireframeRender = true;
}
//Actually render all the gameobjects but update more uniform variables
void MasterRenderer::renderUpdated(std::vector<Light*> lights, Camera *camera, glm::vec4 clipPlane, float fov, glm::vec3 newCameraPosition, glm::vec3 newCameraRotation)
{
	shader.start();

	shader.loadClipPlane(clipPlane);
	shader.loadLights(lights);

	shader.loadViewMatrix(camera->getDifferentViewMatrix(newCameraPosition, newCameraRotation));
	shader.loadProjectionMatrix(camera->getDifferentProjectionMatrix(fov));

	GLfloat near_plane = camera->Near, far_plane = camera->Far;
	glm::mat4 lightProjection = glm::ortho(-500.0f, 500.0f, -500.0f, 500.0f, near_plane, far_plane);
	glm::mat4 lightView = glm::lookAt(
		lights[0]->getPosition(), //Eye Position
		glm::vec3(0.0f, 0.0f, 0.0f), //Position it is looking at
		glm::vec3(0.0f, 1.0f, 0.0f)); //UP direction
	shader.loadLightSpaceMatrix(lightProjection * lightView);

	shader.loadSkyColour(clearColor);

	if (wireframeRender) glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	renderer.render(&gameobjects);

	if (wireframeRender) glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	//Reset projectionmatrix back to normal
	shader.loadProjectionMatrix(camera->getProjectionMatrix());

	shader.stop();

	wireframeRender = false;
}

//Actually render all the gameobjects
void MasterRenderer::render(std::vector<Light*> lights, Camera *camera, glm::vec4 clipPlane)
{
	shader.start();

	shader.loadClipPlane(clipPlane);
	shader.loadLights(lights);
	shader.loadViewMatrix(camera->getViewMatrix());

	GLfloat near_plane = camera->Near, far_plane = camera->Far;
	glm::mat4 lightProjection = glm::ortho(-500.0f, 500.0f, -500.0f, 500.0f, near_plane, far_plane);
	glm::mat4 lightView = glm::lookAt(
		lights[0]->getPosition(),//Eye Position
		glm::vec3(0.0f, 0.0f, 0.0f),//Position it is looking at
		glm::vec3(0.0f, 1.0f, 0.0f));//UP direction
	shader.loadLightSpaceMatrix(lightProjection * lightView);
	
	shader.loadSkyColour(clearColor);

	if(wireframeRender) glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	renderer.render(&gameobjects);

	if (wireframeRender) glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	shader.stop();

	wireframeRender = false;
}

void MasterRenderer::clearRenderPass()
{
	gameobjects.clear();
}