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

void texture2D::loadImage(const char * filename)
{
	textureid = loader.loadTexture(filename, false);
}

void texture2D::Draw(GuiShaderProgram & shader, gameobject &quad)
{
	shader.start();
	glBindVertexArray(quad.getVaoID());
	glEnableVertexAttribArray(0);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_DEPTH_TEST);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureid);

	glm::mat4 matrix = Maths::createTransformationMatrix(getPosition(), getRotationRad(), getScale());
	shader.loadTransformationMatrix(matrix);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, quad.getVertexCount());


	glEnable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);

	glDisableVertexAttribArray(0);
	glBindVertexArray(0);
	shader.stop();
}

glm::vec4 texture2D::getPixelValue(int x, int y)
{
	if (x < 0 || x >= width || y < 0 || y >= height) return glm::vec4(0.0);

	//int datapos = (width * y) + x; //Start top left
	int datapos = (int)((width * ((height - 1) - y)) + x) * 4; //Start bottom left

	glm::vec4 pixelColours = glm::vec4(data[datapos + 2], data[datapos + 1], data[datapos], data[datapos + 4]);

	return pixelColours;
}

void texture2D::setPosition(glm::vec2 pos)
{
	// Convert 0 -> 2 to -1 -> 1
	position = (pos -= 1.0f);
}

glm::vec2 texture2D::getPosition()
{
	return position;
}

void texture2D::setRotation(float rot)
{
	// Convert degrees to radians
	rotation = glm::radians(rot);
}

float texture2D::getRotationRad()
{
	return rotation;
}
float texture2D::getRotationDeg()
{
	return glm::degrees(rotation);
}


void texture2D::setScale(glm::vec2 scal)
{
	scale = scal;
}

glm::vec2 texture2D::getScale()
{
	return scale;
}
