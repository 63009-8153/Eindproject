#ifndef _GUI_SHADER_PROGRAM_
#define _GUI_SHADER_PROGRAM_

class GuiShaderProgram {
public:
	GuiShaderProgram();
	~GuiShaderProgram();

	void load(char vertexFile[], char fragmentFile[]);

	void start();
	void stop();

	void cleanUp();

	void loadTransformationMatrix(glm::mat4 transformationMatrix);

private:
	int programID;
	int vertexShaderID;
	int fragmentShaderID;

	GLuint location_transformationMatrix;

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