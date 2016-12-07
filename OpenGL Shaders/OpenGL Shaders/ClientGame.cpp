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
						networkUpdateFunction = nullptr;
						break;
				}
			}
		}
			break;
		case GAME_PACKET:
		{
			ClientReceivePacket packet;
			packet.deserialize(&(network_data[i]));
			i += sizeof(ClientReceivePacket);

		}
			break;
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

// Send playerData to the server.
void ClientGame::sendPlayerData(playerData &player, packetTypes type)
{
	const unsigned int packet_size = sizeof(ClientSendPacket);
	char packet_data[packet_size];

	ClientSendPacket packet;
	packet.packet_type = type;
	packet.player = player;

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
		nextActionTypes.push_back(type);
	}
	else {
		printf("WARNING -- Already added %d actionTypes.\nThis is the maximum to be send!\nLosing last actionType!\n", MAX_ACTIONS);
	}
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