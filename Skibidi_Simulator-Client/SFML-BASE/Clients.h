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
		JOIN_INFORMATION,
		DISCONNECTED_INFORMATION
	};
private:
	struct client_Information
	{
		std::unique_ptr<sf::TcpSocket> m_socket;
		unsigned short m_ID;
		std::string m_IP;
		bool m_disconnected;
	};

	sf::SocketSelector m_selector;
	std::thread m_receive_thread;
	std::mutex m_delete_client;
	float m_sending_timer;
	bool m_game_is_finish;

	std::list<std::unique_ptr<Clients>> m_clients;

	sf::RectangleShape m_all_clients;
public:
	Clients();
	Clients(std::string _name, sf::Vector2f _position, float _speed);
	Clients(std::string _name, unsigned short _ID, std::string _IP);
	~Clients();

	bool connect(std::string _IP, unsigned short _port, float _time_out = 5.f);

	void clients_information(sf::Packet& _packet);

	void clients_connected(sf::Packet& _packet);

	void clients_disconnected(sf::Packet& _packet);

	void receive();

	void send();

	void update(sf::RenderWindow& _window);

	void draw(sf::RenderWindow& _window);

	sf::Vector2f m_position;
	float m_speed;

	std::string m_name;

	client_Information m_client_information;
};