#include "ClientGame.h"

// Graphics and sound Engine
#include "WEngine/Usage.h"
#include "SAL/CoreSystem.h"

#include "Enemy.h"
#include "Model.h"
#include "Player.h"

#include "Tree.h"


#define WAVE_SPEED 0.03

#define MSAA 1
#define SSAA 2
#define FXAA 3

#define OPTIMIZE_SAFEAREA_DIST 50.0f
#define SMALL_OPTIMIZE_DIST 200.0f
#define OPTIMIZE_DIST 300.0f
#define LARGE_OPTIMIZE_DIST 1000.0f

#define UPDATE_CYCLES_PER_SECOND 60
clock_t programStartClock = std::clock();

// ============  Client Handle Variables ===========
bool clientRunning = false;
ClientGame client;

double	clientLoopDeltaTime = 0,
clientLoopLastRenderTime = 0;

void(*networkUpdateFunction)(void) = nullptr;

// ============  GAME PROGRAM VARIABLES ============

GLFWwindow* window;

SimpleAudioLib::AudioEntity* sound;

float mouseSensitivity = 1.0f;

Player player;
Player otherPlayers[MAX_LOBBYSIZE];

Enemy enemies[MAX_ENEMIES];

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

GLuint SA_T_Floor;
GLuint SA_T_Building[5];
GLuint SA_T_AmmoBoxes[7], SA_TN_AmmoBoxes[2];
GLuint SA_T_Barrels[3];
GLuint SA_T_Barriers, SA_TN_Barriers;
GLuint SA_T_Crate, SA_TN_Crate;
GLuint SA_T_Crate2;
GLuint SA_T_Pallets, SA_TN_Pallets;
GLuint SA_T_SandBag, SA_TN_SandBag;

Model SA_M_Floor;
Model SA_M_Building[1 * 5];
Model SA_M_AmmoBoxes[3 * 7];
Model SA_M_Barrels[6];
Model SA_M_Barriers[4];
Model SA_M_Crate2;
Model SA_M_Crate[6];
Model SA_M_Pallets[2];
Model SA_M_SandBag[2];

GLuint	FM_T_FLATTERRAIN[5];
GLuint	FM_T_TERRAIN,
FM_TN_TERRAIN;
GLuint  FM_T_AMBULANCE[5],
FM_TN_AMBULANCE[5];
GLuint  FM_T_COMMAND_TENT,
FM_TN_COMMAND_TENT;
GLuint  FM_T_ARMY_TENT[3],
FM_TN_ARMY_TENT[2];
GLuint  FM_T_ARMY_TRUCK[5];
GLuint	FM_T_BARRIER,
FM_TN_BARRIER;
GLuint	FM_T_CONTAINER,
FM_TN_CONTAINER;
GLuint	FM_T_FENCE[3],
FM_TN_FENCE[3];
GLuint	FM_T_BROKENFENCE1[3], FM_T_BROKENFENCE2[3],
FM_TN_BROKENFENCE[3];
GLuint	FM_T_ROADLIGHT[3], FM_TS_ROADLIGHT;
GLuint	FM_T_MILITARY_BUNKER[3];
GLuint  FM_T_TRAFFICCONE;
GLuint	FM_T_ROAD[3],
FM_TN_ROAD;
GLuint	FM_T_STATUE,
FM_TN_STATUE;
GLuint	FM_T_RAIL[2];
GLuint	FM_T_TOWNHOUSE;
GLuint	FM_T_WELL,
FM_TN_WELL;
GLuint  T_GUN_WALTER;

Tree  trees;
Terrain FM_M_FLATTERRAIN;
Model FM_M_TERRAIN;
Model FM_M_AMBULANCE[5 * 2];
Model FM_M_COMMAND_TENT;
Model FM_M_ARMY_TENT[3 * 6];
Model FM_M_ARMY_TRUCK[5 * 6];
Model FM_M_BARRIER[25];
Model FM_M_CONTAINER[12];
Model FM_M_BROKEN_FENCE1[3];
Model FM_M_BROKEN_FENCE2[3];
Model FM_M_FENCE[3 * 164];
Model FM_M_ROADLIGHT[3 * 24];
Model FM_M_MILITARY_BUNKER[3 * 3];
Model FM_M_TRAFFICCONE[14];
Model FM_M_ROAD[3];
Model FM_M_STATUE;
Model FM_M_RAIL[2];
Model FM_M_TOWNHOUSE[2];
Model FM_M_WELL;
Model GUN_WALTER;

std::vector<gameobject> animationModels;
std::vector<s_anim> playerAnimations;

std::vector<gameobject> enemyAnimationModels;
std::vector<s_anim> enemyAnimations;

std::vector<texture2D> GuiElements;
texture2D gotoSafeArea, gotoMainMap;

std::vector<Light*> lights;
Light sun, light0, light1, light2;

WaterTile water;

Skybox skybox;

textureCubemap waterReflection;

// ===  GAME INFO  ===
glm::vec3 clearColor = glm::vec3(0.32f, 0.32f, 0.32f);

double last_render_time = 0;
double deltaTime = 0,
frameStartTime = 0,
debugTime = 0;
float fps = 0;
int frame = 0;
int gameState = 0;

float Max_Fps = 61.5f;
bool limit_fps = false;

/* The Anti-Aliasing type used in the program.
Can be one of the following:
* MSAA - MultiSampled Anti-Aliasing
* SSAA - SuperSampled Anti-Aliasing :! Under Contstruction !:
* FXAA - Fast Approximate Anti-Aliasing
*/
int AAType = FXAA;

// ===  FUNCTIONS  ===

// Load a model with only a texture
void loadModel(Model &model, std::string modelFilename, glm::vec3 pos, glm::vec3 rot, glm::vec3 scale, GLuint textureID, float shineDamper, float reflectivity, float ambientLight);
// Load a model with a texture and normalMap
void loadModel(Model &model, std::string modelFilename, glm::vec3 pos, glm::vec3 rot, glm::vec3 scale, GLuint textureID, GLuint normalTextureID, float shineDamper, float reflectivity, float ambientLight);
// Load a model with a texture, normalMap and a shadowMap
void loadModel(Model &model, std::string modelFilename, glm::vec3 pos, glm::vec3 rot, glm::vec3 scale, GLuint textureID, GLuint normalTextureID, GLuint shadowMapID, float shineDamper, float reflectivity, float ambientLight);

// Set a model with only a texture
void loadModel(Model &model, Model &oriModel, glm::vec3 pos, glm::vec3 rot, glm::vec3 scale, GLuint textureID, float shineDamper, float reflectivity, float ambientLight);
// Set a model with a texture and normalMap
void loadModel(Model &model, Model &oriModel, glm::vec3 pos, glm::vec3 rot, glm::vec3 scale, GLuint textureID, GLuint normalTextureID, float shineDamper, float reflectivity, float ambientLight);
// Set a model with a texture, normalMap and a shadowMap
void loadModel(Model &model, Model &oriModel, glm::vec3 pos, glm::vec3 rot, glm::vec3 scale, GLuint textureID, GLuint normalTextureID, GLuint shadowMapID, float shineDamper, float reflectivity, float ambientLight);

// If trapMouseInWindow it returns the changed amount of pixels since last update
// Else if returns the position the mouse is on
glm::vec2 handleMouseInput(bool trapMouseInWindow);
// Handle user input
void handleGameInput();
// Update time stuff
void updateTime();
// Render water textures
void renderWaterTextures();
// Render shadow textures
void renderShadowTexture(Light *shadowLight);
// Render water cubemap : WORK IN PROGRESS :
void renderWaterCubeMap();

// Load and initialise all waterTiles
void loadAndInitialiseWater();
// Load and initialise all GUI elements
void loadAndInitialiseGUI();
// Load and initialise all framebuffers
void loadAllFrameBuffers();
// Load and initialise all renderers
void loadAndInitialiseRenderers();
// Load and initialise the skybox
void loadSkybox();
// Initialise Lights
void initLights();

// Initialise the client
void initialiseClient(char ipAddress[39], char port[5]);
// Client loop
void clientLoop(void *);

// Network functions
void SendInitData();
void SendLobbyData();
void SendGameData();

// Load all graphics
void loadGraphics();
// Load Safe Area Graphics
void LoadGraphics_SafeArea();
// Load Forrest Map Graphics
void LoadGraphics_ForrestMap();
// Load all models
void loadModels();
// Load Safe Area Models
void loadModels_SafeArea();
// Load Forrest Map Models
void LoadModels_ForrestMap();

// Load the Fence model
void loadFence(int iteration, glm::vec3 pos, glm::vec3 rot);
// Load the RoadLight model
void loadRoadLight(int iteration, glm::vec3 pos, glm::vec3 rot);
// Load the TrafficCone model
void loadTrafficCone(int iteration, glm::vec3 pos, glm::vec3 rot);
// Load the ArmyTent model
void loadArmyTent(int iteration, glm::vec3 pos, glm::vec3 rot);
// Load the ArmyTruck model
void loadArmyTruck(int iteration, glm::vec3 pos, glm::vec3 rot);
// Load the tree models
void loadTreeModels();


int main() {

	// Initialise the audioSystem
	SimpleAudioLib::CoreSystem& audioSystem = SimpleAudioLib::CoreSystem::getInstance();
	audioSystem.initWithDefaultDevice();

	/* This is code to load a sound file and then play it */
	sound = audioSystem.createAudioEntityFromFile("res/Sounds/tada.wav");
	// Play sound
	//sound->play(true);

	// ===============  GAME LOGIC ====================

	// Initialise GLFW and throw error if it failed
	if (!glfwInit()) {
		throw std::runtime_error("Failed to initialize GLFW");
		exit(-1);
	}

	// ========  Initialise  ==========
	last_render_time = glfwGetTime();



	//TODO: StartScreen Loop here

	// ===  DISPLAY  ===
	//Create a display and initialise GLEW
	DisplayManager::createDisplay(SCREEN_WIDTH, SCREEN_HEIGHT, PROGRAM_NAME, false);

	//Just disable vsync for max fps :).
	DisplayManager::disableVsync();

	// ===  CAMERA  ===
	//Intialise the camera and set its position and rotation
	camera.Initalise(75.0f, SCREEN_WIDTH, SCREEN_HEIGHT, 0.01f, 1500.0f);
	camera.Set(glm::vec3(0, 10, 0), glm::vec3(0, 0, 0));

	// ===  Framebuffers  ===

	//Initialise framebuffer for cubemap texture waterReflection
	//waterReflection.initialseFrameBuffer(1280);

	// Load and initialise all framebuffers
	loadAllFrameBuffers();
	// Load and initialise all renderers
	loadAndInitialiseRenderers();

	// ===  SKYBOX  ===
	loadSkybox();

	// ===  GUI  ===
	loadAndInitialiseGUI();

	// ===  INPUT  ===
	//Set input mode
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

	//Set input callback
	//glfwSetKeyCallback(window, keyfun);

	// ===  LOAD AND INITIALISE MODELS  ===
	// Load all graphics
	loadGraphics();
	// Load all models and initialise
	loadModels();

	// ============  NETWORKING LOGIC =================

	// Initialise, set the client and connect to the server.
	initialiseClient("127.0.0.1", "6881");

	// ===  LIGHTS  ===
	initLights();

	//Initialise gameState
	gameState = 1;

	player.init(glm::vec3(0), glm::vec3(0), glm::vec3(0), 100);
	//player.active = true;

	// Hide the cursor
	DisplayManager::gameCursor();
	//TODO: glfwCreateCursor can set a picture of the cursor

	// Set the mouse in the middle of the screen at start so there is no unwanted staring rotation
	handleMouseInput(true);

	/*
	//Mouse button input
	glfwSetInputMode(window, GLFW_STICKY_MOUSE_BUTTONS, 1);
	int state = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
	if (state == GLFW_PRESS)
	upgrade_cow();
	*/

	do {

		//renderWaterCubeMap();
		//GuiElements[0].rotation = (float)frame / 100.0f;

		//Prepare rendering on default framebuffer
		MasterRenderer::prepare();

		//Change WaterMoveFactor for random(ish) water movement
		float currentWaterMoveFactor = waterRenderer.getMoveFactor();
		currentWaterMoveFactor += (float)(WAVE_SPEED * deltaTime);
		if (currentWaterMoveFactor > 1) currentWaterMoveFactor -= 1;
		waterRenderer.setMoveFactor(currentWaterMoveFactor);

		/* ========= Add models to list for rendering ============== */

		unsigned int i = 0;

		if (glm::distance(glm::vec3(1300.0f, 0.0f, 0.0f), camera.position) < OPTIMIZE_SAFEAREA_DIST) {

			// Add Floor model to renderList
			modelRenderer.addToRenderList(SA_M_Floor.getModel());
			// Add Building models to renderList
			for (i = 0; i < (1 * 5); i++) modelRenderer.addToRenderList(SA_M_Building[i].getModel());
			// Add Barrel models to renderList
			for (i = 0; i < 6; i++) modelRenderer.addToRenderList(SA_M_Barrels[i].getModel());
			// Add Crate2 model to renderList
			modelRenderer.addToRenderList(SA_M_Crate2.getModel());

			// Add AmmoBox models to renderList
			for (i = 0; i < (3 * 7); i++) normalModelRenderer.addToRenderList(SA_M_AmmoBoxes[i].getModel());
			// Add Barrier models to renderList
			for (i = 0; i < 4; i++) normalModelRenderer.addToRenderList(SA_M_Barriers[i].getModel());
			// Add Crate models to renderList
			for (i = 0; i < 6; i++) normalModelRenderer.addToRenderList(SA_M_Crate[i].getModel());
			// Add Crate models to renderList
			for (i = 0; i < 2; i++) normalModelRenderer.addToRenderList(SA_M_Pallets[i].getModel());
			// Add SandBag model to renderList.
			for (i = 0; i < 2; i++) normalModelRenderer.addToRenderList(SA_M_SandBag[i].getModel());
		}
		else {
			// Add the flat terrain to the terrain renderer
			terrainRenderer.addToRenderList(FM_M_FLATTERRAIN.getModel());
			// Add the background terrain to the renderList
			normalModelRenderer.addToRenderList(FM_M_TERRAIN.getModel());

			// == RENDER LIST == 

			// Add all trees that are within the optimization distance to the renderList
			for (i = 0; i < trees.getTreeCount(); i++) {
				if (glm::distance(trees.getTreeAt(i)->getPosition(), camera.position) < OPTIMIZE_DIST) {
					modelRenderer.addToRenderList(trees.getTreeAt(i));
				}
			}
			// Add all army trucks that are within the optimization distance to the renderList
			for (i = 0; i < 6; i++) {
				if (glm::distance(FM_M_ARMY_TRUCK[5 * i].getModel()->getPosition(), camera.position) < OPTIMIZE_DIST) {
					for (unsigned int j = 0; j < 5; j++) modelRenderer.addToRenderList(FM_M_ARMY_TRUCK[(5 * i) + j].getModel());
				}
			}
			// Add all roadlights that are within the optimization distance to the renderList
			for (i = 0; i < 24; i++) {
				if (glm::distance(FM_M_ROADLIGHT[3 * i].getModel()->getPosition(), camera.position) < OPTIMIZE_DIST) {
					for (unsigned int j = 0; j < 3; j++) modelRenderer.addToRenderList(FM_M_ROADLIGHT[(3 * i) + j].getModel());
				}
			}
			// Add all Military bunkers that are within the optimization distance to the renderList
			for (i = 0; i < 3; i++) {
				if (glm::distance(FM_M_MILITARY_BUNKER[3 * i].getModel()->getPosition(), camera.position) < LARGE_OPTIMIZE_DIST) {
					for (unsigned int j = 0; j < 3; j++) modelRenderer.addToRenderList(FM_M_MILITARY_BUNKER[(3 * i) + j].getModel());
				}
			}
			// Add all traffic cones that are within the optimization distance to the renderList
			for (i = 0; i < 14; i++) {
				if (glm::distance(FM_M_TRAFFICCONE[i].getModel()->getPosition(), camera.position) < SMALL_OPTIMIZE_DIST) {
					modelRenderer.addToRenderList(FM_M_TRAFFICCONE[i].getModel());
				}
			}
			// Add rails that are within the optimization distance to the renderList
			if (glm::distance(FM_M_RAIL[0].getModel()->getPosition(), camera.position) < LARGE_OPTIMIZE_DIST) {
				modelRenderer.addToRenderList(FM_M_RAIL[0].getModel());
				modelRenderer.addToRenderList(FM_M_RAIL[1].getModel());
			}
			// Add all town houses that are within the optimization distance to the renderList
			for (i = 0; i < 2; i++) {
				if (glm::distance(FM_M_TOWNHOUSE[i].getModel()->getPosition(), camera.position) < LARGE_OPTIMIZE_DIST) {
					modelRenderer.addToRenderList(FM_M_TOWNHOUSE[i].getModel());
				}
			}

			// == NORMAL RENDER LIST ==

			// Add all ambulances that are within the optimization distance to the renderList
			for (i = 0; i < 2; i++) {
				if (glm::distance(FM_M_AMBULANCE[5 * i].getModel()->getPosition(), camera.position) < OPTIMIZE_DIST) {
					for (unsigned int j = 0; j < 5; j++) normalModelRenderer.addToRenderList(FM_M_AMBULANCE[(5 * i) + j].getModel());
				}
			}
			// Add the command tent that is within the optimization distance to the renderList
			if (glm::distance(FM_M_COMMAND_TENT.getModel()->getPosition(), camera.position) < LARGE_OPTIMIZE_DIST) {
				normalModelRenderer.addToRenderList(FM_M_COMMAND_TENT.getModel());
			}
			// Add all army tents that are within the optimization distance to the renderList
			for (i = 0; i < 6; i++) {
				if (glm::distance(FM_M_ARMY_TENT[3 * i].getModel()->getPosition(), camera.position) < OPTIMIZE_DIST) {
					for (unsigned int j = 0; j < 3; j++) normalModelRenderer.addToRenderList(FM_M_ARMY_TENT[(3 * i) + j].getModel());
				}
			}
			// Add all barriers that are within the optimization distance to the renderList
			for (i = 0; i < 25; i++) {
				if (glm::distance(FM_M_BARRIER[i].getModel()->getPosition(), camera.position) < SMALL_OPTIMIZE_DIST) {
					normalModelRenderer.addToRenderList(FM_M_BARRIER[i].getModel());
				}
			}
			// Add all containers that are within the optimization distance to the renderList
			for (i = 0; i < 12; i++) {
				if (glm::distance(FM_M_CONTAINER[i].getModel()->getPosition(), camera.position) < OPTIMIZE_DIST) {
					normalModelRenderer.addToRenderList(FM_M_CONTAINER[i].getModel());
				}
			}
			// Add broken fence 1 that is within the optimization distance to the renderList
			if (glm::distance(FM_M_BROKEN_FENCE1[0].getModel()->getPosition(), camera.position) < OPTIMIZE_DIST) {
				for (i = 0; i < 3; i++) normalModelRenderer.addToRenderList(FM_M_BROKEN_FENCE1[i].getModel());
			}
			// Add broken fence 2 that is within the optimization distance to the renderList
			if (glm::distance(FM_M_BROKEN_FENCE2[0].getModel()->getPosition(), camera.position) < OPTIMIZE_DIST) {
				for (i = 0; i < 3; i++) normalModelRenderer.addToRenderList(FM_M_BROKEN_FENCE2[i].getModel());
			}
			// Add all fences that are within the optimization distance to the renderList
			for (i = 0; i < 164; i++) {
				if (glm::distance(FM_M_FENCE[3 * i].getModel()->getPosition(), camera.position) < OPTIMIZE_DIST) {
					for (unsigned int j = 0; j < 3; j++) normalModelRenderer.addToRenderList(FM_M_FENCE[(3 * i) + j].getModel());
				}
			}
			// Add all fences that are within the optimization distance to the renderList
			for (i = 0; i < 3; i++) {
				normalModelRenderer.addToRenderList(FM_M_ROAD[i].getModel());
			}
			// Add the statue that is within the optimization distance to the renderList
			if (glm::distance(FM_M_STATUE.getModel()->getPosition(), camera.position) < SMALL_OPTIMIZE_DIST) {
				normalModelRenderer.addToRenderList(FM_M_STATUE.getModel());
			}
			// Add the well that is within the optimization distance to the renderList
			if (glm::distance(FM_M_WELL.getModel()->getPosition(), camera.position) < OPTIMIZE_DIST) {
				normalModelRenderer.addToRenderList(FM_M_WELL.getModel());
			}
		}

		modelRenderer.addToRenderList(player.gun.gun_model.getModel());

		//Add water to the renderer list
		//waterRenderer.addToRenderList(&water);

		/* =============== Start of rendering ===================== */

		//Render reflection and refraction texture of the water
		//renderWaterTextures();

		//Render the shadow texture
		//renderShadowTexture(&sun);


		// If we can use the useable object at the position we want to draw text that shows the user can use the object by pressing a button
		// Check if we want and can use one of the positions that make the player go to the safe area
		if (player.canUse(glm::vec3(-272.385f, 0.0f, -254.353f), USE_DISTANCE) || player.canUse(glm::vec3(-365.861f, 3.0f, -254.333f), USE_DISTANCE) || player.canUse(glm::vec3(-257.726f, 3.0f, -343.333f), USE_DISTANCE) || player.canUse(glm::vec3(-368.124f, 3.0f, -343.33f), USE_DISTANCE)) {
			// Teleport to the position in the safe area
			GuiElements.push_back(gotoSafeArea);
		}
		// Check if we want and can use the position that makes the player go back to the main map from the safe area
		else if (player.canUse(glm::vec3(1303.24, 0.0, 14.652), USE_DISTANCE)) {
			// Teleport to the position in the main map
			GuiElements.push_back(gotoMainMap);
		}

		glFinish();
		frameStartTime = glfwGetTime();

		//Render scene to multisampled anti-aliased framebuffer
		sceneRenderer.bindRenderToTextureFrameBuffer();
		MasterRenderer::prepare();

		//Render everything

		skybox.render(&camera);

		terrainRenderer.render(lights, &camera, glm::vec4(0, -1, 0, 100000));
		waterRenderer.render(lights, &camera);

		modelRenderer.render(lights, &camera, glm::vec4(0, -1, 0, 100000));
		normalModelRenderer.render(lights, &camera, glm::vec4(0, -1, 0, 100000));

		// Render own player animation
		//player.getAnimModel()->Draw(modelRenderer.shader, lights, &camera, glm::vec4(0, -1, 0, 100000));

		// Render all other player's animations
		for (int i = 0; i < MAX_LOBBYSIZE; i++) {
			if (otherPlayers[i].active) {
				otherPlayers[i].getAnimModel()->Draw(modelRenderer.shader, lights, &camera, glm::vec4(0, -1, 0, 100000));
			}
		}

			// Render all enemy animations
			for (int i = 0; i < MAX_ENEMIES; i++) {
				if (enemies[i].active) {
					if (glm::distance(enemies[i].getPosition(), camera.position) < OPTIMIZE_DIST) {
						enemies[i].getAnimModel()->Draw(normalModelRenderer.shader, lights, &camera, glm::vec4(0, -1, 0, 100000));
					}
				}
			}
		}

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
		GuiElements.clear();


		//Update all time related variables.
		updateTime();

		if (limit_fps) {
			// Limit fps to FPSLIMIT
			while (glfwGetTime() < (last_render_time + (1.0f / Max_Fps))) {}
		}

		// Poll all events
		glfwPollEvents();

		// Update own player
		player.update();

		// Update own animation
		player.updateAnimation(player.networkAnimType);

		for (int i = 0; i < MAX_LOBBYSIZE; i++) {
			// Update all other player's animation
			otherPlayers[i].updateAnimation(otherPlayers[i].networkAnimType);
		}

		for (int i = 0; i < MAX_ENEMIES; i++) {
			// Update the enemy data with the data received from the server
			client.getEnemyData(enemies[i], i);
			// Update the enemy, this will also update the animation
			enemies[i].update();
		}

		// Get and update the player with its new position, it's health and the rest But not the rotation!!!
		client.getPlayerData(player);
		for (int i = 0; i < MAX_LOBBYSIZE; i++) {
			client.getPlayerData(otherPlayers[i], i);
		}

		// Set the rotation of the player for the next update
		client.setPlayerData(player);

		player.update();

		// Set the camera rotation to the players rotation and convert it to radians
		camera.rotation = glm::radians(player.getRotation());
		// Set the camera position
		camera.position = player.getPosition();
		camera.position.y = 6.0f;

		//if (glfwGetKey(window, GLFW_KEY_KP_8) == GLFW_PRESS) camera.position.x += 2.0f; // up
		//else if (glfwGetKey(window, GLFW_KEY_KP_2) == GLFW_PRESS) camera.position.x -= 2.0f; //down

		//if (glfwGetKey(window, GLFW_KEY_KP_4) == GLFW_PRESS) camera.position.z += 2.0f; // left
		//else if (glfwGetKey(window, GLFW_KEY_KP_6) == GLFW_PRESS) camera.position.z -= 2.0f; //right

		//if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) camera.position.y += 1.0f; // left
		//else if (glfwGetKey(window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS) camera.position.y -= 1.0f; //right

		//Set title to hold fps info
		std::string fpsStr = std::string(PROGRAM_NAME) + " FPS: " + std::to_string(fps) + " deltaTime: " + std::to_string(deltaTime * 100) /*+ " Mouse: x: " + std::to_string(rot.x) + " y: " + std::to_string(rot.y)*/;
		DisplayManager::setDisplayTitle(fpsStr.c_str());

		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) gameState = 0;
		if (DisplayManager::updateDisplay() < 0) gameState = 0;
	} while (gameState >= 1);

	// Show the cursor again
	DisplayManager::showCursor();

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

	// Clean up audio
	delete sound;
	sound = NULL;

	SimpleAudioLib::CoreSystem::release();
}


