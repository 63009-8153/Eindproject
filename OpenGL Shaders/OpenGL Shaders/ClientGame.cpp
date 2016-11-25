#include "NetworkHeader.h"

#include "Header.h"

#include "networkData.h"
#include "ClientNetwork.h"

#include "ClientGame.h"

//Constructor
ClientGame::ClientGame() {

}
ClientGame::ClientGame(char ipAddress[39], char port[5])
{
	network = new ClientNetwork(ipAddress, port);
	error = network->getError();
}
//Destructor
ClientGame::~ClientGame()
{
}

void ClientGame::setNetworkAddress(char ipAddress[39], char port[5])
{
	//Remove existing clientNetwork
	network->~ClientNetwork();

	//Create a new clientNetwork with IP-Address and Port
	network = new ClientNetwork(ipAddress, port);
	error = network->getError();

	if (error == 0) {
		char name[10] = "Wouter140";

		playerData player;
		memcpy(&player.playerName, name, strlen(name));
		addActionType(LOBBY_JOIN);

		sendPlayerData(player);
	}
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

		printf("PacketType: %ud", packetType);

		switch (packetType) {

		case LOBBY_PACKET:
		{
			ClientReceivePacketLobby packetLobby;
			packetLobby.deserialize(&(network_data[i]));
			i += sizeof(ClientReceivePacketLobby);

			if (packetLobby.action_types[0] == LOBBY_JOIN) printf("Hello from server!");
		}
			break;
		case GAME_PACKET:
		{
			ClientReceivePacket packetGame;
			packetGame.deserialize(&(network_data[i]));
			i += sizeof(ClientReceivePacket);
		}
			break;
		default:
			printf("Unknown packetType!!");
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
void ClientGame::sendPlayerData(playerData &player)
{
	const unsigned int packet_size = sizeof(ClientSendPacket);
	char packet_data[packet_size];

	ClientSendPacket packet;
	packet.packet_type = GAME_PACKET;
	packet.player = player;

	//Add all set actionTypes to the packet
	for (int i = 0; i < std::min((int)nextActionTypes.size(), MAX_ACTIONS); i++) {
		packet.action_types[i] = nextActionTypes[i];
	}

	nextActionTypes.clear();

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
		printf("Already added %d actionTypes.\nThis is the maximum to be send!\nLosing last actionType!\n", MAX_ACTIONS);
	}
}

//Get the network error.
//Resets to 0 after call
int ClientGame::getError()
{
	int tmperror = error;
	error = 0;
	return tmperror;
}