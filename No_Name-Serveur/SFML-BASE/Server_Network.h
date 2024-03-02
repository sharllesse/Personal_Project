#pragma once
#include "Clients.h"
#include "Projectile.h"
#include "Console.h"

class Room
{
public:
	enum class ROOM_STATE
	{
		RSNULL = -1,
		LOBBY,
		ROOM,
		GAME
	};
public:
	Room();
	Room(std::shared_ptr<Clients>& _room_clients, std::list<us>* _clients_ID, std::shared_ptr<Console>& _main_console, us _port);
	~Room();

	void update();

	void receive();
	void send();

	void update_projectiles();

	void join_room(std::shared_ptr<Clients>& _client);

	us get_port() { return m_port; }
private:
	sf::SocketSelector m_selector;
	sf::TcpListener m_listener;
	
	us m_id;
	us m_port;
	std::string m_name;
	ROOM_STATE m_room_state;

	std::weak_ptr<Console> m_main_server_console_wptr;

	std::list<us>* m_clients_ID;

	std::list<std::shared_ptr<Clients>> m_clients;
	std::list<std::unique_ptr<Projectile>> m_projectiles;

	std::thread m_update_thread;

	bool m_room_is_finish;

	float m_sending_timer;
	unsigned m_projectiles_shooted;
};

class Server_Network
{
private:
	sf::TcpListener m_listener;
	sf::SocketSelector m_selector;
	std::thread m_verify_connection_thread;
	std::shared_ptr<Console> m_console;

	bool m_server_closed;
	bool m_client_wanto_to_create_room;

	float m_sending_timer;

	std::list<std::shared_ptr<Clients>> m_clients;
	std::list<us> m_clients_ID_to_verify;
	std::list<us> m_clients_IDs;
	std::list<std::unique_ptr<Room>> m_rooms;
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

