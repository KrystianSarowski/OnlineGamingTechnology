#include "Game.h"

Game::Game() :
	m_window{ sf::VideoMode{ 500, 500, 32 }, "Project 1" },
	m_state(GameState::SELECT)
{
	m_player[0].setColour(sf::Color::Blue);
	m_player[0].setName("Blue");
	m_player[0].setStartPosition(sf::Vector2f(100.0f, 100.0f));

	m_player[1].setColour(sf::Color::Red);
	m_player[1].setName("Red");
	m_player[1].setStartPosition(sf::Vector2f(450.0f, 200.0f));

	m_player[2].setColour(sf::Color::Yellow);
	m_player[2].setName("Yellow");
	m_player[2].setStartPosition(sf::Vector2f(200.0f, 450.0f));

	reset();
	loadFont();

	m_hostButton.setSize(sf::Vector2f(150.0f, 50.0f));
	m_hostButton.setOrigin(sf::Vector2f(75.0f, 25.0f));
	m_hostButton.setOutlineThickness(5.0f);
	m_hostButton.setFillColor(sf::Color(168, 142, 81));
	m_hostButton.setPosition(sf::Vector2f(125.0f, 250.0f));

	m_clientButton.setSize(sf::Vector2f(150.0f, 50.0f));
	m_clientButton.setOrigin(sf::Vector2f(75.0f, 25.0f));
	m_clientButton.setOutlineThickness(5.0f);
	m_clientButton.setFillColor(sf::Color(168, 142, 81));
	m_clientButton.setPosition(sf::Vector2f(375.0f, 250.0f));

	m_hostText.setString("Host");
	m_hostText.setFont(m_font);
	m_hostText.setCharacterSize(30);
	m_hostText.setOrigin(sf::Vector2f(m_hostText.getLocalBounds().width / 2, m_hostText.getLocalBounds().height / 1.3f));
	m_hostText.setPosition(m_hostButton.getPosition());
	
	m_clientText.setString("Join");
	m_clientText.setFont(m_font);
	m_clientText.setCharacterSize(30);
	m_clientText.setOrigin(sf::Vector2f(m_clientText.getLocalBounds().width / 2, m_clientText.getLocalBounds().height / 1.3f));
	m_clientText.setPosition(m_clientButton.getPosition());

	m_gameStartText.setString("Game Is About To Start");
	m_gameStartText.setFont(m_font);
	m_gameStartText.setCharacterSize(30);
	m_gameStartText.setOrigin(sf::Vector2f(m_gameStartText.getLocalBounds().width / 2, m_gameStartText.getLocalBounds().height / 2));
	m_gameStartText.setPosition(sf::Vector2f(250.0f, 25.0f));

	m_gameOverText.setString("Game Over");
	m_gameOverText.setFont(m_font);
	m_gameOverText.setCharacterSize(30);
	m_gameOverText.setOrigin(sf::Vector2f(m_gameOverText.getLocalBounds().width / 2, m_gameOverText.getLocalBounds().height / 2));
	m_gameOverText.setPosition(sf::Vector2f(250.0f, 25.0f));

	m_waitText.setString("Waiting For More Players...");
	m_waitText.setFont(m_font);
	m_waitText.setCharacterSize(30);
	m_waitText.setOrigin(sf::Vector2f(m_waitText.getLocalBounds().width / 2, m_waitText.getLocalBounds().height / 2));
	m_waitText.setPosition(sf::Vector2f(250.0f, 125.0f));

	m_endText.setFont(m_font);
	m_endText.setCharacterSize(25);

	m_startText.setFont(m_font);
	m_startText.setCharacterSize(25);

	m_timeLastedText.setFont(m_font);
	m_timeLastedText.setCharacterSize(25);
}

void Game::run()
{
	sf::Clock clock;
	sf::Time timeSinceLastUpdate = sf::Time::Zero;
	sf::Time timePerFrame = sf::seconds(1.f / 60.f);	//60 fps

	while (m_window.isOpen())
	{
		processEvents();								//As many as possible
		timeSinceLastUpdate += clock.restart();
		while (timeSinceLastUpdate > timePerFrame)
		{
			timeSinceLastUpdate -= timePerFrame;
			processEvents();							//At least 60 fps
			update(timePerFrame);						//60 fps
		}
		render();										//As many as possible
	}
}

void Game::updatePlayer(PlayerData t_data)
{
	m_player[t_data.m_playerIndex].updatePosition(t_data.m_position);
}

void Game::updateAllPositions(std::array<sf::Vector2f, 3> t_positions)
{
	for (int i = 0; i < 3; i++)
	{
		m_player[i].setPosition(t_positions[i]);
	}
}

