#ifndef _INSTANCE_SHADER_PROGRAM_
#define _INSTANCE_SHADER_PROGRAM_

#include "Light.h"

class InstancedShaderProgram {
public:
	InstancedShaderProgram();
	~InstancedShaderProgram();

	void load(char vertexFile[], char fragmentFile[]);

	void start();
	void stop();

	void cleanUp();

	void loadProjectionMatrix(glm::mat4 matrix);
	void loadViewMatrix(glm::mat4 matrix);
	void loadFakeLighting(bool fakeLighting);
	void loadSkyColour(glm::vec3 colour);

	glm::vec3 getEyeSpacePosition(Light *light, glm::mat4 viewMatrix);

	void connectTextureUnits();

	void loadLights(std::vector<Light*> lights);

private:
	int programID;
	int vertexShaderID;
	int fragmentShaderID;

	static const int MAX_LIGHTS = 4;

	GLuint	location_projectionMatrix,
			location_viewMatrix;

	GLuint	location_lightPosition[MAX_LIGHTS],
			location_lightColour[MAX_LIGHTS],
			location_attenuation[MAX_LIGHTS];

	GLuint	location_useFakeLighting;
	GLuint	location_skyColour;

	GLuint	location_texture0,
			location_texture1,
			location_texture2,
			location_texture3,
			location_texture4,
			location_normalMap;

	static int loadShader(char file[], GLenum type);

	void bindAttribute(int attribute, char variableName[]);
	
	GLuint getUniformLocation(std::string uniformName);

	void loadInt(GLuint location, GLuint value);
	void loadFloat(GLuint location, GLfloat value);
	void loadVector(GLuint location, glm::vec3 vector);
	void loadVector(GLuint location, glm::vec2 vector);
	void loadBoolean(GLuint location, bool value);
	void loadMatrix(GLuint location, glm::mat4 matrix);
};

#endif