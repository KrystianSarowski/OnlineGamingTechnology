#pragma once
#include <SFML/Graphics.hpp>
#include "VectorMath.h"

class Player
{
public:

	Player();

	sf::Vector2f getPosition();
	sf::Vector2f move();
	sf::Vector2f getStartPosition();
	std::string	getName();

	void setColour(sf::Color t_colour);
	void setName(std::string t_newName);
	void setStartPosition(sf::Vector2f t_position);
	void updatePosition(sf::Vector2f t_velocity);
	void setPosition(sf::Vector2f t_newPosition);

	void render(sf::RenderWindow& t_window);

	static float s_radius;
	static float s_speed;
	static float s_worldWidth;
	static float s_worldHeight;

private:

	sf::Vector2f m_startPosition;
	sf::Vector2f m_position;

	sf::CircleShape m_body;

	std::string m_name;

	void checkBoundry();
};