#include "../header.h"

#include "framebuffer.h"

FrameBuffer::FrameBuffer()
{
}

FrameBuffer::~FrameBuffer()
{
}

void FrameBuffer::unbindCurrentFrameBuffer(int DisplayWidth, int DisplayHeight)
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, DisplayWidth, DisplayHeight);
	glBindTexture(GL_TEXTURE_2D, 0); //To make sure the texture is not bound
}

void FrameBuffer::bindFrameBuffer(GLuint framebuffer, int width, int height)
{
	glBindTexture(GL_TEXTURE_2D, 0); //To make sure the texture is not bound
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
	glViewport(0, 0, width, height);
}

void FrameBuffer::resolveToFrameBuffer(GLuint inputFrameBuffer, int iWidth, int iHeight, GLuint outputFrameBuffer, int oWidth, int oHeight)
{
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, outputFrameBuffer);
	glBindFramebuffer(GL_READ_FRAMEBUFFER, inputFrameBuffer);
	glBlitFramebuffer(0, 0, iWidth, iHeight, 0, 0, oWidth, oHeight, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT, GL_NEAREST);
	unbindCurrentFrameBuffer(iWidth, iHeight);
}
void FrameBuffer::resolveToScreen(GLuint inputFrameBuffer, int iWidth, int iHeight, int screenWidth, int screenHeight)
{
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	glBindFramebuffer(GL_READ_FRAMEBUFFER, inputFrameBuffer);
	glDrawBuffer(GL_BACK);
	glBlitFramebuffer(0, 0, iWidth, iHeight, 0, 0, screenWidth, screenHeight, GL_COLOR_BUFFER_BIT, GL_NEAREST);
	unbindCurrentFrameBuffer(iWidth, iHeight);
}

GLuint FrameBuffer::createFrameBuffer()
{
	GLuint framebuffer;
	glGenFramebuffers(1, &framebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
	return framebuffer;
}

GLuint FrameBuffer::createTextureAttachment(int width, int height)
{
	GLuint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, texture, 0);
	return texture;
}

GLuint FrameBuffer::createDepthTextureAttachment(int width, int height)
{
	GLuint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);


	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	GLfloat borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, texture, 0);
	return texture;
}

GLuint FrameBuffer::createDepthBufferAttachment(int width, int height)
{
	GLuint depthBuffer;
	glGenRenderbuffers(1, &depthBuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuffer);
	return depthBuffer;
}
GLuint FrameBuffer::createDepthBufferAttachment(int width, int height, GLenum attachmentFormat)
{
	GLuint depthBuffer;
	glGenRenderbuffers(1, &depthBuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, attachmentFormat, width, height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuffer);
	return depthBuffer;
}

GLuint FrameBuffer::createMultisampledDepthBufferAttachment(int width, int height, int samples)
{
	GLuint depthBuffer;
	glGenRenderbuffers(1, &depthBuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer);
	glRenderbufferStorageMultisample(GL_RENDERBUFFER, samples, GL_DEPTH_COMPONENT24, width, height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuffer);
	return depthBuffer;
}

GLuint FrameBuffer::createMultisampledColourAttachment(int width, int height, int samples)
{
	GLuint colourBuffer;
	glGenRenderbuffers(1, &colourBuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, colourBuffer);
	glRenderbufferStorageMultisample(GL_RENDERBUFFER, samples, GL_RGBA8, width, height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, colourBuffer);
	return colourBuffer;
}

void FrameBuffer::deleteFrameBuffer(GLuint framebufferID)
{
	glDeleteFramebuffers(1, &framebufferID);
}

void FrameBuffer::deleteRenderBuffer(GLuint renderbufferID)
{
	glDeleteRenderbuffers(1, &renderbufferID);
}

void FrameBuffer::deleteTexture(GLuint textureID)
{
	glDeleteTextures(1, &textureID);
}