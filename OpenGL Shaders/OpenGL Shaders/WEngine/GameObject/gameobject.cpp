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

	tiledAmount = glm::vec2(1.0f);
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

	tiledAmount = glm::vec2(1.0f);
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

void gameobject::setTiledAmount(glm::vec2 amount)
{
	tiledAmount = amount;
}

glm::vec2 gameobject::getTiledAmount()
{
	return tiledAmount;
}

void gameobject::Draw(ShaderProgram & shader, std::vector<Light*> lights, Camera * camera, glm::vec4 clipPlane)
{
	shader.start();

	shader.loadClipPlane(clipPlane);
	shader.loadLights(lights);

	shader.loadCameraPosition(camera->position);

	shader.loadViewMatrix(camera->getViewMatrix());

	GLfloat near_plane = camera->Near, far_plane = camera->Far;
	glm::mat4 lightProjection = glm::ortho(-500.0f, 500.0f, -500.0f, 500.0f, near_plane, far_plane);
	glm::mat4 lightView = glm::lookAt(
		lights[0]->getPosition(),//Eye Position
		glm::vec3(0.0f, 0.0f, 0.0f),//Position it is looking at
		glm::vec3(0.0f, 1.0f, 0.0f));//UP direction
	shader.loadLightSpaceMatrix(lightProjection * lightView);

	shader.loadSkyColour(clearColor);

// == Prepare global model

	// Use the vao of this object
	glBindVertexArray(getVaoID());

	//Use the positions location
	glEnableVertexAttribArray(0); //Enable using VAO position 0 (positions)
	glEnableVertexAttribArray(1); //Enable using VAO position 1 (uv coords)
	glEnableVertexAttribArray(2); //Enable using VAO position 2 (normal coords)
	glEnableVertexAttribArray(3); //Enable using VAO position 3 (tangents)

	int textureAmount = getTextureAmount();

	shader.loadNumberOfRows(numberOfRows);

	//Use texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, getTextureID(0));
	if (textureAmount >= 2) {
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, getTextureID(1));
	}
	if (textureAmount >= 3) {
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, getTextureID(2));
	}
	if (textureAmount >= 4) {
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, getTextureID(3));
	}
	if (textureAmount >= 5) {
		glActiveTexture(GL_TEXTURE4);
		glBindTexture(GL_TEXTURE_2D, getTextureID(4));
	}
	if (textureAmount > 6) {
		printf("Maximum supported textures are 5!");
	}

// == End of Perpare global model
// === Prepare model

	//Load the hasTransparency in the shader
	shader.loadFakeLighting(useFakeLighting);
	shader.loadAmbientLight(getAmbientLight());

	//If model has a normal map load this to texture 5
	if (hasNormalMap) {
		glActiveTexture(GL_TEXTURE5);
		glBindTexture(GL_TEXTURE_2D, getNormalMapID());
	}
	//If model has a shadow map load this to texture 6
	if (hasShadowMap) {
		glActiveTexture(GL_TEXTURE6);
		glBindTexture(GL_TEXTURE_2D, getShadowMapID());
	}
	//If model has a specular map load this to texture 7
	shader.loadUseSpecularMap(hasSpecularMap);
	if (hasSpecularMap) {
		glActiveTexture(GL_TEXTURE7);
		glBindTexture(GL_TEXTURE_2D, getSpecularMapID());
	}
	//If model has a enviroment cubemap load this to texture 8
	if (hasReflectionCubeMap) {
		glActiveTexture(GL_TEXTURE8);
		glBindTexture(GL_TEXTURE_CUBE_MAP, getEnviromentCubeMapID());
	}
	shader.loadUseReflectionCubeMap(hasReflectionCubeMap);
	shader.loadUseRefractionCubeMap(hasRefractionCubeMap);

	shader.loadReflectionRefractionRatio(reflectionRefractionRatio);
	shader.loadReflectionColourRatio(reflectionRatio);

	//Create a transformation matrix from the object data
	glm::mat4 transformationMatrix = Maths::createTransformationMatrix(getPosition(), getRotation(), getScale());
	//Load the transformation matrix into the shader for use
	shader.loadTransformationMatrix(transformationMatrix);

	shader.loadOffset(glm::vec2(getTextureXOffset(), getTextureYOffset()));

	//Load the object shinyness and reflectivityness
	shader.loadShineVariables(getShineDamper(), getReflectivity());

// === End of Prepare model
// ==== Render

	// Set the tile amount in the shader
	shader.loadTileAmount(getTiledAmount());

	//If the model is supposed to be using both sides, disable culling
	//if (!cullFaces) MasterRenderer::DisableCulling();

	//Draw each model
	glDrawElements(GL_TRIANGLES, getVertexCount(), GL_UNSIGNED_INT, 0);
	//Always enable culling again
	//if (!cullFaces) MasterRenderer::EnableCulling();

// ==== End of Render
// ===== Unbind

	glDisableVertexAttribArray(0); //Stop using VAO position 0 (positions)
	glDisableVertexAttribArray(1); //Stop using VAO position 1 (uv coords)
	glDisableVertexAttribArray(2); //Stop using VAO position 2 (normal coords)
	glDisableVertexAttribArray(3); //Stop using VAO position 3 (tangents)

	//Unbind vao
	glBindVertexArray(0);

// ===== End of Unbind

	shader.stop();
}