void Game::setStartData(StartData t_data)
{
	changeState(GameState::START);

	m_targetIndex = t_data.m_targetIndex;
	m_playerIndex = t_data.m_playerIndex;

	std::string string;

	string = "You are " + m_player[m_playerIndex].getName() + ".";

	if (m_targetIndex == m_playerIndex)
	{
		string += " Try to run away.";
	}
	else
	{
		string += " Try to catch " + m_player[m_targetIndex].getName() + ".";
	}

	m_startText.setString(string);

	m_centreText = true;
}

void Game::setEndData(EndData t_data)
{
	changeState(GameState::GAMEOVER);

	std::stringstream timeStream;

	timeStream << std::fixed << std::setprecision(2) << t_data.m_timeLasted.asSeconds();

	std::string timeString = timeStream.str();

	if (t_data.m_playerIndex == m_playerIndex)
	{
		m_endText.setString("You caught " + m_player[t_data.m_targetIndex].getName());
		m_timeLastedText.setString(m_player[t_data.m_targetIndex].getName() + " " + timeString + "s");
	}
	else if(t_data.m_targetIndex == m_playerIndex)
	{
		m_endText.setString(m_player[t_data.m_playerIndex].getName() + " has caught you");
		m_timeLastedText.setString("You Lasted " + timeString + "s");
	}
	else
	{ 
		m_endText.setString(m_player[t_data.m_playerIndex].getName() + " has caught " + m_player[t_data.m_targetIndex].getName());
		m_timeLastedText.setString(m_player[t_data.m_targetIndex].getName() + " " + timeString + "s");
	}

	m_centreText = true;
}

void Game::changeState(GameState t_changeState)
{
	m_state = t_changeState;

	if (m_state == GameState::WAITING)
	{
		reset();
	}
}

void Game::loadFont()
{
	if (!m_font.loadFromFile("assets/fonts/ariblk.ttf"))
	{
		std::cout << "Failed loading ariblk.ttf font file" << std::endl;
	}
	else
	{
		std::cout << "Successfully loaded ariblk.ttf font file" << std::endl;
	}
}

void Game::update(sf::Time dt)
{
	if (m_state == GameState::GAMEPLAY)
	{	
		sf::Vector2f moveVector = m_player[m_playerIndex].move();

		if (m_host)
		{
			if (m_server->m_activeConnectionsCount < 2)
			{
				changeState(GameState::WAITING);
				m_server->sendChangeState(GameState::WAITING);
				return;
			}

			m_player[m_playerIndex].updatePosition(moveVector);
			m_timeLasted += dt;

			if (!checkForCollisions())
			{
				std::array<sf::Vector2f, 3> positions;

				for (int i = 0; i < 3; i++)
				{
					positions[i] = m_player[i].getPosition();
				}

				m_server->sendGameUpdate(positions);
			}
		}
		else
		{
			PlayerData data;
			data.m_playerIndex = m_playerIndex;
			data.m_position = moveVector;
			m_client->sendPlayerUpdate(data);
		}
	}

	else if (m_state == GameState::WAITING)
	{
		if (m_host)
		{
			if (m_server->m_activeConnectionsCount == 2)
			{
				m_countDownTime = sf::seconds(3.0f);
				m_state = GameState::START;

				int playerIndex = rand() % 3;
				int targetIndex = rand() % 3;

				m_playerIndex = playerIndex;
				m_targetIndex = targetIndex;

				std::string string;

				string = "You are " + m_player[m_playerIndex].getName() + ".";

				if (m_targetIndex == m_playerIndex)
				{
					string += " Try to run away.";
				}
				else
				{
					string += " Try to catch " + m_player[m_targetIndex].getName() + ".";
				}

				m_startText.setString(string);

				m_centreText = true;

				StartData startData;
				startData.m_targetIndex = targetIndex;

				for (int i = 0; i < 2; i++)
				{
					playerIndex = (playerIndex + 1) % 3;
					startData.m_playerIndex = playerIndex;
					m_server->sendGameStart(startData, i);
				}		
			}
		}
	}

	else if (m_state == GameState::START)
	{
		if (m_host)
		{
			m_countDownTime -= dt;

			if (m_countDownTime <= sf::seconds(0.0f))
			{
				m_state = GameState::GAMEPLAY;
				m_server->sendChangeState(GameState::GAMEPLAY);
			}
		}
	}
}

