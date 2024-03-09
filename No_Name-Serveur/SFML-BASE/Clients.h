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
		JOIN_ROOM,
		LEAVE_ROOM
	};

	enum INFO_TYPE_SERVER_SIDE
	{
		ITSNULL = -1,
		CLIENT_INFORMATION,
		JOIN_INFORMATION,
		DISCONNECTED_INFORMATION,
		PROJECTILES_INFORMATION,
		LOBBY_TO_ROOM_INFORMATION,
		ROOM_TIMED_OUT,
		ROOM_TO_LOBBY_INFORMATION,
		ROOM_INFORMATION
	};
private:
	struct client_Information
	{
		client_Information() :
			m_IP(""), m_disconnected(false), m_ID(0u)
		{
			m_socket = std::make_unique<sf::TcpSocket>();
		}

		std::unique_ptr<sf::TcpSocket> m_socket;
		us m_ID;
		std::string m_IP;

		bool m_disconnected;
	};

	std::string m_name;

	sf::Vector2f m_position;

	float m_speed;
	float m_rotation;
	float m_timed_out_timer;

	bool m_waiting_for_reconnect;
	bool m_is_host;

	client_Information m_client_information;
public:
	Clients();
	~Clients();

	sf::Socket::Status send_packet(sf::Packet& _packet);
	sf::Socket::Status receive_packet(sf::Packet& _packet);

	std::string& get_name() { return m_name; }

	sf::Vector2f& get_position() { return m_position; }

	float& get_speed() { return m_speed; }
	float& get_rotation() { return m_rotation; }
	float& get_timed_out_timer() { return m_timed_out_timer; }

	bool& is_waiting_for_reconnect() { return m_waiting_for_reconnect; }
	bool& is_host() { return m_is_host; }

	client_Information& get_client_information() { return m_client_information; }
};