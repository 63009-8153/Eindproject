#ifndef _MASTER_RENDERER_
	#include "InstancedMasterRenderer.h"
#endif

#ifndef _C_INSTANCE_RENDERER_
#define _C_INSTANCE_RENDERER_

#include "gameobject.h"
#include "InstancedShaderProgram.h"

class InstancedRenderer
{
public:
	InstancedRenderer();
	InstancedRenderer(InstancedShaderProgram *shad);

	void render(std::map<GLuint, std::vector<gameobject>> *gameobjects, int Instance_data_length, GLuint vao, GLuint vbo);

	static void printError(int t);
private:
	void prepareModel(gameobject *object);
	void prepare(GLuint vao);
	void unbindModel();

	InstancedShaderProgram *shader;

	

	int pointer;
};

#endif
