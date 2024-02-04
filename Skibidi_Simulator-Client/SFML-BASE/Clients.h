#pragma once
#include "Tools.h"
#include "Projectile.h"

class Clients
{
public:
	enum INFO_TYPE_CLIENT_SIDE
	{
		ITCNULL = -1,
		TRANSFORM,
		SHOOT
	};

	enum INFO_TYPE_SERVER_SIDE
	{
		ITSNULL = -1,
		CLIENT_INFORMATION,
		JOIN_INFORMATION,
		DISCONNECTED_INFORMATION,
		PROJECTILES_INFORMATION
	};
private:
	struct client_Information
	{
		std::unique_ptr<sf::TcpSocket> m_socket;
		us m_ID;
		std::string m_IP;
		bool m_disconnected;
	};

	sf::SocketSelector m_selector;
	std::thread m_receive_thread;
	std::mutex m_delete_client;
	std::mutex m_delete_projectiles;
	float m_sending_timer;
	bool m_game_is_finish;

	sf::Vector2f m_mouse_position;

	std::list<std::unique_ptr<Clients>> m_clients;
	std::list<std::unique_ptr<Projectile>> m_projectiles;

	sf::RectangleShape m_all_clients;
	sf::CircleShape m_all_projectiles;

	sf::VertexArray m_aim_line;

	bool m_shooted;

	float m_shoot_timer;
public:
	Clients();
	Clients(std::string _name, sf::Vector2f _position, float _speed);
	Clients(std::string _name, us _ID, std::string _IP);
	~Clients();

	bool connect(std::string _IP, us _port, float _time_out = 5.f);

	void clients_information(sf::Packet& _packet);
	void clients_connected(sf::Packet& _packet);
	void clients_disconnected(sf::Packet& _packet);
	void projectiles_information(sf::Packet& _packet);

	void receive();
	void send();

	sf::Socket::Status send_packet(sf::Packet& _packet);
	sf::Socket::Status receive_packet(sf::Packet& _packet);

	void update(sf::RenderWindow& _window);

	void draw_clients(sf::RenderWindow& _window);
	void draw_projectiles(sf::RenderWindow& _window);

	sf::Vector2f m_position;
	float m_speed;
	float m_rotation;

	std::string m_name;

	client_Information m_client_information;
};