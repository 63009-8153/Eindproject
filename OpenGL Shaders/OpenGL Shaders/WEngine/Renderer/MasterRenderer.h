#ifndef _MASTER_RENDERER_
#define _MASTER_RENDERER_

#include "../Camera/camera.h"

#include "../GameObject/gameobject.h"
#include "ShaderProgram.h"

#include "../Lights/Light.h"

#include "renderer.h"

extern glm::vec3 clearColor;

class MasterRenderer
{
public:
	MasterRenderer();
	~MasterRenderer();
	void cleanUp();

	void load(char vertexFile[], char fragmentFile[], Camera *camera);

	static void prepare();

	void addToRenderList(gameobject *object);

	void renderWireframe();
	void renderUpdated(std::vector<Light*> lights, Camera *camera, glm::vec4 clipPlane, float fov, glm::vec3 newCameraPosition, glm::vec3 newCameraRotation);
	void render(std::vector<Light*> lights, Camera *camera, glm::vec4 clipPlane);
	void clearRenderPass();

	static void DisableCulling();
	static void EnableCulling();
	
	std::map<GLuint, std::vector<gameobject>> gameobjects;

	ShaderProgram shader;
private:

	Renderer renderer;

	bool wireframeRender;
};

#endif // !_MASTER_RENDERER_
