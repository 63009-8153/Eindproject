#ifndef _PLAYER_
#define _PLAYER_

#include "Model.h"
#include "AnimationTypes.h"

extern std::vector<gameobject> animationModels;
extern std::vector<s_anim> playerAnimations;

extern double deltaTime;

extern glm::vec2 handleMouseInput(bool trapMouseInWindow);
extern float mouseSensitivity;

class Player : public Model
{
	public:
		Player();
		~Player();

		void init(glm::vec3 position, glm::vec3 rotation, glm::vec3 scale, float _maxHealth);
		void update();

		void setAttackStrength(float dmg);

		gameobject *getAnimModel();

		int loadAnimations(char *animationFolder, int frames, double fps, bool loop);
		GLuint animationTexture;

		void updateAnimation(int currentType);
		void resetAnimation();


		float health, maxHealth;
		glm::vec3 speed;
		float attackStrength;

		int networkAnimType;

	private:

		int currentAnimationType,
			currentAnimationFrame;
		double waitTime;
};

#endif