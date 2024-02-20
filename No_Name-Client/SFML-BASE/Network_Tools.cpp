#include "Network_Tools.h"

sf::Packet& operator>>(sf::Packet& _p, sf::Vector2f& _v)
{
	return _p >> _v.x >> _v.y;
}

sf::Packet& operator>>(sf::Packet& _p, sf::Color& _c)
{
	return _p >> _c.r >> _c.g >> _c.b >> _c.a;
}

sf::Packet& operator<<(sf::Packet& _p, sf::Vector2f& _v)
{
	return _p << _v.x << _v.y;
}

sf::Packet& operator<<(sf::Packet& _p, sf::Color& _c)
{
	return _p << _c.r << _c.g << _c.b << _c.a;
}