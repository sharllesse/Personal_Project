#pragma once
#include "Tools.h"

class Clients
{
private:
	struct client_Information
	{
		std::unique_ptr<sf::TcpSocket> m_socket;
		unsigned short m_ID;
		std::string m_IP;
	};
public:
	Clients();
	Clients(sf::Vector2f _position, float _speed);
	~Clients();

	sf::Vector2f m_position;
	float m_speed;

	std::string m_name;

	client_Information m_client_information;
};