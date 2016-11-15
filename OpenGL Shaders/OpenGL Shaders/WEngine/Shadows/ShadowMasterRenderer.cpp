#include "../header.h"

#include "ShadowMasterRenderer.h"
#include "../FrameBuffers/framebuffer.h"

//Constructor
ShadowMasterRenderer::ShadowMasterRenderer() {}
//Destructor and cleanup
ShadowMasterRenderer::~ShadowMasterRenderer()
{
	cleanUp();
}
void ShadowMasterRenderer::cleanUp()
{
	shader.cleanUp();

	glDeleteFramebuffers(1, &shadowFrameBuffer);
	glDeleteRenderbuffers(1, &shadowDepthTexture);
}

void ShadowMasterRenderer::load(char vertexFile[], char fragmentFile[])
{
	shader.load(vertexFile, fragmentFile);
	renderer = Renderer(&shader);

	initialiseShadowFrameBuffer();
}

//Actually render all the gameobjects
void ShadowMasterRenderer::render(Light *light, Camera *camera, std::map<GLuint, std::vector<gameobject>> *gameobjects)
{
	shader.start();

	GLfloat near_plane = camera->Near, far_plane = camera->Far;
	glm::mat4 lightProjection = glm::ortho(-500.0f, 500.0f, -500.0f, 500.0f, near_plane, far_plane);
	shader.loadProjectionMatrix(lightProjection);

	glm::mat4 lightView = glm::lookAt(
		light->getPosition(),//Eye Position
		glm::vec3(0.0f, 0.0f, 0.0f),//Position it is looking at
		glm::vec3(0.0f, 1.0f, 0.0f));//UP direction
	shader.loadViewMatrix(lightView);

	//glCullFace(GL_FRONT);

	renderer.render(gameobjects);

	//glCullFace(GL_BACK);

	shader.stop();
}

void ShadowMasterRenderer::bindShadowFrameBuffer() {//call before rendering to this FBO
	FrameBuffer::bindFrameBuffer(shadowFrameBuffer, SHADOW_WIDTH, SHADOW_HEIGHT);
	glEnable(GL_DEPTH_TEST);
	glClear(GL_DEPTH_BUFFER_BIT);
}

void ShadowMasterRenderer::unbindCurrentFrameBuffer() {//call to switch to default frame buffer
	FrameBuffer::unbindCurrentFrameBuffer(SCREEN_WIDTH, SCREEN_HEIGHT);
}


GLuint ShadowMasterRenderer::getShadowDepthTexture() {//Get the resulting depth texture
	return shadowDepthTexture;
}

void ShadowMasterRenderer::initialiseShadowFrameBuffer() {
	shadowFrameBuffer = FrameBuffer::createFrameBuffer();
	glDrawBuffer(GL_NONE);
	shadowDepthTexture = FrameBuffer::createDepthTextureAttachment(SHADOW_WIDTH, SHADOW_HEIGHT);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) printf("ShadowFrameBuffer not complete: %x\n", glCheckFramebufferStatus(GL_FRAMEBUFFER));
	unbindCurrentFrameBuffer();
}