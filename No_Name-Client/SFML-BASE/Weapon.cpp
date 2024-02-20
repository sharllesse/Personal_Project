#include "Weapon.h"

Weapon::Weapon() :
	m_bullet_number(0), m_bullet_speed(0.f), m_damage(0.f), m_reload_cooldown(0.f), m_shoot_cooldown(0.f), m_spread(0.f)
{
}

Weapon::Weapon(int _bullet_number, float _bullet_speed, float _reload_cooldown, float _shoot_cooldown, float _damage, float _spread) :
	m_bullet_number(_bullet_number), m_bullet_speed(_bullet_speed), m_damage(_damage), m_reload_cooldown(_reload_cooldown), m_shoot_cooldown(_shoot_cooldown), m_spread(_spread)
{
}

Weapon::~Weapon()
{
}
