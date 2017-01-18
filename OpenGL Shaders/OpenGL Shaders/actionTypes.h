#pragma once

enum actionTypes {
	// General actions
	ACTION_NONE		= 0,

	// Lobby actions
	LOBBY_JOIN		= 10,
	LOBBY_UPDATE	= 11,

	// Game actions
	GAME_START		= 20,
	GAME_STARTED	= 21,
	GAME_DISCONNECT	= 22,
	GAME_RECEIVED_INIT = 23,
	GAME_INITIALISATION = 24,

	// Player actions
	MOVE_FORWARD	= 30,
	MOVE_BACKWARD	= 31,
	MOVE_LEFT		= 32,
	MOVE_RIGHT		= 33,
	MOVE_RUN		= 34,
	JUMP			= 35,

	// Weapon actions
	SHOOT			= 40,
	RELOAD			= 41,
	USE				= 50
};