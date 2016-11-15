#include "../header.h"

#include "PostProcessShaderProgram.h"
#include "PostProcessRenderer.h"

#include "../FrameBuffers/framebuffer.h"
#include "../Utils/loader.h"

gameobject PostProcessRenderer::screenQuad;

PostProcessRenderer::PostProcessRenderer()
{
}
PostProcessRenderer::~PostProcessRenderer()
{
	glDeleteFramebuffers(1, &toTextureFrameBuffer);
	glDeleteTextures(1, &toTextureTexture);
	glDeleteRenderbuffers(1, &toTextureDepthBuffer);

	shader.cleanUp();
}

void PostProcessRenderer::load(glm::vec2 screenSize, int samples)
{
	screenSizes = screenSize;

	toTextureFrameBuffer = FrameBuffer::createFrameBuffer();
	glDrawBuffer(GL_COLOR_ATTACHMENT0);
	toTextureTexture = FrameBuffer::createMultisampledColourAttachment((int)screenSize.x, (int)screenSize.y, samples);
	toTextureDepthBuffer = FrameBuffer::createMultisampledDepthBufferAttachment((int)screenSize.x, (int)screenSize.y, samples);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) printf("ToTexture Framebuffer not complete: %x\n", glCheckFramebufferStatus(GL_FRAMEBUFFER));

	FrameBuffer::unbindCurrentFrameBuffer((int)screenSize.x, (int)screenSize.y);
}
void PostProcessRenderer::load(char vertexFile[], char fragmentFile[], glm::vec2 screenSize, int samples)
{
	float position[] = { -1, 1, -1, -1, 1, 1, 1, -1 };
	PostProcessRenderer::screenQuad = loader.loadToVAO(position, 8, 2);
	screenSizes = screenSize;

	shader.load(vertexFile, fragmentFile);
	shader.loadSize((int)screenSize.x, (int)screenSize.y);

	toTextureFrameBuffer = FrameBuffer::createFrameBuffer();
	glDrawBuffer(GL_COLOR_ATTACHMENT0);
	toTextureTexture = FrameBuffer::createMultisampledColourAttachment((int)screenSize.x, (int)screenSize.y, samples);
	toTextureDepthBuffer = FrameBuffer::createMultisampledDepthBufferAttachment((int)screenSize.x, (int)screenSize.y, samples);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) printf("ToTexture Framebuffer not complete: %x\n", glCheckFramebufferStatus(GL_FRAMEBUFFER));

	FrameBuffer::unbindCurrentFrameBuffer((int)screenSize.x, (int)screenSize.y);
}
void PostProcessRenderer::resolveTo(PostProcessRenderer *renderer)
{
	FrameBuffer::resolveToFrameBuffer(toTextureFrameBuffer, (int)screenSizes.x, (int)screenSizes.y, renderer->getFrameBufferID(), renderer->getFrameBufferSizes().x, renderer->getFrameBufferSizes().y);
}
void PostProcessRenderer::resolveTo()
{
	FrameBuffer::resolveToScreen(toTextureFrameBuffer, (int)screenSizes.x, (int)screenSizes.y, (int)screenSizes.x, (int)screenSizes.y);
}

void PostProcessRenderer::load(glm::vec2 screenSize)
{
	screenSizes = screenSize;

	toTextureFrameBuffer = FrameBuffer::createFrameBuffer();
	glDrawBuffer(GL_COLOR_ATTACHMENT0);
	toTextureTexture = FrameBuffer::createTextureAttachment((int)screenSize.x, (int)screenSize.y);
	toTextureDepthBuffer = FrameBuffer::createDepthTextureAttachment((int)screenSize.x, (int)screenSize.y);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) printf("ToTexture Framebuffer not complete: %x\n", glCheckFramebufferStatus(GL_FRAMEBUFFER));

	FrameBuffer::unbindCurrentFrameBuffer((int)screenSize.x, (int)screenSize.y);
}
void PostProcessRenderer::load(char vertexFile[], char fragmentFile[], glm::vec2 screenSize)
{
	float position[] = { -1, 1, -1, -1, 1, 1, 1, -1 };
	PostProcessRenderer::screenQuad = loader.loadToVAO(position, 8, 2);
	screenSizes = screenSize;

	shader.load(vertexFile, fragmentFile);
	shader.loadSize((int)screenSize.x, (int)screenSize.y);

	toTextureFrameBuffer = FrameBuffer::createFrameBuffer();
	glDrawBuffer(GL_COLOR_ATTACHMENT0);
	toTextureTexture = FrameBuffer::createTextureAttachment((int)screenSize.x, (int)screenSize.y);
	toTextureDepthBuffer = FrameBuffer::createDepthTextureAttachment((int)screenSize.x, (int)screenSize.y);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) printf("ToTexture Framebuffer not complete: %x\n", glCheckFramebufferStatus(GL_FRAMEBUFFER));

	FrameBuffer::unbindCurrentFrameBuffer((int)screenSize.x, (int)screenSize.y);

	/*shader.start();
	shader.connectTextureUnits();
	shader.stop();*/
}

void PostProcessRenderer::renderToFrameBuffer(GLuint imageTextureID)
{
	bindRenderToTextureFrameBuffer();

	shader.start();

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, imageTextureID);
	
	renderTo();

	shader.stop();

	unbindFrameBuffer();
}
void PostProcessRenderer::renderToFrameBuffer(GLuint imageTextureID, GLuint imageTexture2ID)
{
	bindRenderToTextureFrameBuffer();

	shader.start();
	//shader.connectTextureUnits();

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, imageTexture2ID);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, imageTextureID);
	
	renderTo();

	shader.stop();

	unbindFrameBuffer();
}
void PostProcessRenderer::renderToScreen()
{
	shader.start();

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, toTextureTexture);

	renderTo();

	shader.stop();
}
void PostProcessRenderer::renderTo()
{
	glBindVertexArray(PostProcessRenderer::screenQuad.getVaoID()); //Bind VAO
	glEnableVertexAttribArray(0); //Enable attrib 0 (positions)

	glDisable(GL_DEPTH_TEST);

	glClear(GL_COLOR_BUFFER_BIT);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	glEnable(GL_DEPTH_TEST);

	glDisableVertexAttribArray(0); //Disable attrib 0 (positions)
	glBindVertexArray(0); //Unbind VAO
}

void PostProcessRenderer::bindRenderToTextureFrameBuffer()
{
	FrameBuffer::bindFrameBuffer(toTextureFrameBuffer, screenSizes.x, screenSizes.y);
}
void PostProcessRenderer::unbindFrameBuffer()
{
	FrameBuffer::unbindCurrentFrameBuffer(screenSizes.x, screenSizes.y);
}

GLuint PostProcessRenderer::getOutputTexture()
{
	return toTextureTexture;
}
GLuint PostProcessRenderer::getOutputDepthBufferID()
{
	return toTextureDepthBuffer;
}
GLuint PostProcessRenderer::getFrameBufferID()
{
	return toTextureFrameBuffer;
}
glm::vec2 PostProcessRenderer::getFrameBufferSizes()
{
	return screenSizes;
}