// If trapMouseInWindow it returns the changed amount of pixels since last update
// Else if returns the position the mouse is on
glm::vec2 handleMouseInput(bool trapMouseInWindow) {
	// Get mouse position
	double xpos, ypos;
	glfwGetCursorPos(window, &xpos, &ypos);

	glm::vec2 windowPos = DisplayManager::getWindowPosition();
	glm::vec2 windowSize = DisplayManager::getWindowSize();

	glm::vec2 windowCenter = glm::vec2(windowPos.x + (windowSize.x / 2), windowPos.y + (windowSize.y / 2));

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
// Handle user input in game
void handleGameInput()
{
	// Poll all events
	glfwPollEvents();

	// HandleInput from keys

	if (window == nullptr) return;

	// Handle input of forward or backward
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) client.addActionType(MOVE_FORWARD);
	else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) client.addActionType(MOVE_BACKWARD);
	// Handle input of left or right
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) client.addActionType(MOVE_LEFT);
	else if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) client.addActionType(MOVE_RIGHT);

	// Handle input of sprinting
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) client.addActionType(MOVE_RUN);

	// Handle input of shooting
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) client.addActionType(SHOOT);
	
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) client.addActionType(JUMP);

	// Handle input of using
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) client.addActionType(USE);

	// Handle input of reloading
	if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) client.addActionType(RELOAD);
}
// Update time stuff
void updateTime()
{
	// Calculate delta time
	//glfwGetTime() nr of seconds since start of game
	//last_render_time last set time nr seconds since start of game
	deltaTime = glfwGetTime() - last_render_time; //Time in seconds it took for the last frame.
	fps = (float)(1.0f / deltaTime);

	last_render_time = glfwGetTime();

	frame++;
}

// Load and initialise all waterTiles
void loadAndInitialiseWater()
{
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
}
// Load and initialise all GUI elements
void loadAndInitialiseGUI()
{
	// Load gotoSafeArea
	gotoSafeArea.loadImage("res/GUI/gotoSafeArea.bmp", true, false);
	gotoSafeArea.setScale(glm::vec2(0.5f));
	gotoSafeArea.setPosition(glm::vec2(1.0f, 0.5f));

	// Load gotoMainMap
	gotoMainMap.loadImage("res/GUI/gotoMainMap.bmp", true, false);
	gotoMainMap.setScale(glm::vec2(0.5f));
	gotoMainMap.setPosition(glm::vec2(1.0f, 0.5f));
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
	/*
	//Add normalmapped models to renderer list
	normalModelRenderer.addToRenderList(model.getModel());
	normalModelRenderer.addToRenderList(model2.getModel());
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
	*/
}

// Load and initialise all framebuffers
void loadAllFrameBuffers()
{
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

}
// Load and initialise all masterRenderers
void loadAndInitialiseRenderers()
{
	//Load and initialise all masterRenderers
	guiRenderer.load("WEngine/Shaders/Gui/GuiShader.vs", "WEngine/Shaders/Gui/GuiShader.fs");
	modelRenderer.load("WEngine/Shaders/Default/Default.vs", "WEngine/Shaders/Default/Default.fs", &camera);
	normalModelRenderer.load("WEngine/Shaders/NormalMaps/NormalMap.vs", "WEngine/Shaders/NormalMaps/NormalMap.fs", &camera);
	terrainRenderer.load("WEngine/Shaders/Terrain/Terrain.vs", "WEngine/Shaders/Terrain/Terrain.fs", &camera);
	waterRenderer.load("WEngine/Shaders/Water/WaterShader.vs", "WEngine/Shaders/Water/WaterShader.fs", &camera);
	shadowRenderer.load("WEngine/Shaders/Shadows/ShadowShader.vs", "WEngine/Shaders/Shadows/ShadowShader.fs");
}
// Load and initialise the skybox
void loadSkybox()
{
	std::string skyboxTextures[6] = {
		"res/Skybox/hw_desertnight/desert_night_right.bmp", // RIGHT
		"res/Skybox/hw_desertnight/desert_night_left.bmp",  // LEFT
		"res/Skybox/hw_desertnight/desert_night_top.bmp",	// TOP
		"res/Skybox/hw_desertnight/desert_night_bottom.bmp",// BOTTOM
		"res/Skybox/hw_desertnight/desert_night_back.bmp",	// BACK
		"res/Skybox/hw_desertnight/desert_night_front.bmp",	// FRONT
	};
	skybox.load("WEngine/Shaders/Skybox/Skybox.vs", "WEngine/Shaders/Skybox/Skybox.fs", &camera, skyboxTextures);
}
// Initialise Lights
void initLights()
{
	//Initialise lights and add them to the light list
	// Light(Position, Colour)
	sun = Light(glm::vec3(-274.0f, 500.0f, 1353.2f), glm::vec3(0.55f, 0.55f, 0.55f));
	lights.push_back(&sun);
}

// ======  SERVER HANDLE FUNCTIONS  ======

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

	_beginthread(clientLoop, 0, NULL);
}
// The client loop
void clientLoop(void *)
{
	clientRunning = true;

	// Client networking loop
	while (clientRunning) {
		// Receive and parse data.
		client.updateClient();

		// If the function pointer is set to a function, execute this function.
		// This function is to execute a function that sends data to the server.
		if (networkUpdateFunction != nullptr) networkUpdateFunction();

		player.setPosition(client.myPlayerData.position);

		// Limit update cycle amount to UPDATE_CYCLES_PER_SECOND
		while (((float)(std::clock() - programStartClock) / (float)CLOCKS_PER_SEC) < (clientLoopLastRenderTime + (1.0f / (float)UPDATE_CYCLES_PER_SECOND))) {}

		clientLoopDeltaTime = (((float)(std::clock() - programStartClock) / (float)CLOCKS_PER_SEC)) - clientLoopLastRenderTime; //Time in miliseconds it took for the last update cycle.
		clientLoopLastRenderTime = (((float)(std::clock() - programStartClock) / (float)CLOCKS_PER_SEC));
	}

	// End of function, end the thread and release its memory.
	_endthread();
}

// Send intitialisation data
// When just connected to the server and need to send name.
void SendInitData()
{
	char name[] = "Wouter140";

	memcpy(&client.myPlayerData.playerName, name, strlen(name));
	client.myPlayerData.playerNameSize = strlen(name);
	client.addActionType(GAME_INITIALISATION);

	client.sendLobbyUpdate();
}
// Send lobby data
// When in lobby
void SendLobbyData()
{
	//if (/*want to start the game*/)
	//{
	// Add a action Start Game
	client.addActionType(GAME_START);
	// Send the packet to the server
	client.sendLobbyUpdate();
	//}

	// If the lobbyTimer has started running
	if (client.gameStarting)
	{

		client.lobbyTimer -= (float)clientLoopDeltaTime;
		if (client.lobbyTimer <= 0.0f) client.lobbyTimer = 0.0f;
	}
}

// Send game data
// When in game
void SendGameData()
{
	// Update key input
	handleGameInput();

	//if(client.hasActionType())
	client.sendPlayerData();
}

// Load all graphics
void loadGraphics()
{

	// Load all the graphics for the safe area.
	LoadGraphics_SafeArea();

	// Load all the graphics for the forrest map.
	LoadGraphics_ForrestMap();

	Player::animationTexture = loader.loadTexture("res/PlayerAnimations/Textures/playerTextureArmy.bmp", true);

	Enemy::animationTexture = loader.loadTexture("res/EnemyAnimations/Textures/Iron_Man_D.bmp", true);
	Enemy::animationNTexture = loader.loadTexture("res/EnemyAnimations/Textures/Iron_Man_N.bmp", true);
}
// Load Safe Area Graphics
void LoadGraphics_SafeArea()
{
	// Load Floor's textures.
	SA_T_Floor = loader.loadTexture("res/Safe_Area/Floor/Textures/floor.bmp", true);

	// Load Building's textures.
	SA_T_Building[0] = loader.loadTexture("res/Safe_Area/Building/Textures/building.bmp", true); // Building
	SA_T_Building[1] = loader.loadTexture("res/Safe_Area/Building/Textures/building.bmp", true); // Doors
	SA_T_Building[2] = loader.loadTexture("res/Safe_Area/Building/Textures/wood.bmp", true); // Wood
	SA_T_Building[3] = loader.loadTexture("res/Safe_Area/Building/Textures/stroh.bmp", true); // Roof
	SA_T_Building[4] = loader.loadTexture("res/Safe_Area/Building/Textures/wood.bmp", true); // Wheel

																							 // Load AmmoBox's texture and normals.
	SA_T_AmmoBoxes[0] = loader.loadTexture("res/Safe_Area/Weapon_Box/Textures/box_d.bmp", true);
	SA_T_AmmoBoxes[1] = loader.loadTexture("res/Safe_Area/Weapon_Box/Textures/roc_d.bmp", true);
	SA_T_AmmoBoxes[2] = loader.loadTexture("res/Safe_Area/Weapon_Box/Textures/roc_d.bmp", true);
	SA_T_AmmoBoxes[3] = loader.loadTexture("res/Safe_Area/Weapon_Box/Textures/roc_d.bmp", true);
	SA_T_AmmoBoxes[4] = loader.loadTexture("res/Safe_Area/Weapon_Box/Textures/roc_d.bmp", true);
	SA_T_AmmoBoxes[5] = loader.loadTexture("res/Safe_Area/Weapon_Box/Textures/roc_d.bmp", true);
	SA_T_AmmoBoxes[6] = loader.loadTexture("res/Safe_Area/Weapon_Box/Textures/roc_d.bmp", true);
	SA_TN_AmmoBoxes[0] = loader.loadTexture("res/Safe_Area/Weapon_Box/Textures/box_n.bmp", false);
	SA_TN_AmmoBoxes[1] = loader.loadTexture("res/Safe_Area/Weapon_Box/Textures/roc_n.bmp", false);

	// Load Barrel's texture.
	SA_T_Barrels[0] = loader.loadTexture("res/Safe_Area/Barrel/Textures/green_barrel.bmp", true);
	SA_T_Barrels[1] = loader.loadTexture("res/Safe_Area/Barrel/Textures/red_barrel.bmp", true);
	SA_T_Barrels[2] = loader.loadTexture("res/Safe_Area/Barrel/Textures/regular_barrel.bmp", true);

	// Load Barrier's texture and normals.
	SA_T_Barriers = loader.loadTexture("res/Safe_Area/ConcreteBarrier/Textures/Barrier.bmp", true);
	SA_TN_Barriers = loader.loadTexture("res/Safe_Area/ConcreteBarrier/Textures/Normal.bmp", false);

	// Load Crate's texture and normal.
	SA_T_Crate = loader.loadTexture("res/Safe_Area/WoodBox/Textures/diffuse.bmp", true);
	SA_TN_Crate = loader.loadTexture("res/Safe_Area/WoodBox/Textures/normal.bmp", false);

	// Load Crate2's texture.
	SA_T_Crate2 = loader.loadTexture("res/Safe_Area/WoodBox2/Textures/WoodBox2.bmp", true);

	// Load Pallet's textures
	SA_T_Pallets = loader.loadTexture("res/Safe_Area/Pallet/Textures/Pallet_dif.bmp", true);
	SA_TN_Pallets = loader.loadTexture("res/Safe_Area/Pallet/Textures/Pallet_norm.bmp", false);

	// Load SandBag's texture and normal.
	SA_T_SandBag = loader.loadTexture("res/Safe_Area/SandBags/Textures/sandbags_d.bmp", true);
	SA_TN_SandBag = loader.loadTexture("res/Safe_Area/SandBags/Textures/sandbags_n.bmp", false);
}
// Load Forrest Map Graphics
void LoadGraphics_ForrestMap()
{
	trees.setTreeTexture(loader.loadTexture("res/Forrest_Area/Trees/Simple_Tree/Textures/texture.bmp", true));

	FM_T_FLATTERRAIN[0] = loader.loadTexture("res/Forrest_Area/Terrain/Ground/Textures/black.bmp", true);
	FM_T_FLATTERRAIN[1] = loader.loadTexture("res/Forrest_Area/Terrain/Ground/Textures/red.bmp", true);
	FM_T_FLATTERRAIN[2] = loader.loadTexture("res/Forrest_Area/Terrain/Ground/Textures/green.bmp", true);
	FM_T_FLATTERRAIN[3] = loader.loadTexture("res/Forrest_Area/Terrain/Ground/Textures/blue.bmp", true);
	FM_T_FLATTERRAIN[4] = loader.loadTexture("res/Forrest_Area/Terrain/Ground/Textures/TerrainBlendMap.bmp", false);

	FM_T_TERRAIN = loader.loadTexture("res/Forrest_Area/Terrain/Mountains/Textures/Ring_Of_Doom_Texture.bmp", true);
	FM_TN_TERRAIN = loader.loadTexture("res/Forrest_Area/Terrain/Mountains/Textures/Ring_Of_Doom_Normal.bmp", false);

	FM_T_AMBULANCE[0] = loader.loadTexture("res/Forrest_Area/Ambulance/Textures/veh_ambulance_dam_02_d.bmp", true);
	FM_T_AMBULANCE[1] = loader.loadTexture("res/Forrest_Area/Ambulance/Textures/veh_ambulance_dam_02_1_d.bmp", true);
	FM_T_AMBULANCE[2] = loader.loadTexture("res/Forrest_Area/Ambulance/Textures/veh_ambulance_logo_dam_d.bmp", true);
	FM_T_AMBULANCE[3] = loader.loadTexture("res/Forrest_Area/Ambulance/Textures/veh_ambulance_wheels_d.bmp", true);
	FM_T_AMBULANCE[4] = loader.loadTexture("res/Forrest_Area/Ambulance/Textures/veh_ambulance_windows_dam_d.bmp", true);

	FM_TN_AMBULANCE[0] = loader.loadTexture("res/Forrest_Area/Ambulance/Textures/veh_ambulance_dam_02_n.bmp", false);
	FM_TN_AMBULANCE[1] = loader.loadTexture("res/Forrest_Area/Ambulance/Textures/veh_ambulance_dam_02_1_n.bmp", false);
	FM_TN_AMBULANCE[2] = loader.loadTexture("res/Forrest_Area/Ambulance/Textures/veh_ambulance_logo_dam_n.bmp", false);
	FM_TN_AMBULANCE[3] = loader.loadTexture("res/Forrest_Area/Ambulance/Textures/veh_ambulance_wheels_n.bmp", false);
	FM_TN_AMBULANCE[4] = loader.loadTexture("res/Forrest_Area/Ambulance/Textures/veh_ambulance_windows_dam_n.bmp", false);

	FM_T_COMMAND_TENT = loader.loadTexture("res/Forrest_Area/Army_Command_Tent/Textures/Tent_Defuse.bmp", true);
	FM_TN_COMMAND_TENT = loader.loadTexture("res/Forrest_Area/Army_Command_Tent/Textures/Tent_Normal.bmp", false);

	FM_T_ARMY_TENT[0] = loader.loadTexture("res/Forrest_Area/Army_Tent/Textures/inter.bmp", true);
	FM_T_ARMY_TENT[1] = loader.loadTexture("res/Forrest_Area/Army_Tent/Textures/outer.bmp", true);
	FM_T_ARMY_TENT[2] = loader.loadTexture("res/Forrest_Area/Army_Tent/Textures/Window.bmp", true);

	FM_TN_ARMY_TENT[0] = loader.loadTexture("res/Forrest_Area/Army_Tent/Textures/inter_bump.bmp", false);
	FM_TN_ARMY_TENT[1] = loader.loadTexture("res/Forrest_Area/Army_Tent/Textures/outer_bump.bmp", false);

	FM_T_ARMY_TRUCK[0] = loader.loadTexture("res/Forrest_Area/Army_Truck/Textures/box-furgon.bmp", true);
	FM_T_ARMY_TRUCK[1] = loader.loadTexture("res/Forrest_Area/Army_Truck/Textures/cabin.bmp", true);
	FM_T_ARMY_TRUCK[2] = loader.loadTexture("res/Forrest_Area/Army_Truck/Textures/basic.bmp", true);
	FM_T_ARMY_TRUCK[3] = loader.loadTexture("res/Forrest_Area/Army_Truck/Textures/detail_console.bmp", true);
	FM_T_ARMY_TRUCK[4] = loader.loadTexture("res/Forrest_Area/Army_Truck/Textures/U3_wheel.bmp", true);

	FM_T_BARRIER = loader.loadTexture("res/Forrest_Area/Barrier/Textures/Barrier.bmp", true);
	FM_TN_BARRIER = loader.loadTexture("res/Forrest_Area/Barrier/Textures/Normal.bmp", false);

	FM_T_CONTAINER = loader.loadTexture("res/Forrest_Area/Container/Textures/Cargo01_D.bmp", true);
	FM_TN_CONTAINER = loader.loadTexture("res/Forrest_Area/Container/Textures/Cargo01_N.bmp", false);

	FM_T_FENCE[0] = loader.loadTexture("res/Forrest_Area/Fences/Textures/logends.bmp", true);
	FM_TN_FENCE[0] = loader.loadTexture("res/Forrest_Area/Fences/Textures/logends_NRM.bmp", false);
	FM_T_FENCE[1] = loader.loadTexture("res/Forrest_Area/Fences/Textures/logs.bmp", true);
	FM_TN_FENCE[1] = loader.loadTexture("res/Forrest_Area/Fences/Textures/logs_NRM.bmp", false);
	FM_T_FENCE[2] = loader.loadTexture("res/Forrest_Area/Fences/Textures/planks.bmp", true);
	FM_TN_FENCE[2] = loader.loadTexture("res/Forrest_Area/Fences/Textures/planks_NRM.bmp", false);

	FM_T_BROKENFENCE1[0] = loader.loadTexture("res/Forrest_Area/Fences/Textures/logends.bmp", true);
	FM_T_BROKENFENCE1[1] = loader.loadTexture("res/Forrest_Area/Fences/Textures/logs.bmp", true);
	FM_T_BROKENFENCE1[2] = loader.loadTexture("res/Forrest_Area/Fences/Textures/planks.bmp", true);

	FM_T_BROKENFENCE2[0] = loader.loadTexture("res/Forrest_Area/Fences/Textures/logends.bmp", true);
	FM_T_BROKENFENCE2[1] = loader.loadTexture("res/Forrest_Area/Fences/Textures/logs.bmp", true);
	FM_T_BROKENFENCE2[2] = loader.loadTexture("res/Forrest_Area/Fences/Textures/planks.bmp", true);

	FM_T_ROADLIGHT[0] = loader.loadTexture("res/Forrest_Area/RoadLight/Textures/LightHeadText.bmp", true);
	FM_T_ROADLIGHT[1] = loader.loadTexture("res/Forrest_Area/RoadLight/Textures/MetalBase0121_9_S.bmp", true);
	FM_T_ROADLIGHT[2] = loader.loadTexture("res/Forrest_Area/RoadLight/Textures/MetalBase0123_S.bmp", true);

	FM_TS_ROADLIGHT = loader.loadTexture("res/Forrest_Area/RoadLight/Textures/LightHeadSpec.bmp", false);

	FM_T_MILITARY_BUNKER[0] = loader.loadTexture("res/Forrest_Area/Military_Bunker/Textures/corrugated-551291_1920.bmp", true);
	FM_T_MILITARY_BUNKER[1] = loader.loadTexture("res/Forrest_Area/Military_Bunker/Textures/mural.bmp", true);
	FM_T_MILITARY_BUNKER[2] = loader.loadTexture("res/Forrest_Area/Military_Bunker/Textures/_original.bmp", true);

	FM_T_TRAFFICCONE = loader.loadTexture("res/Forrest_Area/Traffic_Cone/Textures/cone2.bmp", true);

	FM_T_ROAD[0] = loader.loadTexture("res/Forrest_Area/Roads/Textures/Roadstraight.bmp", true);
	FM_T_ROAD[1] = loader.loadTexture("res/Forrest_Area/Roads/Textures/Roadstraight.bmp", true);
	FM_T_ROAD[2] = loader.loadTexture("res/Forrest_Area/Roads/Textures/Roadstraight.bmp", true);
	FM_TN_ROAD = loader.loadTexture("res/Forrest_Area/Roads/Textures/Roadstraight_NORMAL.bmp", false);

	FM_T_STATUE = loader.loadTexture("res/Forrest_Area/Statue/Textures/statue_d.bmp", true);
	FM_TN_STATUE = loader.loadTexture("res/Forrest_Area/Statue/Textures/statue_n.bmp", false);

	FM_T_RAIL[0] = loader.loadTexture("res/Forrest_Area/Rails/Textures/Truck_1524_reils.bmp", true);
	FM_T_RAIL[1] = loader.loadTexture("res/Forrest_Area/Rails/Textures/Truck_1524_sleppers.bmp", true);

	FM_T_TOWNHOUSE = loader.loadTexture("res/Forrest_Area/Town_House/Textures/BuildingsTallHouse0065_2_M.bmp", true);

	FM_T_WELL = loader.loadTexture("res/Forrest_Area/Well/Textures/well_01_Diff.bmp", true);
	FM_TN_WELL = loader.loadTexture("res/Forrest_Area/Well/Textures/well_01_Norm.bmp", false);

}

