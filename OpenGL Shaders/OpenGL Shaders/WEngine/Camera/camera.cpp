#include "../header.h"

#include "camera.h"

//Initialise the camera
void Camera::Initalise(float _fov, int _screenWidth, int _screenHeight, float _near, float _far)
{
	fov = _fov;

	screenWidth = _screenWidth;
	screenHeight = _screenHeight;

	Near = _near;
	Far = _far;

	//Create the projection matrix. Since this is not created many times just set this once at start.
	createProjectionMatrix();
}

//Set the camera at a position and rotation.
void Camera::Set(glm::vec3 _position, glm::vec3 _rotation)
{
	position = _position;
	rotation = _rotation;
}

void Camera::LookAt(const float p_EyeX, const float p_EyeY, const float p_EyeZ, const float p_CenterX, const float p_CenterY, const float p_CenterZ)
{
	//TODO: no good code
	rotation *= 0;

	GLfloat l_X = p_EyeX - p_CenterX;
	GLfloat l_Y = p_EyeY - p_CenterY;
	GLfloat l_Z = p_EyeZ - p_CenterZ;

	if (l_X == l_Y && l_Y == l_Z && l_Z == 0.0f) return;
	if (l_X == l_Z && l_Z == 0.0f)
	{
		if (l_Y < 0.0f) rotation.x += -90.0f; else rotation.x += 90.0f;
		glTranslatef(-l_X, -l_Y, -l_Z);
		return;
	}

	GLfloat l_rX = 0.0f;
	GLfloat l_rY = 0.0f;

	GLfloat l_hB;
	GLfloat l_hA = (l_X == 0.0f) ? l_Z : hypot(l_X, l_Z);

	if (l_Z == 0.0f) l_hB = hypot(l_X, l_Y); 
	else l_hB = (l_Y == 0.0f) ? l_hA : hypot(l_Y, l_hA);

	l_rX = asin(l_Y / l_hB) * (180 / _PI);
	l_rY = asin(l_X / l_hA) * (180 / _PI);

	if (l_Z < 0.0f) l_rY += 180.0f; 
	else l_rY = 360.0f - l_rY;

	this->rotation.x += l_rX;
	this->rotation.y += l_rY;

	this->position.x = p_EyeX;
	this->position.y = p_EyeY;
	this->position.z = p_EyeZ;
}

//Get the camera's view matrix
glm::mat4 Camera::getViewMatrix() {
	return Maths::createViewMatrix(position, rotation);
}

glm::mat4 Camera::getDifferentViewMatrix(glm::vec3 _position, glm::vec3 _rotation)
{
	return Maths::createViewMatrix(_position, _rotation);
}
glm::mat4 Camera::getDifferentProjectionMatrix(float _fov)
{
	return glm::perspective(glm::radians(_fov), 1.0f, Near, Far);
}

//Get the camera's projection matrix. This is not changing a lot so the values are stored
glm::mat4 Camera::getProjectionMatrix() {
	return projectionMatrix;
}
//Create the projection matrix.
void Camera::createProjectionMatrix() {
	projectionMatrix = glm::perspective(glm::radians(fov), ((float)screenWidth / (float)screenHeight), Near, Far);
}