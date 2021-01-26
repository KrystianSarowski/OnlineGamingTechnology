#pragma once
#include <cstdint>
#include <vector>
#include <string>
#include "Enums.h"
#include <SFML/Graphics.hpp>
#include <array>

struct PlayerData
{
	int m_playerIndex;
	sf::Vector2f m_velocity;
};

struct StartData
{
	int m_playerIndex;
	int m_targetIndex;
};

struct EndData
{
	int m_targetIndex;
	int m_playerIndex;

	sf::Time m_timeLasted;
};

class Packet
{
public:
	Packet();
	Packet(const char* t_buffer, const int t_size);
	Packet(const PacketType t_packetType);
	Packet(const std::shared_ptr<Packet> t_packet);

	void append(const std::shared_ptr<Packet> t_packet);
	void append(const PacketType t_packetType);
	void append(const std::int32_t t_int32);
	void append(const std::size_t t_index);
	void append(const Packet& t_packet);
	void append(const std::string& t_string);
	void append(const char* t_buffer, const int t_size);
	void append(const PlayerData& t_updateData);
	void append(const GameState& t_state);
	void append(const StartData& t_startData);
	void append(const EndData& t_endData);
	void append(const std::array<sf::Vector2f, 3> t_positions);

	std::vector<int8_t> m_buffer;
};