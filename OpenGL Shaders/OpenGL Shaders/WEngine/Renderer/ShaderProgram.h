#ifndef _SHADER_PROGRAM_
#define _SHADER_PROGRAM_

#include "../Lights/Light.h"

class ShaderProgram {
public:
	ShaderProgram();
	~ShaderProgram();

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
	void loadNumberOfRows(int number);
	void loadOffset(glm::vec2 offset);
	void loadLightSpaceMatrix(glm::mat4 matrix);
	void loadUseSpecularMap(bool useSpecularMap);

	void loadUseReflectionCubeMap(bool useReflectionCubeMap);
	void loadUseRefractionCubeMap(bool useRefractionCubeMap);
	void loadReflectionRefractionRatio(float value);
	void loadReflectionColourRatio(float value);

	void loadCameraPosition(glm::vec3 pos);

	void connectTextureUnits();

	void loadClipPlane(glm::vec4 plane);

	void loadLights(std::vector<Light*> lights);

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

	GLuint	location_skyColour;

	GLuint	location_texture0,
			location_texture1,
			location_texture2,
			location_texture3,
			location_texture4,
			location_normalMap,
			location_shadowMap;

	GLuint	location_specularMap,
			location_usesSpecularMap;

	GLuint	location_enviromentCubeMap,
			location_usesReflectionCubeMap,
			location_usesRefractionCubeMap,
			location_reflectionRefractionRatio,
			location_reflectionColourRatio;

	GLuint	location_numberOfRows,
			location_offset;

	GLuint	location_plane;

	GLuint	location_lightSpaceMatrix,
		location_cameraPosition;

	static int loadShader(char file[], GLenum type);

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