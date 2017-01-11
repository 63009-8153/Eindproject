#pragma once

enum animationType{
	IDLE			= 0,

	WALK_FORWARD	= 1,
	WALK_BACKWARD	= 2,
	WALK_LEFT		= 3,
	WALK_RIGHT		= 4,

	RUN_FORWARD		= 5,

	DYING			= 6
};

enum enemyAnimationType {
	ENEM_WALK_FORWARD = 0,
	ENEM_ATTACK = 1,
	ENEM_DYING = 2
};

// Struct for animation
typedef struct {
	int startframe,
		endframe;
	double fps;
	bool loop;
} s_anim;