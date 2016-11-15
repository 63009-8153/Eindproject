#ifndef _WATER_MASTER_RENDERER_
#define _WATER_MASTER_RENDERER_

#include "../Camera/camera.h"

#include "../GameObject/gameobject.h"

#include "../GameObject/WaterTile.h"

#include "../Lights/Light.h"

#define OFFSET 10
#define UP glm::vec4(0, 1, 0, 0)
#define FORWARD glm::vec4(0, 0, -1, 0);
#define SHADOW_DISTANCE 100

class ShadowBox
{
public:
	ShadowBox(glm::mat4 _lightViewMatrix, Camera *camera);
	~ShadowBox();
	
	void update();
	glm::vec3 getCenter();
	float getWidth();
	float getHeight();
	float getLength();

private:
	float minX, maxX;
	float minY, maxY;
	float minZ, maxZ;
	glm::mat4 lightViewMatrix;
	Camera *cam;

	float farHeight, farWidth, nearHeight, nearWidth;

	glm::vec4 calculateLightSpaceFrustumCorner(glm::vec3 startPoint, glm::vec3 direction, float width);
	glm::mat4 calculateCameraRotationMatrix();
	void calculateWidthAndHeights();
	float getAspectRatio();
};

#endif // !_MASTER_RENDERER_
