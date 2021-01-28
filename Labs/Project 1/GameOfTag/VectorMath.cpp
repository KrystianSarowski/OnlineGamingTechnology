#include "VectorMath.h"

float VectorMath::distance(sf::Vector2f t_vect1, sf::Vector2f t_vect2)
{
    return sqrt(powf(t_vect1.x - t_vect2.x, 2) + powf(t_vect1.y - t_vect2.y, 2));
}

float VectorMath::magnitude(sf::Vector2f t_vect)
{
    return sqrt(t_vect.x * t_vect.x + t_vect.y * t_vect.y);
}

sf::Vector2f VectorMath::unit(sf::Vector2f t_vect)
{
    float mag = magnitude(t_vect);

    if (mag != 0)
    {
        t_vect.x = t_vect.x / mag;
        t_vect.y = t_vect.y / mag;
    }

    return t_vect;
}

sf::Vector2f VectorMath::applyScaler(sf::Vector2f t_vect, float t_scaler)
{
    t_vect.x = t_vect.x * t_scaler;
    t_vect.y = t_vect.y * t_scaler;

    return t_vect;
}