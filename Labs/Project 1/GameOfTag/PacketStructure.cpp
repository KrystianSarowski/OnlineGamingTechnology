#include "PacketStructure.h"

PS::PlayerUpdate::PlayerUpdate(const PlayerData& t_updateData) :
	m_data(t_updateData)
{
}

std::shared_ptr<Packet> PS::PlayerUpdate::toPacket()
{
	std::shared_ptr<Packet> packet = std::make_shared<Packet>();
	packet->append(PacketType::PLAYERUPDATE);
	packet->append(sizeof(m_data));
	packet->append(m_data);
	return packet;
}

PS::ChangeState::ChangeState(const GameState& t_state):
	m_state(t_state)
{
}

std::shared_ptr<Packet> PS::ChangeState::toPacket()
{
	std::shared_ptr<Packet> packet = std::make_shared<Packet>();
	packet->append(PacketType::STATECHANGE);
	packet->append(sizeof(m_state));
	packet->append(m_state);
	return packet;
}

PS::GameStart::GameStart(const StartData& t_startData):
	m_data(t_startData)
{
}

std::shared_ptr<Packet> PS::GameStart::toPacket()
{
	std::shared_ptr<Packet> packet = std::make_shared<Packet>();
	packet->append(PacketType::PLAYERSET);
	packet->append(sizeof(m_data));
	packet->append(m_data);
	return packet;
}

PS::GameUpdate::GameUpdate(const std::array<sf::Vector2f, 3>& t_positions):
	m_positions(t_positions)
{
}

std::shared_ptr<Packet> PS::GameUpdate::toPacket()
{
	std::shared_ptr<Packet> packet = std::make_shared<Packet>();
	packet->append(PacketType::GAMEUPDATE);
	packet->append(sizeof(m_positions));
	packet->append(m_positions);
	return packet;
}

PS::GameEnd::GameEnd(const EndData& t_endData):
	m_data(t_endData)
{

}

std::shared_ptr<Packet> PS::GameEnd::toPacket()
{
	std::shared_ptr<Packet> packet = std::make_shared<Packet>();
	packet->append(PacketType::GAMEEND);
	packet->append(sizeof(m_data));
	packet->append(m_data);
	return packet;
}
