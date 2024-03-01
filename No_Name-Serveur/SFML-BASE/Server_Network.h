#pragma once
#include "Clients.h"
#include "Projectile.h"
#include "Console.h"

class Room
{
public:
	Room();
	Room(std::unique_ptr<Clients>& _room_clients, std::shared_ptr<Console>& _main_console);
	~Room();

	void update();

	void receive();
	void send();

	void update_projectiles();

	void join_room(std::unique_ptr<Clients>& _client);
private:
	sf::SocketSelector m_selector;
	sf::Listener m_listener;
	us m_id;
	std::string m_name;
	std::weak_ptr<Console> m_main_server_console_wptr;

	std::list<std::unique_ptr<Clients>> m_clients;
	std::list<std::unique_ptr<Projectile>> m_projectiles;

	float m_sending_timer;
	unsigned m_projectiles_shooted;
};

class Server_Network
{
private:
	sf::TcpListener m_listener;
	sf::SocketSelector m_selector;
	std::thread m_verify_connection_thread;
	Console m_console;

	bool m_server_closed;
	float m_sending_timer;

	std::list<std::unique_ptr<Clients>> m_clients;
	std::list<std::thread> m_rooms_thread;
public:
	Server_Network();
	~Server_Network();

	us get_random_ID();

	void verify_connection();

	void create_room();
	void join_room();

	void receive();
	void send();

	void update();

	void draw(sf::RenderWindow& _window);
};

