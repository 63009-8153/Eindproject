#ifndef _WATER_MASTER_RENDERER_
#define _WATER_MASTER_RENDERER_

#include "../Camera/camera.h"

#include "../GameObject/gameobject.h"
#include "WaterShaderProgram.h"

#include "../GameObject/WaterTile.h"

#include "../Lights/Light.h"

#define REFLECTION_WIDTH 240
#define REFLECTION_HEIGHT 120

#define REFRACTION_WIDTH 1280
#define REFRACTION_HEIGHT 720

extern glm::vec3 clearColor;

class WaterMasterRenderer
{
public:
	WaterMasterRenderer();
	~WaterMasterRenderer();
	void cleanUp();

	void load(char vertexFile[], char fragmentFile[], Camera *camera);

	void addToRenderList(WaterTile *object);

	void renderWireframe();
	void render(std::vector<Light*> lights, Camera *camera);
	void clearRenderPass();

	static void DisableCulling();
	static void EnableCulling();

	void bindReflectionFrameBuffer();
	void bindRefractionFrameBuffer();
	void unbindCurrentFrameBuffer();

	GLuint getReflectionTexture();
	GLuint getRefractionTexture();
	GLuint getRefractionDepthTexture();

	float getMoveFactor();
	void setMoveFactor(float _moveFactor);
private:
	WaterShaderProgram shader;
	Loader loader;

	gameobject waterTile;

	GLuint reflectionFrameBuffer;
	GLuint reflectionTexture;
	GLuint reflectionDepthBuffer;

	GLuint refractionFrameBuffer;
	GLuint refractionTexture;
	GLuint refractionDepthTexture;

	void initialiseReflectionFrameBuffer();
	void initialiseRefractionFrameBuffer();

	bool wireframeRender;
	float moveFactor;

	std::vector<WaterTile> WaterTiles;
};

#endif // !_MASTER_RENDERER_
