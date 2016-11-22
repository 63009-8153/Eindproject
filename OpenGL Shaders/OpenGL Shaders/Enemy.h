#pragma once

#include "Model.h"

class Enemy : Model
{
public:
	Enemy();
	~Enemy();

	void update();
	void draw();
	void init(glm::vec3 pos, glm::vec3 rot, glm::vec3 scal);
};

