#include "Game.h"

Game::Game() :
	m_window{ sf::VideoMode{ 600, 600, 32 }, "Project 1" },
	m_state(GameState::SELECT)
{
	m_player[0].setColour(sf::Color::Blue);
	m_player[0].setName("Blue");
	m_player[0].setStartPosition(sf::Vector2f(100.0f, 100.0f));

	m_player[1].setColour(sf::Color::Red);
	m_player[1].setName("Red");
	m_player[1].setStartPosition(sf::Vector2f(550.0f, 200.0f));

	m_player[2].setColour(sf::Color::Yellow);
	m_player[2].setName("Yellow");
	m_player[2].setStartPosition(sf::Vector2f(200.0f, 550.0f));

	m_ipInputString = "127.0.0.1";

	reset();
	loadFont();
	setupUI();

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
	m_player[t_data.m_playerIndex].updatePosition(t_data.m_velocity);
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
		m_timeLastedText.setString(m_player[t_data.m_targetIndex].getName() + " lasted " + timeString + "s");
	}
	else if(t_data.m_targetIndex == m_playerIndex)
	{
		m_endText.setString(m_player[t_data.m_playerIndex].getName() + " has caught you");
		m_timeLastedText.setString("You Lasted " + timeString + "s");
	}
	else
	{ 
		m_endText.setString(m_player[t_data.m_playerIndex].getName() + " has caught " + m_player[t_data.m_targetIndex].getName());
		m_timeLastedText.setString(m_player[t_data.m_targetIndex].getName() + " lasted " + timeString + "s");
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
	switch (m_state)
	{
	case GameState::GAMEPLAY:
		updateGameplay(dt);
		break;
	case GameState::WAITING:
		updateWaiting(dt);
		break;
	case GameState::START:
		updateStart(dt);
		break;
	default:
		break;
	}
}

void Game::updateGameplay(sf::Time t_dt)
{
	sf::Vector2f moveVector = m_player[m_playerIndex].move();

	if (m_host)
	{
		m_player[m_playerIndex].updatePosition(moveVector);
		m_timeLasted += t_dt;

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
		if (VectorMath::magnitude(moveVector) != 0)
		{
			PlayerData data;
			data.m_playerIndex = m_playerIndex;
			data.m_velocity = moveVector;
			m_client->sendPlayerUpdate(data);
		}
	}
}

void Game::updateWaiting(sf::Time t_dt)
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
		else
		{
			m_server->sendChangeState(GameState::WAITING);
		}
	}
}

void Game::updateStart(sf::Time t_dt)
{
	if (m_host)
	{
		m_countDownTime -= t_dt;

		if (m_countDownTime <= sf::seconds(0.0f))
		{
			m_state = GameState::GAMEPLAY;
			m_server->sendChangeState(GameState::GAMEPLAY);
		}
	}
}

