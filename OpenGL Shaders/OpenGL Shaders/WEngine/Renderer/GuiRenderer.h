#ifndef GUI_RENDERER
#define GUI_RENDERER

#include "GuiShaderProgram.h"
#include "../GameObject/gameobject.h"
#include "../Utils/loader.h"
#include "../Texture/texture2D.h"

extern Loader loader;

class GuiRenderer
{
public:
	GuiRenderer();
	~GuiRenderer();

	void load(char vertexFile[], char fragmentFile[]);

	void render(std::vector<texture2D> *textures);

private:
	gameobject quad;
	GuiShaderProgram shader;
};

#endif