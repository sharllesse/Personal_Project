#include "Server_Network.h"

Server_Network::Server_Network() :
	m_sending_timer(0.f), m_server_closed(false)
{
	if (m_listener.listen(8000u) == sf::Socket::Done)
	{
		m_listener.setBlocking(false);
		m_selector.add(m_listener);

		m_verify_connection_thread = std::thread(&Server_Network::verify_connection, this);
	}
}

Server_Network::~Server_Network()
{
	m_server_closed = true;

	if (m_verify_connection_thread.joinable())
		m_verify_connection_thread.join();

	m_selector.clear();
}

std::unique_ptr<Server_Network>& Server_Network::get_instance()
{
	if (m_instance_server_network == nullptr)
		m_instance_server_network = std::unique_ptr<Server_Network>(new Server_Network());

	return m_instance_server_network;
}

unsigned short Server_Network::get_random_ID()
{
	unsigned short tmp_int = Tools::rand(10u, 10000u);

	for (auto client = m_clients.begin(); client != m_clients.end();)
	{
		if (tmp_int == (*client)->m_client_information.m_ID)
			tmp_int = Tools::rand(10u, 10000u);
		else
			client++;
	}

	return tmp_int;
}

void Server_Network::verify_connection()
{
	while (!m_server_closed)
	{
		if (m_selector.wait(sf::seconds(0.001f)))
		{
			if (m_selector.isReady(m_listener))
			{
				std::unique_ptr<Clients> tmp_client = std::make_unique<Clients>();
				sf::Socket::Status tmp_statue(m_listener.accept(*tmp_client->m_client_information.m_socket));
				if (tmp_statue == sf::Socket::Done)
				{
					sf::Packet send_packet;
					sf::Packet join_packet;
					sf::Packet receive_packet;

					if (tmp_client->m_client_information.m_socket->receive(receive_packet) == sf::Socket::Done)
					{
						receive_packet >> tmp_client->m_name >> tmp_client->m_client_information.m_IP;

						tmp_client->m_client_information.m_ID = get_random_ID();

						send_packet << tmp_client->m_client_information.m_ID << static_cast<INT_TYPE>(m_clients.size());

						std::for_each(m_clients.begin(), m_clients.end(), [&send_packet](std::unique_ptr<Clients>& _client)
							{
								send_packet << _client->m_name << _client->m_client_information.m_ID << _client->m_client_information.m_IP;
							});

						if (tmp_client->m_client_information.m_socket->send(send_packet) == sf::Socket::Done)
						{
							m_verify_connection_mutex.lock();

							std::cout << "Player [NAME][" << tmp_client->m_name << "] [IP][" << tmp_client->m_client_information.m_IP << "] [ID][" << tmp_client->m_client_information.m_ID << "] connected" << std::endl;

							tmp_client->m_client_information.m_socket->setBlocking(false);
							m_selector.add(*tmp_client->m_client_information.m_socket);
							m_clients.push_back(std::move(tmp_client));

							join_packet << Clients::INFO_TYPE_SERVER_SIDE::JOIN_INFORMATION << static_cast<INT_TYPE>(m_clients.size());
							std::for_each(m_clients.begin(), m_clients.end(), [&join_packet](std::unique_ptr<Clients>& _client)
								{
									join_packet << _client->m_name << _client->m_client_information.m_ID << _client->m_client_information.m_IP;
								});

							std::for_each(m_clients.begin(), m_clients.end(), [&join_packet](std::unique_ptr<Clients>& _client)
								{
									_client->m_client_information.m_socket->send(join_packet);
								});

							m_verify_connection_mutex.unlock();
						}
						else
						{

							tmp_client.reset();
							std::cout << "Failed to send the packet." << std::endl;
						}
					}
					else
					{
						tmp_client.reset();
						std::cout << "Failed to receive the packet." << std::endl;
					}
				}
				else if (tmp_statue == sf::Socket::Error || tmp_statue == sf::Socket::Disconnected)
				{
					tmp_client.reset();
					std::cout << "Failed to connect the client." << std::endl;
				}
			}
		}
	}
}

void Server_Network::receive()
{
	if (m_selector.wait(sf::seconds(0.001f)))
	{
		std::for_each(m_clients.begin(), m_clients.end(), [&](std::unique_ptr<Clients>& _client)
			{
				if (m_selector.isReady(*_client->m_client_information.m_socket))
				{
					sf::Packet receive_packet;
					if (_client->m_client_information.m_socket->receive(receive_packet) == sf::Socket::Done)
					{
						Clients::INFO_TYPE_CLIENT_SIDE tmp_info_type(Clients::INFO_TYPE_CLIENT_SIDE::ITCNULL);

						receive_packet >> tmp_info_type;

						if (tmp_info_type == Clients::INFO_TYPE_CLIENT_SIDE::TRANSFORM)
						{
							receive_packet >> _client->m_position;
						}
					}
				}
			});
	}
}

void Server_Network::send()
{
	m_sending_timer += Tools::get_delta_time();

	if (static_cast<unsigned>(m_clients.size()))
	{
		if (m_sending_timer > 0.00833333f)
		{
			bool tmp_put_one_time(false);

			sf::Packet m_client_packet;
			sf::Packet disconnected_packet;

			m_client_packet << Clients::INFO_TYPE_SERVER_SIDE::CLIENT_INFORMATION << static_cast<INT_TYPE>(m_clients.size());

			std::for_each(m_clients.begin(), m_clients.end(), [&m_client_packet](std::unique_ptr<Clients>& _client)
				{
					m_client_packet << _client->m_client_information.m_ID << _client->m_position;
				});

			for (auto client = m_clients.begin(); client != m_clients.end();)
			{
				sf::Socket::Status tmp_socket_statue((*client)->m_client_information.m_socket->send(m_client_packet));
				if (tmp_socket_statue == sf::Socket::Disconnected || tmp_socket_statue == sf::Socket::Error)
				{
					if (!tmp_put_one_time)
						disconnected_packet << Clients::INFO_TYPE_SERVER_SIDE::DISCONNECTED_INFORMATION;

					disconnected_packet << (*client)->m_client_information.m_ID;

					tmp_put_one_time = true;

					std::cout << "Player [NAME][" << (*client)->m_name << "] [IP][" << (*client)->m_client_information.m_IP << "] [ID][" << (*client)->m_client_information.m_ID << "] disconnected" << std::endl;

					m_selector.remove(*(*client)->m_client_information.m_socket);
					(*client)->m_client_information.m_socket->disconnect();

					client = m_clients.erase(client);
				}
				else
				{
					//SEND OTHER INFORMATION
					client++;
				}
			}

			if (!disconnected_packet.endOfPacket())
			{
				std::for_each(m_clients.begin(), m_clients.end(), [&disconnected_packet](std::unique_ptr<Clients>& _client)
					{
						_client->m_client_information.m_socket->send(disconnected_packet);
					});
			}

			m_sending_timer = 0.f;
		}
	}
}

void Server_Network::update()
{
	this->receive();

	m_verify_connection_mutex.lock();

	this->send();
	
	m_verify_connection_mutex.unlock();
}