void Game::processEvents()
{
	sf::Event event;

	while (m_window.pollEvent(event))
	{
		sf::Vector2f m_pos = m_window.mapPixelToCoords(sf::Mouse::getPosition(m_window));

		if (m_state == GameState::SELECT)
		{
			if (event.type == sf::Event::MouseButtonPressed)
			{
				if (event.key.code == sf::Mouse::Left)
				{
					if (m_hostButton.getGlobalBounds().contains(m_pos))
					{
						startServer();
						m_state = GameState::WAITING;
					}
					else if (m_clientButton.getGlobalBounds().contains(m_pos))
					{
						connect();
						m_state = GameState::WAITING;
					}
				}
			}
		}

		if (event.type == sf::Event::KeyPressed)
		{
			switch (event.key.code)
			{
			case sf::Keyboard::Escape:
				m_window.close();
				break;
			default:
				break;
			}
		}

		if (event.type == sf::Event::Closed)
		{
			m_window.close();
		}
	}
}

void Game::render()
{
	m_window.clear();

	switch (m_state)
	{
	case GameState::SELECT:
		m_window.draw(m_hostButton);
		m_window.draw(m_clientButton);
		m_window.draw(m_hostText);
		m_window.draw(m_clientText);
		break;
	case GameState::WAITING:
		m_window.draw(m_waitText);
		break;
	case GameState::START:
		if (m_centreText)
		{
			centreText();
		}
		m_window.draw(m_gameStartText);
		m_window.draw(m_startText);
		break;
	case GameState::GAMEPLAY:
		for (int i = 0; i < 3; i++)
		{
			m_player[i].render(m_window);
		}
		break;
	case GameState::GAMEOVER:
		if (m_centreText)
		{
			centreText();
		}
		m_window.draw(m_gameOverText);
		m_window.draw(m_endText);
		m_window.draw(m_timeLastedText);
		break;
	default:
		break;
	}

	m_window.display();
}

void Game::startServer()
{
	m_host = true;
	m_server = new Server(1111);
	m_server->m_game = this;

	std::thread serverThread(&Game::ListenForConnections, this);
	serverThread.detach();

	m_serverThread = &serverThread;
}

void Game::connect()
{
	m_client = new Client("127.0.0.1", 1111);
	m_client->m_game = this;

	if (!m_client->connectSocket())
	{
		m_client->disconnect();
		m_window.close();
	}
}

void Game::centreText()
{
	m_endText.setOrigin(sf::Vector2f(m_endText.getLocalBounds().width / 2, m_endText.getLocalBounds().height / 2));
	m_endText.setPosition(sf::Vector2f(250.0f, 75.0f));

	m_gameOverText.setOrigin(sf::Vector2f(m_gameOverText.getLocalBounds().width / 2, m_gameOverText.getLocalBounds().height / 2));
	m_gameOverText.setPosition(sf::Vector2f(250.0f, 25.0f));

	m_gameStartText.setOrigin(sf::Vector2f(m_gameStartText.getLocalBounds().width / 2, m_gameStartText.getLocalBounds().height / 2));
	m_gameStartText.setPosition(sf::Vector2f(250.0f, 25.0f));

	m_startText.setOrigin(sf::Vector2f(m_startText.getLocalBounds().width / 2, m_startText.getLocalBounds().height / 2));
	m_startText.setPosition(sf::Vector2f(250.0f, 75.0f));

	m_timeLastedText.setOrigin(sf::Vector2f(m_timeLastedText.getLocalBounds().width / 2, m_timeLastedText.getLocalBounds().height / 2));
	m_timeLastedText.setPosition(sf::Vector2f(250.0f, 125.0f));

	m_centreText = false;
}

void Game::ListenForConnections()
{
	while(m_server->m_activeConnectionsCount < 10)
	{
		m_server->listenForNewConnection();
	}
}

bool Game::checkForCollisions()
{
	for (int i = 0; i < 3; i++)
	{
		if (m_targetIndex != i)
		{
			if (getDistance(m_player[m_targetIndex].getPosition(), m_player[i].getPosition()) <= Player::s_radius * 2)
			{
				EndData endData;
				endData.m_playerIndex = i;
				endData.m_targetIndex = m_targetIndex;
				endData.m_timeLasted = m_timeLasted;

				m_server->sendGameEnd(endData);
				setEndData(endData);

				return true;
			}
		}
	}

	return false;
}

void Game::reset()
{
	for (int i = 0; i < 3; i++)
	{
		m_player[i].setPosition(m_player[i].getStartPosition());
	}

	m_timeLasted = sf::seconds(0.0f);
}

float Game::getDistance(sf::Vector2f t_pos1, sf::Vector2f t_pos2)
{
	return sqrt(powf(t_pos1.x - t_pos2.x, 2) + powf(t_pos1.y - t_pos2.y, 2));
}
