#include "../header.h"

#include "textureCubemap.h"
#include "../FrameBuffers/framebuffer.h"

textureCubemap::textureCubemap()
{
	textureid = -1;
	initialisedForFrameBuffer = false;
}
textureCubemap::~textureCubemap()
{
	glDeleteTextures(1, &textureid);
}
void textureCubemap::initialseFrameBuffer(int size)
{
	textureSize = size;

	frameBufferId = FrameBuffer::createFrameBuffer();
	glDrawBuffer(GL_COLOR_ATTACHMENT0);
	depthBufferId = FrameBuffer::createDepthBufferAttachment(size, size, GL_DEPTH_COMPONENT24);

	initialisedForFrameBuffer = true;

	createEmptyCubeMap();
}
void textureCubemap::createEmptyCubeMap()
{
	glGenTextures(1, &textureid);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureid);

	for (unsigned int i = 0; i < 6; i++)
	{
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA8, textureSize, textureSize, 0, GL_BGRA, GL_UNSIGNED_BYTE, NULL);
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}
void textureCubemap::loadCubeMap(std::string filename[6])
{
	unsigned char * data[6];
	glm::vec2 sizes[6];

	//Load all 6 images in a texture2D object
	for (unsigned int i = 0; i < 6; i++)
	{
		//Load image in texture2D by filename
		textures[i].loadImage(filename[i].c_str(), true);

		//Save datapointer of each texture2D's image data
		data[i] = textures[i].data;

		//Set the size of the image
		sizes[i] = glm::vec2(textures[i].width, textures[i].height);
	}

	textureid = loader.loadCubeMap(data, sizes, textures[0].imageType);
}

void textureCubemap::bindFrameBuffer()
{
	FrameBuffer::bindFrameBuffer(frameBufferId, textureSize, textureSize);
}
void textureCubemap::bindFrameBufferRenderTexture(GLenum side)
{
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, side, textureid, 0);
}
void textureCubemap::unbindFrameBuffer(int sw, int sh)
{
	FrameBuffer::unbindCurrentFrameBuffer(sw, sh);
}
void textureCubemap::deleteBuffers()
{
	FrameBuffer::deleteRenderBuffer(depthBufferId);
	FrameBuffer::deleteFrameBuffer(frameBufferId);
}