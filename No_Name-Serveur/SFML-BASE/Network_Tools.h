#pragma once
#include "SFML_ENGINE/Tools.h"

#define INT_TYPE sf::Int32
#define PORT 30000u

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