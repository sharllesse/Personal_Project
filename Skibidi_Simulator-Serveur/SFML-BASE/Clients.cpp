#include "Clients.h"

Clients::Clients() :
	m_name("No Name"), m_speed(0.f), m_rotation(0.f)
{
	m_client_information.m_socket = std::make_unique<sf::TcpSocket>();
	m_client_information.m_ID = 0u;
	m_client_information.m_IP = "";
	m_client_information.m_disconnected = false;
}

Clients::Clients(std::string _name, sf::Vector2f _position, float _speed) :
	m_name(_name), m_position(_position), m_speed(_speed), m_rotation(0.f)
{
	m_client_information.m_socket = std::make_unique<sf::TcpSocket>();
	m_client_information.m_ID = 0u;
	m_client_information.m_IP = "";
	m_client_information.m_disconnected = false;
}

Clients::Clients(std::string _name, us _ID, std::string _IP) :
	m_name(_name), m_speed(0.f), m_rotation(0.f)
{
	m_client_information.m_socket = std::make_unique<sf::TcpSocket>();
	m_client_information.m_ID = _ID;
	m_client_information.m_IP = _IP;
	m_client_information.m_disconnected = false;
}

Clients::~Clients()
{
}

sf::Socket::Status Clients::send_packet(sf::Packet& _packet)
{
	return this->m_client_information.m_socket->send(_packet);
}

sf::Socket::Status Clients::receive_packet(sf::Packet& _packet)
{
	return this->m_client_information.m_socket->receive(_packet);
}
