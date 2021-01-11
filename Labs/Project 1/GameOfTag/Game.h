#pragma once
#include "Client.h"
#include "Server.h"
#include <iostream>
#include "Player.h"
#include <iomanip>
#include <sstream>

class Game
{
public:

	Game();
	void run();
	void updatePlayer(PlayerData t_data);
	void updateAllPositions(std::array<sf::Vector2f, 3> t_positions);
	void setStartData(StartData t_data);
	void setEndData(EndData t_data);
	void changeState(GameState t_changeState);

	void loadFont();

private:

	void update(sf::Time dt);
	void processEvents();
	void render();
	void startServer();
	void connect();
	void centreText();
	void ListenForConnections();
	bool checkForCollisions();
	void reset();
	float getDistance(sf::Vector2f t_pos1, sf::Vector2f t_pos2);

	sf::RenderWindow m_window;

	Player m_player[3];
	GameState m_state;

	std::thread* m_serverThread;
	sf::Time m_timeLasted;
	sf::Time m_countDownTime;

	bool m_host{ false };
	bool m_centreText{ false };

	int m_playerIndex{ -1 };
	int m_targetIndex{ -1 };

	Client* m_client{ nullptr };
	Server* m_server{ nullptr };

	sf::RectangleShape m_hostButton;
	sf::RectangleShape m_clientButton;

	sf::Text m_hostText;
	sf::Text m_clientText;

	sf::Text m_startText;
	sf::Text m_waitText;
	sf::Text m_endText;

	sf::Text m_gameStartText;
	sf::Text m_gameOverText;
	sf::Text m_timeLastedText;

	sf::Font m_font;
};