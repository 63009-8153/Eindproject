#ifndef _POST_PROCESS_RENDERER_
#define _POST_PROCESS_RENDERER_

#include "../GameObject/gameobject.h"
#include "../Utils/loader.h"
#include "PostProcessShaderProgram.h"

class PostProcessRenderer
{
public:
	PostProcessRenderer();
	~PostProcessRenderer();

	void load(glm::vec2 screenSize, int samples);
	void load(char vertexFile[], char fragmentFile[], glm::vec2 screenSize, int samples);
	void resolveTo(PostProcessRenderer *renderer);
	void resolveTo();

	void load(glm::vec2 screenSize);
	void load(char vertexFile[], char fragmentFile[], glm::vec2 screenSize);

	void renderTo();
	void renderToFrameBuffer(GLuint imageTextureID);
	void renderToFrameBuffer(GLuint imageTextureID, GLuint imageTexture2ID);
	void renderToScreen();

	void bindRenderToTextureFrameBuffer();
	void unbindFrameBuffer();

	GLuint getOutputTexture();
	GLuint getOutputDepthBufferID();
	GLuint getFrameBufferID();
	glm::vec2 getFrameBufferSizes();

	PostProcessShaderProgram shader;
private:

	GLuint toTextureFrameBuffer,
		toTextureTexture,
		toTextureDepthBuffer;

	glm::vec2 screenSizes;

	static gameobject screenQuad;
	Loader loader;
};

#endif