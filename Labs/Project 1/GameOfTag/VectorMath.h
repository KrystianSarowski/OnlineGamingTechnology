#pragma once
#include "SFML/Graphics.hpp"

class VectorMath
{
public:

	static float distance(sf::Vector2f t_vect1, sf::Vector2f t_vect2);
	static float magnitude(sf::Vector2f t_vect);
	static sf::Vector2f unit(sf::Vector2f t_vect);
	static sf::Vector2f applyScaler(sf::Vector2f t_vect, float t_scaler);

private:

	VectorMath();
};