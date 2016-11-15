#ifndef _WATER_SHADER_PROGRAM_
#define _WATER_SHADER_PROGRAM_

#include "../Lights/Light.h"

class WaterShaderProgram {
public:
	WaterShaderProgram();
	~WaterShaderProgram();

	void load(char vertexFile[], char fragmentFile[]);

	void start();
	void stop();

	void cleanUp();

	void loadAmbientLight(float ambient);

	void loadTransformationMatrix(glm::mat4 matrix);
	void loadProjectionMatrix(glm::mat4 matrix);
	void loadViewMatrix(glm::mat4 matrix);

	void loadShineVariables(float damper, float reflectivity);
	void loadFakeLighting(bool fakeLighting);
	void loadSkyColour(glm::vec3 colour);

	void loadLights(std::vector<Light*> lights);

	void connectTextureUnits();
	void loadMoveFactor(float factor);

	void loadCameraPosition(glm::vec3 pos);

private:
	int programID;
	int vertexShaderID;
	int fragmentShaderID;

	static const int MAX_LIGHTS = 4;

	GLuint	location_transformationMatrix,
			location_projectionMatrix,
			location_viewMatrix;
	GLuint	location_lightPosition[MAX_LIGHTS],
			location_lightColour[MAX_LIGHTS],
			location_attenuation[MAX_LIGHTS];

	GLuint	location_shineDamper,
			location_reflectivity,
			location_useFakeLighting,
			location_ambientLight;

	GLuint	location_reflectiontexture,
			location_refractiontexture,
			location_dudvMap,
			location_normalMap,
			location_depthMap;

	GLuint	location_moveFactor,
			location_cameraPosition;

	GLuint	location_skyColour;

	static GLuint loadShader(char file[], GLenum type);

	void bindAttribute(int attribute, char variableName[]);

	GLuint getUniformLocation(std::string uniformName);

	void loadInt(GLuint location, GLuint value);
	void loadFloat(GLuint location, GLfloat value);
	void loadVector(GLuint location, glm::vec4 vector);
	void loadVector(GLuint location, glm::vec3 vector);
	void loadVector(GLuint location, glm::vec2 vector);
	void loadBoolean(GLuint location, bool value);
	void loadMatrix(GLuint location, glm::mat4 matrix);
};

#endif