// Load all models
std::vector<gameobject> animobjs[9];
std::thread tanim[9];

void anim1() {
	playerAnimations[0] = Player::loadAnimations("res/PlayerAnimations/Idle/", animobjs[0], 0, 32, 15, true);
}
void anim2() {
	playerAnimations[1] = Player::loadAnimations("res/PlayerAnimations/Walk_Forward/", animobjs[1], 32, 31, 62, true);
}
void anim3() {
	playerAnimations[2] = Player::loadAnimations("res/PlayerAnimations/Walk_Backward/", animobjs[2], 63, 31, 62, true);
}
void anim4() {
	playerAnimations[3] = Player::loadAnimations("res/PlayerAnimations/Walk_Left/", animobjs[3], 94, 31, 62, true);
}
void anim5() {
	playerAnimations[4] = Player::loadAnimations("res/PlayerAnimations/Walk_Right/", animobjs[4], 125, 31, 62, true);
}
void anim6() {
	playerAnimations[5] = Player::loadAnimations("res/PlayerAnimations/Run_Forward/", animobjs[5], 156, 16, 62, true);
}

void anim7() {
	enemyAnimations[0] = Enemy::loadAnimations("res/EnemyAnimations/Walk_Forward/", animobjs[6], 0, 38, 25, true);
}
void anim8() {
	enemyAnimations[1] = Enemy::loadAnimations("res/EnemyAnimations/Attack/", animobjs[7], 38, 40, 62, true);
}
void anim9() {
	enemyAnimations[2] = Enemy::loadAnimations("res/EnemyAnimations/Dying/", animobjs[8], 78, 45, 62, true);
}
void loadAnimations()
{
	playerAnimations.resize(6);
	enemyAnimations.resize(3);

	tanim[0] = std::thread(anim1);
	tanim[1] = std::thread(anim2);
	tanim[2] = std::thread(anim3);
	tanim[3] = std::thread(anim4);
	tanim[4] = std::thread(anim5);
	tanim[5] = std::thread(anim6);

	tanim[6] = std::thread(anim7);
	tanim[7] = std::thread(anim8);
	tanim[8] = std::thread(anim9);
}
void endLoadAnimations() {
	tanim[0].join();
	tanim[1].join();
	tanim[2].join();
	tanim[3].join();
	tanim[4].join();
	tanim[5].join();

	tanim[6].join();
	tanim[7].join();
	tanim[8].join();

	animationModels.insert(animationModels.end(), animobjs[0].begin(), animobjs[0].end());
	animationModels.insert(animationModels.end(), animobjs[1].begin(), animobjs[1].end());
	animationModels.insert(animationModels.end(), animobjs[2].begin(), animobjs[2].end());
	animationModels.insert(animationModels.end(), animobjs[3].begin(), animobjs[3].end());
	animationModels.insert(animationModels.end(), animobjs[4].begin(), animobjs[4].end());
	animationModels.insert(animationModels.end(), animobjs[5].begin(), animobjs[5].end());

	enemyAnimationModels.insert(enemyAnimationModels.end(), animobjs[6].begin(), animobjs[6].end());
	enemyAnimationModels.insert(enemyAnimationModels.end(), animobjs[7].begin(), animobjs[7].end());
	enemyAnimationModels.insert(enemyAnimationModels.end(), animobjs[8].begin(), animobjs[8].end());

	Player::createAnimationModels();
	Enemy::createAnimationModels();
}
void loadModels()
{
	// Start loading all animations
	loadAnimations();

	// Load all the models for the safe area.
	loadModels_SafeArea();

	// Load all the models for the forrest map.
	LoadModels_ForrestMap();

	// If loading the animations is not done yet, wait for it here and then create vao's for all objects
	endLoadAnimations();

	T_GUN_WALTER = loader.loadTexture("res/Gun/walter_pk_48/black.bmp", true);
	loadModel(GUN_WALTER, "res/Gun/walter_pk_48/walter.obj", glm::vec3(0, 0, 0), glm::vec3(0, 0, 0), glm::vec3(1.0f), T_GUN_WALTER, 100.0f, 0.1f, 0.4f);
}
// Load Safe Area Models
void loadModels_SafeArea()
{
	// Please note:
	// If taking the positions from 3DS MAX:
	// The position and Rotation Y and Z need to be swapped.
	// Then the Z needs to be inverted on both the position and rotation

	glm::vec3 offsetPos = glm::vec3(1300.0f, 2.824f, 0.0f);
	glm::vec3 additionalScale = glm::vec3(1.0f);

	// Load Floor's moodels.
	loadModel(SA_M_Floor, "res/Safe_Area/Floor/Floor.obj", glm::vec3(0, -0.652, 0) + offsetPos, glm::vec3(0, 0, 0), glm::vec3(1.0f) * additionalScale, SA_T_Floor, 100.0f, -1.0f, 0.4f);
	SA_M_Floor.getModel()->setTiledAmount(glm::vec2(6.0f));

	// Load Building models.
	loadModel(SA_M_Building[0], "res/Safe_Area/Building/Building_Building.obj", glm::vec3(0, 0, 0) * additionalScale + offsetPos, glm::vec3(0, 0, 0), glm::vec3(1.0f) * additionalScale, SA_T_Building[0], 100.0f, 0.1f, 0.4f);
	loadModel(SA_M_Building[1], "res/Safe_Area/Building/Building_Doors.obj", glm::vec3(0, 0, 0) * additionalScale + offsetPos, glm::vec3(0, 0, 0), glm::vec3(1.0f) * additionalScale, SA_T_Building[1], 100.0f, 0.1f, 0.4f);
	loadModel(SA_M_Building[2], "res/Safe_Area/Building/Building_Wood.obj", glm::vec3(0, 0, 0) * additionalScale + offsetPos, glm::vec3(0, 0, 0), glm::vec3(1.0f) * additionalScale, SA_T_Building[2], 100.0f, 0.1f, 0.4f);
	loadModel(SA_M_Building[3], "res/Safe_Area/Building/Building_Roof.obj", glm::vec3(0, 0, 0) * additionalScale + offsetPos, glm::vec3(0, 0, 0), glm::vec3(1.0f) * additionalScale, SA_T_Building[3], 100.0f, 0.1f, 0.4f);
	loadModel(SA_M_Building[4], "res/Safe_Area/Building/Building_Wheel.obj", glm::vec3(0, 0, 0) * additionalScale + offsetPos, glm::vec3(0, 0, 0), glm::vec3(1.0f) * additionalScale, SA_T_Building[4], 100.0f, 0.1f, 0.4f);

	// Load AmmoBoxes models.
	// Front left
	loadModel(SA_M_AmmoBoxes[0], "res/Safe_Area/Weapon_Box/Weapon_Box.obj", glm::vec3(-3.715f, 1.281f, 11.901f) * additionalScale + offsetPos, glm::vec3(0, glm::radians(-133.379f), 0), glm::vec3(1.0f) * additionalScale, SA_T_AmmoBoxes[0], SA_TN_AmmoBoxes[0], 100.0f, 0.1f, 0.6f);
	loadModel(SA_M_AmmoBoxes[1], "res/Safe_Area/Weapon_Box/Weapon_Box_Granade.obj", glm::vec3(-3.715f, 1.281f, 11.901f) * additionalScale + offsetPos, glm::vec3(0, glm::radians(-133.379f), 0), glm::vec3(1.0f) * additionalScale, SA_T_AmmoBoxes[1], SA_TN_AmmoBoxes[1], 20.0f, 0.6f, 0.6f);
	loadModel(SA_M_AmmoBoxes[2], "res/Safe_Area/Weapon_Box/Weapon_Box_Granade2.obj", glm::vec3(-3.715f, 1.281f, 11.901f) * additionalScale + offsetPos, glm::vec3(0, glm::radians(-133.379f), 0), glm::vec3(1.0f) * additionalScale, SA_T_AmmoBoxes[2], SA_TN_AmmoBoxes[1], 20.0f, 0.6f, 0.6f);
	loadModel(SA_M_AmmoBoxes[3], "res/Safe_Area/Weapon_Box/Weapon_Box_Granade3.obj", glm::vec3(-3.715f, 1.281f, 11.901f) * additionalScale + offsetPos, glm::vec3(0, glm::radians(-133.379f), 0), glm::vec3(1.0f) * additionalScale, SA_T_AmmoBoxes[3], SA_TN_AmmoBoxes[1], 20.0f, 0.6f, 0.6f);
	loadModel(SA_M_AmmoBoxes[4], "res/Safe_Area/Weapon_Box/Weapon_Box_Granade4.obj", glm::vec3(-3.715f, 1.281f, 11.901f) * additionalScale + offsetPos, glm::vec3(0, glm::radians(-133.379f), 0), glm::vec3(1.0f) * additionalScale, SA_T_AmmoBoxes[4], SA_TN_AmmoBoxes[1], 20.0f, 0.6f, 0.6f);
	loadModel(SA_M_AmmoBoxes[5], "res/Safe_Area/Weapon_Box/Weapon_Box_Granade5.obj", glm::vec3(-3.715f, 1.281f, 11.901f) * additionalScale + offsetPos, glm::vec3(0, glm::radians(-133.379f), 0), glm::vec3(1.0f) * additionalScale, SA_T_AmmoBoxes[5], SA_TN_AmmoBoxes[1], 20.0f, 0.6f, 0.6f);
	loadModel(SA_M_AmmoBoxes[6], "res/Safe_Area/Weapon_Box/Weapon_Box_Granade6.obj", glm::vec3(-3.715f, 1.281f, 11.901f) * additionalScale + offsetPos, glm::vec3(0, glm::radians(-133.379f), 0), glm::vec3(1.0f) * additionalScale, SA_T_AmmoBoxes[6], SA_TN_AmmoBoxes[1], 20.0f, 0.6f, 0.6f);

	// Back right
	loadModel(SA_M_AmmoBoxes[7], SA_M_AmmoBoxes[0], glm::vec3(3.656f, 1.281f, -13.91f) * additionalScale + offsetPos, glm::vec3(0, glm::radians(79.083f), 0), glm::vec3(1.0f) * additionalScale, SA_T_AmmoBoxes[0], SA_TN_AmmoBoxes[0], 100.0f, 0.1f, 0.6f);
	loadModel(SA_M_AmmoBoxes[8], SA_M_AmmoBoxes[1], glm::vec3(3.656f, 1.281f, -13.91f) * additionalScale + offsetPos, glm::vec3(0, glm::radians(79.083f), 0), glm::vec3(1.0f) * additionalScale, SA_T_AmmoBoxes[1], SA_TN_AmmoBoxes[1], 20.0f, 0.6f, 0.6f);
	loadModel(SA_M_AmmoBoxes[9], SA_M_AmmoBoxes[2], glm::vec3(3.656f, 1.281f, -13.91f) * additionalScale + offsetPos, glm::vec3(0, glm::radians(79.083f), 0), glm::vec3(1.0f) * additionalScale, SA_T_AmmoBoxes[2], SA_TN_AmmoBoxes[1], 20.0f, 0.6f, 0.6f);
	loadModel(SA_M_AmmoBoxes[10], SA_M_AmmoBoxes[3], glm::vec3(3.656f, 1.281f, -13.91f) * additionalScale + offsetPos, glm::vec3(0, glm::radians(79.083f), 0), glm::vec3(1.0f) * additionalScale, SA_T_AmmoBoxes[3], SA_TN_AmmoBoxes[1], 20.0f, 0.6f, 0.6f);
	loadModel(SA_M_AmmoBoxes[11], SA_M_AmmoBoxes[4], glm::vec3(3.656f, 1.281f, -13.91f) * additionalScale + offsetPos, glm::vec3(0, glm::radians(79.083f), 0), glm::vec3(1.0f) * additionalScale, SA_T_AmmoBoxes[4], SA_TN_AmmoBoxes[1], 20.0f, 0.6f, 0.6f);
	loadModel(SA_M_AmmoBoxes[12], SA_M_AmmoBoxes[5], glm::vec3(3.656f, 1.281f, -13.91f) * additionalScale + offsetPos, glm::vec3(0, glm::radians(79.083f), 0), glm::vec3(1.0f) * additionalScale, SA_T_AmmoBoxes[5], SA_TN_AmmoBoxes[1], 20.0f, 0.6f, 0.6f);
	loadModel(SA_M_AmmoBoxes[13], SA_M_AmmoBoxes[6], glm::vec3(3.656f, 1.281f, -13.91f) * additionalScale + offsetPos, glm::vec3(0, glm::radians(79.083f), 0), glm::vec3(1.0f) * additionalScale, SA_T_AmmoBoxes[6], SA_TN_AmmoBoxes[1], 20.0f, 0.6f, 0.6f);

	// Middle room
	loadModel(SA_M_AmmoBoxes[14], SA_M_AmmoBoxes[0], glm::vec3(-5.456f, 1.281f, 1.797f) * additionalScale + offsetPos, glm::vec3(0, glm::radians(-164.795f), 0), glm::vec3(1.0f) * additionalScale, SA_T_AmmoBoxes[0], SA_TN_AmmoBoxes[0], 100.0f, 0.1f, 0.6f);
	loadModel(SA_M_AmmoBoxes[15], SA_M_AmmoBoxes[1], glm::vec3(-5.456f, 1.281f, 1.797f) * additionalScale + offsetPos, glm::vec3(0, glm::radians(-164.795f), 0), glm::vec3(1.0f) * additionalScale, SA_T_AmmoBoxes[1], SA_TN_AmmoBoxes[1], 20.0f, 0.6f, 0.6f);
	loadModel(SA_M_AmmoBoxes[16], SA_M_AmmoBoxes[2], glm::vec3(-5.456f, 1.281f, 1.797f) * additionalScale + offsetPos, glm::vec3(0, glm::radians(-164.795f), 0), glm::vec3(1.0f) * additionalScale, SA_T_AmmoBoxes[2], SA_TN_AmmoBoxes[1], 20.0f, 0.6f, 0.6f);
	loadModel(SA_M_AmmoBoxes[17], SA_M_AmmoBoxes[3], glm::vec3(-5.456f, 1.281f, 1.797f) * additionalScale + offsetPos, glm::vec3(0, glm::radians(-164.795f), 0), glm::vec3(1.0f) * additionalScale, SA_T_AmmoBoxes[3], SA_TN_AmmoBoxes[1], 20.0f, 0.6f, 0.6f);
	loadModel(SA_M_AmmoBoxes[18], SA_M_AmmoBoxes[4], glm::vec3(-5.456f, 1.281f, 1.797f) * additionalScale + offsetPos, glm::vec3(0, glm::radians(-164.795f), 0), glm::vec3(1.0f) * additionalScale, SA_T_AmmoBoxes[4], SA_TN_AmmoBoxes[1], 20.0f, 0.6f, 0.6f);
	loadModel(SA_M_AmmoBoxes[19], SA_M_AmmoBoxes[5], glm::vec3(-5.456f, 1.281f, 1.797f) * additionalScale + offsetPos, glm::vec3(0, glm::radians(-164.795f), 0), glm::vec3(1.0f) * additionalScale, SA_T_AmmoBoxes[5], SA_TN_AmmoBoxes[1], 20.0f, 0.6f, 0.6f);
	loadModel(SA_M_AmmoBoxes[20], SA_M_AmmoBoxes[6], glm::vec3(-5.456f, 1.281f, 1.797f) * additionalScale + offsetPos, glm::vec3(0, glm::radians(-164.795f), 0), glm::vec3(1.0f) * additionalScale, SA_T_AmmoBoxes[6], SA_TN_AmmoBoxes[1], 20.0f, 0.6f, 0.6f);

	// Load Barrels models.
	loadModel(SA_M_Barrels[0], "res/Safe_Area/Barrel/Barrel.obj", glm::vec3(-7.725f, -0.623f, -9.331f) * additionalScale + offsetPos, glm::vec3(0, glm::radians(-22.889f), 0), glm::vec3(1.0f) * additionalScale, SA_T_Barrels[1], 35.0f, 0.5f, 0.1f); // Red
	loadModel(SA_M_Barrels[1], SA_M_Barrels[0], glm::vec3(-4.665f, -0.227f, -14.639f) * additionalScale + offsetPos, glm::vec3(glm::radians(14.743f), glm::radians(-91.479f), glm::radians(-0.389f)), glm::vec3(1.0f) * additionalScale, SA_T_Barrels[0], 35.0f, 0.5f, 0.1f); // Green
	loadModel(SA_M_Barrels[2], SA_M_Barrels[0], glm::vec3(-7.725f, 2.815f, -13.345f) * additionalScale + offsetPos, glm::vec3(glm::radians(1.203f), 0, 0), glm::vec3(1.0f) * additionalScale, SA_T_Barrels[2], 30.0f, 0.5f, 0.1f);
	loadModel(SA_M_Barrels[3], SA_M_Barrels[0], glm::vec3(-7.725f, -0.623f, -14.639f) * additionalScale + offsetPos, glm::vec3(0.0f), glm::vec3(1.0f) * additionalScale, SA_T_Barrels[2], 30.0f, 0.5f, 0.1f);
	loadModel(SA_M_Barrels[4], SA_M_Barrels[0], glm::vec3(-7.725f, -0.623f, -11.614f) * additionalScale + offsetPos, glm::vec3(glm::radians(-12.803f), glm::radians(18.968f), 0), glm::vec3(1.0f) * additionalScale, SA_T_Barrels[2], 30.0f, 0.5f, 0.1f);
	loadModel(SA_M_Barrels[5], SA_M_Barrels[0], glm::vec3(-7.725f, 0.204f, 14.789f) * additionalScale + offsetPos, glm::vec3(glm::radians(-66.793f), glm::radians(0.618f), glm::radians(19.981f)), glm::vec3(1.0f) * additionalScale, SA_T_Barrels[1], 35.0f, 0.5f, 0.1f);

	// Load Barriers models.
	loadModel(SA_M_Barriers[0], "res/Safe_Area/ConcreteBarrier/ConcreteBarrier.obj", glm::vec3(5.746f, 0.734f, 14.737f) * additionalScale + offsetPos, glm::vec3(0.0f), glm::vec3(1.0f) * additionalScale, SA_T_Barriers, SA_TN_Barriers, 100.0f, 0.0f, 0.1f);
	loadModel(SA_M_Barriers[1], SA_M_Barriers[0], glm::vec3(2.4800f, 0.734f, 14.737f) * additionalScale + offsetPos, glm::vec3(0.0f), glm::vec3(1.0f) * additionalScale, SA_T_Barriers, SA_TN_Barriers, 100.0f, 0.1f, 0.1f);
	loadModel(SA_M_Barriers[2], SA_M_Barriers[0], glm::vec3(-0.987f, 0.734f, 14.737f) * additionalScale + offsetPos, glm::vec3(0.0f), glm::vec3(1.0f) * additionalScale, SA_T_Barriers, SA_TN_Barriers, 100.0f, 0.1f, 0.1f);
	loadModel(SA_M_Barriers[3], SA_M_Barriers[0], glm::vec3(-4.258f, 0.734f, 14.737f) * additionalScale + offsetPos, glm::vec3(0.0f), glm::vec3(1.0f) * additionalScale, SA_T_Barriers, SA_TN_Barriers, 100.0f, 0.1f, 0.1f);

	// Load Crates models.
	loadModel(SA_M_Crate[0], "res/Safe_Area/WoodBox/WoodBox.obj", glm::vec3(9.607f, 3.429f, -9.033f) * additionalScale + offsetPos, glm::vec3(0, glm::radians(-18.961f), 0), glm::vec3(1.0f) * additionalScale, SA_T_Crate, SA_TN_Crate, 100.0f, 1.0f, 0.1f);
	loadModel(SA_M_Crate[1], SA_M_Crate[0], glm::vec3(9.031f, 0.783f, -10.471f) * additionalScale + offsetPos, glm::vec3(0), glm::vec3(1.0f) * additionalScale, SA_T_Crate, SA_TN_Crate, 100.0f, 1.0f, 0.1f);
	loadModel(SA_M_Crate[2], SA_M_Crate[0], glm::vec3(9.472f, 0.975f, 4.032f) * additionalScale + offsetPos, glm::vec3(glm::radians(-0.109f), glm::radians(0.046f), glm::radians(15.49f)), glm::vec3(1.0f) * additionalScale, SA_T_Crate, SA_TN_Crate, 100.0f, 1.0f, 0.1f);
	loadModel(SA_M_Crate[3], SA_M_Crate[0], glm::vec3(9.646f, 3.446f, -5.908f) * additionalScale + offsetPos, glm::vec3(0), glm::vec3(1.0f) * additionalScale, SA_T_Crate, SA_TN_Crate, 100.0f, 1.0f, 0.1f);
	loadModel(SA_M_Crate[4], SA_M_Crate[0], glm::vec3(9.646f, 3.759f, 3.778f) * additionalScale + offsetPos, glm::vec3(0, 0, glm::radians(5.235f)), glm::vec3(1.0f) * additionalScale, SA_T_Crate, SA_TN_Crate, 100.0f, 1.0f, 0.1f);
	loadModel(SA_M_Crate[5], SA_M_Crate[0], glm::vec3(9.031f, 0.783f, -6.575f) * additionalScale + offsetPos, glm::vec3(0), glm::vec3(1.0f) * additionalScale, SA_T_Crate, SA_TN_Crate, 100.0f, 1.0f, 0.1f);

	// Load Crate2 model.
	loadModel(SA_M_Crate2, "res/Safe_Area/WoodBox2/WoodBox2.obj", glm::vec3(10.217f, 5.708f, -7.628f) * additionalScale + offsetPos, glm::vec3(0, glm::radians(6.99f), 0), glm::vec3(1.0f) * additionalScale, SA_T_Crate2, 100.0f, 1.0f, 0.1f);

	// Load Pallets models.
	loadModel(SA_M_Pallets[0], "res/Safe_Area/Pallet/Pallet.obj", glm::vec3(9.958f, 1.409f, -12.077f) * additionalScale + offsetPos, glm::vec3(glm::radians(90.0f), glm::radians(29.358f), glm::radians(81.467f)), glm::vec3(1.0f) * additionalScale, SA_T_Pallets, SA_TN_Pallets, 100.0f, 0.1f, 0.1f);
	loadModel(SA_M_Pallets[1], SA_M_Pallets[0], glm::vec3(9.362f, 1.409f, -14.006f) * additionalScale + offsetPos, glm::vec3(glm::radians(90.0f), glm::radians(52.264f), glm::radians(81.467f)), glm::vec3(1.0f) * additionalScale, SA_T_Pallets, SA_TN_Pallets, 100.0f, 0.1f, 0.1f);

	// Load SandBag model.
	loadModel(SA_M_SandBag[0], "res/Safe_Area/SandBags/SandBags.obj", glm::vec3(8.933f, -0.623, 10.087f) * additionalScale + offsetPos, glm::vec3(0, glm::radians(66.783f), 0), glm::vec3(1.0f) * additionalScale, SA_T_SandBag, SA_TN_SandBag, 100.0f, 0.1f, 0.1f);
	loadModel(SA_M_SandBag[1], SA_M_SandBag[0], glm::vec3(-8.436f, -0.623, 4.834f) * additionalScale + offsetPos, glm::vec3(0, glm::radians(89.368f), 0), glm::vec3(1.0f) * additionalScale, SA_T_SandBag, SA_TN_SandBag, 100.0f, 0.1f, 0.1f);
}

