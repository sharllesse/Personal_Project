#include "Projectile.h"

Projectile::Projectile() :
	m_speed(0.f), m_player_ID(0u), m_need_to_be_deleted(false)
{
}

Projectile::Projectile(float _speed, float _rotation, sf::Vector2f _start_position, us _player_ID) :
	m_position(_start_position), m_player_ID(_player_ID), m_speed(_speed), m_need_to_be_deleted(false)
{
	m_velocity = sf::Vector2f(std::cos(_rotation * DEG2RAD), std::sin(_rotation * DEG2RAD)).normalize(false) * m_speed;
}

Projectile::~Projectile()
{
}