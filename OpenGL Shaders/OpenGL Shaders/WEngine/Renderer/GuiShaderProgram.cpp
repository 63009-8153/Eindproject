#include "../header.h"

#include "GuiShaderProgram.h"


//Constuctor
GuiShaderProgram::GuiShaderProgram()
{}
//Destuctor
GuiShaderProgram::~GuiShaderProgram()
{
	//Cleanup the shader
	cleanUp();
}

void GuiShaderProgram::load(char vertexFile[], char fragmentFile[])
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

	//Get uniform locations

	location_transformationMatrix = getUniformLocation("transformationMatrix");

	// Detach and delete the shaders as the program is already compiled
	glDetachShader(programID, vertexShaderID);
	glDetachShader(programID, fragmentShaderID);

	glDeleteShader(vertexShaderID);
	glDeleteShader(fragmentShaderID);

	printf("GuiShader Loaded\n");
}

//Start using this shader
void GuiShaderProgram::start()
{
	glUseProgram(programID);
}
//Stop using this shader
void GuiShaderProgram::stop()
{
	glUseProgram(0);
}

//Clean this shader up
void GuiShaderProgram::cleanUp()
{
	//Stop using the shader if active
	stop();

	glDetachShader(programID, vertexShaderID);
	glDetachShader(programID, fragmentShaderID);

	glDeleteShader(vertexShaderID);
	glDeleteShader(fragmentShaderID);

	glDeleteProgram(programID);
}

void GuiShaderProgram::loadTransformationMatrix(glm::mat4 transformationMatrix)
{
	loadMatrix(location_transformationMatrix, transformationMatrix);
}


//Get the source of a file and create a shader id
int GuiShaderProgram::loadShader(char file[], GLenum type)
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
void GuiShaderProgram::bindAttribute(int attribute, char variableName[])
{
	glBindAttribLocation(programID, attribute, variableName);
}

//Get the location of a unform by name
GLuint GuiShaderProgram::getUniformLocation(std::string uniformName)
{
	return glGetUniformLocation(programID, uniformName.c_str());
}

void GuiShaderProgram::loadInt(GLuint location, GLuint value) {
	glUniform1i(location, value);
}
//Load a float in the shader at uniform location
void GuiShaderProgram::loadFloat(GLuint location, GLfloat value) {
	glUniform1f(location, value);
}
//Load a vector4 in the shader at uniform location
void GuiShaderProgram::loadVector(GLuint location, glm::vec4 vector) {
	glUniform4f(location, vector.x, vector.y, vector.z, vector.w);
}
//Load a vector3 in the shader at uniform location
void GuiShaderProgram::loadVector(GLuint location, glm::vec3 vector) {
	glUniform3f(location, vector.x, vector.y, vector.z);
}
//Load a vector3 in the shader at uniform location
void GuiShaderProgram::loadVector(GLuint location, glm::vec2 vector) {
	glUniform2f(location, vector.x, vector.y);
}
//Load a boolean in the shader at uniform location
void GuiShaderProgram::loadBoolean(GLuint location, bool value) {
	float toLoad = 0;
	if (value) toLoad = 1;
	glUniform1f(location, toLoad);
}
//Load a matrix in the shader at uniform location
void GuiShaderProgram::loadMatrix(GLuint location, glm::mat4 matrix) {
	glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
}