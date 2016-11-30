#include "ClientGame.h"

#include "WEngine/Usage.h"
#include "ReactPhysics3D/reactphysics3d.h"

#define WAVE_SPEED 0.03

#define MSAA 1
#define SSAA 2
#define FXAA 3

#define UPDATE_CYCLES_PER_SECOND 144

// ============  Client Handle Variables ===========
bool clientRunning = false;
ClientGame client;
void clientLoop(void *);

GLFWwindow* window;

Loader loader;
Camera camera;

MasterRenderer modelRenderer,
			   terrainRenderer,
			   normalModelRenderer;
WaterMasterRenderer waterRenderer;
ShadowMasterRenderer shadowRenderer;
GuiRenderer guiRenderer;

//Only render scene to sceneRenderer framebuffer. Then retrieve this texture for use.
PostProcessRenderer sceneRenderer, antiAliasedRenderer;
//Post Process image 
PostProcessRenderer Contrast, VBlur, HBlur, brightFilter, combineFilter;

std::vector<texture2D> GuiElements;

Terrain terrains[2];

std::vector<Light*> lights;

WaterTile water;

gameobject lamps[4];
gameobject model, model2, model3;

Skybox skybox, skybox2;

textureCubemap waterReflection;

texture2D GuiCherry;

glm::vec3 clearColor = glm::vec3(0, 0, 0);

double last_render_time = 0;
double deltaTime = 0,
	   frameStartTime = 0,
	   debugTime = 0;
float fps = 0;
int frame = 0;
int gameState = 0;

/* The Anti-Aliasing type used in the program.
   Can be one of the following:
   * MSAA - MultiSampled Anti-Aliasing
   * SSAA - SuperSampled Anti-Aliasing : Under Contstruction :
   * FXAA - Fast Approximate Anti-Aliasing
*/
int AAType = FXAA;

void handleMouseInput() {
	// Get mouse position
	double xpos, ypos;
	glfwGetCursorPos(window, &xpos, &ypos);

	// Reset mouse position for next frame
	glfwSetCursorPos(window, (SCREEN_WIDTH / 2), (SCREEN_HEIGHT / 2));

	float mxDiff = float((SCREEN_WIDTH  / 2) - xpos);
	float myDiff = float((SCREEN_HEIGHT / 2) - ypos);

}

void handleInput()
{
	//Poll all events
	glfwPollEvents();

	//HandleInput from keys
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) camera.position.x -= (deltaTime * 50);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) camera.position.x += (deltaTime * 50);

	if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) camera.position.y -= (deltaTime * 50);
	if (glfwGetKey(window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS) camera.position.y += (deltaTime * 50);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) camera.position.z -= (deltaTime * 50);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) camera.position.z += (deltaTime * 50);

	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) camera.rotation.x -= (deltaTime * 2);
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) camera.rotation.x += (deltaTime * 2);

	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) camera.rotation.y -= (deltaTime * 2);
	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) camera.rotation.y += (deltaTime * 2);

	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
		camera.position.y = terrains[1].getHeight(camera.position.x, camera.position.z) + 7;
		if (camera.position.y < 4.0f) camera.position.y = 4.0f;
	}
}

void updateTime()
{
	// Calculate delta time
	//glfwGetTime() nr of seconds since start of game
	//last_render_time last set time nr seconds since start of game
	deltaTime = glfwGetTime() - last_render_time; //Time in seconds it took for the last frame.
	fps = (1 / deltaTime);

	last_render_time = glfwGetTime();

	frame++;
}

