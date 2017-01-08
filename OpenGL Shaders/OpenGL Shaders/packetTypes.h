#pragma once

enum packetTypes {
	// Lobby packet, to be send when the game is still in the lobby
	LOBBY_PACKET		= 0,

	// Game packet, to be send when the game has started
	GAME_PACKET			= 1,

	// Initialisation packet, when the client has just been accepted by the server
	INITALISATION_PACKET = 2,

	// Heartbeat packet, send at an interval to check if the connection is still active and get the connection speed.
	HEARTBEAT_PACKET	= 3
};