// Load Forrest Map Models
void LoadModels_ForrestMap()
{
	glm::vec3 multiPos, multiRot;

	// == AMBULANCE ==
	{
		float shine = 100.0f,
			reflect = 0.1f,
			ambient = 0.3f;

		float rshine = 100.0f,
			rreflect = 0.7f,
			rambient = 0.3f;

		multiPos = glm::vec3(-4.67f, 0.0f, 199.825f);
		multiRot = glm::vec3(0.0f, glm::radians(32.168f), 0.0f);
		loadModel(FM_M_AMBULANCE[0], "res/Forrest_Area/Ambulance/Frame.obj", multiPos, multiRot, glm::vec3(1.0f), FM_T_AMBULANCE[0], FM_TN_AMBULANCE[0], shine, reflect, ambient);
		loadModel(FM_M_AMBULANCE[1], "res/Forrest_Area/Ambulance/Frame2.obj", multiPos, multiRot, glm::vec3(1.0f), FM_T_AMBULANCE[1], FM_TN_AMBULANCE[1], shine, reflect, ambient);
		loadModel(FM_M_AMBULANCE[2], "res/Forrest_Area/Ambulance/Logo's.obj", multiPos, multiRot, glm::vec3(1.0f), FM_T_AMBULANCE[2], FM_TN_AMBULANCE[2], shine, reflect, ambient);
		loadModel(FM_M_AMBULANCE[3], "res/Forrest_Area/Ambulance/Wheels.obj", multiPos, multiRot, glm::vec3(1.0f), FM_T_AMBULANCE[3], FM_TN_AMBULANCE[3], shine, reflect, ambient);
		loadModel(FM_M_AMBULANCE[4], "res/Forrest_Area/Ambulance/Windows.obj", multiPos, multiRot, glm::vec3(1.0f), FM_T_AMBULANCE[4], FM_TN_AMBULANCE[4], rshine, rreflect, rambient);

		multiPos = glm::vec3(-136.118f, 0.0f, -299.611f);
		multiRot = glm::vec3(0.0f, glm::radians(35.111f), 0.0f);
		loadModel(FM_M_AMBULANCE[5], FM_M_AMBULANCE[0], multiPos, multiRot, glm::vec3(1.0f), FM_T_AMBULANCE[0], FM_TN_AMBULANCE[0], shine, reflect, ambient);
		loadModel(FM_M_AMBULANCE[6], FM_M_AMBULANCE[1], multiPos, multiRot, glm::vec3(1.0f), FM_T_AMBULANCE[1], FM_TN_AMBULANCE[1], shine, reflect, ambient);
		loadModel(FM_M_AMBULANCE[7], FM_M_AMBULANCE[2], multiPos, multiRot, glm::vec3(1.0f), FM_T_AMBULANCE[2], FM_TN_AMBULANCE[2], shine, reflect, ambient);
		loadModel(FM_M_AMBULANCE[8], FM_M_AMBULANCE[3], multiPos, multiRot, glm::vec3(1.0f), FM_T_AMBULANCE[3], FM_TN_AMBULANCE[3], shine, reflect, ambient);
		loadModel(FM_M_AMBULANCE[9], FM_M_AMBULANCE[4], multiPos, multiRot, glm::vec3(1.0f), FM_T_AMBULANCE[4], FM_TN_AMBULANCE[4], rshine, rreflect, rambient);
	}

	// == ARMY_COMMAND_TENT ==
	{
		float shine = 100.0f,
			reflect = 0.1f,
			ambient = 0.25f;

		multiPos = glm::vec3(-244.072f, -0.061f, -447.298f);
		multiRot = glm::vec3(0.0f);
		loadModel(FM_M_COMMAND_TENT, "res/Forrest_Area/Army_Command_Tent/CommandTents.obj", multiPos, multiRot, glm::vec3(1.0f), FM_T_COMMAND_TENT, FM_TN_COMMAND_TENT, shine, reflect, ambient);
	}

	// == ARMY_TENT ==
	{
		float shine = 100.0f,
			reflect = 0.1f,
			ambient = 0.2f;

		multiPos = glm::vec3(-323.534f, 20.376f, -75.994f);
		multiRot = glm::vec3(0.0f, glm::radians(90.0f), 0.0f);
		loadModel(FM_M_ARMY_TENT[0], "res/Forrest_Area/Army_Tent/Inside.obj", multiPos, multiRot, glm::vec3(1.0f), FM_T_ARMY_TENT[0], FM_TN_ARMY_TENT[0], shine, reflect, ambient);
		loadModel(FM_M_ARMY_TENT[1], "res/Forrest_Area/Army_Tent/Outside.obj", multiPos, multiRot, glm::vec3(1.0f), FM_T_ARMY_TENT[1], FM_TN_ARMY_TENT[1], shine, reflect, ambient);
		loadModel(FM_M_ARMY_TENT[2], "res/Forrest_Area/Army_Tent/Windows.obj", multiPos, multiRot, glm::vec3(1.0f), FM_T_ARMY_TENT[2], shine, reflect, ambient);

		//loadArmyTent(0, glm::vec3(-323.534f, 20.376f, -75.994f), glm::vec3(0.0f, glm::radians(90.0f), 0.0f));
		loadArmyTent(1, glm::vec3(-323.534f, 20.376f, -0.8f), glm::vec3(0.0f, glm::radians(90.0f), 0.0f));
		loadArmyTent(2, glm::vec3(-310.218f, 20.376f, 77.699f), glm::vec3(0.0f, glm::radians(90.0f), 0.0f));
		loadArmyTent(3, glm::vec3(-262.446f, 20.376f, -51.462f), glm::vec3(0.0f, glm::radians(-90.0f), 0.0f));
		loadArmyTent(4, glm::vec3(-273.387f, 20.376f, 18.574f), glm::vec3(0.0f, glm::radians(-90.0f), 0.0f));
		loadArmyTent(5, glm::vec3(-262.446f, 20.376f, 88.61f), glm::vec3(0.0f, glm::radians(-90.0f), 0.0f));
	}

	// == ARMY_TRUCK ==
	{
		float shine = 100.0f,
			reflect = 0.1f,
			ambient = 0.2f;

		float rshine = 30.0f,
			rreflect = 0.25f,
			rambient = 0.2f;

		float rrshine = 80.0f,
			rrreflect = 0.5f,
			rrambient = 0.2f;

		multiPos = glm::vec3(-221.392f, 0.0f, -158.281f);
		multiRot = glm::vec3(0.0f, glm::radians(-46.325f), 0.0f);
		loadModel(FM_M_ARMY_TRUCK[0], "res/Forrest_Area/Army_Truck/Box.obj", multiPos, multiRot, glm::vec3(1.0f), FM_T_ARMY_TRUCK[0], shine, reflect, ambient);
		loadModel(FM_M_ARMY_TRUCK[1], "res/Forrest_Area/Army_Truck/Cabin.obj", multiPos, multiRot, glm::vec3(1.0f), FM_T_ARMY_TRUCK[1], rshine, rreflect, rambient);
		loadModel(FM_M_ARMY_TRUCK[2], "res/Forrest_Area/Army_Truck/Frame.obj", multiPos, multiRot, glm::vec3(1.0f), FM_T_ARMY_TRUCK[2], rshine, rreflect, rambient);
		loadModel(FM_M_ARMY_TRUCK[3], "res/Forrest_Area/Army_Truck/Lights.obj", multiPos, multiRot, glm::vec3(1.0f), FM_T_ARMY_TRUCK[3], rrshine, rrreflect, rrambient);
		loadModel(FM_M_ARMY_TRUCK[4], "res/Forrest_Area/Army_Truck/Wheels.obj", multiPos, multiRot, glm::vec3(1.0f), FM_T_ARMY_TRUCK[4], shine, reflect, ambient);

		//loadArmyTruck(0, glm::vec3(-221.392f, 0.0f, -158.281f), glm::vec3(0.0f, glm::radians(-46.325f), 0.0f));
		loadArmyTruck(1, glm::vec3(-221.392f, 0.0f, -190.219f), glm::vec3(0.0f, glm::radians(-74.909f), 0.0f));
		loadArmyTruck(2, glm::vec3(-221.392f, 0.0f, -218.456f), glm::vec3(0.0f, glm::radians(-74.909f), 0.0f));
		loadArmyTruck(3, glm::vec3(-221.392f, 0.0f, -244.777f), glm::vec3(0.0f, glm::radians(106.488f), 0.0f));
		loadArmyTruck(4, glm::vec3(-280.253f, 0.0f, -303.281f), glm::vec3(0.0f, glm::radians(-81.785f), 0.0f));
		loadArmyTruck(5, glm::vec3(-429.82f, 0.0f, 163.502f), glm::vec3(0.0f, glm::radians(18.667f), 0.0f));
	}

	// == BARRIER ==
	{
		float shine = 100.0f,
			reflect = 0.1f,
			ambient = 0.15f;

		loadModel(FM_M_BARRIER[0], "res/Forrest_Area/Barrier/barrier.obj", glm::vec3(-143.101f, 1.833f, -496.647f), glm::vec3(0.0f, glm::radians(-171.108f), 0.0f), glm::vec3(1.0f), FM_T_BARRIER, FM_TN_BARRIER, shine, reflect, ambient);
		loadModel(FM_M_BARRIER[1], FM_M_BARRIER[0], glm::vec3(-138.153f, 1.833f, -497.33f), glm::vec3(0.0f, glm::radians(-2.893f), 0.0f), glm::vec3(1.0f), FM_T_BARRIER, FM_TN_BARRIER, shine, reflect, ambient);
		loadModel(FM_M_BARRIER[2], FM_M_BARRIER[0], glm::vec3(-133.234f, 1.833f, -497.587f), glm::vec3(0.0f, glm::radians(-171.369f), 0.0f), glm::vec3(1.0f), FM_T_BARRIER, FM_TN_BARRIER, shine, reflect, ambient);
		loadModel(FM_M_BARRIER[3], FM_M_BARRIER[0], glm::vec3(-128.054f, 1.833f, -497.537f), glm::vec3(0.0f, glm::radians(169.062f), 0.0f), glm::vec3(1.0f), FM_T_BARRIER, FM_TN_BARRIER, shine, reflect, ambient);
		loadModel(FM_M_BARRIER[4], FM_M_BARRIER[0], glm::vec3(-123.147f, 1.833f, -496.198f), glm::vec3(0.0f, glm::radians(-20.006f), 0.0f), glm::vec3(1.0f), FM_T_BARRIER, FM_TN_BARRIER, shine, reflect, ambient);
		loadModel(FM_M_BARRIER[5], FM_M_BARRIER[0], glm::vec3(-117.907f, 1.833f, -495.336f), glm::vec3(0.0f), glm::vec3(1.0f), FM_T_BARRIER, FM_TN_BARRIER, shine, reflect, ambient);
		loadModel(FM_M_BARRIER[6], FM_M_BARRIER[0], glm::vec3(-112.459f, 1.833f, -495.602f), glm::vec3(0.0f, glm::radians(-172.327f), 0.0f), glm::vec3(1.0f), FM_T_BARRIER, FM_TN_BARRIER, shine, reflect, ambient);

		loadModel(FM_M_BARRIER[7], FM_M_BARRIER[0], glm::vec3(-141.758f, 1.833f, 153.703f), glm::vec3(0.0f, glm::radians(112.686f), 0.0f), glm::vec3(1.0f), FM_T_BARRIER, FM_TN_BARRIER, shine, reflect, ambient);
		loadModel(FM_M_BARRIER[8], FM_M_BARRIER[0], glm::vec3(-138.351f, 1.833f, 159.479f), glm::vec3(0.0f, glm::radians(-56.24f), 0.0f), glm::vec3(1.0f), FM_T_BARRIER, FM_TN_BARRIER, shine, reflect, ambient);
		loadModel(FM_M_BARRIER[9], FM_M_BARRIER[0], glm::vec3(-135.995f, 1.833f, 165.182f), glm::vec3(0.0f, glm::radians(-62.378f), 0.0f), glm::vec3(1.0f), FM_T_BARRIER, FM_TN_BARRIER, shine, reflect, ambient);
		loadModel(FM_M_BARRIER[10], FM_M_BARRIER[0], glm::vec3(-132.799f, 1.833f, 169.891f), glm::vec3(0.0f, glm::radians(135.924f), 0.0f), glm::vec3(1.0f), FM_T_BARRIER, FM_TN_BARRIER, shine, reflect, ambient);
		loadModel(FM_M_BARRIER[11], FM_M_BARRIER[0], glm::vec3(-127.808f, 1.833f, 173.611f), glm::vec3(0.0f, glm::radians(-30.061f), 0.0f), glm::vec3(1.0f), FM_T_BARRIER, FM_TN_BARRIER, shine, reflect, ambient);
		loadModel(FM_M_BARRIER[12], FM_M_BARRIER[0], glm::vec3(-122.116f, 1.833f, 176.671f), glm::vec3(0.0f, glm::radians(-21.642f), 0.0f), glm::vec3(1.0f), FM_T_BARRIER, FM_TN_BARRIER, shine, reflect, ambient);

		loadModel(FM_M_BARRIER[13], FM_M_BARRIER[0], glm::vec3(370.609f, 1.833f, -278.094f), glm::vec3(0.0f, glm::radians(-81.108f), 0.0f), glm::vec3(1.0f), FM_T_BARRIER, FM_TN_BARRIER, shine, reflect, ambient);
		loadModel(FM_M_BARRIER[14], FM_M_BARRIER[0], glm::vec3(369.925f, 1.833f, -283.042f), glm::vec3(0.0f, glm::radians(87.107f), 0.0f), glm::vec3(1.0f), FM_T_BARRIER, FM_TN_BARRIER, shine, reflect, ambient);
		loadModel(FM_M_BARRIER[15], FM_M_BARRIER[0], glm::vec3(369.668f, 1.833f, -287.961f), glm::vec3(0.0f, glm::radians(-81.369f), 0.0f), glm::vec3(1.0f), FM_T_BARRIER, FM_TN_BARRIER, shine, reflect, ambient);
		loadModel(FM_M_BARRIER[16], FM_M_BARRIER[0], glm::vec3(369.718f, 1.833f, -293.141f), glm::vec3(0.0f, glm::radians(-100.938f), 0.0f), glm::vec3(1.0f), FM_T_BARRIER, FM_TN_BARRIER, shine, reflect, ambient);
		loadModel(FM_M_BARRIER[17], FM_M_BARRIER[0], glm::vec3(371.057f, 1.833f, -298.048f), glm::vec3(0.0f, glm::radians(69.994f), 0.0f), glm::vec3(1.0f), FM_T_BARRIER, FM_TN_BARRIER, shine, reflect, ambient);
		loadModel(FM_M_BARRIER[18], FM_M_BARRIER[0], glm::vec3(371.919f, 1.833f, -303.288f), glm::vec3(0.0f, glm::radians(90.0f), 0.0f), glm::vec3(1.0f), FM_T_BARRIER, FM_TN_BARRIER, shine, reflect, ambient);
		loadModel(FM_M_BARRIER[19], FM_M_BARRIER[0], glm::vec3(371.653f, 1.833f, -308.736f), glm::vec3(0.0f, glm::radians(-82.327f), 0.0f), glm::vec3(1.0f), FM_T_BARRIER, FM_TN_BARRIER, shine, reflect, ambient);

		loadModel(FM_M_BARRIER[20], FM_M_BARRIER[0], glm::vec3(369.925f, 1.833f, 176.461f), glm::vec3(0.0f, glm::radians(87.107f), 0.0f), glm::vec3(1.0f), FM_T_BARRIER, FM_TN_BARRIER, shine, reflect, ambient);
		loadModel(FM_M_BARRIER[21], FM_M_BARRIER[0], glm::vec3(369.668f, 1.833f, 171.542f), glm::vec3(0.0f, glm::radians(-81.369f), 0.0f), glm::vec3(1.0f), FM_T_BARRIER, FM_TN_BARRIER, shine, reflect, ambient);
		loadModel(FM_M_BARRIER[22], FM_M_BARRIER[0], glm::vec3(369.718f, 1.833f, 166.362f), glm::vec3(0.0f, glm::radians(-100.938f), 0.0f), glm::vec3(1.0f), FM_T_BARRIER, FM_TN_BARRIER, shine, reflect, ambient);
		loadModel(FM_M_BARRIER[23], FM_M_BARRIER[0], glm::vec3(371.057f, 1.833f, 161.456f), glm::vec3(0.0f, glm::radians(69.994f), 0.0f), glm::vec3(1.0f), FM_T_BARRIER, FM_TN_BARRIER, shine, reflect, ambient);
		loadModel(FM_M_BARRIER[24], FM_M_BARRIER[0], glm::vec3(371.919f, 1.833f, 156.215f), glm::vec3(0.0f, glm::radians(90.0f), 0.0f), glm::vec3(1.0f), FM_T_BARRIER, FM_TN_BARRIER, shine, reflect, ambient);

	}

	// == CONTAINER ==
	{
		float shine = 35.0f,
			reflect = 0.5f,
			ambient = 0.25f;

		loadModel(FM_M_CONTAINER[0], "res/Forrest_Area/Container/Container.obj", glm::vec3(-31.262f, 0.0f, -161.87f), glm::vec3(0.0f, glm::radians(-56.409f), 0.0f), glm::vec3(1.0f), FM_T_CONTAINER, FM_TN_CONTAINER, shine, reflect, ambient);
		loadModel(FM_M_CONTAINER[1], FM_M_CONTAINER[0], glm::vec3(-31.262f, 10.397f, -161.87f), glm::vec3(0.0f, glm::radians(109.951f), 0.0f), glm::vec3(1.0f), FM_T_CONTAINER, FM_TN_CONTAINER, shine, reflect, ambient);
		loadModel(FM_M_CONTAINER[2], FM_M_CONTAINER[0], glm::vec3(-50.499f, 0.0f, -150.363f), glm::vec3(0.0f, glm::radians(-146.494f), 0.0f), glm::vec3(1.0f), FM_T_CONTAINER, FM_TN_CONTAINER, shine, reflect, ambient);
		loadModel(FM_M_CONTAINER[3], FM_M_CONTAINER[0], glm::vec3(-50.499f, 10.397f, -150.363f), glm::vec3(0.0f, glm::radians(67.108f), 0.0f), glm::vec3(1.0f), FM_T_CONTAINER, FM_TN_CONTAINER, shine, reflect, ambient);
		loadModel(FM_M_CONTAINER[4], FM_M_CONTAINER[0], glm::vec3(-26.27f, 0.0f, -127.293f), glm::vec3(0.0f, glm::radians(166.165f), 0.0f), glm::vec3(1.0f), FM_T_CONTAINER, FM_TN_CONTAINER, shine, reflect, ambient);
		loadModel(FM_M_CONTAINER[5], FM_M_CONTAINER[0], glm::vec3(-26.27f, 6.034f, -144.865f), glm::vec3(glm::radians(2.238f), glm::radians(71.634f), glm::radians(30.544f)), glm::vec3(1.0f), FM_T_CONTAINER, FM_TN_CONTAINER, shine, reflect, ambient);

		loadModel(FM_M_CONTAINER[6], FM_M_CONTAINER[0], glm::vec3(-334.474f, 0.0f, -414.971f), glm::vec3(0.0f, glm::radians(-146.494f), 0.0f), glm::vec3(1.0f), FM_T_CONTAINER, FM_TN_CONTAINER, shine, reflect, ambient);
		loadModel(FM_M_CONTAINER[7], FM_M_CONTAINER[0], glm::vec3(-342.599f, 0.0f, -437.529f), glm::vec3(0.0f, glm::radians(144.614f), 0.0f), glm::vec3(1.0f), FM_T_CONTAINER, FM_TN_CONTAINER, shine, reflect, ambient);

		loadModel(FM_M_CONTAINER[8], FM_M_CONTAINER[0], glm::vec3(5.661f, 0.0f, -161.87f), glm::vec3(0.0f, glm::radians(-56.409f), 0.0f), glm::vec3(1.0f), FM_T_CONTAINER, FM_TN_CONTAINER, shine, reflect, ambient);
		loadModel(FM_M_CONTAINER[9], FM_M_CONTAINER[0], glm::vec3(25.421f, 0.0f, -198.133f), glm::vec3(0.0f, glm::radians(30.923f), 0.0f), glm::vec3(1.0f), FM_T_CONTAINER, FM_TN_CONTAINER, shine, reflect, ambient);
		loadModel(FM_M_CONTAINER[10], FM_M_CONTAINER[0], glm::vec3(5.661f, 0.0f, -181.868f), glm::vec3(0.0f, glm::radians(35.986f), 0.0f), glm::vec3(1.0f), FM_T_CONTAINER, FM_TN_CONTAINER, shine, reflect, ambient);
		loadModel(FM_M_CONTAINER[11], FM_M_CONTAINER[0], glm::vec3(25.703f, 5.595f, -181.868f), glm::vec3(glm::radians(-0.822f), glm::radians(-57.57f), glm::radians(-27.23f)), glm::vec3(1.0f), FM_T_CONTAINER, FM_TN_CONTAINER, shine, reflect, ambient);

	}

	// == FENCE ==
	{
		float shine = 100.0f,
			reflect = 0.1f,
			ambient = 0.1f;

		multiPos = glm::vec3(-403.134f, 0.0f, -448.357f);
		multiRot = glm::vec3(0.0f, glm::radians(32.405f), 0.0f);

		loadModel(FM_M_FENCE[0], "res/Forrest_Area/Fences/Normal_Fence/Fence_Logends.obj", multiPos, multiRot, glm::vec3(1.0f), FM_T_FENCE[0], FM_TN_FENCE[0], shine, reflect, ambient);
		loadModel(FM_M_FENCE[1], "res/Forrest_Area/Fences/Normal_Fence/Fence_Logs.obj", multiPos, multiRot, glm::vec3(1.0f), FM_T_FENCE[1], FM_TN_FENCE[1], shine, reflect, ambient);
		loadModel(FM_M_FENCE[2], "res/Forrest_Area/Fences/Normal_Fence/Fence_Planks.obj", multiPos, multiRot, glm::vec3(1.0f), FM_T_FENCE[2], FM_TN_FENCE[2], shine, reflect, ambient);

		//loadFence(0, glm::vec3(-403.134f, 0.0f, -448.357f), glm::vec3(0.0f, glm::radians(32.405f), 0.0f));
		loadFence(1, glm::vec3(-385.056f, 0.0f, -452.927f), glm::vec3(0.0f, glm::radians(0.000f), 0.0f));
		loadFence(2, glm::vec3(-364.48f, 0.0f, -452.927f), glm::vec3(0.0f, glm::radians(0.000f), 0.0f));
		loadFence(3, glm::vec3(-344.05f, 0.0f, -452.927f), glm::vec3(0.0f, glm::radians(0.000f), 0.0f));
		loadFence(4, glm::vec3(-332.509f, 0.0f, -464.564f), glm::vec3(0.0f, glm::radians(90.000f), 0.0f));
		loadFence(5, glm::vec3(-332.509f, 0.0f, -485.124f), glm::vec3(0.0f, glm::radians(90.000f), 0.0f));
		loadFence(6, glm::vec3(196.556f, 0.0f, 410.01f), glm::vec3(0.0f, glm::radians(30.702f), 0.0f));
		loadFence(7, glm::vec3(204.024f, 0.0f, 394.745f), glm::vec3(0.0f, glm::radians(90.000f), 0.0f));
		loadFence(8, glm::vec3(213.265f, 0.0f, 383.673f), glm::vec3(0.0f, glm::radians(0.000f), 0.0f));
		loadFence(9, glm::vec3(233.577f, 0.0f, 383.673f), glm::vec3(0.0f, glm::radians(0.000f), 0.0f));
		loadFence(10, glm::vec3(253.888f, 0.0f, 383.673f), glm::vec3(0.0f, glm::radians(0.000f), 0.0f));
		loadFence(11, glm::vec3(264.193f, 0.0f, 371.992f), glm::vec3(0.0f, glm::radians(90.000f), 0.0f));
		loadFence(12, glm::vec3(264.193f, 0.0f, 352.079f), glm::vec3(0.0f, glm::radians(90.000f), 0.0f));
		loadFence(13, glm::vec3(264.193f, 0.0f, 332.166f), glm::vec3(0.0f, glm::radians(90.000f), 0.0f));
		loadFence(14, glm::vec3(264.193f, 0.0f, 312.253f), glm::vec3(0.0f, glm::radians(90.000f), 0.0f));
		loadFence(15, glm::vec3(264.193f, 0.0f, 292.34f), glm::vec3(0.0f, glm::radians(90.000f), 0.0f));
		loadFence(16, glm::vec3(274.613f, 0.0f, 281.701f), glm::vec3(0.0f, glm::radians(0.000f), 0.0f));
		loadFence(17, glm::vec3(295.69f, 0.0f, 281.701f), glm::vec3(0.0f, glm::radians(0.000f), 0.0f));
		loadFence(18, glm::vec3(316.766f, 0.0f, 281.701f), glm::vec3(0.0f, glm::radians(0.000f), 0.0f));
		loadFence(19, glm::vec3(337.843f, 0.0f, 281.701f), glm::vec3(0.0f, glm::radians(0.000f), 0.0f));
		loadFence(20, glm::vec3(358.92f, 0.0f, 281.701f), glm::vec3(0.0f, glm::radians(0.000f), 0.0f));
		loadFence(21, glm::vec3(370.923f, 0.0f, 269.961f), glm::vec3(0.0f, glm::radians(90.000f), 0.0f));
		loadFence(22, glm::vec3(370.923f, 0.0f, 249.659f), glm::vec3(0.0f, glm::radians(90.000f), 0.0f));
		loadFence(23, glm::vec3(370.923f, 0.0f, 229.931f), glm::vec3(0.0f, glm::radians(90.000f), 0.0f));
		loadFence(24, glm::vec3(370.923f, 0.0f, 210.136f), glm::vec3(0.0f, glm::radians(90.000f), 0.0f));
		loadFence(25, glm::vec3(370.923f, 0.0f, 190.342f), glm::vec3(0.0f, glm::radians(90.000f), 0.0f));
		loadFence(26, glm::vec3(370.923f, 0.0f, 143.255f), glm::vec3(0.0f, glm::radians(90.000f), 0.0f));
		loadFence(27, glm::vec3(370.923f, 0.0f, 123.684f), glm::vec3(0.0f, glm::radians(90.000f), 0.0f));
		loadFence(28, glm::vec3(370.923f, 0.0f, 103.388f), glm::vec3(0.0f, glm::radians(90.000f), 0.0f));
		loadFence(29, glm::vec3(370.923f, 0.0f, 83.0925f), glm::vec3(0.0f, glm::radians(90.000f), 0.0f));
		loadFence(30, glm::vec3(370.923f, 0.0f, 62.7966f), glm::vec3(0.0f, glm::radians(90.000f), 0.0f));
		loadFence(31, glm::vec3(370.923f, 0.0f, 42.5007f), glm::vec3(0.0f, glm::radians(90.000f), 0.0f));
		loadFence(32, glm::vec3(370.923f, 0.0f, 22.2048f), glm::vec3(0.0f, glm::radians(90.000f), 0.0f));
		loadFence(33, glm::vec3(370.923f, 0.0f, 1.90894f), glm::vec3(0.0f, glm::radians(90.000f), 0.0f));
		loadFence(34, glm::vec3(370.923f, 0.0f, -18.3869f), glm::vec3(0.0f, glm::radians(90.000f), 0.0f));
		loadFence(35, glm::vec3(370.923f, 0.0f, -38.6828f), glm::vec3(0.0f, glm::radians(90.000f), 0.0f));
		loadFence(36, glm::vec3(370.923f, 0.0f, -58.9787f), glm::vec3(0.0f, glm::radians(90.000f), 0.0f));
		loadFence(37, glm::vec3(370.923f, 0.0f, -79.2745f), glm::vec3(0.0f, glm::radians(90.000f), 0.0f));
		loadFence(38, glm::vec3(370.923f, 0.0f, -99.5704f), glm::vec3(0.0f, glm::radians(90.000f), 0.0f));
		loadFence(39, glm::vec3(370.923f, 0.0f, -119.866f), glm::vec3(0.0f, glm::radians(90.000f), 0.0f));
		loadFence(40, glm::vec3(370.923f, 0.0f, -140.162f), glm::vec3(0.0f, glm::radians(90.000f), 0.0f));
		loadFence(41, glm::vec3(370.923f, 0.0f, -160.458f), glm::vec3(0.0f, glm::radians(90.000f), 0.0f));
		loadFence(42, glm::vec3(370.923f, 0.0f, -180.754f), glm::vec3(0.0f, glm::radians(90.000f), 0.0f));
		loadFence(43, glm::vec3(370.923f, 0.0f, -201.05f), glm::vec3(0.0f, glm::radians(90.000f), 0.0f));
		loadFence(44, glm::vec3(370.923f, 0.0f, -221.346f), glm::vec3(0.0f, glm::radians(90.000f), 0.0f));
		loadFence(45, glm::vec3(370.923f, 0.0f, -241.641f), glm::vec3(0.0f, glm::radians(90.000f), 0.0f));
		loadFence(46, glm::vec3(370.923f, 0.0f, -261.937f), glm::vec3(0.0f, glm::radians(90.000f), 0.0f));
		loadFence(47, glm::vec3(370.923f, 0.0f, -322.825f), glm::vec3(0.0f, glm::radians(90.000f), 0.0f));
		loadFence(48, glm::vec3(370.923f, 0.0f, -343.121f), glm::vec3(0.0f, glm::radians(90.000f), 0.0f));
		loadFence(49, glm::vec3(370.923f, 0.0f, -363.417f), glm::vec3(0.0f, glm::radians(90.000f), 0.0f));
		loadFence(50, glm::vec3(370.923f, 0.0f, -383.713f), glm::vec3(0.0f, glm::radians(90.000f), 0.0f));
		loadFence(51, glm::vec3(370.923f, 0.0f, -404.008f), glm::vec3(0.0f, glm::radians(90.000f), 0.0f));
		loadFence(52, glm::vec3(370.923f, 0.0f, -424.304f), glm::vec3(0.0f, glm::radians(90.000f), 0.0f));
		loadFence(53, glm::vec3(370.923f, 0.0f, -444.6f), glm::vec3(0.0f, glm::radians(90.000f), 0.0f));
		loadFence(54, glm::vec3(370.923f, 0.0f, -464.896f), glm::vec3(0.0f, glm::radians(90.000f), 0.0f));
		loadFence(55, glm::vec3(370.923f, 0.0f, -485.192f), glm::vec3(0.0f, glm::radians(90.000f), 0.0f));
		loadFence(56, glm::vec3(-412.732f, 0.0f, 244.558f), glm::vec3(0.0f, glm::radians(-0.000f), 0.0f));
		loadFence(57, glm::vec3(-168.994f, 0.0f, 338.279f), glm::vec3(0.0f, glm::radians(90.000f), 0.0f));
		loadFence(58, glm::vec3(-169.139f, 0.0f, 317.995f), glm::vec3(0.0f, glm::radians(90.000f), 0.0f));
		loadFence(59, glm::vec3(178.823f, 0.0f, 413.125f), glm::vec3(0.0f, glm::radians(-1.655f), 0.0f));
		loadFence(60, glm::vec3(159.003f, 0.0f, 413.125f), glm::vec3(0.0f, glm::radians(-1.655f), 0.0f));
		loadFence(61, glm::vec3(139.182f, 0.0f, 413.125f), glm::vec3(0.0f, glm::radians(-1.655f), 0.0f));
		loadFence(62, glm::vec3(119.362f, 0.0f, 413.125f), glm::vec3(0.0f, glm::radians(-1.655f), 0.0f));
		loadFence(63, glm::vec3(100.003f, 0.0f, 413.125f), glm::vec3(0.0f, glm::radians(-1.655f), 0.0f));
		loadFence(64, glm::vec3(80.6435f, 0.0f, 413.125f), glm::vec3(0.0f, glm::radians(-1.655f), 0.0f));
		loadFence(65, glm::vec3(61.2842f, 0.0f, 413.125f), glm::vec3(0.0f, glm::radians(-1.655f), 0.0f));
		loadFence(66, glm::vec3(41.9249f, 0.0f, 413.125f), glm::vec3(0.0f, glm::radians(-1.655f), 0.0f));
		loadFence(67, glm::vec3(21.9557f, 0.0f, 413.125f), glm::vec3(0.0f, glm::radians(-1.655f), 0.0f));
		loadFence(68, glm::vec3(1.98649f, 0.0f, 413.125f), glm::vec3(0.0f, glm::radians(-1.655f), 0.0f));
		loadFence(69, glm::vec3(-17.9827f, 0.0f, 413.125f), glm::vec3(0.0f, glm::radians(-1.655f), 0.0f));
		loadFence(70, glm::vec3(-37.9519f, 0.0f, 413.125f), glm::vec3(0.0f, glm::radians(-1.655f), 0.0f));
		loadFence(71, glm::vec3(-57.9211f, 0.0f, 413.125f), glm::vec3(0.0f, glm::radians(-1.655f), 0.0f));
		loadFence(72, glm::vec3(-77.8903f, 0.0f, 413.125f), glm::vec3(0.0f, glm::radians(-1.655f), 0.0f));
		loadFence(73, glm::vec3(-97.8595f, 0.0f, 413.125f), glm::vec3(0.0f, glm::radians(-1.655f), 0.0f));
		loadFence(74, glm::vec3(-117.829f, 0.0f, 413.125f), glm::vec3(0.0f, glm::radians(-1.655f), 0.0f));
		loadFence(75, glm::vec3(-137.798f, 0.0f, 413.125f), glm::vec3(0.0f, glm::radians(-1.655f), 0.0f));
		loadFence(76, glm::vec3(-157.767f, 0.0f, 413.125f), glm::vec3(0.0f, glm::radians(-1.655f), 0.0f));
		loadFence(77, glm::vec3(-168.86f, 0.0f, 400.975f), glm::vec3(0.0f, glm::radians(90.000f), 0.0f));
		loadFence(78, glm::vec3(-168.86f, 0.0f, 379.225f), glm::vec3(0.0f, glm::radians(90.000f), 0.0f));
		loadFence(79, glm::vec3(-168.86f, 0.0f, 358.356f), glm::vec3(0.0f, glm::radians(90.000f), 0.0f));
		loadFence(80, glm::vec3(-174.888f, 0.0f, 306.986f), glm::vec3(0.0f, glm::radians(0.000f), 0.0f));
		loadFence(81, glm::vec3(-194.123f, 0.0f, 307.141f), glm::vec3(0.0f, glm::radians(0.000f), 0.0f));
		loadFence(82, glm::vec3(-213.109f, 0.0f, 307.154f), glm::vec3(0.0f, glm::radians(0.000f), 0.0f));
		loadFence(83, glm::vec3(-231.892f, 0.0f, 307.226f), glm::vec3(0.0f, glm::radians(0.000f), 0.0f));
		loadFence(84, glm::vec3(-250.731f, 0.0f, 307.404f), glm::vec3(0.0f, glm::radians(0.000f), 0.0f));
		loadFence(85, glm::vec3(-269.836f, 0.0f, 307.436f), glm::vec3(0.0f, glm::radians(0.000f), 0.0f));
		loadFence(86, glm::vec3(-289.197f, 0.0f, 307.66f), glm::vec3(0.0f, glm::radians(0.000f), 0.0f));
		loadFence(87, glm::vec3(-308.559f, 0.0f, 307.66f), glm::vec3(0.0f, glm::radians(0.000f), 0.0f));
		loadFence(88, glm::vec3(-327.92f, 0.0f, 307.66f), glm::vec3(0.0f, glm::radians(0.000f), 0.0f));
		loadFence(89, glm::vec3(-347.282f, 0.0f, 307.66f), glm::vec3(0.0f, glm::radians(0.000f), 0.0f));
		loadFence(90, glm::vec3(-357.905f, 0.0f, 295.293f), glm::vec3(0.0f, glm::radians(90.000f), 0.0f));
		loadFence(91, glm::vec3(-358.571f, 0.0f, 275.967f), glm::vec3(0.0f, glm::radians(-90.000f), 0.0f));
		loadFence(92, glm::vec3(-358.454f, 0.0f, 255.756f), glm::vec3(0.0f, glm::radians(-90.000f), 0.0f));
		loadFence(93, glm::vec3(-370.487f, 0.0f, 244.558f), glm::vec3(0.0f, glm::radians(-0.000f), 0.0f));
		loadFence(94, glm::vec3(-392.091f, 0.0f, 244.558f), glm::vec3(0.0f, glm::radians(-0.000f), 0.0f));
		loadFence(95, glm::vec3(-424.628f, 0.0f, 234.342f), glm::vec3(0.0f, glm::radians(-90.000f), 0.0f));
		loadFence(96, glm::vec3(-424.628f, 0.0f, 212.956f), glm::vec3(0.0f, glm::radians(-90.000f), 0.0f));
		loadFence(97, glm::vec3(-424.628f, 0.0f, 191.493f), glm::vec3(0.0f, glm::radians(-90.000f), 0.0f));
		loadFence(98, glm::vec3(-424.628f, 0.0f, 142.083f), glm::vec3(0.0f, glm::radians(-90.000f), 0.0f));
		loadFence(99, glm::vec3(-424.628f, 0.0f, 123.013f), glm::vec3(0.0f, glm::radians(-90.000f), 0.0f));
		loadFence(100, glm::vec3(-424.628f, 0.0f, 104.416f), glm::vec3(0.0f, glm::radians(-90.000f), 0.0f));
		loadFence(101, glm::vec3(-424.628f, 0.0f, 85.8182f), glm::vec3(0.0f, glm::radians(-90.000f), 0.0f));
		loadFence(102, glm::vec3(-424.628f, 0.0f, 67.2206f), glm::vec3(0.0f, glm::radians(-90.000f), 0.0f));
		loadFence(103, glm::vec3(-424.628f, 0.0f, 48.623f), glm::vec3(0.0f, glm::radians(-90.000f), 0.0f));
		loadFence(104, glm::vec3(-424.628f, 0.0f, 30.0255f), glm::vec3(0.0f, glm::radians(-90.000f), 0.0f));
		loadFence(105, glm::vec3(-424.628f, 0.0f, 11.4279f), glm::vec3(0.0f, glm::radians(-90.000f), 0.0f));
		loadFence(106, glm::vec3(-424.628f, 0.0f, -7.16971f), glm::vec3(0.0f, glm::radians(-90.000f), 0.0f));
		loadFence(107, glm::vec3(-424.628f, 0.0f, -25.7673f), glm::vec3(0.0f, glm::radians(-90.000f), 0.0f));
		loadFence(108, glm::vec3(-424.628f, 0.0f, -44.3649f), glm::vec3(0.0f, glm::radians(-90.000f), 0.0f));
		loadFence(109, glm::vec3(-424.628f, 0.0f, -62.9625f), glm::vec3(0.0f, glm::radians(-90.000f), 0.0f));
		loadFence(110, glm::vec3(-424.628f, 0.0f, -81.5601f), glm::vec3(0.0f, glm::radians(-90.000f), 0.0f));
		loadFence(111, glm::vec3(-424.628f, 0.0f, -100.158f), glm::vec3(0.0f, glm::radians(-90.000f), 0.0f));
		loadFence(112, glm::vec3(-424.628f, 0.0f, -118.755f), glm::vec3(0.0f, glm::radians(-90.000f), 0.0f));
		loadFence(113, glm::vec3(-424.628f, 0.0f, -137.353f), glm::vec3(0.0f, glm::radians(-90.000f), 0.0f));
		loadFence(114, glm::vec3(-424.628f, 0.0f, -155.95f), glm::vec3(0.0f, glm::radians(-90.000f), 0.0f));
		loadFence(115, glm::vec3(-424.628f, 0.0f, -174.548f), glm::vec3(0.0f, glm::radians(-90.000f), 0.0f));
		loadFence(116, glm::vec3(-424.628f, 0.0f, -193.146f), glm::vec3(0.0f, glm::radians(-90.000f), 0.0f));
		loadFence(117, glm::vec3(-424.628f, 0.0f, -211.743f), glm::vec3(0.0f, glm::radians(-90.000f), 0.0f));
		loadFence(118, glm::vec3(-424.628f, 0.0f, -230.341f), glm::vec3(0.0f, glm::radians(-90.000f), 0.0f));
		loadFence(119, glm::vec3(-424.628f, 0.0f, -248.938f), glm::vec3(0.0f, glm::radians(-90.000f), 0.0f));
		loadFence(120, glm::vec3(-424.628f, 0.0f, -267.536f), glm::vec3(0.0f, glm::radians(-90.000f), 0.0f));
		loadFence(121, glm::vec3(-424.628f, 0.0f, -286.134f), glm::vec3(0.0f, glm::radians(-90.000f), 0.0f));
		loadFence(122, glm::vec3(-424.628f, 0.0f, -304.731f), glm::vec3(0.0f, glm::radians(-90.000f), 0.0f));
		loadFence(123, glm::vec3(-424.628f, 0.0f, -323.329f), glm::vec3(0.0f, glm::radians(-90.000f), 0.0f));
		loadFence(124, glm::vec3(-424.628f, 0.0f, -341.926f), glm::vec3(0.0f, glm::radians(-90.000f), 0.0f));
		loadFence(125, glm::vec3(-424.628f, 0.0f, -360.524f), glm::vec3(0.0f, glm::radians(-90.000f), 0.0f));
		loadFence(126, glm::vec3(-424.628f, 0.0f, -379.122f), glm::vec3(0.0f, glm::radians(-90.000f), 0.0f));
		loadFence(127, glm::vec3(-424.628f, 0.0f, -397.719f), glm::vec3(0.0f, glm::radians(-90.000f), 0.0f));
		loadFence(128, glm::vec3(-424.628f, 0.0f, -416.317f), glm::vec3(0.0f, glm::radians(-90.000f), 0.0f));
		loadFence(129, glm::vec3(360.855f, 0.0f, -495.461f), glm::vec3(0.0f, glm::radians(0.000f), 0.0f));
		loadFence(130, glm::vec3(342.312f, 0.0f, -495.461f), glm::vec3(0.0f, glm::radians(0.000f), 0.0f));
		loadFence(131, glm::vec3(323.87f, 0.0f, -495.461f), glm::vec3(0.0f, glm::radians(0.000f), 0.0f));
		loadFence(132, glm::vec3(305.428f, 0.0f, -495.461f), glm::vec3(0.0f, glm::radians(0.000f), 0.0f));
		loadFence(133, glm::vec3(286.986f, 0.0f, -495.461f), glm::vec3(0.0f, glm::radians(0.000f), 0.0f));
		loadFence(134, glm::vec3(268.312f, 0.0f, -495.461f), glm::vec3(0.0f, glm::radians(0.000f), 0.0f));
		loadFence(135, glm::vec3(249.638f, 0.0f, -495.461f), glm::vec3(0.0f, glm::radians(0.000f), 0.0f));
		loadFence(136, glm::vec3(230.964f, 0.0f, -495.461f), glm::vec3(0.0f, glm::radians(0.000f), 0.0f));
		loadFence(137, glm::vec3(212.29f, 0.0f, -495.461f), glm::vec3(0.0f, glm::radians(0.000f), 0.0f));
		loadFence(138, glm::vec3(-418.66f, 0.0f, -434.914f), glm::vec3(0.0f, glm::radians(-126.756f), 0.0f));
		loadFence(139, glm::vec3(-321.944f, 0.0f, -495.461f), glm::vec3(0.0f, glm::radians(0.000f), 0.0f));
		loadFence(140, glm::vec3(-303.401f, 0.0f, -495.461f), glm::vec3(0.0f, glm::radians(0.000f), 0.0f));
		loadFence(141, glm::vec3(-284.859f, 0.0f, -495.461f), glm::vec3(0.0f, glm::radians(0.000f), 0.0f));
		loadFence(142, glm::vec3(-266.317f, 0.0f, -495.461f), glm::vec3(0.0f, glm::radians(0.000f), 0.0f));
		loadFence(143, glm::vec3(-247.774f, 0.0f, -495.461f), glm::vec3(0.0f, glm::radians(0.000f), 0.0f));
		loadFence(144, glm::vec3(-229.232f, 0.0f, -495.461f), glm::vec3(0.0f, glm::radians(0.000f), 0.0f));
		loadFence(145, glm::vec3(-210.689f, 0.0f, -495.461f), glm::vec3(0.0f, glm::radians(0.000f), 0.0f));
		loadFence(146, glm::vec3(-192.147f, 0.0f, -495.461f), glm::vec3(0.0f, glm::radians(0.000f), 0.0f));
		loadFence(147, glm::vec3(-173.604f, 0.0f, -495.461f), glm::vec3(0.0f, glm::radians(0.000f), 0.0f));
		loadFence(148, glm::vec3(-155.062f, 0.0f, -495.461f), glm::vec3(0.0f, glm::radians(0.000f), 0.0f));
		loadFence(149, glm::vec3(193.616f, 0.0f, -495.461f), glm::vec3(0.0f, glm::radians(0.000f), 0.0f));
		loadFence(150, glm::vec3(175.256f, 0.0f, -495.461f), glm::vec3(0.0f, glm::radians(0.000f), 0.0f));
		loadFence(151, glm::vec3(156.896f, 0.0f, -495.461f), glm::vec3(0.0f, glm::radians(0.000f), 0.0f));
		loadFence(152, glm::vec3(138.537f, 0.0f, -495.461f), glm::vec3(0.0f, glm::radians(0.000f), 0.0f));
		loadFence(153, glm::vec3(120.177f, 0.0f, -495.461f), glm::vec3(0.0f, glm::radians(0.000f), 0.0f));
		loadFence(154, glm::vec3(101.817f, 0.0f, -495.461f), glm::vec3(0.0f, glm::radians(0.000f), 0.0f));
		loadFence(155, glm::vec3(83.4568f, 0.0f, -495.461f), glm::vec3(0.0f, glm::radians(0.000f), 0.0f));
		loadFence(156, glm::vec3(65.0968f, 0.0f, -495.461f), glm::vec3(0.0f, glm::radians(0.000f), 0.0f));
		loadFence(157, glm::vec3(46.7369f, 0.0f, -495.461f), glm::vec3(0.0f, glm::radians(0.000f), 0.0f));
		loadFence(158, glm::vec3(28.377f, 0.0f, -495.461f), glm::vec3(0.0f, glm::radians(0.000f), 0.0f));
		loadFence(159, glm::vec3(-26.7028f, 0.0f, -495.461f), glm::vec3(0.0f, glm::radians(0.000f), 0.0f));
		loadFence(160, glm::vec3(-45.0628f, 0.0f, -495.461f), glm::vec3(0.0f, glm::radians(0.000f), 0.0f));
		loadFence(161, glm::vec3(-63.4227f, 0.0f, -495.461f), glm::vec3(0.0f, glm::radians(0.000f), 0.0f));
		loadFence(162, glm::vec3(-81.7826f, 0.0f, -495.461f), glm::vec3(0.0f, glm::radians(0.000f), 0.0f));
		loadFence(163, glm::vec3(-100.16f, 0.0f, -495.461f), glm::vec3(0.0f, glm::radians(0.000f), 0.0f));
	}

	// == BROKENFENCE1 == 
	{
		float shine = 100.0f,
			reflect = 0.1f,
			ambient = 0.1f;

		multiPos = glm::vec3(-12.478f, 0.0f, -495.964f);
		multiRot = glm::vec3(0.0f);

		loadModel(FM_M_BROKEN_FENCE1[0], "res/Forrest_Area/Fences/Broken_Fence/Broken1/Broken1_Logends.obj", multiPos, multiRot, glm::vec3(1.0f), FM_T_BROKENFENCE1[0], FM_TN_FENCE[0], shine, reflect, ambient);
		loadModel(FM_M_BROKEN_FENCE1[1], "res/Forrest_Area/Fences/Broken_Fence/Broken1/Broken1_Logs.obj", multiPos, multiRot, glm::vec3(1.0f), FM_T_BROKENFENCE1[1], FM_TN_FENCE[1], shine, reflect, ambient);
		loadModel(FM_M_BROKEN_FENCE1[2], "res/Forrest_Area/Fences/Broken_Fence/Broken1/Broken1_Planks.obj", multiPos, multiRot, glm::vec3(1.0f), FM_T_BROKENFENCE1[2], FM_TN_FENCE[2], shine, reflect, ambient);
	}

	// == BROKENFENCE2 ==
	{
		float shine = 100.0f,
			reflect = 0.1f,
			ambient = 0.1f;

		multiPos = glm::vec3(12.993f, 0.0f, -495.246f);
		multiRot = glm::vec3(0.0f);

		loadModel(FM_M_BROKEN_FENCE2[0], "res/Forrest_Area/Fences/Broken_Fence/Broken2/Broken2_Logends.obj", multiPos, multiRot, glm::vec3(1.0f), FM_T_BROKENFENCE2[0], FM_TN_FENCE[0], shine, reflect, ambient);
		loadModel(FM_M_BROKEN_FENCE2[1], "res/Forrest_Area/Fences/Broken_Fence/Broken2/Broken2_Logs.obj", multiPos, multiRot, glm::vec3(1.0f), FM_T_BROKENFENCE2[1], FM_TN_FENCE[1], shine, reflect, ambient);
		loadModel(FM_M_BROKEN_FENCE2[2], "res/Forrest_Area/Fences/Broken_Fence/Broken2/Broken2_Planks.obj", multiPos, multiRot, glm::vec3(1.0f), FM_T_BROKENFENCE2[2], FM_TN_FENCE[2], shine, reflect, ambient);
	}

	// == MILITARY_BUNKER ==
	{
		float shine = 100.0f,
			reflect = 0.1f,
			ambient = 0.2f;

		multiPos = glm::vec3(-315.2f, 12.943f, -345.934f);
		multiRot = glm::vec3(0.0f, glm::radians(-90.0f), 0.0f);
		loadModel(FM_M_MILITARY_BUNKER[0], "res/Forrest_Area/Military_Bunker/Bunker_roof.obj", multiPos, multiRot, glm::vec3(1.0f), FM_T_MILITARY_BUNKER[0], 60.0f, 0.25f, ambient);
		loadModel(FM_M_MILITARY_BUNKER[1], "res/Forrest_Area/Military_Bunker/Bunker_Side2.obj", multiPos, multiRot, glm::vec3(1.0f), FM_T_MILITARY_BUNKER[1], shine, reflect, ambient);
		loadModel(FM_M_MILITARY_BUNKER[2], "res/Forrest_Area/Military_Bunker/Bunker_Side1.obj", multiPos, multiRot, glm::vec3(1.0f), FM_T_MILITARY_BUNKER[2], shine, reflect, ambient);

		multiPos = glm::vec3(-276.797f, 12.943f, -202.188f);
		multiRot = glm::vec3(0.0f);
		loadModel(FM_M_MILITARY_BUNKER[3], FM_M_MILITARY_BUNKER[0], multiPos, multiRot, glm::vec3(1.0f), FM_T_MILITARY_BUNKER[0], 60.0f, 0.25f, ambient);
		loadModel(FM_M_MILITARY_BUNKER[4], FM_M_MILITARY_BUNKER[1], multiPos, multiRot, glm::vec3(1.0f), FM_T_MILITARY_BUNKER[1], shine, reflect, ambient);
		loadModel(FM_M_MILITARY_BUNKER[5], FM_M_MILITARY_BUNKER[2], multiPos, multiRot, glm::vec3(1.0f), FM_T_MILITARY_BUNKER[2], shine, reflect, ambient);

		multiPos = glm::vec3(-370.587f, 12.943f, -202.188f);
		multiRot = glm::vec3(0.0f);
		loadModel(FM_M_MILITARY_BUNKER[6], FM_M_MILITARY_BUNKER[0], multiPos, multiRot, glm::vec3(1.0f), FM_T_MILITARY_BUNKER[0], 60.0f, 0.25f, ambient);
		loadModel(FM_M_MILITARY_BUNKER[7], FM_M_MILITARY_BUNKER[1], multiPos, multiRot, glm::vec3(1.0f), FM_T_MILITARY_BUNKER[1], shine, reflect, ambient);
		loadModel(FM_M_MILITARY_BUNKER[8], FM_M_MILITARY_BUNKER[2], multiPos, multiRot, glm::vec3(1.0f), FM_T_MILITARY_BUNKER[2], shine, reflect, ambient);
	}

	// == RAILS ==
	{
		//Sleppers
		loadModel(FM_M_RAIL[0], "res/Forrest_Area/Rails/IronRail.obj", glm::vec3(-0.115f, 0.0f, -425.944f), glm::vec3(0.0f), glm::vec3(1.0f), FM_T_RAIL[0], 35.0f, 0.5f, 0.1f);
		FM_M_RAIL[0].getModel()->setTiledAmount(glm::vec2(0.0f, 10.0f));

		//Steel Rail
		loadModel(FM_M_RAIL[1], "res/Forrest_Area/Rails/Sleppers.obj", glm::vec3(-0.115f, 0.243f, -449.193f), glm::vec3(0.0f), glm::vec3(1.0f), FM_T_RAIL[1], 100.0f, 0.1f, 0.25f);
	}

	// == ROADLIGHT ==
	{
		float shine = 35.0f,
			reflect = 0.5f,
			ambient = 0.1f;

		multiPos = glm::vec3(-101.98f, -0.036581f, 146.328f);
		multiRot = glm::radians(glm::vec3(0.0f, 0.000f, 0.0f));

		loadModel(FM_M_ROADLIGHT[0], "res/Forrest_Area/RoadLight/Lamp.obj", multiPos, multiRot, glm::vec3(1.0f), FM_T_ROADLIGHT[0], shine, reflect, ambient);
		FM_M_ROADLIGHT[0].getModel()->setSpecularMap(FM_TS_ROADLIGHT);
		loadModel(FM_M_ROADLIGHT[1], "res/Forrest_Area/RoadLight/LampPole.obj", multiPos, multiRot, glm::vec3(1.0f), FM_T_ROADLIGHT[1], shine, reflect, ambient);
		loadModel(FM_M_ROADLIGHT[2], "res/Forrest_Area/RoadLight/Lamp-Pole_Connector.obj", multiPos, multiRot, glm::vec3(1.0f), FM_T_ROADLIGHT[2], shine, reflect, ambient);

		//loadRoadLight(0, glm::vec3(-101.98f, -0.036581f, 146.328f), glm::radians(glm::vec3(0.0f, 0.000f, 0.0f)));
		loadRoadLight(1, glm::vec3(-101.98f, -0.036581f, 76.2742f), glm::radians(glm::vec3(0.0f, 0.000f, 0.0f)));
		loadRoadLight(2, glm::vec3(-101.98f, -0.036581f, 6.21997f), glm::radians(glm::vec3(0.0f, 0.000f, 0.0f)));
		loadRoadLight(3, glm::vec3(-101.98f, -0.036581f, -63.8343f), glm::radians(glm::vec3(0.0f, 0.000f, 0.0f)));
		loadRoadLight(4, glm::vec3(-101.98f, -0.036581f, -133.889f), glm::radians(glm::vec3(0.0f, 0.000f, 0.0f)));
		loadRoadLight(5, glm::vec3(-101.98f, -0.036581f, -203.943f), glm::radians(glm::vec3(0.0f, 0.000f, 0.0f)));
		loadRoadLight(6, glm::vec3(-101.98f, -0.036581f, -273.997f), glm::radians(glm::vec3(0.0f, 0.000f, 0.0f)));
		loadRoadLight(7, glm::vec3(-101.98f, -0.036581f, -344.051f), glm::radians(glm::vec3(0.0f, 0.000f, 0.0f)));
		loadRoadLight(8, glm::vec3(-101.98f, -0.036581f, -414.106f), glm::radians(glm::vec3(0.0f, 0.000f, 0.0f)));
		loadRoadLight(9, glm::vec3(-101.98f, -0.036581f, -484.16f), glm::radians(glm::vec3(0.0f, 0.000f, 0.0f)));
		loadRoadLight(10, glm::vec3(-101.98f, -0.036581f, -554.214f), glm::radians(glm::vec3(0.0f, 0.000f, 0.0f)));
		loadRoadLight(11, glm::vec3(-27.6026f, -0.036581f, 146.328f), glm::radians(glm::vec3(0.0f, 0.000f, 0.0f)));
		loadRoadLight(12, glm::vec3(51.53f, -0.036581f, 146.328f), glm::radians(glm::vec3(0.0f, 0.000f, 0.0f)));
		loadRoadLight(13, glm::vec3(130.663f, -0.036581f, 146.328f), glm::radians(glm::vec3(0.0f, 0.000f, 0.0f)));
		loadRoadLight(14, glm::vec3(209.795f, -0.036581f, 146.328f), glm::radians(glm::vec3(0.0f, 0.000f, 0.0f)));
		loadRoadLight(15, glm::vec3(288.928f, -0.036581f, 146.328f), glm::radians(glm::vec3(0.0f, 0.000f, 0.0f)));
		loadRoadLight(16, glm::vec3(368.06f, -0.036581f, 146.328f), glm::radians(glm::vec3(0.0f, 0.000f, 0.0f)));
		loadRoadLight(17, glm::vec3(-195.646f, -0.036581f, 146.328f), glm::radians(glm::vec3(0.0f, 0.000f, 0.0f)));
		loadRoadLight(18, glm::vec3(-284.557f, -0.036581f, 146.328f), glm::radians(glm::vec3(0.0f, 0.000f, 0.0f)));
		loadRoadLight(19, glm::vec3(-373.469f, -0.036581f, 146.328f), glm::radians(glm::vec3(0.0f, 0.000f, 0.0f)));
		loadRoadLight(20, glm::vec3(-10.7945f, -0.036581f, -273.997f), glm::radians(glm::vec3(0.0f, 0.000f, 0.0f)));
		loadRoadLight(21, glm::vec3(85.1462f, -0.036581f, -273.997f), glm::radians(glm::vec3(0.0f, 0.000f, 0.0f)));
		loadRoadLight(22, glm::vec3(181.087f, -0.036581f, -273.997f), glm::radians(glm::vec3(0.0f, 0.000f, 0.0f)));
		loadRoadLight(23, glm::vec3(372.968f, -0.036581f, -273.997f), glm::radians(glm::vec3(0.0f, 0.000f, 0.0f)));
	}

	// == ROAD ==
	{
		float shine = 60.0f,
			reflect = 0.3f,
			ambient = 0.3f;

		loadModel(FM_M_ROAD[0], "res/Forrest_Area/Roads/ArmyRoad.obj", glm::vec3(-130.899f, 0.125f, -254.0f), glm::vec3(0.0f), glm::vec3(1.0f), FM_T_ROAD[0], FM_TN_ROAD, shine, reflect, ambient);
		loadModel(FM_M_ROAD[1], "res/Forrest_Area/Roads/BuildingRoad.obj", glm::vec3(191.358f, 0.15f, -293.095f), glm::radians(glm::vec3(0.000f, 90.000f, 0.000f)), glm::vec3(1.0f), FM_T_ROAD[1], FM_TN_ROAD, shine, reflect, ambient);
		loadModel(FM_M_ROAD[2], "res/Forrest_Area/Roads/TRoad.obj", glm::vec3(-9.782f, 0.05f, 166.212f), glm::radians(glm::vec3(0.000f, 90.000f, 0.000f)), glm::vec3(1.0f), FM_T_ROAD[2], FM_TN_ROAD, shine, reflect, ambient);

		FM_M_ROAD[0].getModel()->setTiledAmount(glm::vec2(0.0f, 100.0f));
		FM_M_ROAD[1].getModel()->setTiledAmount(glm::vec2(0.0f, 100.0f));
		FM_M_ROAD[2].getModel()->setTiledAmount(glm::vec2(0.0f, 100.0f));
	}

	// == STATUE ==
	{
		float shine = 35.0f,
			reflect = 0.5f,
			ambient = 0.1f;

		loadModel(FM_M_STATUE, "res/Forrest_Area/Statue/Statue.obj", glm::vec3(232.965f, 0.0f, -10.63f), glm::vec3(0.0f), glm::vec3(1.0f), FM_T_STATUE, FM_TN_STATUE, shine, reflect, ambient);
	}

	// == TERRAIN ==
	{
		float shine = 1000.0f,
			reflect = 0.1f,
			ambient = 0.1f;

		loadModel(FM_M_TERRAIN, "res/Forrest_Area/Terrain/Mountains/Mountains.obj", glm::vec3(0.0f, -32.0f, 0.0f), glm::vec3(0.0f), glm::vec3(1.0f), FM_T_TERRAIN, FM_TN_TERRAIN, shine, reflect, ambient);

		FM_M_FLATTERRAIN.create(&loader.loadObjFile("res/Forrest_Area/Terrain/Ground/GroundPlane.obj", false, false), glm::vec3(0.0f, 0.0f, 2.0f), glm::vec3(0.0f), FM_T_FLATTERRAIN[0], FM_T_FLATTERRAIN[1], FM_T_FLATTERRAIN[2], FM_T_FLATTERRAIN[3], FM_T_FLATTERRAIN[4]);
	}

	// == TOWN_HOUSE ==
	{
		float shine = 100.0f,
			reflect = 0.1f,
			ambient = 0.1f;

		loadModel(FM_M_TOWNHOUSE[0], "res/Forrest_Area/Town_House/TownHouse.obj", glm::vec3(284.8f, 22.299f, -351.721f), glm::vec3(0.0f), glm::vec3(1.0f), FM_T_TOWNHOUSE, shine, reflect, ambient);
		loadModel(FM_M_TOWNHOUSE[1], FM_M_TOWNHOUSE[0], glm::vec3(282.153f, 22.299f, -247.98), glm::vec3(0.0f, glm::radians(-90.0f), 0.0f), glm::vec3(1.0f), FM_T_TOWNHOUSE, shine, reflect, ambient);
	}

	// == TRAFFIC_CONE ==
	{
		float shine = 35.0f,
			reflect = 0.5f,
			ambient = 0.1f;

		loadModel(FM_M_TRAFFICCONE[0], "res/Forrest_Area/Traffic_Cone/TrafficCone.obj", glm::vec3(-99.7265f, 1.14487f, -283.624f), glm::radians(glm::vec3(0.000f, -115.923f, 0.000f)), glm::vec3(1.0f), FM_T_TRAFFICCONE, shine, reflect, ambient);

		//loadTrafficCone(0, glm::vec3(-99.7265f, 1.14487f, -283.624f), glm::radians(glm::vec3(0.000f, -115.923f, 0.000f)));
		loadTrafficCone(1, glm::vec3(-99.1198f, 1.14487f, -288.298f), glm::radians(glm::vec3(0.000f, 60.946f, 0.000f)));
		loadTrafficCone(2, glm::vec3(-99.2452f, 1.14487f, -293.27f), glm::radians(glm::vec3(0.000f, -66.510f, 0.000f)));
		loadTrafficCone(3, glm::vec3(-100.242f, 0.581069f, -298.043f), glm::radians(glm::vec3(-177.934f, 169.543f, -67.776f)));
		loadTrafficCone(4, glm::vec3(-98.6604f, 1.14487f, -302.862f), glm::radians(glm::vec3(0.000f, -10.501f, 0.000f)));
		loadTrafficCone(5, glm::vec3(-141.113f, 1.14487f, 161.808f), glm::radians(glm::vec3(0.000f, 0.000f, 0.000f)));
		loadTrafficCone(6, glm::vec3(-137.975f, 1.14487f, 171.176f), glm::radians(glm::vec3(0.000f, 53.338f, 0.000f)));
		loadTrafficCone(7, glm::vec3(-33.5519f, 1.14487f, 180.658f), glm::radians(glm::vec3(0.000f, 6.541f, 0.000f)));
		loadTrafficCone(8, glm::vec3(-27.6242f, 1.14487f, 177.319f), glm::radians(glm::vec3(0.000f, -94.199f, 0.000f)));
		loadTrafficCone(9, glm::vec3(-19.875f, 1.14487f, 175.868f), glm::radians(glm::vec3(0.000f, 150.562f, 0.000f)));
		loadTrafficCone(10, glm::vec3(-11.8955f, 1.14487f, 175.261f), glm::radians(glm::vec3(0.000f, -33.153f, 0.000f)));
		loadTrafficCone(11, glm::vec3(-5.74136f, 1.14487f, 176.016f), glm::radians(glm::vec3(0.000f, 99.130f, 0.000f)));
		loadTrafficCone(12, glm::vec3(0.588318f, 1.14487f, 176.853f), glm::radians(glm::vec3(0.000f, -41.157f, 0.000f)));
		loadTrafficCone(13, glm::vec3(6.25226f, 1.14487f, 178.912f), glm::radians(glm::vec3(0.000f, -0.902f, 0.000f)));
	}

	// == TREE ==
	trees.setTreeModel(&loader.loadObjFile("res/Forrest_Area/Trees/Simple_Tree/SimpleTree.obj", false, false));
	loadTreeModels();

	// == WELL ==
	{
		float shine = 100.0f,
			reflect = 0.1f,
			ambient = 0.1f;

		loadModel(FM_M_WELL, "res/Forrest_Area/Well/Well.obj", glm::vec3(43.876f, 0.0f, 305.417f), glm::vec3(0.0f, glm::radians(-141.404f), 0.0f), glm::vec3(1.0f), FM_T_WELL, FM_TN_WELL, shine, reflect, ambient);
	}
}
// Load the Fence model
void loadFence(int iteration, glm::vec3 pos, glm::vec3 rot)
{
	float shine = 35.0f,
		reflect = 0.5f,
		ambient = 0.1f;

	loadModel(FM_M_FENCE[(iteration * 3)], FM_M_FENCE[0], pos, rot, glm::vec3(1.0f), FM_T_FENCE[0], FM_TN_FENCE[0], shine, reflect, ambient);
	loadModel(FM_M_FENCE[(iteration * 3) + 1], FM_M_FENCE[1], pos, rot, glm::vec3(1.0f), FM_T_FENCE[1], FM_TN_FENCE[1], shine, reflect, ambient);
	loadModel(FM_M_FENCE[(iteration * 3) + 2], FM_M_FENCE[2], pos, rot, glm::vec3(1.0f), FM_T_FENCE[2], FM_TN_FENCE[2], shine, reflect, ambient);
}
// Load the RoadLight model
void loadRoadLight(int iteration, glm::vec3 pos, glm::vec3 rot)
{
	float shine = 35.0f,
		reflect = 0.5f,
		ambient = 0.1f;

	loadModel(FM_M_ROADLIGHT[(iteration * 3)], FM_M_ROADLIGHT[0], pos, rot, glm::vec3(1.0f), FM_T_ROADLIGHT[0], shine, reflect, ambient);
	FM_M_ROADLIGHT[(iteration * 3)].getModel()->setSpecularMap(FM_TS_ROADLIGHT);
	loadModel(FM_M_ROADLIGHT[(iteration * 3) + 1], FM_M_ROADLIGHT[1], pos, rot, glm::vec3(1.0f), FM_T_ROADLIGHT[1], shine, reflect, ambient);
	loadModel(FM_M_ROADLIGHT[(iteration * 3) + 2], FM_M_ROADLIGHT[2], pos, rot, glm::vec3(1.0f), FM_T_ROADLIGHT[2], shine, reflect, ambient);
}
// Load the TrafficCone model
void loadTrafficCone(int iteration, glm::vec3 pos, glm::vec3 rot)
{
	float shine = 35.0f,
		reflect = 0.5f,
		ambient = 0.1f;

	loadModel(FM_M_TRAFFICCONE[iteration], FM_M_TRAFFICCONE[0], pos, rot, glm::vec3(1.0f), FM_T_TRAFFICCONE, shine, reflect, ambient);
}
// Load the ArmyTent model
void loadArmyTent(int iteration, glm::vec3 pos, glm::vec3 rot)
{
	float shine = 100.0f,
		reflect = 0.1f,
		ambient = 0.2f;

	loadModel(FM_M_ARMY_TENT[(iteration * 3)], FM_M_ARMY_TENT[0], pos, rot, glm::vec3(1.0f), FM_T_ARMY_TENT[0], FM_TN_ARMY_TENT[0], shine, reflect, ambient);
	loadModel(FM_M_ARMY_TENT[(iteration * 3) + 1], FM_M_ARMY_TENT[1], pos, rot, glm::vec3(1.0f), FM_T_ARMY_TENT[1], FM_TN_ARMY_TENT[1], shine, reflect, ambient);
	loadModel(FM_M_ARMY_TENT[(iteration * 3) + 2], FM_M_ARMY_TENT[2], pos, rot, glm::vec3(1.0f), FM_T_ARMY_TENT[2], shine, reflect, ambient);
}
// Load the ArmyTruck model
void loadArmyTruck(int iteration, glm::vec3 pos, glm::vec3 rot)
{
	float shine = 100.0f,
		reflect = 0.1f,
		ambient = 0.2f;

	float rshine = 30.0f,
		rreflect = 0.25f,
		rambient = 0.2f;

	float rrshine = 80.0f,
		rrreflect = 0.5f,
		rrambient = 0.2f;

	loadModel(FM_M_ARMY_TRUCK[(iteration * 5)], FM_M_ARMY_TRUCK[0], pos, rot, glm::vec3(1.0f), FM_T_ARMY_TRUCK[0], shine, reflect, ambient);
	loadModel(FM_M_ARMY_TRUCK[(iteration * 5) + 1], FM_M_ARMY_TRUCK[1], pos, rot, glm::vec3(1.0f), FM_T_ARMY_TRUCK[1], rshine, rreflect, rambient);
	loadModel(FM_M_ARMY_TRUCK[(iteration * 5) + 2], FM_M_ARMY_TRUCK[2], pos, rot, glm::vec3(1.0f), FM_T_ARMY_TRUCK[2], rshine, rreflect, rambient);
	loadModel(FM_M_ARMY_TRUCK[(iteration * 5) + 3], FM_M_ARMY_TRUCK[3], pos, rot, glm::vec3(1.0f), FM_T_ARMY_TRUCK[3], rrshine, rrreflect, rrambient);
	loadModel(FM_M_ARMY_TRUCK[(iteration * 5) + 4], FM_M_ARMY_TRUCK[4], pos, rot, glm::vec3(1.0f), FM_T_ARMY_TRUCK[4], shine, reflect, ambient);
}

