#pragma once

enum animationType{
	IDLE			= 0,
	DYING			= 1,

	WALK_FORWARD	= 10,
	WALK_BACKWARD	= 11,
	WALK_LEFT		= 12,
	WALK_RIGHT		= 13,

	RUN_FORWARD		= 20,
	
};

// Struct for animation
typedef struct {
	int startframe,
		endframe;
	double fps;
	bool loop;
} s_anim;