#pragma once
#include "Client.h"
#include "Server.h"
#include <iostream>
#include "Player.h"
#include <iomanip>
#include <sstream>
#include <array>

class Game
{
public:

	Game();
	void run();

	//Functions used for accepting incoming data.
	void updatePlayer(PlayerData t_data);
	void updateAllPositions(std::array<sf::Vector2f, 3> t_positions);
	void setStartData(StartData t_data);
	void setEndData(EndData t_data);
	void changeState(GameState t_changeState);

	void loadFont();

private:

	//Update Functions.
	void update(sf::Time dt);
	void updateGameplay(sf::Time t_dt);
	void updateWaiting(sf::Time t_dt);
	void updateStart(sf::Time t_dt);

	//Process input functions.
	void processEvents();
	void processSelectIP(sf::Event t_event);

	//Render Functions.
	void render();

	//Server Functions.
	void startServer(bool t_broadcastPublicaly);
	void listenForConnections();

	//Client Functions.
	void connect();

	void reset();

	//Game UI functions.
	void setupUI();
	void centreText();

	//Collision Functions.
	bool checkForCollisions();

	sf::RenderWindow m_window;

	bool m_host{ false };
	bool m_centreText{ false };

	int m_playerIndex{ -1 };
	int m_targetIndex{ -1 };

	Player m_player[3];

	GameState m_state;

	Client* m_client{ nullptr };
	Server* m_server{ nullptr };

	std::string m_ipInputString;

	std::array<char, 11> m_ipValidValues = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '.' };

	std::thread* m_serverThread;

	sf::Time m_timeLasted;
	sf::Time m_countDownTime;

	sf::RectangleShape m_hostButton;
	sf::RectangleShape m_clientButton;
	sf::RectangleShape m_ipConfirmButton;
	sf::RectangleShape m_broadcastYesButton;
	sf::RectangleShape m_broadcastNoButton;

	sf::Text m_hostText;
	sf::Text m_clientText;
	sf::Text m_ipConfirmText;

	sf::Text m_startText;
	sf::Text m_waitText;
	sf::Text m_endText;

	sf::Text m_gameStartText;
	sf::Text m_gameOverText;
	sf::Text m_serverClosedText;
	sf::Text m_timeLastedText;

	sf::Text m_ipHelpText;
	sf::Text m_ipValueText;

	sf::Text m_broadcastPubliclyText;
	sf::Text m_broadcastYesText;
	sf::Text m_broadcastNoText;

	sf::Font m_font;
};