// Load the tree models
void loadTreeModels()
{
	trees.addTreePos(510.37f, -1.3942f, -188.909f);
	trees.addTreePos(510.223f, -1.3942f, -146.52f);
	trees.addTreePos(511.303f, -1.3942f, -111.255f);
	trees.addTreePos(472.268f, -1.3942f, -254.624f);
	trees.addTreePos(482.733f, -1.3942f, -219.426f);
	trees.addTreePos(476.333f, -1.3942f, -176.438f);
	trees.addTreePos(474.689f, -1.3942f, -125.962f);
	trees.addTreePos(476.232f, -1.3942f, -91.6603f);
	trees.addTreePos(485.694f, -1.3942f, -48.4302f);
	trees.addTreePos(428.882f, -1.3942f, -247.058f);
	trees.addTreePos(431.7f, -1.3942f, -210.503f);
	trees.addTreePos(434.445f, -1.3942f, -172.775f);
	trees.addTreePos(430.433f, -1.3942f, -123.026f);
	trees.addTreePos(440.818f, -1.3942f, -91.6603f);
	trees.addTreePos(428.822f, -1.3942f, -37.7779f);
	trees.addTreePos(441.171f, -1.3942f, -5.49011f);
	trees.addTreePos(427.562f, -1.3942f, 39.2772f);
	trees.addTreePos(428.951f, -1.3942f, 76.565f);
	trees.addTreePos(430.849f, -1.3942f, 120.11f);
	trees.addTreePos(393.232f, -1.3942f, -248.643f);
	trees.addTreePos(393.22f, -1.3942f, -218.826f);
	trees.addTreePos(391.975f, -1.3942f, -180.887f);
	trees.addTreePos(390.429f, -1.3942f, -126.677f);
	trees.addTreePos(389.161f, -1.3942f, -91.6603f);
	trees.addTreePos(392.313f, -1.3942f, -43.0784f);
	trees.addTreePos(394.846f, -1.3942f, -2.52667f);
	trees.addTreePos(395.125f, -1.3942f, 37.595f);
	trees.addTreePos(389.032f, -1.3942f, 88.2142f);
	trees.addTreePos(392.362f, -1.3942f, 125.313f);
	trees.addTreePos(159.628f, -1.3942f, -251.346f);
	trees.addTreePos(129.532f, -1.3942f, -249.328f);
	trees.addTreePos(93.9764f, -1.3942f, -254.624f);
	trees.addTreePos(59.4794f, -1.3942f, -252.072f);
	trees.addTreePos(23.1865f, -1.3942f, -249.961f);
	trees.addTreePos(-49.3007f, -1.3942f, -251.042f);
	trees.addTreePos(-89.1849f, -1.3942f, -254.624f);
	trees.addTreePos(172.259f, -1.3942f, -227.16f);
	trees.addTreePos(134.787f, -1.3942f, -210.265f);
	trees.addTreePos(104.786f, -1.3942f, -223.743f);
	trees.addTreePos(71.112f, -1.3942f, -219.019f);
	trees.addTreePos(-56.2334f, -1.3942f, -213.824f);
	trees.addTreePos(-87.4605f, -1.3942f, -216.301f);
	trees.addTreePos(338.868f, -1.3942f, -172.635f);
	trees.addTreePos(272.849f, -1.3942f, -178.928f);
	trees.addTreePos(305.971f, -1.3942f, -170.85f);
	trees.addTreePos(238.094f, -1.3942f, -169.638f);
	trees.addTreePos(202.646f, -1.3942f, -169.612f);
	trees.addTreePos(238.562f, 18.4748f, -177.741f);
	trees.addTreePos(128.701f, -1.3942f, -169.523f);
	trees.addTreePos(95.5391f, -1.3942f, -176.438f);
	trees.addTreePos(59.7336f, -1.3942f, -168.45f);
	trees.addTreePos(-90.6139f, -1.3942f, -176.438f);
	trees.addTreePos(345.164f, -1.3942f, -134.049f);
	trees.addTreePos(285.959f, -1.3942f, -134.049f);
	trees.addTreePos(311.264f, -1.3942f, -134.049f);
	trees.addTreePos(250.833f, -1.3942f, -134.049f);
	trees.addTreePos(217.899f, -1.3942f, -134.049f);
	trees.addTreePos(190.338f, -1.3942f, -131.046f);
	trees.addTreePos(162.226f, -1.3942f, -129.133f);
	trees.addTreePos(116.86f, -1.3942f, -134.049f);
	trees.addTreePos(78.864f, -1.3942f, -130.799f);
	trees.addTreePos(43.9673f, -1.3942f, -118.102f);
	trees.addTreePos(-90.2832f, -1.3942f, -138.208f);
	trees.addTreePos(336.731f, -1.3942f, -91.6603f);
	trees.addTreePos(273.842f, -1.3942f, -86.9848f);
	trees.addTreePos(303.257f, -1.3942f, -87.472f);
	trees.addTreePos(247.6f, -1.3942f, -97.8999f);
	trees.addTreePos(215.176f, -1.3942f, -97.9017f);
	trees.addTreePos(183.076f, -1.3942f, -88.6574f);
	trees.addTreePos(147.435f, -1.3942f, -97.2777f);
	trees.addTreePos(120.812f, -1.3942f, -89.9894f);
	trees.addTreePos(83.8146f, -1.3942f, -88.6093f);
	trees.addTreePos(43.9673f, -1.3942f, -74.5573f);
	trees.addTreePos(346.296f, -1.3942f, -48.5752f);
	trees.addTreePos(275.84f, -1.3942f, -55.3018f);
	trees.addTreePos(306.367f, -1.3942f, -41.4534f);
	trees.addTreePos(246.867f, -1.3942f, -45.6176f);
	trees.addTreePos(216.357f, -1.3942f, -54.2632f);
	trees.addTreePos(179.767f, -1.3942f, -41.6319f);
	trees.addTreePos(150.93f, -1.3942f, -52.3492f);
	trees.addTreePos(117.493f, -1.3942f, -43.9465f);
	trees.addTreePos(74.9996f, -1.3942f, -48.5752f);
	trees.addTreePos(43.9673f, -1.3942f, -36.1482f);
	trees.addTreePos(10.731f, -1.3942f, -43.769f);
	trees.addTreePos(-19.2495f, -1.3942f, -42.9766f);
	trees.addTreePos(-49.6906f, -1.3942f, -48.5752f);
	trees.addTreePos(-86.4926f, -1.3942f, -48.5752f);
	trees.addTreePos(339.193f, -1.3942f, -5.49011f);
	trees.addTreePos(277.106f, -1.3942f, -5.49011f);
	trees.addTreePos(308.76f, -1.3942f, -5.49011f);
	trees.addTreePos(180.714f, -1.3942f, 5.4649f);
	trees.addTreePos(150.93f, -1.3942f, -5.49011f);
	trees.addTreePos(113.599f, -1.3942f, -2.56543f);
	trees.addTreePos(79.0496f, -1.3942f, -11.8541f);
	trees.addTreePos(43.9673f, -1.3942f, 10.594f);
	trees.addTreePos(13.3615f, -1.3942f, -6.79065f);
	trees.addTreePos(-25.6304f, -1.3942f, 1.32043f);
	trees.addTreePos(-59.6166f, -1.3942f, -10.9769f);
	trees.addTreePos(-87.9814f, -1.3942f, -8.26892f);
	trees.addTreePos(338.82f, -1.3942f, 37.595f);
	trees.addTreePos(276.711f, -1.3942f, 37.595f);
	trees.addTreePos(308.395f, -1.3942f, 37.595f);
	trees.addTreePos(252.343f, -1.3942f, 37.595f);
	trees.addTreePos(214.467f, -1.3942f, 37.595f);
	trees.addTreePos(183.572f, -1.3942f, 37.595f);
	trees.addTreePos(154.14f, -1.3942f, 37.595f);
	trees.addTreePos(117.118f, -1.3942f, 41.3545f);
	trees.addTreePos(82.4829f, -1.3942f, 41.6178f);
	trees.addTreePos(46.3526f, -1.3942f, 37.8831f);
	trees.addTreePos(4.68806f, -1.3942f, 36.2371f);
	trees.addTreePos(-21.7356f, -1.3942f, 42.4605f);
	trees.addTreePos(-55.5196f, -1.3942f, 32.9075f);
	trees.addTreePos(-89.2128f, -1.3942f, 44.7902f);
	trees.addTreePos(338.547f, -1.3942f, 80.6801f);
	trees.addTreePos(282.073f, -1.3942f, 80.6801f);
	trees.addTreePos(310.76f, -1.3942f, 80.6801f);
	trees.addTreePos(253.025f, -1.3942f, 80.6801f);
	trees.addTreePos(214.786f, -1.3942f, 80.6801f);
	trees.addTreePos(186.158f, -1.3942f, 80.6801f);
	trees.addTreePos(143.533f, -1.3942f, 80.6801f);
	trees.addTreePos(119.458f, -1.3942f, 77.048f);
	trees.addTreePos(81.7111f, -1.3942f, 82.7693f);
	trees.addTreePos(47.1596f, -1.3942f, 87.7859f);
	trees.addTreePos(12.4795f, -1.3942f, 76.7079f);
	trees.addTreePos(-26.3493f, -1.3942f, 78.361f);
	trees.addTreePos(-55.2969f, -1.3942f, 87.7054f);
	trees.addTreePos(-87.8759f, -1.3942f, 83.6824f);
	trees.addTreePos(6.6485f, -1.3942f, 121.856f);
	trees.addTreePos(-90.1776f, -1.3942f, 121.973f);
	trees.addTreePos(-61.1513f, -1.3942f, 120.237f);
	trees.addTreePos(-25.6646f, -1.3942f, 122.11f);
	trees.addTreePos(40.9637f, -1.3942f, 119.364f);
	trees.addTreePos(75.1138f, -1.3942f, 120.725f);
	trees.addTreePos(110.888f, -1.3942f, 112.76f);
	trees.addTreePos(150.318f, -1.3942f, 119.7f);
	trees.addTreePos(181.717f, -1.3942f, 120.828f);
	trees.addTreePos(216.442f, -1.3942f, 120.828f);
	trees.addTreePos(242.735f, -1.3942f, 123.765f);
	trees.addTreePos(273.531f, -1.3942f, 117.965f);
	trees.addTreePos(314.24f, -1.3942f, 123.765f);
	trees.addTreePos(340.476f, -1.3942f, 123.765f);
	trees.addTreePos(-334.655f, -1.3942f, 249.84f);
	trees.addTreePos(-389.384f, -1.3942f, 201.971f);
	trees.addTreePos(-353.408f, -1.3942f, 206.138f);
	trees.addTreePos(-313.412f, -1.3942f, 201.384f);
	trees.addTreePos(-319.861f, -1.3942f, 246.072f);
	trees.addTreePos(-324.774f, -1.3942f, 281.634f);
	trees.addTreePos(-176.213f, -1.3942f, 196.701f);
	trees.addTreePos(-175.022f, -1.3942f, 249.097f);
	trees.addTreePos(-178.214f, -1.3942f, 293.011f);
	trees.addTreePos(-212.587f, -1.3942f, 274.891f);
	trees.addTreePos(-212.587f, -1.3942f, 236.755f);
	trees.addTreePos(-207.451f, -1.3942f, 203.39f);
	trees.addTreePos(-244.74f, -1.3942f, 204.862f);
	trees.addTreePos(-241.819f, -1.3942f, 239.774f);
	trees.addTreePos(-244.146f, -1.3942f, 274.349f);
	trees.addTreePos(-284.386f, -1.3942f, 201.16f);
	trees.addTreePos(-278.316f, -1.3942f, 237.873f);
	trees.addTreePos(-283.004f, -1.3942f, 277.784f);
	trees.addTreePos(-73.5049f, -1.3942f, 433.269f);
	trees.addTreePos(-42.9479f, -1.3942f, 433.269f);
	trees.addTreePos(-15.0706f, -1.3942f, 433.269f);
	trees.addTreePos(35.9378f, -1.3942f, 433.269f);
	trees.addTreePos(67.4227f, -1.3942f, 433.269f);
	trees.addTreePos(102.485f, -1.3942f, 431.08f);
	trees.addTreePos(137.874f, -1.3942f, 433.269f);
	trees.addTreePos(304.658f, -1.3942f, 309.354f);
	trees.addTreePos(333.377f, -1.3942f, 309.919f);
	trees.addTreePos(392.339f, -1.3942f, 197.976f);
	trees.addTreePos(349.707f, -1.3942f, 242.136f);
	trees.addTreePos(359.395f, -1.3942f, 204.989f);
	trees.addTreePos(266.239f, -1.3942f, 248.5f);
	trees.addTreePos(261.219f, -1.3942f, 203.888f);
	trees.addTreePos(291.439f, -1.3942f, 213.225f);
	trees.addTreePos(299.829f, -1.3942f, 249.729f);
	trees.addTreePos(326.89f, -1.3942f, 242.136f);
	trees.addTreePos(325.645f, -1.3942f, 203.272f);
	trees.addTreePos(-140.409f, -1.3942f, 203.913f);
	trees.addTreePos(-109.687f, -1.3942f, 197.976f);
	trees.addTreePos(-80.2751f, -1.3942f, 205.347f);
	trees.addTreePos(-39.8637f, -1.3942f, 204.033f);
	trees.addTreePos(3.30345f, -1.3942f, 193.35f);
	trees.addTreePos(28.3016f, -1.3942f, 197.976f);
	trees.addTreePos(63.647f, -1.45883f, 208.603f);
	trees.addTreePos(96.4626f, -1.19998f, 203.856f);
	trees.addTreePos(131.583f, -1.3942f, 201.928f);
	trees.addTreePos(205.386f, -1.3942f, 201.529f);
	trees.addTreePos(174.026f, -1.3942f, 200.56f);
	trees.addTreePos(232.059f, -1.3942f, 203.177f);
	trees.addTreePos(-145.042f, -1.3942f, 237.067f);
	trees.addTreePos(-117.206f, -1.3942f, 242.136f);
	trees.addTreePos(-74.237f, -1.3942f, 248.059f);
	trees.addTreePos(-39.8637f, -1.3942f, 246.973f);
	trees.addTreePos(30.0835f, -1.3942f, 242.136f);
	trees.addTreePos(65.3831f, -1.39414f, 238.765f);
	trees.addTreePos(103.134f, -1.3942f, 240.626f);
	trees.addTreePos(138.585f, -1.42655f, 239.77f);
	trees.addTreePos(202.98f, -1.3942f, 245.622f);
	trees.addTreePos(172.94f, -1.3942f, 244.422f);
	trees.addTreePos(230.578f, -1.3942f, 236.194f);
	trees.addTreePos(-145.042f, -1.3942f, 292.876f);
	trees.addTreePos(-109.54f, -1.3942f, 282.017f);
	trees.addTreePos(-70.7432f, -1.3942f, 277.82f);
	trees.addTreePos(-43.8957f, -1.3942f, 287.112f);
	trees.addTreePos(-8.55703f, -1.3942f, 252.547f);
	trees.addTreePos(-145.042f, -1.3942f, 317.127f);
	trees.addTreePos(-113.587f, -1.3942f, 321.348f);
	trees.addTreePos(-77.8135f, -1.3942f, 318.271f);
	trees.addTreePos(-41.5551f, -1.3942f, 329.865f);
	trees.addTreePos(-9.49355f, -1.3942f, 305.256f);
	trees.addTreePos(-145.042f, -1.3942f, 359.81f);
	trees.addTreePos(-137.408f, -1.3942f, 393.759f);
	trees.addTreePos(-108.133f, -1.3942f, 360.68f);
	trees.addTreePos(-100.233f, -1.3942f, 396.143f);
	trees.addTreePos(-75.6451f, -1.3942f, 356.955f);
	trees.addTreePos(-72.9427f, -1.3942f, 392.94f);
	trees.addTreePos(-37.3121f, -1.3942f, 373.974f);
	trees.addTreePos(-36.4633f, -1.3942f, 399.972f);
	trees.addTreePos(-4.31627f, -1.3942f, 339.974f);
	trees.addTreePos(-5.27694f, -1.3942f, 388.39f);
	trees.addTreePos(75.889f, -1.3942f, 287.368f);
	trees.addTreePos(230.293f, -1.3942f, 279.767f);
	trees.addTreePos(199.035f, -1.3942f, 291.701f);
	trees.addTreePos(170.492f, -1.3942f, 278.144f);
	trees.addTreePos(137.579f, -1.3942f, 279.37f);
	trees.addTreePos(105.914f, -1.3942f, 275.627f);
	trees.addTreePos(232.451f, -1.3942f, 317.765f);
	trees.addTreePos(197.208f, -1.3942f, 328.111f);
	trees.addTreePos(169.162f, -1.3942f, 327.359f);
	trees.addTreePos(135.264f, -1.3942f, 312.38f);
	trees.addTreePos(103.339f, -1.3942f, 321.181f);
	trees.addTreePos(30.1273f, -1.3942f, 392.971f);
	trees.addTreePos(63.724f, -1.3942f, 391.729f);
	trees.addTreePos(99.0039f, -1.3942f, 385.803f);
	trees.addTreePos(132.462f, -1.3942f, 389.825f);
	trees.addTreePos(169.509f, -1.3942f, 393.693f);
	trees.addTreePos(24.8913f, -1.3942f, 360.68f);
	trees.addTreePos(63.1984f, -1.3942f, 362.817f);
	trees.addTreePos(99.0039f, -1.3942f, 370.726f);
	trees.addTreePos(130.335f, -1.3942f, 358.061f);
	trees.addTreePos(164.317f, -1.3942f, 355.167f);
	trees.addTreePos(204.949f, -1.3942f, 367.084f);
	trees.addTreePos(232.809f, -1.3942f, 358.849f);
	trees.addTreePos(-176.219f, -1.3942f, 394.858f);
	trees.addTreePos(-184.952f, -1.3942f, 364.623f);
	trees.addTreePos(-215.151f, -1.3942f, 358.869f);
	trees.addTreePos(-240.512f, -1.3942f, 356.643f);
	trees.addTreePos(-351.634f, -1.3942f, 322.568f);
	trees.addTreePos(-323.854f, -1.3942f, 322.568f);
	trees.addTreePos(-296.073f, -1.3942f, 325.945f);
	trees.addTreePos(-268.293f, -1.3942f, 317.959f);
	trees.addTreePos(-244.109f, -1.3942f, 314.399f);
	trees.addTreePos(-212.732f, -1.3942f, 319.631f);
	trees.addTreePos(-189.139f, -1.3942f, 329.09f);
	trees.addTreePos(-367.773f, -1.3942f, 290.245f);
	trees.addTreePos(-397.568f, -1.3942f, 255.174f);
	trees.addTreePos(-367.773f, -1.3942f, 249.573f);
	trees.addTreePos(-451.101f, -1.3942f, 130.233f);
	trees.addTreePos(-470.177f, -1.3942f, 98.8667f);
	trees.addTreePos(-443.326f, -1.3942f, 106.303f);
	trees.addTreePos(-498.668f, -1.3942f, -157.258f);
	trees.addTreePos(-498.668f, -1.3942f, -123.404f);
	trees.addTreePos(-498.668f, -1.3942f, -95.0724f);
	trees.addTreePos(-498.668f, -1.3942f, -61.3362f);
	trees.addTreePos(-472.789f, -1.3942f, -161.35f);
	trees.addTreePos(-470.177f, -1.3942f, -125.189f);
	trees.addTreePos(-467.093f, -1.3942f, -95.0724f);
	trees.addTreePos(-472.502f, -1.3942f, -64.9563f);
	trees.addTreePos(-470.177f, -1.3942f, 51.9967f);
	trees.addTreePos(-471.354f, -1.3942f, 35.8274f);
	trees.addTreePos(-471.664f, -1.3942f, 9.96429f);
	trees.addTreePos(-471.923f, -1.3942f, -22.4282f);
	trees.addTreePos(-442.562f, -1.3942f, 70.1604f);
	trees.addTreePos(-440.383f, -1.3942f, 43.7691f);
	trees.addTreePos(-440.383f, -1.3942f, 3.1452f);
	trees.addTreePos(-438.497f, -1.3942f, -27.3075f);
	trees.addTreePos(-442.347f, -1.3942f, -60.8234f);
	trees.addTreePos(-440.383f, -1.3942f, -102.884f);
	trees.addTreePos(-443.196f, -1.3942f, -129.714f);
	trees.addTreePos(-443.55f, -1.3942f, -158.845f);
	trees.addTreePos(-452.656f, -1.3942f, -195.856f);
	trees.addTreePos(-439.855f, -1.3942f, -227.976f);
	trees.addTreePos(-445.518f, -1.3942f, -256.088f);
	trees.addTreePos(-438.495f, -1.3942f, -286.205f);
	trees.addTreePos(301.334f, -1.3942f, -445.237f);
	trees.addTreePos(307.081f, -1.3942f, -421.127f);
	trees.addTreePos(-24.836f, -1.3942f, -326.756f);
	trees.addTreePos(207.503f, -1.3942f, -594.716f);
	trees.addTreePos(200.533f, -1.3942f, -559.103f);
	trees.addTreePos(202.98f, -1.3942f, -537.569f);
	trees.addTreePos(198.956f, -1.3942f, -511.051f);
	trees.addTreePos(206.105f, -1.3942f, -419.659f);
	trees.addTreePos(199.682f, -1.3942f, -447.221f);
	trees.addTreePos(198.424f, -1.3942f, -383.147f);
	trees.addTreePos(202.98f, -1.3942f, -356.309f);
	trees.addTreePos(203.543f, -1.3942f, -330.651f);
	trees.addTreePos(411.228f, -1.3942f, -322.995f);
	trees.addTreePos(384.512f, -1.3942f, -330.832f);
	trees.addTreePos(341.647f, -1.3942f, -331.339f);
	trees.addTreePos(231.82f, -1.3942f, -334.843f);
	trees.addTreePos(167.65f, -1.3942f, -326.251f);
	trees.addTreePos(135.264f, -1.3942f, -324.048f);
	trees.addTreePos(102.874f, -1.3942f, -328.837f);
	trees.addTreePos(68.4435f, -1.3942f, -325.993f);
	trees.addTreePos(33.8894f, -1.3942f, -328.7f);
	trees.addTreePos(-54.5836f, -1.3942f, -326.756f);
	trees.addTreePos(-87.3699f, -1.3942f, -327.101f);
	trees.addTreePos(-173.457f, -1.3942f, -326.756f);
	trees.addTreePos(-446.888f, -1.3942f, -322.829f);
	trees.addTreePos(416.617f, -1.3942f, -354.516f);
	trees.addTreePos(382.791f, -1.3942f, -354.042f);
	trees.addTreePos(337.143f, -1.3942f, -356.872f);
	trees.addTreePos(278.799f, -1.3942f, -356.872f);
	trees.addTreePos(238.128f, -1.3942f, -359.573f);
	trees.addTreePos(164.268f, -1.3942f, -353.094f);
	trees.addTreePos(135.264f, -1.3942f, -359.572f);
	trees.addTreePos(95.7977f, -1.3942f, -356.872f);
	trees.addTreePos(59.6089f, -1.3942f, -356.872f);
	trees.addTreePos(27.2019f, -1.3942f, -358.894f);
	trees.addTreePos(-35.8428f, -1.3942f, -360.748f);
	trees.addTreePos(-78.8778f, -1.3942f, -356.872f);
	trees.addTreePos(-167.958f, -1.3942f, -356.872f);
	trees.addTreePos(-438.659f, -1.3942f, -356.872f);
	trees.addTreePos(413.646f, -1.3942f, -385.592f);
	trees.addTreePos(379.885f, -1.3942f, -384.104f);
	trees.addTreePos(341.285f, -1.3942f, -391.026f);
	trees.addTreePos(232.231f, -1.3942f, -385.109f);
	trees.addTreePos(175.614f, -1.3942f, -387.224f);
	trees.addTreePos(135.077f, -1.3942f, -385.168f);
	trees.addTreePos(95.8827f, -1.3942f, -387.151f);
	trees.addTreePos(67.1888f, -1.3942f, -389.096f);
	trees.addTreePos(24.2998f, -1.3942f, -384.586f);
	trees.addTreePos(-46.3798f, -1.3942f, -386.988f);
	trees.addTreePos(-80.7916f, -1.3942f, -386.988f);
	trees.addTreePos(-172.765f, -1.3942f, -386.988f);
	trees.addTreePos(-402.05f, -1.3942f, -386.988f);
	trees.addTreePos(-448.033f, -1.3942f, -386.988f);
	trees.addTreePos(412.102f, -1.3942f, -422.142f);
	trees.addTreePos(382.785f, -1.3942f, -420.642f);
	trees.addTreePos(342.953f, -1.3942f, -415.18f);
	trees.addTreePos(275.024f, -1.3942f, -415.686f);
	trees.addTreePos(238.636f, -1.3942f, -408.702f);
	trees.addTreePos(167.166f, -1.3942f, -415.046f);
	trees.addTreePos(131.951f, -1.3942f, -420.465f);
	trees.addTreePos(107.083f, -1.3942f, -418.642f);
	trees.addTreePos(68.9129f, -1.3942f, -415.692f);
	trees.addTreePos(33.15f, -1.3942f, -417.104f);
	trees.addTreePos(-35.8428f, -1.3942f, -417.104f);
	trees.addTreePos(-73.8705f, -1.3942f, -417.104f);
	trees.addTreePos(-167.944f, -1.3942f, -417.104f);
	trees.addTreePos(-392.334f, -1.3942f, -417.104f);
	trees.addTreePos(-442.323f, -1.3942f, -417.104f);
	trees.addTreePos(413.646f, -1.3942f, -444.241f);
	trees.addTreePos(385.253f, -1.3942f, -445.232f);
	trees.addTreePos(336.343f, -1.3942f, -444.754f);
	trees.addTreePos(269.501f, -1.3942f, -439.262f);
	trees.addTreePos(237.747f, -1.3942f, -441.268f);
	trees.addTreePos(170.492f, -1.3942f, -445.168f);
	trees.addTreePos(132.37f, -1.3942f, -441.003f);
	trees.addTreePos(95.2837f, -1.3942f, -448.319f);
	trees.addTreePos(63.1984f, -1.3942f, -442.575f);
	trees.addTreePos(28.3016f, -1.3942f, -447.033f);
	trees.addTreePos(-42.3098f, -1.3942f, -454.105f);
	trees.addTreePos(-80.7916f, -1.3942f, -447.221f);
	trees.addTreePos(-168.262f, -1.3942f, -447.221f);
	trees.addTreePos(-395.434f, -1.3942f, -447.221f);
	trees.addTreePos(381.055f, -1.3942f, -483.841f);
	trees.addTreePos(-350.485f, -1.3942f, -477.337f);
	trees.addTreePos(304.957f, -1.3942f, -508.527f);
	trees.addTreePos(270.594f, -1.3942f, -513.071f);
	trees.addTreePos(237.126f, -1.3942f, -509.518f);
	trees.addTreePos(168.498f, -1.3942f, -515.113f);
	trees.addTreePos(133.05f, -1.3942f, -513.038f);
	trees.addTreePos(99.0039f, -1.3942f, -506.025f);
	trees.addTreePos(70.91f, -1.3942f, -511.047f);
	trees.addTreePos(30.4602f, -1.3942f, -510.484f);
	trees.addTreePos(-43.3557f, -1.3942f, -509.295f);
	trees.addTreePos(-86.7702f, -1.3942f, -509.674f);
	trees.addTreePos(-168.611f, -1.3942f, -507.453f);
	trees.addTreePos(-210.961f, -1.3942f, -507.453f);
	trees.addTreePos(-256.688f, -1.3942f, -507.453f);
	trees.addTreePos(-296.03f, -1.3942f, -507.453f);
	trees.addTreePos(270.483f, -1.3942f, -539.099f);
	trees.addTreePos(237.131f, -1.3942f, -537.569f);
	trees.addTreePos(176.682f, -1.3942f, -540.088f);
	trees.addTreePos(139.852f, -1.3942f, -537.569f);
	trees.addTreePos(102.321f, -1.3942f, -540.933f);
	trees.addTreePos(64.1454f, -1.3942f, -539.847f);
	trees.addTreePos(30.08f, -1.3942f, -540.318f);
	trees.addTreePos(-22.9534f, -1.3942f, -541.208f);
	trees.addTreePos(-71.1327f, -1.3942f, -539.281f);
	trees.addTreePos(-180.348f, -1.3942f, -537.569f);
	trees.addTreePos(-220.632f, -1.3942f, -537.569f);
	trees.addTreePos(237.137f, -1.3942f, -564.634f);
	trees.addTreePos(168.489f, -1.3942f, -567.685f);
	trees.addTreePos(129.66f, -1.3942f, -571.224f);
	trees.addTreePos(95.5085f, -1.3942f, -567.685f);
	trees.addTreePos(65.7838f, -1.3942f, -567.685f);
	trees.addTreePos(33.3723f, -1.3942f, -567.685f);
	trees.addTreePos(-40.0067f, -1.3942f, -571.93f);
	trees.addTreePos(-89.3413f, -1.3942f, -572.72f);
	trees.addTreePos(-170.689f, -1.3942f, -567.685f);
	trees.addTreePos(170.492f, -1.3942f, -594.196f);
	trees.addTreePos(136.139f, -1.3942f, -593.704f);
	trees.addTreePos(96.2224f, -1.3942f, -595.083f);
	trees.addTreePos(66.1854f, -1.3942f, -596.162f);
	trees.addTreePos(27.6027f, -1.3942f, -604.067f);
	trees.addTreePos(-25.3409f, -1.3942f, -594.198f);
	trees.addTreePos(-77.4866f, -1.3942f, -593.83f);

	// Finally initialse the trees to get a random rotation
	trees.initialseNewTrees();
}


