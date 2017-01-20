#include "Header.h"

#include "Enemy.h"

GLuint Enemy::animationTexture;
GLuint Enemy::animationNTexture;

//Constructor
Enemy::Enemy()
{
	currentAnimationType = IDLE;
	networkAnimType = IDLE;
	currentAnimationFrame = 0;

	animationExtraTime = 0;

	active = false;
}
//Destructor
Enemy::~Enemy()
{
}

//Initialise the enemy and set its position, rotation and scale. 
//Also se the maxHealth of the enemy and its attack strength.
//Active is set to false here. Use setActive to set active to true and use the enemy.
void Enemy::init(glm::vec3 position, glm::vec3 rotation, glm::vec3 scale, float _maxHealth, float _attackStrength)
{
	Initialise(position, rotation, scale);

	maxHealth = _maxHealth;
	health = maxHealth;

	attackStrength = _attackStrength;
	speed = glm::vec3(0.0f);

	currentAnimationType = ENEM_WALK_FORWARD;
	networkAnimType = ENEM_WALK_FORWARD;
	currentAnimationFrame = 0;

	animationExtraTime = 0;

	active = false;
}

//Set Active.
void Enemy::setActive()
{
	active = true;
}
//Set Active and set a new position and rotation.
void Enemy::setActive(glm::vec3 position, glm::vec3 rotation)
{
	setActive(position);
	setRotation(rotation);
}
//Set Active and set a new position.
void Enemy::setActive(glm::vec3 position)
{
	setActive();
	setPosition(position);
}

gameobject *Enemy::getAnimModel(){
	if (currentAnimationFrame >= enemyAnimationModels.size()) currentAnimationFrame = (enemyAnimationModels.size() - 1);

	enemyAnimationModels[currentAnimationFrame].setPosition(glm::vec3(getPosition().x, getPosition().y - 2.0f, getPosition().z));
	enemyAnimationModels[currentAnimationFrame].setRotation(glm::radians(-glm::vec3(0.0f, getRotation().y + 180.0f, 0.0f)));
	enemyAnimationModels[currentAnimationFrame].setScale(glm::vec3(0.02f));

	return &enemyAnimationModels[currentAnimationFrame];
}

s_anim Enemy::loadAnimations(char * animationFolder, std::vector<gameobject> &gameobjects, int startframe, int frames, double fps, bool loop)
{
	s_anim a;
	a.startframe = startframe;
	a.endframe = a.startframe + (frames - 1);
	a.loop = loop;
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

void Enemy::createAnimationModels()
{
	for (unsigned int i = 0; i < enemyAnimationModels.size(); i++) {
		gameobject model = gameobject(&loader.loadToVAO(enemyAnimationModels[i].vertices, enemyAnimationModels[i].vertexCount, 
											enemyAnimationModels[i].indices, enemyAnimationModels[i].indicesCount, 
											enemyAnimationModels[i].uvs, enemyAnimationModels[i].uvsSize, 
											enemyAnimationModels[i].normals, enemyAnimationModels[i].normalsSize, 
											enemyAnimationModels[i].tangents, enemyAnimationModels[i].tangentsSize));
		model.init(); 
		model.addTexture(animationTexture);
		model.setNormalMap(animationNTexture);
		model.setAmbientLight(0.15f);

		enemyAnimationModels.at(i) = model;
	}
}

void Enemy::updateAnimation(int currentType)
{
	if (!active) {
		return;
	}
	if (currentType < 0 || currentType > 2) currentType = currentAnimationType;

	bool animationEnded = false;

	// Get the amount of time each animationframe has to take
	double animationFrameTime = (1.0 / enemyAnimations[currentAnimationType].fps);
	// Calculate how many animaitionframes have past since the last update
	int newFrames = (int)floor((deltaTime + animationExtraTime) / animationFrameTime);

	// Amount of time since last animation frame minus the time of the amount of frames we do this update.
	animationExtraTime = ((deltaTime + animationExtraTime) - (newFrames * animationFrameTime));

	// If the currentAnimationFrame plus the amount of new frames is less than or equal to the last frame of the animation, add the amount of newFrames to the animation
	if ((currentAnimationFrame + newFrames) < (enemyAnimations[currentAnimationType].endframe)) currentAnimationFrame += newFrames;
	else if (enemyAnimations[currentAnimationType].loop) {
		// If we want to loop and the amount of new frames is over the end, get the amount of frames past the end.
		int overEndFrames = (currentAnimationFrame + newFrames);
		// While the overEndFrames is over the last frame of the animation, subtract the amount of frames in the animation from the amount of frames we are over the end.
		while (overEndFrames > (enemyAnimations[currentAnimationType].endframe)) {
			overEndFrames -= (enemyAnimations[currentAnimationType].endframe - enemyAnimations[currentAnimationType].startframe) + 1;
		}

		// Set the new current animationframe to be the new frame
		currentAnimationFrame = overEndFrames;

		// Set the animation to be ended
		animationEnded = true;
	}
	else {
		// If the animation is at its end, and the animation does not loop, set the current frame to the last frame of the animation.
		currentAnimationFrame = enemyAnimations[currentAnimationType].endframe;
		// Set the animation to be ended
		animationEnded = true;
	}

	// If the current animation is not the same as the requested animation and the current animation is done playing, set the new animation and start it.
	if (currentType != currentAnimationType) {
		if ((currentAnimationType == ENEM_ATTACK && animationEnded) || currentAnimationType != ENEM_ATTACK) {
			currentAnimationType = currentType;
			resetAnimation();
		}
	}
}

void Enemy::resetAnimation()
{
	currentAnimationFrame = enemyAnimations[currentAnimationType].startframe;
}

//Update the enemy.
void Enemy::update(){
	updateAnimation(networkAnimType);
}