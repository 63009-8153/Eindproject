#ifndef _POST_PROCESS_SHADER_PROGRAM_
#define _POST_PROCESS_SHADER_PROGRAM_

class PostProcessShaderProgram {
public:
	PostProcessShaderProgram();
	~PostProcessShaderProgram();

	void load(char vertexFile[], char fragmentFile[]);

	void connectTextureUnits();
	void loadContrast(float contrast);
	void loadHighlightAmount(float amount);
	void loadSize(int width, int height);
	void loadInverseFilterTextureSize(glm::vec3 value);
	void loadFXAAParameters(float span_max, float reduce_min, float reduce_mul);

	void start();
	void stop();

	void cleanUp();

private:
	int programID;
	int vertexShaderID;
	int fragmentShaderID;

	GLuint	location_windowtexture,
			location_texture2;
	GLuint	location_contrast,
			location_highlightAmount;
	GLuint	location_targetWidth,
			location_targetHeight;
	GLuint	location_inverseFilterTextureSize;
	GLuint	location_fxaa_span_max,
			location_fxaa_reduce_min,
			location_fxaa_reduce_mul;

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