//  =========  Initialise a model  ===========

// Load a model with only a texture
void loadModel(Model &model, // Variable to set
	std::string modelFilename, // Model filename
	glm::vec3 pos, glm::vec3 rot, glm::vec3 scale, // Initial Position, rotation and scale
	GLuint textureID,  // Textures
	float shineDamper, float reflectivity,
	float ambientLight) // Reflectivity
{
	// Load modeldata and set the variable.
	model.setModel(&loader.loadObjFile(modelFilename.c_str(), false, false));

	// Initialise model
	model.Initialise(pos, rot, scale);

	// Add a texture
	model.getModel()->addTexture(textureID);

	// Set the shineDamper
	model.getModel()->setShineDamper(shineDamper);
	// Set the reflectivity
	model.getModel()->setReflectivity(reflectivity);

	// Set the ambientLight
	model.getModel()->setAmbientLight(ambientLight);
}
// Load a model with a texture and normalMap
void loadModel(Model &model, // Variable to set
	std::string modelFilename, // Model filename
	glm::vec3 pos, glm::vec3 rot, glm::vec3 scale, // Initial Position, rotation and scale
	GLuint textureID, GLuint normalTextureID, // Textures
	float shineDamper, float reflectivity,
	float ambientLight) // Reflectivity
{
	loadModel(model, modelFilename, pos, rot, scale, textureID, shineDamper, reflectivity, ambientLight);

	// Set the normalmap
	model.getModel()->setNormalMap(normalTextureID);
}
// Load a model with a texture, normalMap and a shadowMap
void loadModel(Model &model, // Variable to set
	std::string modelFilename, // Model filename
	glm::vec3 pos, glm::vec3 rot, glm::vec3 scale, // Initial Position, rotation and scale
	GLuint textureID, GLuint normalTextureID, GLuint shadowMapID, // Textures
	float shineDamper, float reflectivity,
	float ambientLight)
{
	// Load the model
	loadModel(model, modelFilename, pos, rot, scale, textureID, normalTextureID, shineDamper, reflectivity, ambientLight);

	// Set the shadowmap
	model.getModel()->setShadowMap(shadowMapID);
}

