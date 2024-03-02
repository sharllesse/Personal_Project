#pragma once
#include "Network_Tools.h"

class Clients
{
public:
	enum INFO_TYPE_CLIENT_SIDE
	{
		ITCNULL = -1,
		TRANSFORM,
		SHOOT,
		CREATE_ROOM,
		JOIN_ROOM
	};

	enum INFO_TYPE_SERVER_SIDE
	{
		ITSNULL = -1,
		CLIENT_INFORMATION,
		JOIN_INFORMATION,
		DISCONNECTED_INFORMATION,
		PROJECTILES_INFORMATION,
		LOBBY_TO_ROOM_INFORMATION
	};
private:
	struct client_Information
	{
		std::unique_ptr<sf::TcpSocket> m_socket;
		us m_ID;
		std::string m_IP;

		bool m_disconnected;
	};
public:
	Clients();
	Clients(std::string _name, sf::Vector2f _position, float _speed);
	Clients(std::string _name, us _ID, std::string _IP);
	~Clients();

	std::string m_name;
	sf::Vector2f m_position;
	float m_speed;
	float m_rotation;

	sf::Socket::Status send_packet(sf::Packet& _packet);
	sf::Socket::Status receive_packet(sf::Packet& _packet);

	client_Information m_client_information;
};