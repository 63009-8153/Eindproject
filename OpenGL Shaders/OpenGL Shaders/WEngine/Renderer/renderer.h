#ifndef _MASTER_RENDERER_
#include "MasterRenderer.h"
#endif

#ifndef _C_RENDERER_
#define _C_RENDERER_

#include "../GameObject/gameobject.h"
#include "ShaderProgram.h"

class Renderer
{
public:
	Renderer();
	Renderer(ShaderProgram *shad);

	void render(std::map<GLuint, std::vector<gameobject>> *gameobjects);
private:
	void prepareMassModel(gameobject *object);
	void unbindModel();
	void prepareModelInstance(gameobject *object);

	ShaderProgram *shader;
};

#endif
