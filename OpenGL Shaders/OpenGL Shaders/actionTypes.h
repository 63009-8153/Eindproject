#pragma once

enum actionTypes {
	// General actions
	ACTION_NONE		= 0,

	// Lobby actions
	LOBBY_JOIN		= 10,
	LOBBY_UPDATE	= 11,

	// Game actions
	GAME_START		= 20,
	GAME_DISCONNECT	= 21,
	GAME_RECEIVED_INIT = 22,
	GAME_INITIALISATION = 23,

	// Player actions
	MOVE_FORWARD	= 30,
	MOVE_BACKWARD	= 31,
	MOVE_LEFT		= 32,
	MOVE_RIGHT		= 33,

	// Weapon actions
	SHOOT_ONCE		= 40,
	SHOOT_HOLD		= 41,
};