#include "header.h"

#include "InstancedMasterRenderer.h"

//Constructor
InstanceMasterRenderer::InstanceMasterRenderer(){}
//Destructor and cleanup
InstanceMasterRenderer::~InstanceMasterRenderer()
{
	cleanUp();
}
void InstanceMasterRenderer::cleanUp()
{
	shader.cleanUp();
}

void InstanceMasterRenderer::load(char vertexFile[], char fragmentFile[], Camera *camera)
{
	shader.load(vertexFile, fragmentFile);
	renderer = InstancedRenderer(&shader);

	//Set the projection matrix since this will not be changed a lot.
	shader.start();
	shader.loadProjectionMatrix(camera->getProjectionMatrix());
	shader.connectTextureUnits();
	shader.stop();

	InstanceMasterRenderer::EnableCulling();
	
	wireframeRender = false;
}

void InstanceMasterRenderer::setModel(GLuint _vao, int _Max_instances, int _Instance_data_length)
{
	Max_instances = _Max_instances;
	Instance_data_length = _Instance_data_length;
	vao = _vao;

	Loader loader;
	vbo = loader.createEmptyVbo(Instance_data_length * Max_instances);

	//Position, textureCoords, Normals and tangents
	//Are in position 0, 1, 2 and 3 respectively.
	//Thats why transformationMatrix starts at 4

	//Set vao attribute with vbo data at with size and offset and make it a instanced attribute.
	//loader.addInstancedAttribute(VAOid, VBOid, Attribute number, Size, Instance_data_length, Offset);
	//transformationMatrix
	loader.addInstancedAttribute(vao, vbo, 4, 4, Instance_data_length, 0);
	loader.addInstancedAttribute(vao, vbo, 5, 4, Instance_data_length, 4);
	loader.addInstancedAttribute(vao, vbo, 6, 4, Instance_data_length, 8);
	loader.addInstancedAttribute(vao, vbo, 7, 4, Instance_data_length, 12);
	//ShineReflectAmbient
	loader.addInstancedAttribute(vao, vbo, 8, 3, Instance_data_length, 16);
	//offset
	loader.addInstancedAttribute(vao, vbo, 9, 2, Instance_data_length, 19);

	int pointer = 0;
	GLfloat *vboData = new GLfloat[2 * 21];

	vboData[pointer++] = 1.0f;
	vboData[pointer++] = 1.0f;
	vboData[pointer++] = 1.0f;
	vboData[pointer++] = 1.0f;
	vboData[pointer++] = 1.0f;
	vboData[pointer++] = 1.0f;
	vboData[pointer++] = 1.0f;
	vboData[pointer++] = 1.0f;
	vboData[pointer++] = 1.0f;
	vboData[pointer++] = 1.0f;
	vboData[pointer++] = 1.0f;
	vboData[pointer++] = 1.0f;
	vboData[pointer++] = 1.0f;
	vboData[pointer++] = 1.0f;
	vboData[pointer++] = 1.0f;
	vboData[pointer++] = 1.0f;
	vboData[pointer++] = 10.0f;
	vboData[pointer++] = 0.001f;
	vboData[pointer++] = 0.2f;

	vboData[pointer++] = 0.0f;
	vboData[pointer++] = 0.0f;

	vboData[pointer++] = 1.0f;
	vboData[pointer++] = 1.0f;
	vboData[pointer++] = 1.0f;
	vboData[pointer++] = 1.0f;
	vboData[pointer++] = 1.0f;
	vboData[pointer++] = 1.0f;
	vboData[pointer++] = 1.0f;
	vboData[pointer++] = 1.0f;
	vboData[pointer++] = 1.0f;
	vboData[pointer++] = 1.0f;
	vboData[pointer++] = 1.0f;
	vboData[pointer++] = 1.0f;
	vboData[pointer++] = 1.0f;
	vboData[pointer++] = 1.0f;
	vboData[pointer++] = 1.0f;
	vboData[pointer++] = 1.0f;
	vboData[pointer++] = 10.0f;
	vboData[pointer++] = 0.001f;
	vboData[pointer++] = 0.2f;

	vboData[pointer++] = 0.0f;
	vboData[pointer++] = 0.0f;

	//printf("Error1: %d\n", error);
	//Bind VBO for use
	glBindBuffer(GL_ARRAY_BUFFER, vbo);

	glBufferData(GL_ARRAY_BUFFER, (5 * 21) * sizeof(GLfloat), vboData, GL_DYNAMIC_DRAW);
	//Set VBO Data
	//glBufferData(GL_ARRAY_BUFFER, (42 * sizeof(GLfloat)), NULL, GL_DYNAMIC_DRAW);

	//Update buffer data
	//glBufferSubData(Target, offset(bytes), size to be copied(bytes), data)
	//glBufferSubData(GL_ARRAY_BUFFER, 0, ((2 * 21) * sizeof(GLfloat)), vboData);

	//Unbind VBO
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void InstanceMasterRenderer::DisableCulling() {
	//Only draw the triangles which face the camera
	glDisable(GL_CULL_FACE);
}
void InstanceMasterRenderer::EnableCulling() {
	//Only draw the triangles which face the camera
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
}

//Static function that prepares the rendering
void InstanceMasterRenderer::prepare()
{
	//Enable Depth Test in OpenGL
	glEnable(GL_DEPTH_TEST);

	//Clear the screen
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//Default background color
	glClearColor(clearColor.x, clearColor.y, clearColor.z, 1.0f);
}

//Add gameobject to the rendering list.
//This way all gameobject that are the same, get rendered with the same texture and shader.
//This has great performance increse! So good practise :)
void InstanceMasterRenderer::addToRenderList(gameobject *object)
{
	GLuint textID = object->getTextureID(0);

	if (gameobjects.find(textID) != gameobjects.end()) {
		gameobjects.find(textID)->second.push_back(*object);
	}
	else {
		gameobjects.insert(std::pair<GLuint, std::vector<gameobject>>(textID, std::vector<gameobject>()));
		gameobjects.find(textID)->second.push_back(*object);
	}
}

void InstanceMasterRenderer::renderWireframe()
{
	wireframeRender = true;
}

//Actually render all the gameobjects
void InstanceMasterRenderer::render(std::vector<Light*> lights, Camera *camera)
{
	shader.start();
	shader.connectTextureUnits();
	shader.loadLights(lights);
	shader.loadViewMatrix(camera->getViewMatrix());

	shader.loadSkyColour(clearColor);

	if(wireframeRender) glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	renderer.render(&gameobjects, Instance_data_length, vao, vbo);

	if (wireframeRender) glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	shader.stop();
	gameobjects.clear();

	wireframeRender = false;
}