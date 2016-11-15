#include "../header.h"

#include "WaterShaderProgram.h"


//Constuctor
WaterShaderProgram::WaterShaderProgram()
{}
//Destuctor
WaterShaderProgram::~WaterShaderProgram()
{
	//Cleanup the shader
	cleanUp();
}

void WaterShaderProgram::load(char vertexFile[], char fragmentFile[])
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
	
	location_reflectiontexture = getUniformLocation("reflectionTexture0");
	location_refractiontexture = getUniformLocation("refractionTexture1");
	location_dudvMap = getUniformLocation("dudvMap");
	location_normalMap = getUniformLocation("normalMap");
	location_depthMap = getUniformLocation("depthMap");

	location_moveFactor = getUniformLocation("moveFactor");
	location_cameraPosition = getUniformLocation("cameraPosition");

	location_shineDamper = getUniformLocation("shineDamper");
	location_reflectivity = getUniformLocation("reflectivity");

	location_ambientLight = getUniformLocation("ambientLight");
	location_skyColour = getUniformLocation("skyColour");

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

	printf("WaterShader Loaded\n");
}

//Start using this shader
void WaterShaderProgram::start()
{
	glUseProgram(programID);
}
//Stop using this shader
void WaterShaderProgram::stop()
{
	glUseProgram(0);
}

//Clean this shader up
void WaterShaderProgram::cleanUp()
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
void WaterShaderProgram::loadAmbientLight(float ambient) {
	loadFloat(location_ambientLight, ambient);
}
//Load the transformation matrix in the shader
void WaterShaderProgram::loadTransformationMatrix(glm::mat4 matrix)
{
	loadMatrix(location_transformationMatrix, matrix);
}
//Load the projection matrix in the shader
void WaterShaderProgram::loadProjectionMatrix(glm::mat4 matrix)
{
	loadMatrix(location_projectionMatrix, matrix);
}
//Load the view matrix in the shader
void WaterShaderProgram::loadViewMatrix(glm::mat4 matrix)
{
	loadMatrix(location_viewMatrix, matrix);
}
//Load the shineVariables in the shader. For specular lighting
void WaterShaderProgram::loadShineVariables(float damper, float reflectivity)
{
	loadFloat(location_shineDamper, damper);
	loadFloat(location_reflectivity, reflectivity);
}
//Load fake lighting in the shader. If true all normals are upward.
void WaterShaderProgram::loadFakeLighting(bool fakeLighting) {
	loadBoolean(location_useFakeLighting, fakeLighting);
}
//Set skyColour for fog
void WaterShaderProgram::loadSkyColour(glm::vec3 colour) {
	loadVector(location_skyColour, colour);
}

//Load a light
void WaterShaderProgram::loadLights(std::vector<Light*> lights) {
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

void WaterShaderProgram::connectTextureUnits()
{
	loadInt(location_reflectiontexture, 1);
	loadInt(location_refractiontexture, 2);
	loadInt(location_dudvMap, 3);
	loadInt(location_normalMap, 4);
	loadInt(location_depthMap, 5);
}
void WaterShaderProgram::loadMoveFactor(float factor)
{
	loadFloat(location_moveFactor, factor);
}
void WaterShaderProgram::loadCameraPosition(glm::vec3 pos)
{
	loadVector(location_cameraPosition, pos);
}

//Get the source of a file and create a shader id
GLuint WaterShaderProgram::loadShader(char file[], GLenum type)
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

	//Debug shaderCode
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
void WaterShaderProgram::bindAttribute(int attribute, char variableName[])
{
	glBindAttribLocation(programID, attribute, variableName);
}

//Get the location of a unform by name
GLuint WaterShaderProgram::getUniformLocation(std::string uniformName)
{
	return glGetUniformLocation(programID, uniformName.c_str());
}

void WaterShaderProgram::loadInt(GLuint location, GLuint value) {
	glUniform1i(location, value);
}
//Load a float in the shader at uniform location
void WaterShaderProgram::loadFloat(GLuint location, GLfloat value) {
	glUniform1f(location, value);
}
//Load a vector4 in the shader at uniform location
void WaterShaderProgram::loadVector(GLuint location, glm::vec4 vector) {
	glUniform4f(location, vector.x, vector.y, vector.z, vector.w);
}
//Load a vector3 in the shader at uniform location
void WaterShaderProgram::loadVector(GLuint location, glm::vec3 vector) {
	glUniform3f(location, vector.x, vector.y, vector.z);
}
//Load a vector2 in the shader at uniform location
void WaterShaderProgram::loadVector(GLuint location, glm::vec2 vector) {
	glUniform2f(location, vector.x, vector.y);
}
//Load a boolean in the shader at uniform location
void WaterShaderProgram::loadBoolean(GLuint location, bool value) {
	float toLoad = 0;
	if (value) toLoad = 1;
	glUniform1f(location, toLoad);
}
//Load a matrix in the shader at uniform location
void WaterShaderProgram::loadMatrix(GLuint location, glm::mat4 matrix) {
	glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
}