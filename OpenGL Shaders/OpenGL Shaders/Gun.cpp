#include "Gun.h"

Gun::Gun()
{
}

Gun::~Gun()
{
}

void Gun::Init(glm::vec3 position, glm::vec3 rotation, Model &gunModel, Model &gunMuzzleFlash, double Timer)
{
	gun_model = gunModel;
	gun_model.setPosition(position);
	gun_model.setRotation(rotation);

	gun_muzzleflash = gunMuzzleFlash;
	gun_muzzleflash.setPosition(position);
	gun_muzzleflash.setRotation(rotation);

	shootTimer = Timer;

	currentSound = 0;
}

// -4 & 20

float calculated_rotation(float rot)
{
	//Limit rotation between 20 and -4
	float r = rot;
	if (r > 20) r = 20;
	if (r < -4) r = -4;
	return r;
};

void Gun::Update(glm::vec3 position, glm::vec3 rotation)
{
	glm::vec3 rot = rotation;
	rot = glm::radians(rot);

	position.x += cos(rot.y) * 0.7f;
	position.y = 4.7f;
	position.z += sin(rot.y) * 0.7f;

	gun_model.setPosition(glm::vec3(position.x, position.y, position.z));
	gun_model.setRotation(glm::radians(glm::vec3(rotation.z, -rotation.y - 90.0f, rotation.x)));

	gun_muzzleflash.setPosition(glm::vec3(position.x, position.y, position.z));
	gun_muzzleflash.setRotation(glm::radians(glm::vec3(rotation.z, -rotation.y - 90.0f, rotation.x)));
}

void Gun::createSound(SimpleAudioLib::CoreSystem& audioSystem, const char * filepath, unsigned int iterations, float gain)
{
	for (unsigned int i = 0; i < iterations; i++)
	{
		SimpleAudioLib::AudioEntity* tempSound = audioSystem.createAudioEntityFromFile(filepath);
		tempSound->setGain(gain);
		shootSounds.push_back(tempSound);
	}
}

void Gun::playSound()
{
	shootSounds[currentSound]->rewind();
	shootSounds[currentSound]->play();

	currentSound++;
	if (currentSound >= shootSounds.size()) currentSound = 0;
}

Model* Gun::getGunModel()
{
	return &gun_model;
}

Model * Gun::getGunMuzzleFlash()
{
	return &gun_muzzleflash;
}

double Gun::getShootTimer()
{
	return shootTimer;
}

void Gun::cleanupSound()
{
	for (unsigned int i = 0; i < shootSounds.size(); i++) {
		delete shootSounds[i];
		shootSounds[i] = NULL;
	}
}
