#include "Packet.h"
#include <WinSock2.h>

Packet::Packet()
{
}

Packet::Packet(const char* t_buffer, const int t_size)
{
	append(t_buffer, t_size);
}

Packet::Packet(const PacketType t_packetType)
{
	append(t_packetType);
}

Packet::Packet(const std::shared_ptr<Packet> t_packet)
{
	append(t_packet);
}

void Packet::append(const std::shared_ptr<Packet> t_packet)
{
	append((const char*)&(t_packet->m_buffer[0]), t_packet->m_buffer.size());
}

void Packet::append(const PacketType t_packetType)
{
	append((std::int32_t)t_packetType);
}

void Packet::append(const std::int32_t t_int32)
{
	std::int32_t value = htonl((std::int32_t)t_int32);
	append((const char*)&value, sizeof(std::int32_t));
}

void Packet::append(const std::size_t t_index)
{
	append((std::int32_t)t_index);
}

void Packet::append(const Packet& t_packet)
{
	append((const char*)&t_packet.m_buffer, t_packet.m_buffer.size());
}

void Packet::append(const std::string& t_string)
{
	append(t_string.c_str(), t_string.size());
}

void Packet::append(const char* t_buffer, const int t_size)
{
	m_buffer.insert(m_buffer.end(), t_buffer, t_buffer + t_size);
}

void Packet::append(const PlayerData& t_updateData)
{
	append((const char*)&t_updateData, sizeof(PlayerData));
}

void Packet::append(const GameState& t_state)
{
	append((const char*)&t_state, sizeof(GameState));
}

void Packet::append(const StartData& t_startData)
{
	append((const char*)&t_startData, sizeof(StartData));
}

void Packet::append(const EndData& t_endData)
{
	append((const char*)&t_endData, sizeof(EndData));
}

void Packet::append(const std::array<sf::Vector2f, 3> t_positions)
{
	append((const char*)&t_positions[0], sizeof(t_positions));
}
