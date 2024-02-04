#pragma once
#include "Tools.h"

class Weapon
{
public:
	Weapon();
	Weapon(int _bullet_number, float _bullet_speed, float _reload_cooldown, float _shoot_cooldown, float _damage, float _spread);
	~Weapon();
private:
	float m_bullet_speed;
	float m_reload_cooldown;
	float m_shoot_cooldown;
	float m_damage;
	float m_spread;

	int m_bullet_number;
	
	sf::Texture m_UI_icon;
};