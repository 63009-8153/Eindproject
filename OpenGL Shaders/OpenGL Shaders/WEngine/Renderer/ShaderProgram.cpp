#include "../header.h"

#include "ShaderProgram.h"


//Constuctor
ShaderProgram::ShaderProgram()
{}
//Destuctor
ShaderProgram::~ShaderProgram()
{
	//Cleanup the shader
	cleanUp();
}

void ShaderProgram::load(char vertexFile[], char fragmentFile[])
{
	//Get Shader sources
	vertexShaderID = loadShader(vertexFile, GL_VERTEX_SHADER);
	fragmentShaderID = loadShader(fragmentFile, GL_FRAGMENT_SHADER);

	//Create a program for the shaders
	programID = glCreateProgram();

	//Attach the shader sources to the program
	glAttachShader(programID, vertexShaderID);
	glAttachShader(programID, fragmentShaderID);

	//Bind in variables
	bindAttribute(0, "position"); //Bind position variable to VAO position 0
	bindAttribute(1, "textureCoords"); //Bind textureCoords variable to VAO position 1
	bindAttribute(2, "normal"); //Bind normal variable to VAO position 2
	bindAttribute(3, "tangents"); //Bind tangents variable to VAO position 3

	//Link the program
	glLinkProgram(programID);

	GLint success;
	glGetProgramiv(programID, GL_LINK_STATUS, &success);
	if (!success)
	{
		GLchar infoLog[512];
		glGetProgramInfoLog(programID, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}

	//Validate the program
	glValidateProgram(programID);

	//Get all the uniform locations
	location_transformationMatrix = getUniformLocation("transformationMatrix");
	location_projectionMatrix = getUniformLocation("projectionMatrix");
	location_viewMatrix = getUniformLocation("viewMatrix");
	location_shineDamper = getUniformLocation("shineDamper");
	location_reflectivity = getUniformLocation("reflectivity");
	location_useFakeLighting = getUniformLocation("useFakeLighting");
	location_ambientLight = getUniformLocation("ambientLight");
	location_skyColour = getUniformLocation("skyColour");

	location_texture0 = getUniformLocation("textureSampler0");
	location_texture1 = getUniformLocation("textureSampler1");
	location_texture2 = getUniformLocation("textureSampler2");
	location_texture3 = getUniformLocation("textureSampler3");
	location_texture4 = getUniformLocation("textureSampler4");

	location_specularMap = getUniformLocation("specularMap");
	location_usesSpecularMap = getUniformLocation("usesSpecularMap");

	location_numberOfRows = getUniformLocation("numberOfRows");
	location_offset = getUniformLocation("offset");

	location_normalMap = getUniformLocation("normalMap");
	location_shadowMap = getUniformLocation("shadowMap");

	location_enviromentCubeMap = getUniformLocation("enviromentCubeMap");
	location_usesReflectionCubeMap = getUniformLocation("usesReflectionCubeMap");
	location_usesRefractionCubeMap = getUniformLocation("usesRefractionCubeMap");
	location_reflectionRefractionRatio = getUniformLocation("reflectionRefractionRatio");
	location_reflectionColourRatio = getUniformLocation("reflectionColourRatio");

	location_plane = getUniformLocation("plane");
	
	location_lightSpaceMatrix = getUniformLocation("lightSpaceMatrix");
	location_cameraPosition = getUniformLocation("cameraPosition");

	for (int i = 0; i < MAX_LIGHTS; i++) {
		location_lightPosition[i] = getUniformLocation("lightPosition[" + std::to_string(i) + "]");
		location_lightColour[i] = getUniformLocation("lightColour[" + std::to_string(i) + "]");
		location_attenuation[i] = getUniformLocation("attenuation[" + std::to_string(i) + "]");
	}

	// Detach and delete the shaders as the program is already compiled
	glDetachShader(programID, vertexShaderID);
	glDetachShader(programID, fragmentShaderID);

	glDeleteShader(vertexShaderID);
	glDeleteShader(fragmentShaderID);

	printf("Shader Loaded\n");
}

//Start using this shader
void ShaderProgram::start()
{
	glUseProgram(programID);
}
//Stop using this shader
void ShaderProgram::stop()
{
	glUseProgram(0);
}

//Clean this shader up
void ShaderProgram::cleanUp()
{
	//Stop using the shader if active
	stop();

	glDetachShader(programID, vertexShaderID);
	glDetachShader(programID, fragmentShaderID);

	glDeleteShader(vertexShaderID);
	glDeleteShader(fragmentShaderID);

	glDeleteProgram(programID);
}

//Set the ambient light of the object
void ShaderProgram::loadAmbientLight(float ambient) {
	loadFloat(location_ambientLight, ambient);
}
//Load the transformation matrix in the shader
void ShaderProgram::loadTransformationMatrix(glm::mat4 matrix)
{
	loadMatrix(location_transformationMatrix, matrix);
}
//Load the projection matrix in the shader
void ShaderProgram::loadProjectionMatrix(glm::mat4 matrix)
{
	loadMatrix(location_projectionMatrix, matrix);
}
//Load the view matrix in the shader
void ShaderProgram::loadViewMatrix(glm::mat4 matrix)
{
	loadMatrix(location_viewMatrix, matrix);
}
//Load the shineVariables in the shader. For specular lighting
void ShaderProgram::loadShineVariables(float damper, float reflectivity)
{
	loadFloat(location_shineDamper, damper);
	loadFloat(location_reflectivity, reflectivity);
}
//Load fake lighting in the shader. If true all normals are upward.
void ShaderProgram::loadFakeLighting(bool fakeLighting) {
	loadBoolean(location_useFakeLighting, fakeLighting);
}
//Set skyColour for fog
void ShaderProgram::loadSkyColour(glm::vec3 colour) {
	loadVector(location_skyColour, colour);
}
void ShaderProgram::loadNumberOfRows(int number)
{
	loadFloat(location_numberOfRows, (GLfloat)number);
}
void ShaderProgram::loadOffset(glm::vec2 offset)
{
	loadVector(location_offset, offset);
}

glm::vec3 getEyeSpacePosition(Light *light, glm::mat4 viewMatrix)
{
	glm::vec3 position = light->getPosition();
	glm::vec4 eyeSpacePos = glm::vec4(position.x, position.y, position.z, 1.0f);
	eyeSpacePos = viewMatrix * eyeSpacePos;
	return glm::vec3(eyeSpacePos);
}

void ShaderProgram::loadLightSpaceMatrix(glm::mat4 matrix)
{
	loadMatrix(location_lightSpaceMatrix, matrix);
}
void ShaderProgram::loadUseSpecularMap(bool useSpecularMap)
{
	loadBoolean(location_usesSpecularMap, useSpecularMap);
}
void ShaderProgram::loadUseReflectionCubeMap(bool useReflectionCubeMap)
{
	loadBoolean(location_usesReflectionCubeMap, useReflectionCubeMap);
}
void ShaderProgram::loadUseRefractionCubeMap(bool useRefractionCubeMap)
{
	loadBoolean(location_usesRefractionCubeMap, useRefractionCubeMap);
}
void ShaderProgram::loadReflectionRefractionRatio(float value)
{
	loadFloat(location_reflectionRefractionRatio, value);
}
void ShaderProgram::loadReflectionColourRatio(float value)
{
	loadFloat(location_reflectionColourRatio, value);
}

void ShaderProgram::loadCameraPosition(glm::vec3 pos)
{
	loadVector(location_cameraPosition, pos);
}

void ShaderProgram::connectTextureUnits() {
	loadInt(location_texture0, 0);
	loadInt(location_texture1, 1);
	loadInt(location_texture2, 2);
	loadInt(location_texture3, 3);
	loadInt(location_texture4, 4);
	loadInt(location_normalMap, 5);
	loadInt(location_shadowMap, 6);
	loadInt(location_specularMap, 7);

	loadInt(location_enviromentCubeMap, 8);
}

void ShaderProgram::loadClipPlane(glm::vec4 plane) {
	loadVector(location_plane, plane);
}

//Load a light
void ShaderProgram::loadLights(std::vector<Light*> lights) {
	for (int i = 0; i < MAX_LIGHTS; i++) {
		if (i < lights.size()) {
			loadVector(location_lightPosition[i], lights[i]->getPosition());
			loadVector(location_lightColour[i], lights[i]->getColour());
			loadVector(location_attenuation[i], lights[i]->getAttenuation());
		}
		else {
			loadVector(location_lightPosition[i], glm::vec3(0, 0, 0));
			loadVector(location_lightColour[i], glm::vec3(0, 0, 0));
			loadVector(location_attenuation[i], glm::vec3(1, 0, 0));
		}
	}
}

//Get the source of a file and create a shader id
int ShaderProgram::loadShader(char file[], GLenum type)
{
	//Retrieve the source code from file
	std::string source;
	std::ifstream shaderFile;

	// ensures ifstream objects can throw exceptions:
	shaderFile.exceptions(std::ifstream::badbit);
	try
	{
		// Open files
		shaderFile.open(file);

		std::stringstream shaderStream;

		// Read file's buffer contents into streams
		shaderStream << shaderFile.rdbuf();

		// close file handlers
		shaderFile.close();

		// Convert stream into string
		source = shaderStream.str();
	}
	catch (std::ifstream::failure e)
	{
		std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
	}
	const GLchar* shaderCode = source.c_str();

	//printf("%s\n", shaderCode);

	//Compile shaders
	GLuint shaderID;

	// Create Shader
	shaderID = glCreateShader(type);
	glShaderSource(shaderID, 1, &shaderCode, NULL);
	glCompileShader(shaderID);

	// Print compile errors if any
	GLint success;
	glGetShaderiv(shaderID, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		GLchar infoLog[512];
		glGetShaderInfoLog(shaderID, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	return shaderID;
}

//Bind attribute of VAO to a variable in the shader
void ShaderProgram::bindAttribute(int attribute, char variableName[])
{
	glBindAttribLocation(programID, attribute, variableName);
}

//Get the location of a unform by name
GLuint ShaderProgram::getUniformLocation(std::string uniformName)
{
	return glGetUniformLocation(programID, uniformName.c_str());
}

void ShaderProgram::loadInt(GLuint location, GLuint value) {
	glUniform1i(location, value);
}
//Load a float in the shader at uniform location
void ShaderProgram::loadFloat(GLuint location, GLfloat value) {
	glUniform1f(location, value);
}
//Load a vector4 in the shader at uniform location
void ShaderProgram::loadVector(GLuint location, glm::vec4 vector) {
	glUniform4f(location, vector.x, vector.y, vector.z, vector.w);
}
//Load a vector3 in the shader at uniform location
void ShaderProgram::loadVector(GLuint location, glm::vec3 vector) {
	glUniform3f(location, vector.x, vector.y, vector.z);
}
//Load a vector3 in the shader at uniform location
void ShaderProgram::loadVector(GLuint location, glm::vec2 vector) {
	glUniform2f(location, vector.x, vector.y);
}
//Load a boolean in the shader at uniform location
void ShaderProgram::loadBoolean(GLuint location, bool value) {
	float toLoad = 0;
	if (value) toLoad = 1;
	glUniform1f(location, toLoad);
}
//Load a matrix in the shader at uniform location
void ShaderProgram::loadMatrix(GLuint location, glm::mat4 matrix) {
	glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
}