void Game::processSelectIP(sf::Event t_event)
{
	if (t_event.type == sf::Event::TextEntered)
	{
		unsigned short unicode = t_event.text.unicode;

		if (unicode == 8)
		{
			if (m_ipInputString.size() > 0)
			{
				m_ipInputString = m_ipInputString.substr(0, m_ipInputString.size() - 1);

				m_ipValueText.setString(m_ipInputString);
				m_ipValueText.setOrigin(sf::Vector2f(m_ipValueText.getLocalBounds().width / 2, m_ipValueText.getLocalBounds().height / 2));
				m_ipValueText.setPosition(m_ipValueText.getPosition());
			}
		}
		else if (m_ipInputString.size() < 15)
		{
			auto it = std::find(std::begin(m_ipValidValues), std::end(m_ipValidValues), static_cast<char>(unicode));

			if (it != std::end(m_ipValidValues))
			{
				m_ipInputString += static_cast<char>(unicode);

				m_ipValueText.setString(m_ipInputString);
				m_ipValueText.setOrigin(sf::Vector2f(m_ipValueText.getLocalBounds().width / 2, m_ipValueText.getLocalBounds().height / 2));
				m_ipValueText.setPosition(m_ipValueText.getPosition());
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

		switch (m_state)
		{
		case GameState::SELECT:
			if (event.type == sf::Event::MouseButtonPressed)
			{
				if (event.key.code == sf::Mouse::Left)
				{
					if (m_hostButton.getGlobalBounds().contains(m_pos))
					{
						m_state = GameState::SELECTBROADCAST;
					}
					else if (m_clientButton.getGlobalBounds().contains(m_pos))
					{
						m_state = GameState::SELECTIP;
					}
				}
			}
			break;
		case GameState::SELECTIP:
			processSelectIP(event);

			if (event.type == sf::Event::MouseButtonPressed)
			{
				if (event.key.code == sf::Mouse::Left)
				{
					if (m_ipConfirmButton.getGlobalBounds().contains(m_pos))
					{
						connect();
						m_state = GameState::WAITING;
					}
				}
			}
			break;
		case GameState::SELECTBROADCAST:
			if (event.type == sf::Event::MouseButtonPressed)
			{
				if (event.key.code == sf::Mouse::Left)
				{
					if (m_broadcastYesButton.getGlobalBounds().contains(m_pos))
					{
						startServer(true);
						m_state = GameState::WAITING;
					}
					else if (m_broadcastNoButton.getGlobalBounds().contains(m_pos))
					{
						startServer(false);
						m_state = GameState::WAITING;
					}
				}
			}
			break;
		default:
			break;
		}

		if (event.type == sf::Event::KeyPressed)
		{
			switch (event.key.code)
			{
			case sf::Keyboard::Escape:
				if (!m_host)
				{
					if (m_client != nullptr)
					{
						m_client->disconnect();
					}
				}
				m_window.close();
				break;
			default:
				break;
			}
		}

		if (event.type == sf::Event::Closed)
		{
			if (!m_host)
			{
				if (m_client != nullptr)
				{
					m_client->disconnect();
				}
			}
			m_window.close();
		}
	}
}

void Game::render()
{
	m_window.clear(sf::Color(40, 40, 40));

	switch (m_state)
	{
	case GameState::SELECT:
		m_window.draw(m_hostButton);
		m_window.draw(m_clientButton);
		m_window.draw(m_hostText);
		m_window.draw(m_clientText);
		break;
	case GameState::SELECTIP:
		m_window.draw(m_ipHelpText);
		m_window.draw(m_ipValueText);
		m_window.draw(m_ipConfirmButton);
		m_window.draw(m_ipConfirmText);
		break;
	case GameState::SELECTBROADCAST:
		m_window.draw(m_broadcastYesButton);
		m_window.draw(m_broadcastNoButton);
		m_window.draw(m_broadcastYesText);
		m_window.draw(m_broadcastNoText);
		m_window.draw(m_broadcastPubliclyText);
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
	case GameState::SERVERCLOSED:
		m_window.draw(m_serverClosedText);
		break;
	default:
		break;
	}

	m_window.display();
}

void Game::startServer(bool t_broadcastPublicaly)
{
	m_host = true;
	m_server = new Server(8000, t_broadcastPublicaly);
	m_server->m_game = this;

	std::thread serverThread(&Game::listenForConnections, this);
	serverThread.detach();

	m_serverThread = &serverThread;
}

void Game::connect()
{
	m_client = new Client(m_ipInputString.c_str(), 8000);
	m_client->m_game = this;

	if (!m_client->connectSocket())
	{
		m_client->disconnect();
		m_window.close();
	}
}

void Game::setupUI()
{
	m_hostButton.setSize(sf::Vector2f(150.0f, 50.0f));
	m_hostButton.setOrigin(sf::Vector2f(75.0f, 25.0f));
	m_hostButton.setOutlineThickness(5.0f);
	m_hostButton.setFillColor(sf::Color(168, 142, 81));
	m_hostButton.setPosition(sf::Vector2f(150.0f, 300.0f));

	m_clientButton.setSize(sf::Vector2f(150.0f, 50.0f));
	m_clientButton.setOrigin(sf::Vector2f(75.0f, 25.0f));
	m_clientButton.setOutlineThickness(5.0f);
	m_clientButton.setFillColor(sf::Color(168, 142, 81));
	m_clientButton.setPosition(sf::Vector2f(450.0f, 300.0f));

	m_ipConfirmButton.setSize(sf::Vector2f(150.0f, 50.0f));
	m_ipConfirmButton.setOrigin(sf::Vector2f(75.0f, 25.0f));
	m_ipConfirmButton.setOutlineThickness(5.0f);
	m_ipConfirmButton.setFillColor(sf::Color(168, 142, 81));
	m_ipConfirmButton.setPosition(sf::Vector2f(300.0f, 400.0f));

	m_broadcastNoButton.setSize(sf::Vector2f(150.0f, 50.0f));
	m_broadcastNoButton.setOrigin(sf::Vector2f(75.0f, 25.0f));
	m_broadcastNoButton.setOutlineThickness(5.0f);
	m_broadcastNoButton.setFillColor(sf::Color(168, 142, 81));
	m_broadcastNoButton.setPosition(sf::Vector2f(450.0f, 300.0f));
	
	m_broadcastYesButton.setSize(sf::Vector2f(150.0f, 50.0f));
	m_broadcastYesButton.setOrigin(sf::Vector2f(75.0f, 25.0f));
	m_broadcastYesButton.setOutlineThickness(5.0f);
	m_broadcastYesButton.setFillColor(sf::Color(168, 142, 81));
	m_broadcastYesButton.setPosition(sf::Vector2f(150.0f, 300.0f));

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

	m_broadcastNoText.setString("No");
	m_broadcastNoText.setFont(m_font);
	m_broadcastNoText.setCharacterSize(30);
	m_broadcastNoText.setOrigin(sf::Vector2f(m_broadcastNoText.getLocalBounds().width / 2, m_broadcastNoText.getLocalBounds().height / 1.3f));
	m_broadcastNoText.setPosition(m_broadcastNoButton.getPosition());

	m_broadcastYesText.setString("Yes");
	m_broadcastYesText.setFont(m_font);
	m_broadcastYesText.setCharacterSize(30);
	m_broadcastYesText.setOrigin(sf::Vector2f(m_broadcastYesText.getLocalBounds().width / 2, m_broadcastYesText.getLocalBounds().height / 1.3f));
	m_broadcastYesText.setPosition(m_broadcastYesButton.getPosition());

	m_ipConfirmText.setString("Confirm");
	m_ipConfirmText.setFont(m_font);
	m_ipConfirmText.setCharacterSize(30);
	m_ipConfirmText.setOrigin(sf::Vector2f(m_ipConfirmText.getLocalBounds().width / 2, m_ipConfirmText.getLocalBounds().height / 1.3f));
	m_ipConfirmText.setPosition(m_ipConfirmButton.getPosition());

	m_broadcastPubliclyText.setString("Do you wish broadcast publicly?");
	m_broadcastPubliclyText.setFont(m_font);
	m_broadcastPubliclyText.setCharacterSize(30);
	m_broadcastPubliclyText.setOrigin(sf::Vector2f(m_broadcastPubliclyText.getLocalBounds().width / 2, m_broadcastPubliclyText.getLocalBounds().height / 2));
	m_broadcastPubliclyText.setPosition(sf::Vector2f(300.0f, 50.0f));

	m_gameStartText.setString("Game Is About To Start");
	m_gameStartText.setFont(m_font);
	m_gameStartText.setCharacterSize(30);
	m_gameStartText.setOrigin(sf::Vector2f(m_gameStartText.getLocalBounds().width / 2, m_gameStartText.getLocalBounds().height / 2));
	m_gameStartText.setPosition(sf::Vector2f(300.0f, 50.0f));

	m_gameOverText.setString("Game Over");
	m_gameOverText.setFont(m_font);
	m_gameOverText.setCharacterSize(30);
	m_gameOverText.setOrigin(sf::Vector2f(m_gameOverText.getLocalBounds().width / 2, m_gameOverText.getLocalBounds().height / 2));
	m_gameOverText.setPosition(sf::Vector2f(300.0f, 50.0f));

	m_waitText.setString("Waiting For More Players...");
	m_waitText.setFont(m_font);
	m_waitText.setCharacterSize(30);
	m_waitText.setOrigin(sf::Vector2f(m_waitText.getLocalBounds().width / 2, m_waitText.getLocalBounds().height / 2));
	m_waitText.setPosition(sf::Vector2f(300.0f, 125.0f));

	m_serverClosedText.setString("Server has shut down...");
	m_serverClosedText.setFont(m_font);
	m_serverClosedText.setCharacterSize(30);
	m_serverClosedText.setOrigin(sf::Vector2f(m_serverClosedText.getLocalBounds().width / 2, m_serverClosedText.getLocalBounds().height / 2));
	m_serverClosedText.setPosition(sf::Vector2f(300.0f, 125.0f));

	m_ipHelpText.setString("Enter the ip of the host:");
	m_ipHelpText.setFont(m_font);
	m_ipHelpText.setCharacterSize(30);
	m_ipHelpText.setOrigin(sf::Vector2f(m_ipHelpText.getLocalBounds().width / 2, m_ipHelpText.getLocalBounds().height / 2));
	m_ipHelpText.setPosition(sf::Vector2f(300.0f, 100.0f));

	m_ipValueText.setString(m_ipInputString);
	m_ipValueText.setFont(m_font);
	m_ipValueText.setCharacterSize(30);
	m_ipValueText.setOrigin(sf::Vector2f(m_ipValueText.getLocalBounds().width / 2, m_ipValueText.getLocalBounds().height / 2));
	m_ipValueText.setPosition(sf::Vector2f(300.0f, 150.0f));

	m_endText.setFont(m_font);
	m_endText.setCharacterSize(25);

	m_startText.setFont(m_font);
	m_startText.setCharacterSize(25);

	m_timeLastedText.setFont(m_font);
	m_timeLastedText.setCharacterSize(25);
}

void Game::centreText()
{
	m_endText.setOrigin(sf::Vector2f(m_endText.getLocalBounds().width / 2, m_endText.getLocalBounds().height / 2));
	m_endText.setPosition(sf::Vector2f(300.0f, 100.0f));

	m_gameOverText.setOrigin(sf::Vector2f(m_gameOverText.getLocalBounds().width / 2, m_gameOverText.getLocalBounds().height / 2));
	m_gameOverText.setPosition(sf::Vector2f(300.0f, 50.0f));

	m_gameStartText.setOrigin(sf::Vector2f(m_gameStartText.getLocalBounds().width / 2, m_gameStartText.getLocalBounds().height / 2));
	m_gameStartText.setPosition(sf::Vector2f(300.0f, 50.0f));

	m_startText.setOrigin(sf::Vector2f(m_startText.getLocalBounds().width / 2, m_startText.getLocalBounds().height / 2));
	m_startText.setPosition(sf::Vector2f(300.0f, 100.0f));

	m_timeLastedText.setOrigin(sf::Vector2f(m_timeLastedText.getLocalBounds().width / 2, m_timeLastedText.getLocalBounds().height / 2));
	m_timeLastedText.setPosition(sf::Vector2f(300.0f, 150.0f));

	m_centreText = false;
}

void Game::listenForConnections()
{
	while(true)
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
			if (VectorMath::distance(m_player[m_targetIndex].getPosition(), m_player[i].getPosition()) <= Player::s_radius * 2)
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