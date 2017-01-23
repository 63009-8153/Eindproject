#include "ClientGame.h"

// Constructor
ClientGame::ClientGame() {
	networkConnected = false;
}
// Constructor
// Connect to the server on ipAddress and port
ClientGame::ClientGame(char ipAddress[39], char port[5])
{
	networkConnected = true;

	//Create a new clientNetwork with IP-Address and Port
	network = new ClientNetwork(ipAddress, port);
	errors = network->getErrors();

	for (unsigned int i = 0; i < errors.size(); i++) {
		switch (errors[i]) {
			case ALL_CONNECTING_SOCKETS_ERROR:
				networkConnected = false;
				break;

			default:
				break;
		}
	}

	lobbyTimer = MAX_STARTTIME;
}
// Destructor
ClientGame::~ClientGame()
{
}

// Update the client with server data.
void ClientGame::updateClient()
{
	if (!networkConnected) return;

	int data_length = network->receivePackets(network_data);

    // No data recieved
	if (data_length <= 0) {
		return;
	}

	// Loop through packets
	unsigned int i = 0;
	while (i < (unsigned int)data_length)
	{
		packetTypes packetType;
		memcpy(&packetType, &(network_data[i]), sizeof(packetTypes));

		switch (packetType) {
			// LobbyPacket is received when in the lobby
			case LOBBY_PACKET:
			{
				ClientReceivePacketLobby packet;
				packet.deserialize(&(network_data[i]));
				i += sizeof(ClientReceivePacketLobby);

				// Save all clients
				for (unsigned int l = 0; l < MAX_LOBBYSIZE; l++){
					allClients[l] = packet.players[l];
				}
			
				// Check the action types
				for (unsigned int t = 0; t < MAX_ACTIONS; t++)
				{
					switch (packet.action_types[t])
					{
						// If the server has received our initialisation packet, stop sending it.
						case GAME_RECEIVED_INIT:
							networkUpdateFunction = SendLobbyData;
							break;
						case GAME_START:
							gameStarting = true;
							break;
						case GAME_STARTED:
							// Set the networkFunction to the game update function
							printf("Game started!\n");
							networkUpdateFunction = SendGameData;
							gameStarted = true;
							gameStarting = false;
							break;
					}
				}

				lobbyTimer = packet.startTimer;
			}
				break;
			// GamePacket is received when playing the game
			case GAME_PACKET:
			{
				ClientReceivePacket packet;
				packet.deserialize(&(network_data[i]));
				i += sizeof(ClientReceivePacket);

				// Set the lobbysize
				actualLobbySize = std::min(packet.lobbySize, (unsigned int)MAX_LOBBYSIZE);
			
				// Get all playerData
				for (unsigned int k = 0; k < actualLobbySize; k++)
				{
					allClients[k] = packet.players[k];

					if (myClientID == packet.players[k].playerID){
						bool sh = myPlayerData.shooting;

						myPlayerData == packet.players[k];

						if (sh) myPlayerData.shooting = true;
					}
				}
				// Get all enemies
				for (unsigned int l = 0; l < MAX_ENEMIES; l++){
					allEnemies[l] = packet.enemies[l];
				}
			}
				break;
			// InitialisationPacket is received when we first connect to the server
			case INITALISATION_PACKET:
			{
				ClientReceivePacketLobby packet;
				packet.deserialize(&(network_data[i]));
				i += sizeof(ClientReceivePacketLobby);

				// We got accepted by the server and received our clientID
				myClientID = (unsigned int)packet.players[0].playerID;

				printf("INFO:  -- Client accepted by server! We got clientID: %d\n", packet.players[0].playerID);
			}
				break;
			// HeartbeatPacket is received when the server wants to check if we are still an active client
			case HEARTBEAT_PACKET:
			{
				i += sizeof(ClientReceivePacketLobby);

				// We received a heartbeat packet from the server, instantly send one back
				printf("INFO:  -- Sending Heartbeat response!\n");
				sendHeartbeatPacket();
			}
				break;

			default:
				ClientReceivePacket packet;
				i += sizeof(ClientReceivePacket);
				printf("ERROR: -- Packet received with Unknown packetType %u!!\n", packetType);
				break;
		}
	}
}
// Disconnect from the server.
void ClientGame::disconnect()
{
	if (!networkConnected) return;

	const unsigned int packet_size = sizeof(ClientSendPacket);
	char packet_data[packet_size];

	ClientSendPacket packet;
	packet.packet_type = GAME_PACKET;
	packet.action_types[0] = GAME_DISCONNECT;

	packet.serialize(packet_data);

	NetworkServices::sendMessage(network->getSocket(), packet_data, packet_size);
}

// Update the playerdata of own playerData
void ClientGame::setPlayerData(Player & player)
{
	if (!networkConnected) return;

	myPlayerData.rotation = player.getRotation();
}

// Update the player with specified playerData
void ClientGame::getPlayerData(Player & player, int index)
{
	if (!networkConnected) return;

	if (allClients[index].playerID == myClientID) {
		player.active = false;
		return;
	}

	player.active = allClients[index].activePlayer;

	if (player.active) {

		// Set the position
		if (glm::distance(glm::vec3(player.getPosition().x, 0.0f, player.getPosition().z), glm::vec3(allClients[index].position.x, 0.0f, allClients[index].position.z)) < 100.0f || player.lastTimePositionChange) {
			player.setPosition(glm::vec3(allClients[index].position.x, 0.0f, allClients[index].position.z));
			player.setRotation(allClients[index].rotation);
			player.lastTimePositionChange = false;
		}
		else {
			printf("Position difference is larger than max!\n");
			player.lastTimePositionChange = true;
		}

		// Set the health
		player.health = allClients[index].health;
		player.maxHealth = allClients[index].maxHealth;
		player.ammo = allClients[index].ammo;
		player.points = allClients[index].points;

		if (allClients[index].currentWeapon >= 0 && allClients[index].currentWeapon < 2) {
			player.currentGun = allClients[index].currentWeapon;
		}

		player.networkAnimType = allClients[index].animType;
	}
}