// Set a model with only a texture
void loadModel(Model &model, Model &oriModel, // Variable to set
	glm::vec3 pos, glm::vec3 rot, glm::vec3 scale, // Initial Position, rotation and scale
	GLuint textureID,  // Textures
	float shineDamper, float reflectivity,
	float ambientLight) // Reflectivity
{
	model.setModel(oriModel.getModel());

	// Initialise model
	model.Initialise(pos, rot, scale);

	// Add a texture
	model.getModel()->addTexture(textureID);

	// Set the shineDamper
	model.getModel()->setShineDamper(shineDamper);
	// Set the reflectivity
	model.getModel()->setReflectivity(reflectivity);

	// Set the ambientLight
	model.getModel()->setAmbientLight(ambientLight);
}
// Set a model with a texture and normalMap
void loadModel(Model &model, Model &oriModel,// Variable to set
	glm::vec3 pos, glm::vec3 rot, glm::vec3 scale, // Initial Position, rotation and scale
	GLuint textureID, GLuint normalTextureID, // Textures
	float shineDamper, float reflectivity,
	float ambientLight) // Reflectivity
{
	loadModel(model, oriModel, pos, rot, scale, textureID, shineDamper, reflectivity, ambientLight);

	// Set the normalmap
	model.getModel()->setNormalMap(normalTextureID);
}
// Set a model with a texture, normalMap and a shadowMap
void loadModel(Model &model, Model &oriModel,// Variable to set
	glm::vec3 pos, glm::vec3 rot, glm::vec3 scale, // Initial Position, rotation and scale
	GLuint textureID, GLuint normalTextureID, GLuint shadowMapID, // Textures
	float shineDamper, float reflectivity,
	float ambientLight)
{
	// Load the model
	loadModel(model, oriModel, pos, rot, scale, textureID, normalTextureID, shineDamper, reflectivity, ambientLight);

	// Set the shadowmap
	model.getModel()->setShadowMap(shadowMapID);
}