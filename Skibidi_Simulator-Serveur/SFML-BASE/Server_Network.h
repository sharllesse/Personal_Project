#pragma once
#include "Clients.h"
#include "Projectile.h"
#include "Console.h"

class Server_Network
{
private:
	sf::TcpListener m_listener;
	sf::SocketSelector m_selector;

	std::thread m_verify_connection_thread;

	bool m_server_closed;

	float m_sending_timer;

	std::list<std::unique_ptr<Clients>> m_clients;
	std::list<std::unique_ptr<Projectile>> m_projectiles;

	unsigned m_projectiles_shooted;

	Console m_console;
public:
	Server_Network();
	~Server_Network();

	us get_random_ID();

	void verify_connection();

	void receive();
	void send();

	void update();

	void update_projectiles();

	void draw(sf::RenderWindow& _window);
};

