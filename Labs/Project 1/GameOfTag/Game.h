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

	//Upates the position of the player on map with the passed in struct.
	void updatePlayer(PlayerData t_data);

	//Sets the position of all the players within the game to the the passed in positions.
	void updateAllPositions(std::array<sf::Vector2f, 3> t_positions);

	//Sets the data from the passed in struct to display the approperate message and
	//set all the vraiables correctly.
	void setStartData(StartData t_data);

	//Sets the data from the passed in struct to display the approperate message.
	void setEndData(EndData t_data);

	//Changes the state of the game to the passed in state.
	void changeState(GameState t_changeState);

	void loadFont();

private:

	//Update Functions.
	void update(sf::Time dt);

	//Updates the position of all Players on map if this game is a host and sends it to other Clients.
	//Otherwise sends the velocity player to the client.
	void updateGameplay(sf::Time t_dt);

	//Does nothing while waithin for more Players to connect.
	//If the game is server it sends change state message to other Clients.
	void updateWaiting(sf::Time t_dt);

	//Counts down the time while the while the game is server and when the 
	//time reaches 0 the state of this game and other Clients to gameplay.
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

	//Window in which all the UI and game object are drawn.
	sf::RenderWindow m_window;

	//Bool for if this game is ran by the host.
	bool m_host{ false };

	//Bool for if we need to re centre text on screen.
	bool m_centreText{ false };

	//The index of the Player that is controled by this user.
	int m_playerIndex{ -1 };

	//The index of the Player that needs to be caught in order for the game to end.
	int m_targetIndex{ -1 };

	//All the Player that exist in the game.
	Player m_player[3];

	//The current state the game is in.
	GameState m_state;

	//Pointer to an instace of the client.
	Client* m_client{ nullptr };

	//Pointer to an instance of the server.
	Server* m_server{ nullptr };

	//The string which stores the ip entered by the user.
	std::string m_ipInputString;

	//Array used to filter the values for the ip input.
	std::array<char, 11> m_ipValidValues = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '.' };

	//Thread on which the server runs.
	std::thread* m_serverThread;

	//The time for which the running Player ha lasted for.
	sf::Time m_timeLasted;

	//The delay before the game starts after all Players connected.
	sf::Time m_countDownTime;

	//Rectangle shapes used to represent all the UI buttons.
	sf::RectangleShape m_hostButton;
	sf::RectangleShape m_clientButton;
	sf::RectangleShape m_ipConfirmButton;
	sf::RectangleShape m_broadcastYesButton;
	sf::RectangleShape m_broadcastNoButton;

	//Text used to display the verious UI text messages.
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