void renderWaterTextures()
{
	//Enable clipdistance 0
	glEnable(GL_CLIP_DISTANCE0);

	//Bind Reflection framebuffer
	waterRenderer.bindReflectionFrameBuffer();
	//Prepare rendering on the reflection framebuffer
	MasterRenderer::prepare();

	//Position the camera down to get the proper reflection
	float distance = 2 * (camera.position.y - water.getWaterTile()->getPosition().y);
	camera.position.y -= distance;
	camera.rotation.x *= -1; //Invert pitch

	//Render all objects to the reflection of the water
	skybox.render(&camera);
	terrainRenderer.render(lights, &camera, glm::vec4(0, 1, 0, -water.getWaterTile()->getPosition().y + 1.0f));
	//modelRenderer.render(lights, &camera, glm::vec4(0, 1, 0, -water.getWaterTile()->getPosition().y + 1.0f));
	//normalModelRenderer.render(lights, &camera, glm::vec4(0, 1, 0, -water.getWaterTile()->getPosition().y));

	camera.position.y += distance; //Position the camera back up
	camera.rotation.x *= -1; //Invert pitch back to normal

	//Bind Refraction framebuffer
	waterRenderer.bindRefractionFrameBuffer();
	//Prepare rendering on the Refraction framebuffer
	MasterRenderer::prepare();

	//Render all elements to the refraction of the water
	terrainRenderer.render(lights, &camera, glm::vec4(0, -1, 0, water.getWaterTile()->getPosition().y + 1.0f));
	//modelRenderer.render(lights, &camera, glm::vec4(0, -1, 0, water.getWaterTile()->getPosition().y + 1.0f));
	//normalModelRenderer.render(lights, &camera, glm::vec4(0, -1, 0, water.getWaterTile()->getPosition().y));

	waterRenderer.unbindCurrentFrameBuffer();

	//Disable clipdistance 0
	glDisable(GL_CLIP_DISTANCE0);
}

void renderShadowTexture(Light *shadowLight)
{
	//Render all objects that have to cast a shadow to the shadowmap
	shadowRenderer.bindShadowFrameBuffer();

	//Render all objects to the shadow buffer
	//shadowRenderer.render(shadowLight, &camera, &terrainRenderer.gameobjects);
	shadowRenderer.render(shadowLight, &camera, &modelRenderer.gameobjects);
	shadowRenderer.render(shadowLight, &camera, &normalModelRenderer.gameobjects);

	shadowRenderer.unbindCurrentFrameBuffer();
}

void renderWaterCubeMap()
{
	//Add normalmapped models to renderer list
	normalModelRenderer.addToRenderList(&model);
	normalModelRenderer.addToRenderList(&model2);
	//Add terrain models to renderer list
	terrainRenderer.addToRenderList(terrains[0].getModel());
	terrainRenderer.addToRenderList(terrains[1].getModel());

	glm::vec3 reflectionPosition = glm::vec3(0.0f, 10.0f, 0.0f); //Has to be a vec3 of the position
	const glm::vec3 faceRotations[6] = {
		glm::vec3(0, glm::radians(-90.0f), glm::radians(180.0f)),
		glm::vec3(0, glm::radians(90.0f),  glm::radians(180.0f)),
		glm::vec3(glm::radians(-90.0f), 0, 0),//
		glm::vec3(glm::radians(90.0f),  0, 0),//
		glm::vec3(0, glm::radians(180.0f), glm::radians(180.0f)),
		glm::vec3(0, 0,	glm::radians(180.0f))
	};

	waterReflection.bindFrameBuffer();
	for (unsigned int i = 0; i < 6; i++) {
		//Bind the right side
		waterReflection.bindFrameBufferRenderTexture(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i);
		//Clear the buffer
		MasterRenderer::prepare();
		//Render

		skybox.renderUpdated(&camera, 90.0f, reflectionPosition, faceRotations[i]);

		//normalModelRenderer.renderUpdated(lights, &camera, glm::vec4(0, -1, 0, 100000), 90.0f, reflectionPosition, faceRotations[i]); //Normal render
		terrainRenderer.renderUpdated(lights, &camera, glm::vec4(0, -1, 0, 100000), 90.0f, reflectionPosition, faceRotations[i]); //Normal render

		//glEnable(GL_CLIP_DISTANCE0);
		//normalModelRenderer.renderUpdated(lights, &camera, glm::vec4(0, 1, 0, -water.getWaterTile()->getPosition().y + 1.0f), 90.0f, reflectionPosition, faceRotations[i]); //Reflection render
		//normalModelRenderer.renderUpdated(lights, &camera, glm::vec4(0, -1, 0, water.getWaterTile()->getPosition().y + 1.0f), 90.0f, reflectionPosition, faceRotations[i]); //Refraction render
		//glDisable(GL_CLIP_DISTANCE0);
	}
	waterReflection.unbindFrameBuffer(SCREEN_WIDTH, SCREEN_HEIGHT);
	//waterReflection.deleteBuffers(); //Only if we dont want to use the framebuffer ever again

	normalModelRenderer.clearRenderPass();
	terrainRenderer.clearRenderPass();
}

