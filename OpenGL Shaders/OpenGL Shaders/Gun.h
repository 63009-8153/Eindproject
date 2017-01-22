#ifndef __GUN__
#define __GUN__

#include "Header.h"

#include "Model.h"

#include "SAL/CoreSystem.h"

class Gun
{
	public:
		Gun();
		~Gun();

		void Init(glm::vec3 position, glm::vec3 rotation, Model &gunModel, Model &gunMuzzleFlash, double Timer);
		void Update(glm::vec3 position, glm::vec3 rotation);

		void createSound(SimpleAudioLib::CoreSystem& audioSystem, const char * filepath, unsigned int iterations, float gain = 1.0f);
		void playSound();

		Model* getGunModel();
		Model* getGunMuzzleFlash();

		double getShootTimer();

		void cleanupSound();

	private:
		Model gun_model;
		Model gun_muzzleflash;
		double shootTimer;

		std::vector<SimpleAudioLib::AudioEntity*> shootSounds;
		int currentSound;
};
#endif // !__GUN__