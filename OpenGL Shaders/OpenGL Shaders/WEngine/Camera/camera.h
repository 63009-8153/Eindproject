#ifndef _CAMERA_
#define _CAMERA_

#include "../Utils/Maths.h"

class Camera
{
public:
	void Initalise(float _fov, int _screenWidth, int _screenHeight, float _near, float _far);

	void Set(glm::vec3 _position, glm::vec3 _rotation);

	void LookAt(const float p_EyeX, const float p_EyeY, const float p_EyeZ, const float p_CenterX, const float p_CenterY, const float p_CenterZ);

	glm::mat4 getProjectionMatrix();
	glm::mat4 getViewMatrix();

	glm::mat4 getDifferentViewMatrix(glm::vec3 _position, glm::vec3 _rotation);
	glm::mat4 getDifferentProjectionMatrix(float _fov);

	glm::vec3 position, 
			  rotation;

	int screenWidth, screenHeight;
	float Near, Far;
	float fov;

private:
	glm::mat4 projectionMatrix;

	void createProjectionMatrix();
	
};

#endif

