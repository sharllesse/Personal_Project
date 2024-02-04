#pragma once

#include "SFML/Audio.hpp"
#include "SFML/Network.hpp"
#include "SFML/Graphics.hpp"

#include <string>
#include <fstream>
#include <iostream>
#include <sstream>

#include <vector>
#include <list>

#include <memory>
#include <assert.h>
#include <random>
#include <stack>
#include <stdlib.h>
#include <concepts>
#include <thread>
#include <mutex>

#define CONNECTION_FAIL -1
#define INT_TYPE sf::Int32

#define PI 3.14159265f
#define RAD2DEG 57.295779f
#define DEG2RAD 0.017453f

typedef unsigned short us;

template <typename T>
concept Out = requires(T a)
{
	{ std::hash<T>{}(a) } -> std::convertible_to<INT_TYPE>;
};

template <typename T>
concept In = requires(T a)
{
	{ std::hash<INT_TYPE>{}(a) } -> std::convertible_to<T>;
};

template <In T>
sf::Packet& operator<<(sf::Packet& packet, T&& code)
{
	INT_TYPE code_(static_cast<INT_TYPE>(code));
	return packet << code_;
}

template <In T>
sf::Packet& operator<<(sf::Packet& packet, T& code)
{
	INT_TYPE code_(static_cast<INT_TYPE>(code));
	return packet << code_;
}

template<Out T>
sf::Packet& operator>>(sf::Packet& packet, T& code)
{
	INT_TYPE code_;
	packet >> code_;
	code = static_cast<T>(code_);
	return packet;
}

sf::Packet& operator>>(sf::Packet& _p, sf::Vector2f& _v);

sf::Packet& operator>>(sf::Packet& _p, sf::Color& _c);

sf::Packet& operator<<(sf::Packet& _p, sf::Vector2f& _v);

sf::Packet& operator<<(sf::Packet& _p, sf::Color& _c);

namespace Tools
{
	float get_delta_time();

	void restart_clock();

	template <class _Type>
	_Type rand(_Type _min, _Type _max)
	{
		std::random_device rd;
		std::mt19937 mt(rd());
		std::uniform_real_distribution<double> dist(_min, _max + ((_min == _max) ? 0 : 1));

		return static_cast<_Type>(dist(mt));
	}

	// POINT/CIRCLE
	bool point_circle(sf::Vector2<float> _mpos, sf::Vector2<float> _cpos, float _r);

	// CIRCLE/CIRCLE
	bool circle_circle(sf::Vector2<float> _c1pos, float _c1r, sf::Vector2<float> _c2pos, float _c2r);

	// POINT/RECTANGLE
	bool point_rect(sf::Vector2<float> _mpos, sf::FloatRect _rect);

	// RECTANGLE/RECTANGLE
	bool rect_rect(sf::FloatRect _rect1, sf::FloatRect _rect2);

	// CIRCLE/RECTANGLE
	bool circle_rect(sf::Vector2<float> cpos, float radius, sf::FloatRect rect);

	float get_distance(sf::Vector2f a_, sf::Vector2f b_);

	float get_norme(sf::Vector2f a);

	sf::Vector2f truncate(sf::Vector2f _v, float _m);

	sf::Vector2f normalize(sf::Vector2f v);
}