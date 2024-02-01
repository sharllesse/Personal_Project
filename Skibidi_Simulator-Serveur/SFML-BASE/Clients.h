#pragma once
#include "Tools.h"

class Clients
{
public:
	enum INFO_TYPE_CLIENT_SIDE
	{
		ITCNULL = -1,
		TRANSFORM
	};

	enum INFO_TYPE_SERVER_SIDE
	{
		ITSNULL = -1,
		CLIENT_INFORMATION,
		JOIN_INFORMATION
	};
private:
	struct client_Information
	{
		std::unique_ptr<sf::TcpSocket> m_socket;
		unsigned short m_ID;
		std::string m_IP;
	};
public:
	Clients();
	Clients(std::string _name, sf::Vector2f _position, float _speed);
	Clients(std::string _name, unsigned short _ID, std::string _IP);
	~Clients();

	sf::Vector2f m_position;
	float m_speed;

	std::string m_name;

	client_Information m_client_information;
};