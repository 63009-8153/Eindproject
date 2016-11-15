#include "../header.h"
#include "../Utils/loader.h"

#include "WaterMasterRenderer.h"
#include "../FrameBuffers/framebuffer.h"

//Constructor
WaterMasterRenderer::WaterMasterRenderer(){
	moveFactor = 0;
}
//Destructor and cleanup
WaterMasterRenderer::~WaterMasterRenderer()
{
	cleanUp();
}
void WaterMasterRenderer::cleanUp()
{
	shader.cleanUp();

	glDeleteFramebuffers(1, &reflectionFrameBuffer);
	glDeleteTextures(1, &reflectionTexture);
	glDeleteRenderbuffers(1, &reflectionDepthBuffer);

	glDeleteFramebuffers(1, &refractionFrameBuffer);
	glDeleteTextures(1, &refractionTexture);
	glDeleteRenderbuffers(1, &refractionDepthTexture);
}

void WaterMasterRenderer::load(char vertexFile[], char fragmentFile[], Camera *camera)
{
	shader.load(vertexFile, fragmentFile);

	//Set the projection matrix since this will not be changed a lot.
	shader.start();
	shader.loadProjectionMatrix(camera->getProjectionMatrix());
	
	shader.stop();

	WaterMasterRenderer::EnableCulling();
	
	wireframeRender = false;

	// Just x and z vectex positions here, y is set to 0 in v.shader
	GLfloat vertices[12] = { 
		-1, -1, 
		-1, 1, 
		1, -1,

		1, -1, 
		-1, 1, 
		1, 1 
	};
	waterTile = loader.loadToVAO(vertices, 12, 2);

	initialiseReflectionFrameBuffer();
	initialiseRefractionFrameBuffer();
}

void WaterMasterRenderer::DisableCulling() {
	//Only draw the triangles which face the camera
	glDisable(GL_CULL_FACE);
}
void WaterMasterRenderer::EnableCulling() {
	//Only draw the triangles which face the camera
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
}

//Add WaterTile to the rendering list.
void WaterMasterRenderer::addToRenderList(WaterTile *object)
{
	WaterTiles.push_back(*object);
}

void WaterMasterRenderer::renderWireframe()
{
	wireframeRender = true;
}

//Actually render all the gameobjects
void WaterMasterRenderer::render(std::vector<Light*> lights, Camera *camera)
{
	WaterMasterRenderer::DisableCulling();
	shader.start();

	shader.loadMoveFactor(moveFactor);

	shader.loadLights(lights);

	//shader.loadLights(lights);
	shader.loadViewMatrix(camera->getViewMatrix());
	shader.loadCameraPosition(camera->position);
	shader.connectTextureUnits();

	shader.loadSkyColour(clearColor);

	glBindVertexArray(waterTile.getVaoID());
	glEnableVertexAttribArray(0);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	if(wireframeRender) glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	for (unsigned int i = 0; i < WaterTiles.size(); i++)
	{
		//Create a transformation matrix from the object data and load it to the shader
		glm::mat4 transformationMatrix = Maths::createTransformationMatrix(WaterTiles[i].getWaterTile()->getPosition(), WaterTiles[i].getWaterTile()->getRotation(), WaterTiles[i].getWaterTile()->getScale());
		shader.loadTransformationMatrix(transformationMatrix);

		shader.loadAmbientLight(WaterTiles[i].getWaterTile()->getAmbientLight());

		//Load the object shinyness and reflectivityness
		shader.loadShineVariables(WaterTiles[i].getWaterTile()->getShineDamper(), WaterTiles[i].getWaterTile()->getReflectivity());
		
		//Reflection Texture
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, WaterTiles[i].getReflectionTexture());

		//Refraction Texture
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, WaterTiles[i].getRefractionTexture());

		//DuDv Map
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, WaterTiles[i].getDuDvTexture());

		//Normal map
		glActiveTexture(GL_TEXTURE4);
		glBindTexture(GL_TEXTURE_2D, WaterTiles[i].getNormalMapTexture());

		//Depth Texture
		glActiveTexture(GL_TEXTURE5);
		glBindTexture(GL_TEXTURE_2D, WaterTiles[i].getRefractionDepthTexture());

		glDrawArrays(GL_TRIANGLES, 0, waterTile.getVertexCount());
		//glDrawElements(GL_TRIANGLES, waterTile.getVertexCount(), GL_UNSIGNED_INT, 0);
	}
	
	if (wireframeRender) glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glDisableVertexAttribArray(0);
	glBindVertexArray(0);

	glDisable(GL_BLEND);

	shader.stop();

	wireframeRender = false;
}
void WaterMasterRenderer::clearRenderPass()
{
	WaterTiles.clear();
}

float WaterMasterRenderer::getMoveFactor()
{
	return moveFactor;
}
void WaterMasterRenderer::setMoveFactor(float _moveFactor)
{
	moveFactor = _moveFactor;
}


void WaterMasterRenderer::bindReflectionFrameBuffer() {//call before rendering to this FBO
	FrameBuffer::bindFrameBuffer(reflectionFrameBuffer, REFLECTION_WIDTH, REFLECTION_HEIGHT);
}
void WaterMasterRenderer::bindRefractionFrameBuffer() {//call before rendering to this FBO
	FrameBuffer::bindFrameBuffer(refractionFrameBuffer, REFRACTION_WIDTH, REFRACTION_HEIGHT);
}

void WaterMasterRenderer::unbindCurrentFrameBuffer() {//call to switch to default frame buffer
	FrameBuffer::unbindCurrentFrameBuffer(SCREEN_WIDTH, SCREEN_HEIGHT);
}

GLuint WaterMasterRenderer::getReflectionTexture() {//get the resulting texture
	return reflectionTexture;
}
GLuint WaterMasterRenderer::getRefractionTexture() {//get the resulting texture
	return refractionTexture;
}
GLuint WaterMasterRenderer::getRefractionDepthTexture() {//get the resulting depth texture
	return refractionDepthTexture;
}

void WaterMasterRenderer::initialiseReflectionFrameBuffer() {
	reflectionFrameBuffer = FrameBuffer::createFrameBuffer();
	glDrawBuffer(GL_COLOR_ATTACHMENT0);
	reflectionTexture = FrameBuffer::createTextureAttachment(REFLECTION_WIDTH, REFLECTION_HEIGHT);
	reflectionDepthBuffer = FrameBuffer::createDepthBufferAttachment(REFLECTION_WIDTH, REFLECTION_HEIGHT);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) printf("ReflectionFrameBuffer not complete: %x\n", glCheckFramebufferStatus(GL_FRAMEBUFFER));
	unbindCurrentFrameBuffer();
}
void WaterMasterRenderer::initialiseRefractionFrameBuffer() {
	refractionFrameBuffer = FrameBuffer::createFrameBuffer();
	glDrawBuffer(GL_COLOR_ATTACHMENT0);
	refractionTexture = FrameBuffer::createTextureAttachment(REFRACTION_WIDTH, REFRACTION_HEIGHT);
	refractionDepthTexture = FrameBuffer::createDepthTextureAttachment(REFRACTION_WIDTH, REFRACTION_HEIGHT);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) printf("RefractionFrameBuffer not complete: %x\n", glCheckFramebufferStatus(GL_FRAMEBUFFER));
	unbindCurrentFrameBuffer();
}