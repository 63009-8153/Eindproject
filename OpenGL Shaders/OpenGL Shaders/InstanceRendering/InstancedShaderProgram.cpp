#include "header.h"

#include "InstancedShaderProgram.h"


//Constuctor
InstancedShaderProgram::InstancedShaderProgram()
{}
//Destuctor
InstancedShaderProgram::~InstancedShaderProgram()
{
	//Cleanup the shader
	cleanUp();
}

void InstancedShaderProgram::load(char vertexFile[], char fragmentFile[])
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

	bindAttribute(4, "transformationMatrix"); //Bind transformationMatrix variable to VAO position 4

	bindAttribute(8, "ShineReflectAmbient"); //Bind ShineReflectAmbient variable to VAO position 8
	
	bindAttribute(9, "offset"); //Bind offset variable to VAO position 9

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
	location_projectionMatrix = getUniformLocation("projectionMatrix");
	location_viewMatrix = getUniformLocation("viewMatrix");

	location_useFakeLighting = getUniformLocation("useFakeLighting");
	location_skyColour = getUniformLocation("skyColour");

	location_texture0 = getUniformLocation("textureSampler0");
	location_texture1 = getUniformLocation("textureSampler1");
	location_texture2 = getUniformLocation("textureSampler2");
	location_texture3 = getUniformLocation("textureSampler3");
	location_texture4 = getUniformLocation("textureSampler4");
	
	location_normalMap = getUniformLocation("normalMap");

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
void InstancedShaderProgram::start()
{
	glUseProgram(programID);
}
//Stop using this shader
void InstancedShaderProgram::stop()
{
	glUseProgram(0);
}

//Clean this shader up
void InstancedShaderProgram::cleanUp()
{
	//Stop using the shader if active
	stop();

	glDetachShader(programID, vertexShaderID);
	glDetachShader(programID, fragmentShaderID);

	glDeleteShader(vertexShaderID);
	glDeleteShader(fragmentShaderID);

	glDeleteProgram(programID);
}

//Load the projection matrix in the shader
void InstancedShaderProgram::loadProjectionMatrix(glm::mat4 matrix)
{
	loadMatrix(location_projectionMatrix, matrix);
}
//Load the view matrix in the shader
void InstancedShaderProgram::loadViewMatrix(glm::mat4 matrix)
{
	loadMatrix(location_viewMatrix, matrix);
}
//Load fake lighting in the shader. If true all normals are upward.
void InstancedShaderProgram::loadFakeLighting(bool fakeLighting) {
	loadBoolean(location_useFakeLighting, fakeLighting);
}
//Set skyColour for fog
void InstancedShaderProgram::loadSkyColour(glm::vec3 colour) {
	loadVector(location_skyColour, colour);
}

glm::vec3 InstancedShaderProgram::getEyeSpacePosition(Light *light, glm::mat4 viewMatrix)
{
	glm::vec3 position = light->getPosition();
	glm::vec4 eyeSpacePos = glm::vec4(position.x, position.y, position.z, 1.0f);
	eyeSpacePos = viewMatrix * eyeSpacePos;
	return glm::vec3(eyeSpacePos);
}

void InstancedShaderProgram::connectTextureUnits() {
	loadInt(location_texture0, 0);
	loadInt(location_texture1, 1);
	loadInt(location_texture2, 2);
	loadInt(location_texture3, 3);
	loadInt(location_texture4, 4);
	loadInt(location_normalMap, 5);
}

//Load a light
void InstancedShaderProgram::loadLights(std::vector<Light*> lights) {
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
int InstancedShaderProgram::loadShader(char file[], GLenum type)
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
void InstancedShaderProgram::bindAttribute(int attribute, char variableName[])
{
	glBindAttribLocation(programID, attribute, variableName);
}

//Get the location of a unform by name
GLuint InstancedShaderProgram::getUniformLocation(std::string uniformName)
{
	return glGetUniformLocation(programID, uniformName.c_str());
}

void InstancedShaderProgram::loadInt(GLuint location, GLuint value) {
	glUniform1i(location, value);
}
//Load a float in the shader at uniform location
void InstancedShaderProgram::loadFloat(GLuint location, GLfloat value) {
	glUniform1f(location, value);
}
//Load a vector3 in the shader at uniform location
void InstancedShaderProgram::loadVector(GLuint location, glm::vec3 vector){
	glUniform3f(location, vector.x, vector.y, vector.z);
}
//Load a vector3 in the shader at uniform location
void InstancedShaderProgram::loadVector(GLuint location, glm::vec2 vector) {
	glUniform2f(location, vector.x, vector.y);
}
//Load a boolean in the shader at uniform location
void InstancedShaderProgram::loadBoolean(GLuint location, bool value) {
	float toLoad = 0;
	if (value) toLoad = 1;
	glUniform1f(location, toLoad);
}
//Load a matrix in the shader at uniform location
void InstancedShaderProgram::loadMatrix(GLuint location, glm::mat4 matrix){
	glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
}