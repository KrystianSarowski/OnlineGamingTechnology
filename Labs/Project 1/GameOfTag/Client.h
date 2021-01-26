#pragma once
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define NOMINMAX
#include <WinSock2.h> 
#include <string> 
#include <fstream>
#include "PacketManager.h" 

class Game;

class Client
{
public:

	Client(const char* t_ip, int t_port);
	~Client();

	bool connectSocket();
	void sendPlayerUpdate(const PlayerData& t_updateData);
	void sendGameStart(const StartData& t_startData);

	void disconnect();

	Game* m_game;

private:

	bool closeConnection();
	bool processPacketType(const PacketType t_packetType);

	static void clientThread(Client& t_client);
	static void packetSenderThread(Client& t_client);

	//Send Functions
	bool sendAll(const char* t_data, int t_totalBytes);

	//Get Functions
	bool recieveAll(char* t_data, int t_totalBytes);
	bool getInt32t(std::int32_t& t_int32t);
	bool getPacketType(PacketType& t_packetType);
	bool getGameStart(StartData& t_startData);
	bool getGameEnd(EndData& t_endData);
	bool getChangeState(GameState& t_changeState);
	bool getUpdateGame(std::array<sf::Vector2f, 3>& t_positions);

	//Variables
	bool m_terminateThreads = false;
	bool m_isConnected = false;

	SOCKET m_connection;
	SOCKADDR_IN m_addr;
	PacketManager m_packetManager;

	std::thread m_packetSendThread;
	std::thread m_listenThread;
};