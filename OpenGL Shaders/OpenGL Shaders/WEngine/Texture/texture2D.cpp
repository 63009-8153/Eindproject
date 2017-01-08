#include "../header.h"

#include "texture2D.h"

texture2D::texture2D()
{
	width = 0;
	height = 0;
	filesize = 0;
	textureid = -1;

	position = glm::vec2(0.0f);
	rotation = 0.0f;
	scale = glm::vec2(1.0f);
}
texture2D::~texture2D()
{
}

void texture2D::loadImage(const char * filename, bool bmpAlign, bool generateMipmaps)
{
	textureid = loader.loadTextureInData(filename, bmpAlign, generateMipmaps, data, width, height, filesize, imageType);
}

void texture2D::loadImage(const char * filename, bool bmpAlign)
{
	loader.loadTextureInData(filename, bmpAlign, data, width, height, filesize, imageType);
}

glm::vec4 texture2D::getPixelValue(int x, int y)
{
	if (x < 0 || x >= width || y < 0 || y >= height) return glm::vec4(0.0);

	//int datapos = (width * y) + x; //Start top left
	int datapos = (int)((width * ((height - 1) - y)) + x) * 4; //Start bottom left

	glm::vec4 pixelColours = glm::vec4(data[datapos + 2], data[datapos + 1], data[datapos], data[datapos + 4]);

	return pixelColours;
}