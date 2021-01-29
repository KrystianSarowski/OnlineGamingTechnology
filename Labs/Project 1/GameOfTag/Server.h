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

	//Socket to which the client is bound to.
	SOCKET m_socket;

	//The manager of all the packets that the client has sent/
	PacketManager m_packetManager;

	//The unique id of the connection.
	int m_id = 0;
};

class Server
{
public:

	Server(int t_port, bool t_broadcastPublically = false);
	~Server();
	
	//Listens for a new client and binds it to a socket if a connection is established succesfuly.
	//The connection is not established fully if there are already two active connections.
	bool listenForNewConnection();
	
	//The unique identifer counter that is given to each of the connections.
	int m_idCounter = 0;

	//The number of active connections within the connections vector.
	int m_activeConnectionsCount = 0;

	//Recives the int32 value from the buffer of the passed in connection. Returns if it was succesful.
	bool getint32t(std::shared_ptr<Connection> t_connection, std::int32_t& t_int32t);

	//Recives the PacketType value from the buffer of the passed in connection. Returns if it was succesful.
	bool getPacketType(std::shared_ptr<Connection> t_connection, PacketType& t_packetType);

	//Recives the PlayerData value from the buffer of the passed in connection. Returns if it was succesful.
	bool getPlayerUpdate(std::shared_ptr<Connection> t_connection, PlayerData& t_updateData);

	//Sends all the game start data to the particular connection using the index.
	void sendGameStart(StartData& t_startData, int t_index);

	//Sends all the end data as a packet to each of he connections.
	void sendGameEnd(EndData& t_endData);

	//Sends all the positions as a Gameupdate packet to each of the connections.
	void sendGameUpdate(std::array<sf::Vector2f, 3>& t_positions);

	//Converts the Gamestate in a class and then converts that class into a packet of integers
	//and sends it to each of the Clients within the connections.
	void sendChangeState(const GameState& t_gameState);

	//A pointer to a instance of the game.
	Game* m_game;

private:

	//Sends all the data as bytes to the passed in connection.
	bool sendAll(std::shared_ptr<Connection> t_connection, const char* t_data, int t_totalBytes);

	//Recives the enteried buffer of that from the passed in connection as bytes within the range of the passed in size.
	bool recieveAll(std::shared_ptr<Connection> t_connection, char* t_data, int t_totalBytes);

	//Process the packet recived from a client. If it is of a valid packet type it gets converted
	//back from the buffer to the orignal data and applied to the instance of the game.
	bool processPacket(std::shared_ptr<Connection> t_connection, PacketType t_packetType);

	//Function that runs on a thread that is used to get and process the packets recived
	//from a particular Client.
	static void clientHandlerThread(Server& t_server, std::shared_ptr<Connection> t_connection);

	//Function that runs on a thread that is used to send packets to each of the connections as
	//long as there are still pending packets within the packet manager.
	static void packetSenderThread(Server& t_server);

	//Removes the connection of a client that has disconnected or to whom the connection was lost from the
	//vector of all the binded connections.
	void disconnectClient(std::shared_ptr<Connection> t_connection);

	//A vector of all the Clients connected to the server who the server has bound to a particular socket.
	std::vector<std::shared_ptr<Connection>> m_connections;

	//Mutex used to lock out the connections vector such that we prevent other threads from doing work on it.
	std::shared_mutex m_mutexConnectionManager;

	SOCKADDR_IN m_addr;	//The address of the socket.
	SOCKET m_listener;	//The socket on which the server is listening for new connections.

	//Bool for if all the threads should be terminated.
	bool m_terminateThreads = false;
	std::vector<std::thread*> m_threads;	//A vector of all the threads that are ran on the server.
};