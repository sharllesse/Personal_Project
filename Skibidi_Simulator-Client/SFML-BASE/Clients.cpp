#include "Clients.h"

Clients::Clients() :
	m_speed(0.f)
{
	m_client_information.m_socket = std::make_unique<sf::TcpSocket>();
	m_client_information.m_ID = 0u;
}

Clients::Clients(sf::Vector2f _position, float _speed) :
	m_position(_position), m_speed(_speed)
{
	m_client_information.m_socket = std::make_unique<sf::TcpSocket>();
	m_client_information.m_ID = 0u;
	m_client_information.m_IP = "";
}

Clients::~Clients()
{
}