void initialiseClient()
{
	client = ClientGame("127.0.0.1", "6881");

	// Start a new thread and run the serverLoop function.
	_beginthread(clientLoop, 0, NULL);
}

void clientLoop(void *)
{
	double clientLoopDeltaTime = 0,
		clientLoopLastRenderTime = 0;

	clientRunning = true;
	bool inLobby = true;

	while (clientRunning) {

		// Accept new connections
		client.updateClient();

		//Send game packets
		if (inLobby) {
			
		}
		else {
			//Send game packets if game has started
			//client.sendPlayerData();
		}

		// Limit update cycle amount to UPDATE_CYCLES_PER_SECOND
		while (glfwGetTime() < (last_render_time + (1.0f / (float)UPDATE_CYCLES_PER_SECOND))) {}

		clientLoopDeltaTime = glfwGetTime() - clientLoopLastRenderTime; //Time in seconds it took for the last update cycle.
		clientLoopLastRenderTime = glfwGetTime();
	}
	//TODO: End thread on end program

	// End of function, end the thread and release its memory.
	_endthread();
}

int main() {
	initialiseClient();

	// Initialise GLFW and throw error if it failed
	if (!glfwInit()){
		throw std::runtime_error("Failed to initialize GLFW");
		return -1;
	}

	//Initialise
	last_render_time = glfwGetTime();

	//Create a display and initialise GLEW
	DisplayManager::createDisplay(SCREEN_WIDTH, SCREEN_HEIGHT, PROGRAM_NAME, false);

	//Just disable vsync for max fps.
	DisplayManager::disableVsync();

	//glfwSetKeyCallback(window, keyfun);

	//Intialise the camera and set its position
	camera.Initalise(75.0f, SCREEN_WIDTH, SCREEN_HEIGHT, 0.01f, 1000.0f);
	camera.Set(glm::vec3(0, 10, -50), glm::vec3(0, _PI, 0));

	//Initialise framebuffer for cubemap texture waterReflection
	waterReflection.initialseFrameBuffer(1280);

	//Load and intialise all postprocessors
	if (AAType == MSAA) {
		sceneRenderer.load(glm::vec2(SCREEN_WIDTH, SCREEN_HEIGHT), 8);
		antiAliasedRenderer.load(glm::vec2(SCREEN_WIDTH, SCREEN_HEIGHT));
	}
	else if (AAType == SSAA) {
		sceneRenderer.load(glm::vec2(SCREEN_WIDTH * 4, SCREEN_HEIGHT * 4));
		//TODO: create a shader that downsamples the image created!
		antiAliasedRenderer.load(glm::vec2(SCREEN_WIDTH, SCREEN_HEIGHT));
	}
	else if (AAType == FXAA) {
		sceneRenderer.load(glm::vec2(SCREEN_WIDTH, SCREEN_HEIGHT));
		antiAliasedRenderer.load("WEngine/Shaders/AntiAliasing/AAVertex.vs", "WEngine/Shaders/AntiAliasing/FXAA.fs", glm::vec2(SCREEN_WIDTH, SCREEN_HEIGHT));
		antiAliasedRenderer.shader.loadInverseFilterTextureSize(glm::vec3((1.0f / SCREEN_WIDTH), (1.0f / SCREEN_HEIGHT), 0.0f));
		antiAliasedRenderer.shader.loadFXAAParameters((8.0f), (1.0f / 128.0f), (1.0f / 8.0f));
	}
	Contrast.load("WEngine/Shaders/PostProcessing/ContrastEffect.vs", "WEngine/Shaders/PostProcessing/ContrastEffect.fs", glm::vec2(SCREEN_WIDTH, SCREEN_HEIGHT));
	VBlur.load("WEngine/Shaders/PostProcessing/VerticalGaussionBlur.vs", "WEngine/Shaders/PostProcessing/GaussionBlur.fs", glm::vec2(SCREEN_WIDTH / 5, SCREEN_HEIGHT / 5));
	HBlur.load("WEngine/Shaders/PostProcessing/HorizontalGaussionBlur.vs", "WEngine/Shaders/PostProcessing/GaussionBlur.fs", glm::vec2(SCREEN_WIDTH / 5, SCREEN_HEIGHT / 5));
	brightFilter.load("WEngine/Shaders/PostProcessing/simpleVertex.vs", "WEngine/Shaders/PostProcessing/brightFilter.fs", glm::vec2(SCREEN_WIDTH, SCREEN_HEIGHT));
	combineFilter.load("WEngine/Shaders/PostProcessing/simpleVertex.vs", "WEngine/Shaders/PostProcessing/combineFilter.fs", glm::vec2(SCREEN_WIDTH, SCREEN_HEIGHT));
	combineFilter.shader.start();
	combineFilter.shader.connectTextureUnits();
	combineFilter.shader.stop();

	//Load and initialise all masterRenderers
	guiRenderer.load("WEngine/Shaders/Gui/GuiShader.vs", "WEngine/Shaders/Gui/GuiShader.fs");
	modelRenderer.load("WEngine/Shaders/Default/Default.vs", "WEngine/Shaders/Default/Default.fs", &camera);
	normalModelRenderer.load("WEngine/Shaders/NormalMaps/NormalMap.vs", "WEngine/Shaders/NormalMaps/NormalMap.fs", &camera);
	terrainRenderer.load("WEngine/Shaders/Terrain/Terrain.vs", "WEngine/Shaders/Terrain/Terrain.fs", &camera);
	waterRenderer.load("WEngine/Shaders/Water/WaterShader.vs", "WEngine/Shaders/Water/WaterShader.fs", &camera);
	shadowRenderer.load("WEngine/Shaders/Shadows/ShadowShader.vs", "WEngine/Shaders/Shadows/ShadowShader.fs");

	std::string skyboxTextures[6] = {
		"res/Skybox/hw_desertnight/desert_night_right.bmp",
		"res/Skybox/hw_desertnight/desert_night_left.bmp",
		"res/Skybox/hw_desertnight/desert_night_top.bmp",
		"res/Skybox/hw_desertnight/desert_night_bottom.bmp",
		"res/Skybox/hw_desertnight/desert_night_back.bmp",
		"res/Skybox/hw_desertnight/desert_night_front.bmp",
	};
	skybox.load("WEngine/Shaders/Skybox/Skybox.vs", "WEngine/Shaders/Skybox/Skybox.fs", &camera, skyboxTextures);
	skybox2.load("WEngine/Shaders/Skybox/Skybox.vs", "WEngine/Shaders/Skybox/Skybox.fs", &camera, skyboxTextures);

	GuiCherry.loadImage("res/Cherry/cherry.bmp", true, false);
	GuiCherry.rotation = glm::radians(10.0f);
	GuiCherry.scale = glm::vec2(0.5f);
	GuiCherry.position = glm::vec2(-0.5f, 0.5f);
	
	//Set input mode
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

	//Initialise gameState
	gameState = 1;

	//Load all multitexture textures
	GLuint	floorTextureGrass = loader.loadTexture("res/Terrain/grassy2.bmp", true),
			floorTextureR	  = loader.loadTexture("res/Terrain/mud.bmp", true),
			floorTextureG	  = loader.loadTexture("res/Terrain/grassFlowers.bmp", true),
			floorTextureB	  = loader.loadTexture("res/Terrain/path.bmp", true),
			floorBlendMap	  = loader.loadTexture("res/Terrain/blendMap.bmp", true);
	//Create terrains with a heightmap, set position and all multitexture textures
	terrains[0].createWithHeightmap("res/heightmap.bmp", -1, -1, &loader, floorTextureGrass, floorTextureR, floorTextureG, floorTextureB, floorBlendMap);
	terrains[1].createWithHeightmap("res/heightmap.bmp",  0, -1, &loader, floorTextureGrass, floorTextureR, floorTextureG, floorTextureB, floorBlendMap);
	//Set ambientLight on the terrain models
	terrains[0].getModel()->setAmbientLight(0.2f);
	terrains[1].getModel()->setAmbientLight(0.2f);
	//Set shadowmap texture on the terrain
	terrains[0].getModel()->setShadowMap(shadowRenderer.getShadowDepthTexture());
	terrains[1].getModel()->setShadowMap(shadowRenderer.getShadowDepthTexture());

	//Load lamp models and texture
	GLuint lampTexture = loader.loadTexture("res/Lantern/lantern.bmp", true);
	GLuint lampSpecularMap = loader.loadTexture("res/Lantern/lanternS.bmp", false);
	gameobject lampModel = loader.loadObjFile("res/Lantern/lantern.obj", true, false);

	//Set lamp model and texture
	for (int i = 0; i < 4; i++){
		lamps[i] = gameobject(&lampModel);
		lamps[i].addTexture(lampTexture);
		lamps[i].setSpecularMap(lampSpecularMap);
	}

	//Initialise the models position, rotation and scale.
	lamps[0].init(glm::vec3(20, terrains[1].getHeight(20, -30), -30), glm::vec3(0, 0, 0), glm::vec3(1.3f, 1.3f, 1.3f));
	lamps[1].init(glm::vec3(370, terrains[1].getHeight(370, -300), -300), glm::vec3(0, 0, 0), glm::vec3(1.3f, 1.3f, 1.3f));
	lamps[2].init(glm::vec3(293, terrains[1].getHeight(293, -305), -305), glm::vec3(0, 0, 0), glm::vec3(1.3f, 1.3f, 1.3f));
	lamps[3].init(glm::vec3(0, 0, 0), glm::vec3(0, 0, 0), glm::vec3(1.3f, 1.3f, 1.3f));

	//Create a model with a .obj file
	model = gameobject(&loader.loadObjFile("res/Barrel/barrel.obj", true, false));
	model2 = gameobject(&model);

	//Load the texture and normalmap for the barrel model
	GLuint barrelTexture = loader.loadTexture("res/Barrel/barrel.bmp", true);
	GLuint barrelNormal = loader.loadTexture("res/Barrel/barrelNormal.bmp", true);

	//Load the texture for the model and give it to the model.
	model.addTexture(barrelTexture);
	//Set the normal map texture to the model
	model.setNormalMap(barrelNormal);
	//Initialise the models position, rotation and scale.
	model.init(glm::vec3(0, 0, 0), glm::vec3(0, 0, 0), glm::vec3(1.5f, 1.5f, 1.5f));
	//Set shinyness to model
	model.setShineDamper(100);
	model.setReflectivity(1);
	//Set model ambient light
	model.setAmbientLight(0.1f);
	model.setShadowMap(shadowRenderer.getShadowDepthTexture());

	/*model.setEnviromentCubeMapID(waterReflection.textureid);
	model.hasReflectionCubeMap = true;
	model.hasRefractionCubeMap = false;
	model.reflectionRatio = 0.5f;
	model.reflectionRefractionRatio = 0.5f;*/

	//Load the texture for the model and give it to the model.
	model2.addTexture(barrelTexture);
	//Set the normal map texture to the model
	model2.setNormalMap(barrelNormal);
	//Initialise the models position, rotation and scale.
	model2.init(glm::vec3(20, terrains[1].getHeight(20, -10), -300), glm::vec3(0, 0, 0), glm::vec3(1.5f, 1.5f, 1.5f));
	//Set shinyness to model
	model2.setShineDamper(100);
	model2.setReflectivity(1);
	//Set model ambient light
	model2.setAmbientLight(0.1f);
	model2.setShadowMap(shadowRenderer.getShadowDepthTexture());
	
	//Create lights
	Light sun(glm::vec3(0, 1000, -7000), glm::vec3(0.35f, 0.35f, 0.35f));
	Light light0(glm::vec3(185, terrains[1].getHeight(185, -293) + 13.2f, -293), glm::vec3(2, 0, 0), glm::vec3(1, 0.01f, 0.002f)); //Red
	Light light1(glm::vec3(370, terrains[1].getHeight(370, -300) + 13.2f, -300), glm::vec3(0, 2, 2), glm::vec3(1, 0.01f, 0.002f));
	Light light2(glm::vec3(293, terrains[1].getHeight(293, -305) + 13.2f, -305), glm::vec3(2, 2, 0), glm::vec3(1, 0.01f, 0.002f));

	//Add lights to the list
	lights.push_back(&sun);
	lights.push_back(&light0);
	lights.push_back(&light1);
	lights.push_back(&light2);

	//Create a waterTile and set its attributes
	water = WaterTile(glm::vec3(0, 2, -400), glm::vec3(0, 0, 0), glm::vec3(800, 500, 400));
	water.setReflectionTexture(waterRenderer.getReflectionTexture());
	water.setRefractionTexture(waterRenderer.getRefractionTexture());
	water.setDuDvTexture(loader.loadTexture("res/Water/WaterDuDvMap.bmp", false));
	water.setNormalMapTexture(loader.loadTexture("res/Water/WaterNormalMap.bmp", false));
	water.setRefractionDepthTexture(waterRenderer.getRefractionDepthTexture());
	water.getWaterTile()->setShineDamper(100);
	water.getWaterTile()->setReflectivity(1);
	water.getWaterTile()->setAmbientLight(0.5f);
	water.getWaterTile()->setShadowMap(shadowRenderer.getShadowDepthTexture());

	lights[0]->setPosition(glm::vec3(sin((float)frame / 100.0f) * 100.0f, 100.0f, cos((float)frame / 100.0f) * 100.0f));

	renderWaterCubeMap();
	//skybox2.cubeMap.textureid = waterReflection.textureid;

	//GuiElements.push_back(GuiCherry);

	do {
		//renderWaterCubeMap();
		//GuiElements[0].rotation = (float)frame / 100.0f;

		//Prepare rendering on default framebuffer
		MasterRenderer::prepare();

		//Change WaterMoveFactor for random(ish) water movement
		float currentWaterMoveFactor = waterRenderer.getMoveFactor();
		currentWaterMoveFactor += WAVE_SPEED * deltaTime;
		if (currentWaterMoveFactor > 1) currentWaterMoveFactor -= 1;
		waterRenderer.setMoveFactor(currentWaterMoveFactor);
		
		/* ========= Add models to list for rendering ============== */
		
		//Add terrains to renderer list.
		terrainRenderer.addToRenderList(terrains[0].getModel());
		terrainRenderer.addToRenderList(terrains[1].getModel());

		//Add normalmapped models to renderer list
		normalModelRenderer.addToRenderList(&model);
		normalModelRenderer.addToRenderList(&model2);

		//Add lantern models
		for (int i = 0; i < 4; i++) modelRenderer.addToRenderList(&lamps[i]);

		//Add water to the renderer list
		waterRenderer.addToRenderList(&water);
		
		/* Performance, until here ~0.1 ms*/
		/* =============== Start of rendering ===================== */
		
		/* Performance render to water fbos, ~0.45 ms*/
		//Render reflection and refraction texture of the water
		renderWaterTextures();
		
		/* Performance render to water fbos, ~0.55 ms*/
		//Render the shadow texture
		renderShadowTexture(&sun);

		glFinish();
		frameStartTime = glfwGetTime();

		/* Performance render to MSAA fbo, ~2.0 ms */
		//Render scene to multisampled framebuffer
		sceneRenderer.bindRenderToTextureFrameBuffer();
			MasterRenderer::prepare();

			//Render everything
			//if(glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS) skybox.render(&camera);
			//else skybox2.render(&camera);
			skybox.render(&camera);

			terrainRenderer.render(lights, &camera, glm::vec4(0, -1, 0, 100000));
			waterRenderer.render(lights, &camera);

			modelRenderer.render(lights, &camera, glm::vec4(0, -1, 0, 100000));
			normalModelRenderer.render(lights, &camera, glm::vec4(0, -1, 0, 100000));
		
		sceneRenderer.unbindFrameBuffer();

		glFinish();
		debugTime = glfwGetTime();

		/* =================== Post processing below! ============== */

		if (AAType == MSAA) {
			//Resolve multisampled framebuffer to antialiased framebuffer
			sceneRenderer.resolveTo(&antiAliasedRenderer);
		}
		else if (AAType == SSAA) {
			//TODO: Create a way to resolve a SuperSampled framebuffer
		}
		else if (AAType == FXAA) {
			//Render image to antiAliased buffed with the FXAA Shader
			antiAliasedRenderer.renderToFrameBuffer(sceneRenderer.getOutputTexture());
		}

		//antiAliasedRenderer.renderToScreen();
		
		//Renderer antialiased framebuffer to brightfilter
		brightFilter.renderToFrameBuffer(antiAliasedRenderer.getOutputTexture());
		brightFilter.renderToScreen();

		//Blur the brightfilter
		HBlur.renderToFrameBuffer(brightFilter.getOutputTexture());
		VBlur.renderToFrameBuffer(HBlur.getOutputTexture());

		//Combine the brightfiltered framebuffer and the antialiased framebuffer into one
		combineFilter.shader.loadHighlightAmount(1.0f);
		combineFilter.renderToFrameBuffer(antiAliasedRenderer.getOutputTexture(), VBlur.getOutputTexture());
		//combineFilter.renderToScreen();

		//Rendered combined image to contrast framebuffer
		Contrast.shader.loadContrast(0.02f);
		Contrast.renderToFrameBuffer(combineFilter.getOutputTexture());

		//Render contrast framebuffer to the screen
		Contrast.renderToScreen();

		guiRenderer.render(&GuiElements);
		
		//Swap Buffers (Send image to the screen)
		glfwSwapBuffers(window);
	
		/* ================== End of rendering =======================*/


		//Clear all objects from renderlists.
		modelRenderer.clearRenderPass();
		normalModelRenderer.clearRenderPass();
		terrainRenderer.clearRenderPass();
		waterRenderer.clearRenderPass();

		//Update all time related variables.
		updateTime();

		// Limit fps to FPSLIMIT
		//while (glfwGetTime() < (last_render_time + (1.0f / (float)FPSLIMIT))) {}

		//Update inputs
		handleInput();


		lights[0]->setPosition(glm::vec3(sin((float)frame / 100.0f) * 100.0f, 100.0f, cos((float)frame / 100.0f) * 100.0f));
		lamps[3].setPosition(glm::vec3(sin((float)frame / 100.0f) * 100.0f, 100.0f, cos((float)frame / 100.0f) * 100.0f));

		//Set title to hold fps info
		std::string fpsStr = std::string(PROGRAM_NAME) + " FPS: " + std::to_string(fps) + " deltaTime: " + std::to_string(deltaTime * 100) + " debugTime: " + std::to_string((debugTime - frameStartTime) * 100);
		DisplayManager::setDisplayTitle(fpsStr.c_str());

		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) gameState = 0;
		if (DisplayManager::updateDisplay() < 0) gameState = 0;
	}while (gameState >= 1);

	//Close and Cleanup the display
	DisplayManager::closeDisplay();

	//Clean all programs up
	modelRenderer.cleanUp();
	normalModelRenderer.cleanUp();
	terrainRenderer.cleanUp(); 
	waterRenderer.cleanUp();
	shadowRenderer.cleanUp();

	loader.cleanUp();

	//Terminate glfw
	glfwTerminate();
}