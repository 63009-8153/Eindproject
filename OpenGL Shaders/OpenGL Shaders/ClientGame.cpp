#include "ClientGame.h"

//Constructor
ClientGame::ClientGame() {
}
ClientGame::ClientGame(char ipAddress[39], char port[5])
{
	//Create a new clientNetwork with IP-Address and Port
	network = new ClientNetwork(ipAddress, port);
	errors = network->getErrors();
}
//Destructor
ClientGame::~ClientGame()
{
}


//Update the client with server data.
void ClientGame::updateClient()
{
	int data_length = network->receivePackets(network_data);

    //no data recieved
	if (data_length <= 0) {
		return;
	}

	//Loop through packets
	unsigned int i = 0;
	while (i < (unsigned int)data_length)
	{
		packetTypes packetType;
		memcpy(&packetType, &(network_data[i]), sizeof(packetTypes));

		printf("INFO:  -- Packet received with Type: %u\n", packetType);

		switch (packetType) {

		case LOBBY_PACKET:
		{
			ClientReceivePacketLobby packet;
			packet.deserialize(&(network_data[i]));
			i += sizeof(ClientReceivePacketLobby);

			for (unsigned int l = 0; l < MAX_LOBBYSIZE; l++)
			{
				allClients[l] = packet.players[l];
			}
			
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

			myClientID = (unsigned int)packet.players[0].playerID;

			printf("INFO:  -- Client accepted by server! We got clientID: %d\n", packet.players[0].playerID);

			
		}
			break;
		case HEARTBEAT_PACKET:
		{
			i += sizeof(ClientReceivePacketLobby);

			printf("INFO:  -- Sending Heartbeat response!\n");
			sendHeartbeatPacket();
		}
			break;
		default:
			printf("ERROR: -- Packet received with Unknown packetType!!\n");
			break;
		}
	}
}
//Disconnect from the server.
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

//Send playerData to the server.
void ClientGame::sendPlayerData(playerData &player, packetTypes type)
{
	const unsigned int packet_size = sizeof(ClientSendPacket);
	char packet_data[packet_size];

	ClientSendPacket packet;
	packet.packet_type = type;
	packet.player = player;

	//Add all set actionTypes to the packet
	for (int i = 0; i < MAX_ACTIONS; i++) {
		if(i < (int)nextActionTypes.size()) packet.action_types[i] = nextActionTypes[i];
		else packet.action_types[i] = ACTION_NONE;
	}

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

//Add an actionType to the next packet send to the server.
void ClientGame::addActionType(actionTypes type)
{
	if (nextActionTypes.size() < MAX_ACTIONS) {
		nextActionTypes.push_back(type);
	}
	else {
		printf("WARNING -- Already added %d actionTypes.\nThis is the maximum to be send!\nLosing last actionType!\n", MAX_ACTIONS);
	}
}

//Get the network error.
//Resets to 0 after call
std::vector<networkingErrors> ClientGame::getErrors()
{
	std::vector<networkingErrors> tmperrors = errors;
	errors.clear();

	return tmperrors;
}