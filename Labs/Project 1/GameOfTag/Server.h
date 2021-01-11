#pragma once
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define NOMINMAX
#include <WinSock2.h> 
#include <string>
#include "PacketManager.h" 
#include <vector> 
#include <shared_mutex> 

class Game;

class Connection
{
public:
	Connection(SOCKET t_socket)
		:m_socket(t_socket)
	{
	}

	SOCKET m_socket;
	PacketManager m_packetManager;
	int m_id = 0;
};

class Server
{
public:

	Server(int t_port, bool t_broadcastPublically = false);
	~Server();
	
	bool listenForNewConnection();
	
	int m_idCounter = 0;
	int m_activeConnectionsCount = 0;

	bool getint32t(std::shared_ptr<Connection> t_connection, std::int32_t& t_int32t);
	bool getPacketType(std::shared_ptr<Connection> t_connection, PacketType& t_packetType);

	void sendString(std::shared_ptr<Connection> t_connection, const std::string& t_string);
	bool getString(std::shared_ptr<Connection> t_connection, std::string& t_string);

	void sendPlayerUpdate(PlayerData& t_updateData);
	bool getPlayerUpdate(std::shared_ptr<Connection> t_connection, PlayerData& t_updateData);

	void sendGameStart(StartData& t_startData, int t_index);
	bool getGameStart(std::shared_ptr<Connection> t_connection, StartData& t_startData);

	void sendGameEnd(EndData& t_endData);

	void sendGameUpdate(std::array<sf::Vector2f, 3>& t_positions);

	void sendChangeState(const GameState& t_gameState);

	Game* m_game;

private:

	bool sendAll(std::shared_ptr<Connection> t_connection, const char* t_data, int t_totalBytes);
	bool recieveAll(std::shared_ptr<Connection> t_connection, char* t_data, int t_totalBytes);

	bool processPacket(std::shared_ptr<Connection> t_connection, PacketType t_packetType);

	static void clientHandlerThread(Server& t_server, std::shared_ptr<Connection> t_connection);
	static void packetSenderThread(Server& t_server);

	void disconnectClient(std::shared_ptr<Connection> t_connection);

	std::vector<std::shared_ptr<Connection>> m_connections;
	std::shared_mutex m_mutexConnectionManager;

	SOCKADDR_IN m_addr;
	SOCKET m_listener;

	bool m_terminateThreads = false;
	std::vector<std::thread*> m_threads;
};