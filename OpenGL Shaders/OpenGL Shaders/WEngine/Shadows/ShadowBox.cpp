#include "../header.h"
#include "../Utils/loader.h"

#include "ShadowBox.h"
#include "../FrameBuffers/framebuffer.h"

//Constructor
ShadowBox::ShadowBox(glm::mat4 _lightViewMatrix, Camera *camera)
{
	lightViewMatrix = _lightViewMatrix;
	cam = camera;
	calculateWidthAndHeights();
}
//Destructor and cleanup
ShadowBox::~ShadowBox()
{
}

void ShadowBox::update()
{

}
glm::vec3 ShadowBox::getCenter()
{
	float x = (minX + maxX) / 2.0f;
	float y = (minY + maxY) / 2.0f;
	float z = (minZ + maxZ) / 2.0f;
	glm::vec3 cen = glm::vec3(x, y, z);
	glm::mat4 invertedLight = glm::mat4();
	invertedLight = glm::inverse(lightViewMatrix);
	return glm::vec3(glm::translate(invertedLight, cen)[3]);
}
float ShadowBox::getWidth()
{
	return 0;
}
float ShadowBox::getHeight()
{
	return 0;
}
float ShadowBox::getLength()
{
	return 0;
}


glm::vec4 ShadowBox::calculateLightSpaceFrustumCorner(glm::vec3 startPoint, glm::vec3 direction, float width)
{
	return glm::vec4();
}
glm::mat4 ShadowBox::calculateCameraRotationMatrix()
{
	return glm::mat4();
}
void ShadowBox::calculateWidthAndHeights()
{
	farWidth = (float)(SHADOW_DISTANCE * std::tan(glm::radians(75.0f)));
	nearWidth = (float)(0.01 * std::tan(glm::radians(75.0f)));
	farHeight = farWidth / getAspectRatio();
	nearHeight = nearWidth / getAspectRatio();
}
float ShadowBox::getAspectRatio()
{
	return (float)SCREEN_WIDTH / SCREEN_HEIGHT;
}
