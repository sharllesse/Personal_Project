#pragma once
#include "Network_Tools.h"

class Projectile
{
public:
	Projectile();
	Projectile(float _speed, float _rotation, sf::Vector2f _start_position, us _player_ID);
	~Projectile();

	sf::Vector2f& get_position() { return m_position; }
	sf::Vector2f& get_velocity() { return m_velocity; }

	us& get_player_ID() { return m_player_ID; }

	bool& get_deleted() { return m_need_to_be_deleted; }

	float get_speed() { return m_speed; }
private:
	sf::Vector2f m_position;
	sf::Vector2f m_velocity;

	us m_player_ID;

	bool m_need_to_be_deleted;

	float m_speed;
};