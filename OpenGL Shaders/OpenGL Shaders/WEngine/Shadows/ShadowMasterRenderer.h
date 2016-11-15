#ifndef _SHADOW_MASTER_RENDERER_
#define _SHADOW_MASTER_RENDERER_

#include "../Camera/camera.h"

#include "../GameObject/gameobject.h"
#include "../Renderer/ShaderProgram.h"

#include "../Lights/Light.h"

#include "../Renderer/renderer.h"

#define SHADOW_WIDTH 4096
#define SHADOW_HEIGHT 4096

extern glm::vec3 clearColor;

class ShadowMasterRenderer
{
public:
	ShadowMasterRenderer();
	~ShadowMasterRenderer();
	void cleanUp();

	void load(char vertexFile[], char fragmentFile[]);

	void render(Light *light, Camera *camera, std::map<GLuint, std::vector<gameobject>> *gameobjects);

	void bindShadowFrameBuffer();
	void unbindCurrentFrameBuffer();

	GLuint getShadowDepthTexture();
private:

	Renderer renderer;
	ShaderProgram shader;

	GLuint shadowFrameBuffer;
	GLuint shadowDepthTexture;

	void initialiseShadowFrameBuffer();
};

#endif // !_MASTER_RENDERER_
