

#include "Player.h"

GLuint Player::animationTexture;

//Constructor
Player::Player()
{
	currentAnimationType = IDLE;
	networkAnimType = IDLE;
	currentAnimationFrame = 0;

	animationExtraTime = 0;

	active = false;
}
//Destructor
Player::~Player()
{
}

//Initialise the player with its position, rotation and scale. 
//Also set the maxHealth of the player.
//This sets the attack strength to 100. This is knive kill damage. 
//To set the attack strength, use setAttackStrenght(float dmg).
void Player::init(glm::vec3 position, glm::vec3 rotation, glm::vec3 scale, float _maxHealth)
{
	Initialise(position, rotation, scale);
	maxHealth = _maxHealth;
	health = maxHealth;

	speed = glm::vec3(0.0f);

	//Set attackStrength to knife damage
	attackStrength = 100.0f;

	currentAnimationType = IDLE;
	networkAnimType = IDLE;
	currentAnimationFrame = 0;

	animationExtraTime = 0;

	active = false;
	gun.Init(position, rotation);
}

//Set the amount of damage the player does.
void Player::setAttackStrength(float dmg)
{
	attackStrength = dmg;
}

gameobject *Player::getAnimModel()
{
	if (currentAnimationFrame >= animationModels.size()) currentAnimationFrame = (animationModels.size() - 1);

	animationModels[currentAnimationFrame].setPosition(getPosition());
	animationModels[currentAnimationFrame].setRotation(glm::radians(-glm::vec3(0.0f, getRotation().y + 180.0f, 0.0f)));
	animationModels[currentAnimationFrame].setScale(glm::vec3(0.04f));

	return &animationModels[currentAnimationFrame];
}

s_anim Player::loadAnimations(char * animationFolder, std::vector<gameobject> &gameobjects, int startframe, int frames, double fps, bool loop)
{
	s_anim a;
	a.startframe = startframe;
	a.endframe	 = a.startframe + (frames - 1);
	a.loop  = loop;
	a.fps = fps;

	for (unsigned int i = 0; i < frames; i++)
	{
		std::string prenumber = "";
		if (i < 10) prenumber = "0";
		std::string filepath = animationFolder + prenumber + std::to_string(i) + ".obj";
		gameobject model = loader.loadObjFileData(filepath.c_str(), false, false);

		gameobjects.push_back(model);
	}

	return a;
}

void Player::createAnimationModels()
{
	for (unsigned int i = 0; i < animationModels.size(); i++) {
		gameobject model = loader.loadToVAO(animationModels[i].vertices, animationModels[i].vertexCount,
											animationModels[i].indices, animationModels[i].indicesCount,
											animationModels[i].uvs, animationModels[i].uvsSize,
											animationModels[i].normals, animationModels[i].normalsSize,
											animationModels[i].tangents, animationModels[i].tangentsSize);
		model.init();
		model.addTexture(animationTexture);
		model.setAmbientLight(0.15f);

		animationModels.at(i) = model;
	}
}

void Player::updateAnimation(int currentType)
{
	if (!active) return;
	if (currentType < 0 || currentType > 5) currentType = currentAnimationType;

	bool animationEnded = false;

	// Get the amount of time each animationframe has to take
	double animationFrameTime = (1.0 / playerAnimations[currentAnimationType].fps);
	// Calculate how many animaitionframes have past since the last update
	int newFrames = (int)floor((deltaTime + animationExtraTime) / animationFrameTime);

	// Amount of time since last animation frame minus the time of the amount of frames we do this update.
	animationExtraTime = ((deltaTime + animationExtraTime) - (newFrames * animationFrameTime));

	// If the currentAnimationFrame plus the amount of new frames is less than or equal to the last frame of the animation, add the amount of newFrames to the animation
	if ((currentAnimationFrame + newFrames) < (playerAnimations[currentAnimationType].endframe)) currentAnimationFrame += newFrames;
	else if (playerAnimations[currentAnimationType].loop) {
		// If we want to loop and the amount of new frames is over the end, get the amount of frames past the end.
		int overEndFrames = (currentAnimationFrame + newFrames);
		// While the overEndFrames is over the last frame of the animation, subtract the amount of frames in the animation from the amount of frames we are over the end.
		while (overEndFrames > (playerAnimations[currentAnimationType].endframe)) {
			overEndFrames -= (playerAnimations[currentAnimationType].endframe - playerAnimations[currentAnimationType].startframe) + 1;
		}

		// Set the new current animationframe to be the new frame
		currentAnimationFrame = overEndFrames;

		// Set the animation to be ended
		animationEnded = true;
	}
	else {
		// If the animation is at its end, and the animation does not loop, set the current frame to the last frame of the animation.
		currentAnimationFrame = playerAnimations[currentAnimationType].endframe;
		// Set the animation to be ended
		animationEnded = true;
	}

	// If the current animation is not the same as the requested animation and the current animation is done playing, set the new animation and start it.
	if (currentType != currentAnimationType) {
		currentAnimationType = currentType;
		resetAnimation();
	}
}

void Player::resetAnimation()
{
	currentAnimationFrame = playerAnimations[currentAnimationType].startframe;
}

bool Player::canUse(glm::vec3 useObjectOrigin, float maxDist)
{
	return (glm::distance(getPosition(), useObjectOrigin) <= maxDist);
}

//Update the player
void Player::update()
{
	glm::vec2 changedMousePos = handleMouseInput(true);
	changedMousePos *= mouseSensitivity;

	// Rotate the x and y axis of the player with the mouse
	glm::vec3 rot = getRotation();
	rot.y -= changedMousePos.x * 0.1f;
	rot.x -= changedMousePos.y * 0.1f;

	// Clamp x to -90 and 90 deg
	if (rot.x < -90.0f) rot.x = -90.0f;
	else if (rot.x > 90.0f) rot.x = 90.0f;
	// Clamp y to -360 and 360 deg
	if (rot.y > 360) rot.y -= 360.0f;
	else if (rot.y < -360) rot.y += 360.0f;

	// Set the player rotation from mouse input
	setRotation(rot);
	gun.Update(getPosition(), getRotation());
	//TODO: player update
}