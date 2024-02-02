#pragma once
#include "Clients.h"

#define Server Server_Network::get_instance()

class Server_Network
{
private:
	Server_Network();

	static inline std::unique_ptr<Server_Network> m_instance_server_network = nullptr;

	sf::TcpListener m_listener;
	sf::SocketSelector m_selector;

	std::thread m_verify_connection_thread;

	std::mutex m_verify_connection_mutex;

	bool m_server_closed;

	float m_sending_timer;

	std::list<std::unique_ptr<Clients>> m_clients;
public:
	~Server_Network();

	static std::unique_ptr<Server_Network>& get_instance();

	unsigned short get_random_ID();

	void verify_connection();

	void receive();

	void send();

	void update();
};

