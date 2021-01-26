#pragma once
#include "Packet.h"
#include <string> 
#include <memory> 
#include <array>

namespace PS
{
	class ChangeState
	{
	public:
		ChangeState(const GameState& t_state);
		std::shared_ptr<Packet> toPacket();
	private:

		GameState m_state;
	};

	class GameStart
	{
	public:
		GameStart(const StartData& t_startData);
		std::shared_ptr<Packet> toPacket();

	private:

		StartData m_data;
	};

	class PlayerUpdate
	{
	public:
		PlayerUpdate(const PlayerData& t_updateData);
		std::shared_ptr<Packet> toPacket();
	private:

		PlayerData m_data;
	};

	class GameUpdate
	{
	public:
		GameUpdate(const std::array<sf::Vector2f, 3>& t_positions);
		std::shared_ptr<Packet> toPacket();
	private:

		std::array<sf::Vector2f, 3> m_positions;
	};

	class GameEnd
	{
	public:
		GameEnd(const EndData& t_endData);
		std::shared_ptr<Packet> toPacket();

	private:

		EndData m_data;
	};
}