#ifndef _WATERTILE_
#define _WATERTILE_

#include "gameobject.h"

class WaterTile
{
public:
	WaterTile();
	WaterTile(glm::vec3 position, glm::vec3 rotation, glm::vec3 scale);
	~WaterTile();

	gameobject* getWaterTile();

	void setReflectionTexture(GLuint text);
	void setRefractionTexture(GLuint text);
	void setRefractionDepthTexture(GLuint text);
	void setDuDvTexture(GLuint text);
	void setNormalMapTexture(GLuint text);

	GLuint getReflectionTexture();
	GLuint getRefractionTexture();
	GLuint getRefractionDepthTexture();
	GLuint getDuDvTexture();
	GLuint getNormalMapTexture();

private:

	gameobject waterTile;

	GLuint reflectionText,
		   refractionText,
		   refractionDepthText,
		   DuDvMap,
		   NormalMap;


};

#endif