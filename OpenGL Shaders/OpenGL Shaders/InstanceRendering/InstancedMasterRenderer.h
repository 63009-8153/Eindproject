#ifndef _INSTANCE_MASTER_RENDERER_
#define _INSTANCE_MASTER_RENDERER_

#include "Camera.h"

#include "gameobject.h"
#include "InstancedShaderProgram.h"

#include "Light.h"

#include "Instancedrenderer.h"

#include "loader.h"

extern glm::vec3 clearColor;

class InstanceMasterRenderer
{
public:
	InstanceMasterRenderer();
	~InstanceMasterRenderer();
	void cleanUp();

	void load(char vertexFile[], char fragmentFile[], Camera *camera);
	void setModel(GLuint _vao, int Max_instances, int Instance_data_length);

	static void prepare();

	void addToRenderList(gameobject *object);

	void renderWireframe();
	void render(std::vector<Light*> lights, Camera *camera);

	static void DisableCulling();
	static void EnableCulling();
private:

	InstancedRenderer renderer;
	InstancedShaderProgram shader;

	bool wireframeRender;

	GLuint vao, vbo;

	int Max_instances;
	int Instance_data_length;

	std::map<GLuint, std::vector<gameobject>> gameobjects;
};

#endif // !_MASTER_RENDERER_
