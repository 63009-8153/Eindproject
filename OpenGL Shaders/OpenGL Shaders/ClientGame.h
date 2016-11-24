#pragma once
class ClientGame
{
public:
	//Constructor
	ClientGame();
	//Destructor
	~ClientGame();

	//Update the client with server data.
	void updateClient();
	//Disconnect from the server.
	void disconnect();

	//Send playerData to the server.
	void sendPlayerData(playerData &player);

	//Get the network error.
	//Resets to 0 after call
	int getError();

	//ClientNetwork that controls the connection to the server.
	ClientNetwork* network;

private:
	int error;

	char network_data[MAX_PACKET_SIZE];

};