// Update the playerdata of own playerData
void ClientGame::getPlayerData(Player & player)
{
	if (!networkConnected) return;

	for (unsigned int i = 0; i < MAX_LOBBYSIZE; i++)
	{
		// If the current client is myplayer client
		if (allClients[i].playerID == myClientID)
		{
			// Set the position
			// Set the position
			if (glm::distance(glm::vec3(player.getPosition().x, 0.0f, player.getPosition().z), glm::vec3(allClients[i].position.x, 0.0f, allClients[i].position.z)) < 100.0f || player.lastTimePositionChange) {
				player.setPosition(glm::vec3(allClients[i].position.x, 0.0f, allClients[i].position.z));
				player.lastTimePositionChange = false;
			}
			else {
				player.lastTimePositionChange = true;
				printf("Position difference is larger than max!\n");
			}

			// Set the health
			player.health = allClients[i].health;
			player.maxHealth = allClients[i].maxHealth;

			// Set the velocity
			player.setVelocity(allClients[i].velocity);
			player.ammo = allClients[i].ammo;
			player.points = allClients[i].points;

			if (allClients[i].currentWeapon >= 0 && allClients[i].currentWeapon < 2) {
				player.currentGun = allClients[i].currentWeapon;
			}
			
			player.networkAnimType = allClients[i].animType;
			return;
		}
	}
}

// Update the enemy with specified enemyData index
void ClientGame::getEnemyData(Enemy &enem, int index)
{
	if (!networkConnected) return;

	enem.active = allEnemies[index].active;

	enem.setPosition(allEnemies[index].position);
	enem.setRotation(allEnemies[index].rotation);
	enem.health = allEnemies[index].health;
	enem.maxHealth = allEnemies[index].maxHealth;

	enem.networkAnimType = allEnemies[index].animType;
}

// Send my playerData to the server.
// This function is only to be send during the game
void ClientGame::sendPlayerData()
{
	if (!networkConnected) return;

	const unsigned int packet_size = sizeof(ClientSendPacket);
	char packet_data[packet_size];

	ClientSendPacket packet;
	packet.packet_type = GAME_PACKET;

	packet.player = myPlayerData;

	// Add all set actionTypes to the packet
	for (int i = 0; i < MAX_ACTIONS; i++) {
		if(i < (int)nextActionTypes.size()) packet.action_types[i] = nextActionTypes[i];
		else packet.action_types[i] = ACTION_NONE;
	}
	// Clear the actions
	nextActionTypes.clear();

	packet.serialize(packet_data);

	NetworkServices::sendMessage(network->getSocket(), packet_data, packet_size);
}

// Send a lobbyUpdate packet to the server
// This function is only to be send during the lobby
void ClientGame::sendLobbyUpdate()
{
	if (!networkConnected) return;

	const unsigned int packet_size = sizeof(ClientSendPacketLobby);
	char packet_data[packet_size];

	ClientSendPacketLobby packet;
	packet.packet_type = LOBBY_PACKET;

	packet.player = myPlayerData;

	// Add all set actionTypes to the packet
	for (int i = 0; i < MAX_ACTIONS; i++) {
		if (i < (int)nextActionTypes.size()) packet.action_types[i] = nextActionTypes[i];
		else packet.action_types[i] = ACTION_NONE;
	}
	// Clear the actions
	nextActionTypes.clear();

	packet.serialize(packet_data);

	NetworkServices::sendMessage(network->getSocket(), packet_data, packet_size);
}

// Send an heartbeat response packet to the server
void ClientGame::sendHeartbeatPacket()
{
	if (!networkConnected) return;

	const unsigned int packet_size = sizeof(ClientSendPacket);
	char packet_data[packet_size];

	ClientSendPacket packet;
	packet.packet_type = HEARTBEAT_PACKET;

	packet.serialize(packet_data);

	NetworkServices::sendMessage(network->getSocket(), packet_data, packet_size);
}

// Add an actionType to the next packet send to the server.
void ClientGame::addActionType(actionTypes type)
{
	if (!networkConnected) return;

	// If we are not at MAX_ACTIONS add the action, else dont add it.
	if (nextActionTypes.size() < MAX_ACTIONS) {
		// Check if the action is not already in the action list.
		for (unsigned int i = 0; i < nextActionTypes.size(); i++)
		{
			if (nextActionTypes[i] == type)
			{
				return;
			}
		}
		nextActionTypes.push_back(type);
	}
	else {
		printf("WARNING -- Already added %d actionTypes.\nThis is the maximum to be send!\nLosing last actionType!\n", MAX_ACTIONS);
	}
}

bool ClientGame::hasActionType()
{
	if (!networkConnected) return false;

	if (nextActionTypes.size() > 0) return true;
	return false;
}

// Get the network error.
// Resets to 0 after call
std::vector<networkingErrors> ClientGame::getErrors()
{
	// Return the errors
	std::vector<networkingErrors> tmperrors = errors;
	errors.clear();

	return tmperrors;
}