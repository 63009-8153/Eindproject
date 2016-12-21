#include "ClientGame.h"

// Constructor
ClientGame::ClientGame() {
}
// Constructor
// Connect to the server on ipAddress and port
ClientGame::ClientGame(char ipAddress[39], char port[5])
{
	//Create a new clientNetwork with IP-Address and Port
	network = new ClientNetwork(ipAddress, port);
	errors = network->getErrors();

	lobbyTimer = MAX_STARTTIME;
}
// Destructor
ClientGame::~ClientGame()
{
}


// Update the client with server data.
void ClientGame::updateClient()
{
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
			for (unsigned int l = 0; l < MAX_LOBBYSIZE; l++)
			{
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

				if (myClientID == packet.players[k].playerID)
				{
					myPlayerData = packet.players[k];
				}
			}
			// Get all enemies
			for (unsigned int l = 0; l < MAX_ENEMIES; l++) 
			{
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
			printf("ERROR: -- Packet received with Unknown packetType %u!!\n", packetType);
			return;
			break;
		}
	}
}
// Disconnect from the server.
void ClientGame::disconnect()
{
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
	myPlayerData.rotation = player.getRotation();
	printf("Set: %f | %f | %f\n", myPlayerData.rotation.x, myPlayerData.rotation.y, myPlayerData.rotation.z);
}

// Update the playerdata of own playerData
void ClientGame::getPlayerData(Player & player)
{
	for (unsigned int i = 0; i < MAX_LOBBYSIZE; i++)
	{
		if (allClients[i].playerID == myClientID)
		{
			// Set the position
			player.setPosition(allClients[i].position);

			// Set the health
			player.health = allClients[i].health;
			player.maxHealth = allClients[i].maxHealth;
			// Set the velocity
			player.setVelocity(allClients[i].velocity);

			return;
		}
	}
}

// Send my playerData to the server.
// This function is only to be send during the game
void ClientGame::sendPlayerData()
{
	const unsigned int packet_size = sizeof(ClientSendPacket);
	char packet_data[packet_size];

	ClientSendPacket packet;
	packet.packet_type = GAME_PACKET;

	packet.player = myPlayerData;
	printf("Get: %f | %f | %f\n", myPlayerData.rotation.x, myPlayerData.rotation.y, myPlayerData.rotation.z);

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