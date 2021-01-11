#pragma once

enum class PacketType
{
	MESSAGE,
	PLAYERSET,
	STATECHANGE,
	PLAYERUPDATE,
	GAMEUPDATE,
	GAMEEND
};

enum class GameState
{
	SELECT,
	WAITING,
	START,
	GAMEPLAY,
	GAMEOVER
};