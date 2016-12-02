#include "ClientGame.h"

#include "WEngine/Usage.h"
#include "ReactPhysics3D/reactphysics3d.h"

#define WAVE_SPEED 0.03

#define MSAA 1
#define SSAA 2
#define FXAA 3

#define UPDATE_CYCLES_PER_SECOND 144
clock_t programStartClock = std::clock();

// ============  Client Handle Variables ===========
bool clientRunning = false;
ClientGame client;
void clientLoop(void *);


// ============  GAME PROGRAM VARIABLES ============

GLFWwindow* window;
void (*networkUpdateFunction)(void) = nullptr;

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


// == RENDER ELEMENTS ==
std::vector<texture2D> GuiElements;

Terrain terrains[2];

std::vector<Light*> lights;

WaterTile water;

gameobject lamps[4];
gameobject model, model2, model3;

Skybox skybox, skybox2;

textureCubemap waterReflection;

texture2D GuiCherry;

// == GAME INFO ==
glm::vec3 clearColor = glm::vec3(0, 0, 0);

double last_render_time = 0;
double deltaTime = 0,
	   frameStartTime = 0,
	   debugTime = 0;
float fps = 0;
int frame = 0;
int gameState = 0;

int Max_Fps = 60;
bool limit_fps = true;

/* The Anti-Aliasing type used in the program.
   Can be one of the following:
   * MSAA - MultiSampled Anti-Aliasing
   * SSAA - SuperSampled Anti-Aliasing :! Under Contstruction !:
   * FXAA - Fast Approximate Anti-Aliasing
*/
int AAType = FXAA;

// ==  FUNCTIONS  ==

// If trapMouseInWindow it returns the changed amount of pixels since last update
// Else if returns the position the mouse is on
glm::vec2 handleMouseInput(bool trapMouseInWindow);
// Handle user input
void handleInput();
// Update time stuff
void updateTime();
// Render water textures
void renderWaterTextures();
// Render shadow textures
void renderShadowTexture(Light *shadowLight);
// Render water cubemap : WORK IN PROGRESS :
void renderWaterCubeMap();

// Initialise the client
void initialiseClient(char ipAddress[39], char port[5]);
// Client loop
void clientLoop(void *);

// Network functions
void SendInitData();
void SendLobbyData();
void SendGameData();

int main() {

	// ============  NETWORKING LOGIC =================

	// Initialise, set the client and connect to the server.
	initialiseClient("127.0.0.1", "6881");

	// ===============  GAME LOGIC ====================

	// Initialise GLFW and throw error if it failed
	if (!glfwInit()){
		throw std::runtime_error("Failed to initialize GLFW");
		return -1;
	}

	// ========  Initialise  ==========
	last_render_time = glfwGetTime();

	// ==== DISPLAY ====

	//Create a display and initialise GLEW
	DisplayManager::createDisplay(SCREEN_WIDTH, SCREEN_HEIGHT, PROGRAM_NAME, false);

	//Just disable vsync for max fps.
	DisplayManager::disableVsync();

	//glfwSetKeyCallback(window, keyfun);

	// ==== CAMERA ====

	//Intialise the camera and set its position
	camera.Initalise(75.0f, SCREEN_WIDTH, SCREEN_HEIGHT, 0.01f, 1000.0f);
	camera.Set(glm::vec3(0, 10, -50), glm::vec3(0, _PI, 0));

	// ==== Framebuffers ====

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

	// ==== SKYBOX ====

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

	// ==== GUI ====

	GuiCherry.loadImage("res/Cherry/cherry.bmp", true, false);
	GuiCherry.rotation = glm::radians(10.0f);
	GuiCherry.scale = glm::vec2(0.5f);
	GuiCherry.position = glm::vec2(-0.5f, 0.5f);
	
	// ==== INPUT ====

	//Set input mode
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

	//Initialise gameState
	gameState = 1;

	// ==== MODELS ====

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
		
		/* =============== Start of rendering ===================== */
		
		//Render reflection and refraction texture of the water
		renderWaterTextures();
		
		//Render the shadow texture
		renderShadowTexture(&sun);

		glFinish();
		frameStartTime = glfwGetTime();

		//Render scene to multisampled framebuffer
		sceneRenderer.bindRenderToTextureFrameBuffer();
			MasterRenderer::prepare();

			//Render everything
			skybox.render(&camera);

			terrainRenderer.render(lights, &camera, glm::vec4(0, -1, 0, 100000));
			waterRenderer.render(lights, &camera);

			modelRenderer.render(lights, &camera, glm::vec4(0, -1, 0, 100000));
			normalModelRenderer.render(lights, &camera, glm::vec4(0, -1, 0, 100000));
		
		sceneRenderer.unbindFrameBuffer();

		glFinish();
		debugTime = glfwGetTime();

		/* =================== Post processing below! ============== */

		// Handle antialiasing
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

		// =====  GUI ELEMENTS =====

		// Render GuiElements
		guiRenderer.render(&GuiElements);
		
		//Swap Buffers (Send image to the screen)
		glfwSwapBuffers(window);
	
		/* ================== End of rendering ======================= */

		//Clear all objects from renderlists.
		modelRenderer.clearRenderPass();
		normalModelRenderer.clearRenderPass();
		terrainRenderer.clearRenderPass();
		waterRenderer.clearRenderPass();


		//Update all time related variables.
		updateTime();

		if (limit_fps) {
			// Limit fps to FPSLIMIT
			while (glfwGetTime() < (last_render_time + (1.0f / (float)Max_Fps))) {}
		}

		//Update input
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

	//Make the clientloop stop running and end its thread
	clientRunning = false;

	//Terminate glfw
	glfwTerminate();
}

