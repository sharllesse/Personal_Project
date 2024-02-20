#pragma once
#include "Network_Tools.h"

class Projectile
{
public:
	Projectile();
	Projectile(float _speed, float _rotation, sf::Vector2f _start_position, us _player_ID);
	~Projectile();

	sf::Vector2f m_position;
	sf::Vector2f m_velocity;
	sf::Vector2f m_start_position;

	us m_player_ID;

	bool m_need_to_be_deleted;
private:
	float m_speed;

};