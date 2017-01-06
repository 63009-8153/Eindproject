#ifndef _ENEMY_
#define _ENEMY_

#include "Model.h"
#include "AnimationTypes.h"

extern std::vector<gameobject> enemyAnimationModels;
extern std::vector<s_anim> enemyAnimations;

extern double deltaTime;

class Enemy : public Model
{
	public:
		Enemy();
		~Enemy();

		void init(glm::vec3 position, glm::vec3 rotation, glm::vec3 scale, float _maxHealth, float _attackStrength);
		void update();

		void setActive();
		void setActive(glm::vec3 position, glm::vec3 rotation);
		void setActive(glm::vec3 position);


		gameobject *getAnimModel();

		static int loadAnimations(char *animationFolder, int frames, double fps, bool loop);
		static GLuint animationTexture;

		void updateAnimation(int currentType);
		void resetAnimation();

		int networkAnimType;

		bool active;

		float health, maxHealth;
		glm::vec3 speed;
		float attackStrength;

	private:

		int currentAnimationType,
			currentAnimationFrame;
		double animationExtraTime;
};

#endif