// If trapMouseInWindow it returns the changed amount of pixels since last update
// Else if returns the position the mouse is on
glm::vec2 handleMouseInput(bool trapMouseInWindow) {
	// Get mouse position
	double xpos, ypos;
	glfwGetCursorPos(window, &xpos, &ypos);

	glm::vec2 windowPos = DisplayManager::getWindowPosition();
	glm::vec2 windowSize = DisplayManager::getWindowSize();

	glm::vec2 windowCenter = glm::vec2(windowPos.x + windowSize.x / 2, windowPos.y + windowSize.y / 2);

	glm::vec2 difference;

	// Reset mouse position for next frame	
	if (trapMouseInWindow) {
		difference = glm::vec2(windowCenter.x - xpos, windowCenter.y - ypos);
		glfwSetCursorPos(window, windowCenter.x, windowCenter.y);
	}
	else {
		difference = glm::vec2(xpos, ypos);
	}

	return difference;
}
// Handle user input
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
// Update time stuff
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
// Render water textures
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
// Render shadow textures
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

// Render water cubemap : WORK IN PROGRESS :
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

// Intialise the client and connect to the server on ipAddress and port
void initialiseClient(char ipAddress[39], char port[5])
{
	// Initialise the client, create a connection and try to connect to the ip and port of the server.
	client = ClientGame(ipAddress, port);

	// Check if there are any intialisation errors. 
	// If this error is fatal, we stop creating the clientNetwork.
	std::vector<networkingErrors> clientErrors = client.getErrors();
	for (unsigned int i = 0; i < clientErrors.size(); i++)
	{
		switch (clientErrors[i]) {
		case WSA_STARTUP_ERROR:
			printf("ERROR -- Creating client WSAStartup\n");
			return;
			break;
		case GET_ADDR_INFO_ERROR:
			printf("ERROR -- Creating client getaddrinfo\n");
			return;
			break;
		case CREATE_SOCKET_ERROR:
			printf("ERROR -- Creating client createSocket\n");
			return;
			break;
		case CONNECT_SOCKET_ERROR:
			printf("ERROR -- Creating client connectSocket\n");
			break;
		case ALL_CONNECTING_SOCKETS_ERROR:
			printf("ERROR -- Creating client connecting all sockets\n");
			return;
			break;
		case SET_NONBLOCKING_ERROR:
			printf("ERROR -- Creating client nonblocking\n");
			return;
			break;
		}
	}
	
	networkUpdateFunction = SendInitData;

	// Start a new thread and run the serverLoop function.
	_beginthread(clientLoop, 0, NULL);
}
// The client loop
void clientLoop(void *)
{
	double clientLoopDeltaTime = 0,
		clientLoopLastRenderTime = 0;
	clientRunning = true;

	// Client networking loop
	while (clientRunning) {

		// Receive and parse data.
		client.updateClient();

		// If the function pointer is set to a function, execute this function.
		// This function is to execute a function that sends data to the server.
		if (networkUpdateFunction != nullptr) networkUpdateFunction();

		// Limit update cycle amount to UPDATE_CYCLES_PER_SECOND
		while (((float)(std::clock() - programStartClock) / (float)CLOCKS_PER_SEC) < (clientLoopLastRenderTime + (1.0f / (float)UPDATE_CYCLES_PER_SECOND))) {}

		clientLoopDeltaTime = (((float)(std::clock() - programStartClock) / (float)CLOCKS_PER_SEC)) - clientLoopLastRenderTime; //Time in miliseconds it took for the last update cycle.
		clientLoopLastRenderTime = (((float)(std::clock() - programStartClock) / (float)CLOCKS_PER_SEC));
	}

	// End of function, end the thread and release its memory.
	_endthread();
}


// ======  SERVER HANDLE FUNCTIONS  ======

// Send intitialisation data
// When just connected to the server and need to send name.
void SendInitData()
{
	char name[] = "Wouter140";

	playerData player;
	memcpy(&player.playerName, name, strlen(name));
	player.playerNameSize = strlen(name);
	client.addActionType(GAME_INITIALISATION);

	client.sendPlayerData(player, LOBBY_PACKET);
}
// Send lobby data
// When in lobby
void SendLobbyData()
{

}

// Send game data
// When in game
void SendGameData()
{

}