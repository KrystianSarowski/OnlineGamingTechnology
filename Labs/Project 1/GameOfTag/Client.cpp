#include "Client.h"
#include <Ws2tcpip.h>
#include <iostream>
#pragma comment(lib,"ws2_32.lib")
#include "PacketStructure.h"
#include "Game.h"

Client::Client(const char* t_ip, int t_port)
{
	//Winsock Startup
	WSAData wsaData;
	WORD dllVersion = MAKEWORD(2, 1);

	if (WSAStartup(dllVersion, &wsaData) != 0)
	{
		MessageBoxA(NULL, "Winsock startup failed", "Error", MB_OK | MB_ICONERROR);
		exit(0);
	}

	inet_pton(AF_INET, t_ip, &m_addr.sin_addr.s_addr);	//Address (127.0.0.1) = localhost (this pc)
	m_addr.sin_port = htons(t_port);					//Port 
	m_addr.sin_family = AF_INET;						//IPv4 Socket
}

Client::~Client()
{
	closeConnection();
	m_packetSendThread.join();
	m_listenThread.join();
}

bool Client::connectSocket()
{
	m_connection = socket(AF_INET, SOCK_STREAM, 0);
	int sizeofaddr = sizeof(m_addr);

	if (connect(m_connection, (SOCKADDR*)&m_addr, sizeofaddr) != 0)
	{
		MessageBoxA(0, "Failed to Connect", "Error", MB_OK | MB_ICONERROR);
		return false;
	}

	std::cout << "Connected!" << std::endl;

	m_packetSendThread = std::thread(packetSenderThread, std::ref(*this));
	m_packetSendThread.detach();

	m_listenThread = std::thread(clientThread, std::ref(*this));
	m_listenThread.detach();

	return true;
}

bool Client::closeConnection()
{
	m_terminateThreads = true;

	if (closesocket(m_connection) == SOCKET_ERROR)
	{
		if (WSAGetLastError() == WSAENOTSOCK)
		{
			return true;
		}

		std::string ErrorMessage = "Failed to close the socket. Winsock Error: " + std::to_string(WSAGetLastError()) + ".";
		MessageBoxA(0, ErrorMessage.c_str(), "Error", MB_OK | MB_ICONERROR);
		return false;
	}

	return true;
}

bool Client::processPacketType(const PacketType t_packetType)
{
	switch (t_packetType)
	{
	case PacketType::PLAYERSET:
	{
		StartData data;
		if (!getGameStart(data))
		{
			return false;
		}
		m_game->setStartData(data);
		break;
	}
	case PacketType::PLAYERUPDATE:
	{
		break;
	}
	case PacketType::GAMEUPDATE:
	{
		std::array<sf::Vector2f, 3> positions;
		if (!getUpdateGame(positions))
		{
			return false;
		}
		m_game->updateAllPositions(positions);
		break;
	}
	case PacketType::GAMEEND:
	{
		EndData data;
		if (!getGameEnd(data))
		{
			return false;
		}
		m_game->setEndData(data);
		break;
	}
	case PacketType::STATECHANGE:
	{
		GameState changeState;
		if (!getChangeState(changeState))
		{
			return false;
		}
		m_game->changeState(changeState);
		break;
	}

	default:
		std::cout << "Unrecognized PacketType: " << (std::int32_t)t_packetType << std::endl;
		break;
	}

	return true;
}

void Client::clientThread(Client& t_client)
{
	PacketType packetType;
	while (true)
	{
		if (t_client.m_terminateThreads == true)
		{
			break;
		}

		if (!t_client.getPacketType(packetType))
		{
			break;
		}

		if (!t_client.processPacketType(packetType))
		{
			break;
		}
	}

	std::cout << "Lost connection to the server." << std::endl;
	t_client.m_terminateThreads = true;

	if (t_client.closeConnection())
	{
		std::cout << "Socket to the server was closed successfully." << std::endl;
	}
	else
	{
		std::cout << "Socket was not able to be closed." << std::endl;
	}

	t_client.m_game->changeState(GameState::SERVERCLOSED);
}

void Client::packetSenderThread(Client& t_client)
{
	while (true)
	{
		if (t_client.m_terminateThreads == true)
		{
			break;
		}

		while (t_client.m_packetManager.hasPendingPackets())
		{
			std::shared_ptr<Packet> packets = t_client.m_packetManager.retrieve();
			if (!t_client.sendAll((const char*)(&packets->m_buffer[0]), packets->m_buffer.size()))
			{
				std::cout << "Failed to send packet to server..." << std::endl;
				break;
			}
		}
		Sleep(5);
	}

	std::cout << "Packet thread closing..." << std::endl;
}

void Client::disconnect()
{
	m_packetManager.clear();
	closesocket(m_connection);
	std::cout << "Disconnected from server." << std::endl;
}


bool Client::recieveAll(char* t_data, int t_totalBytes)
{
	int bytesReceived = 0;

	while (bytesReceived < t_totalBytes)
	{
		int returnCheck = recv(m_connection, t_data + bytesReceived, t_totalBytes - bytesReceived, NULL);

		if (returnCheck == SOCKET_ERROR)
		{
			return false;
		}

		bytesReceived += returnCheck;
	}

	return true;
}

bool Client::getInt32t(std::int32_t& t_int32t)
{
	if (!recieveAll((char*)&t_int32t, sizeof(std::int32_t)))
	{
		return false;
	}

	t_int32t = ntohl(t_int32t);
	return true;
}

bool Client::getPacketType(PacketType& t_packetType)
{
	std::int32_t packetTypeInt;
	if (!getInt32t(packetTypeInt))
	{
		return false;
	}

	t_packetType = (PacketType)packetTypeInt;
	return true;
}

bool Client::sendAll(const char* t_data, int t_totalBytes)
{
	int bytesSent = 0;

	while (bytesSent < t_totalBytes)
	{
		int returnCheck = send(m_connection, t_data + bytesSent, t_totalBytes - bytesSent, NULL);

		if (returnCheck == SOCKET_ERROR)
		{
			return false;
		}

		bytesSent += returnCheck;
	}

	return true;
}

bool Client::getGameStart(StartData& t_startData)
{
	int32_t bufferlength;
	if (!getInt32t(bufferlength))
	{
		return false;
	}

	if (bufferlength == 0)
	{
		return true;
	}

	return recieveAll((char*)&t_startData, bufferlength);
}

bool Client::getGameEnd(EndData& t_endData)
{
	int32_t bufferlength;
	if (!getInt32t(bufferlength))
	{
		return false;
	}

	if (bufferlength == 0)
	{
		return true;
	}

	return recieveAll((char*)&t_endData, bufferlength);
}

bool Client::getChangeState(GameState& t_changeState)
{
	int32_t bufferlength;
	if (!getInt32t(bufferlength))
	{
		return false;
	}

	if (bufferlength == 0)
	{
		return true;
	}

	return recieveAll((char*)&t_changeState, bufferlength);
}

bool Client::getUpdateGame(std::array<sf::Vector2f, 3>& t_positions)
{
	int32_t bufferlength;
	if (!getInt32t(bufferlength))
	{
		return false;
	}

	if (bufferlength == 0)
	{
		return true;
	}

	return recieveAll((char*)&t_positions[0], bufferlength);;
}

void Client::sendPlayerUpdate(const PlayerData& t_updateData)
{
	PS::PlayerUpdate gameUpdate(t_updateData);
	m_packetManager.append(gameUpdate.toPacket());
}

void Client::sendGameStart(const StartData& t_startData)
{
	PS::GameStart gameStart(t_startData);
	m_packetManager.append(gameStart.toPacket());
}