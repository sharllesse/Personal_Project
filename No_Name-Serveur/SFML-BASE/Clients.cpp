#include "Clients.h"

Clients::Clients() :
	m_name("No Name"), m_speed(0.f), m_rotation(0.f), m_waiting_for_reconnect(false), m_client_information(), m_timed_out_timer(0.f), m_is_host(false)
{
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
