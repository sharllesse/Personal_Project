#include "Clients.h"

unsigned short get_random_ID(std::vector<std::unique_ptr<Clients>>& _clients, std::unique_ptr<Clients>& _main_client)
{
	unsigned short tmp_int = Tools::rand(10u, 10000u);

	for (auto client = _clients.begin(); client != _clients.end();)
	{
		if (tmp_int == (*client)->m_client_information.m_ID)
			tmp_int = Tools::rand(10u, 10000u);
		else
			client++;
	}

	return tmp_int;
}

int main()
{
	sf::TcpListener m_listener;
	sf::SocketSelector m_selector;

	float m_sending_timer(0.f);

	std::vector<std::unique_ptr<Clients>> m_clients;

	if (m_listener.listen(8000u) == sf::Socket::Done)
	{
		m_listener.setBlocking(false);
		m_selector.add(m_listener);
	}

	while (true)
	{
		Tools::restart_clock();

		if (m_selector.wait(sf::seconds(0.001f)))
		{
			if (m_selector.isReady(m_listener))
			{
				std::unique_ptr<Clients> tmp_client = std::make_unique<Clients>();
				if (m_listener.accept(*tmp_client->m_client_information.m_socket) == sf::Socket::Done)
				{
					sf::Packet send_packet;
					sf::Packet join_packet;
					sf::Packet receive_packet;

					if (tmp_client->m_client_information.m_socket->receive(receive_packet) == sf::Socket::Done)
					{
						receive_packet >> tmp_client->m_name >> tmp_client->m_client_information.m_IP;

						tmp_client->m_client_information.m_ID = get_random_ID(m_clients, tmp_client);

						send_packet << tmp_client->m_client_information.m_ID << static_cast<INT_TYPE>(m_clients.size());

						std::for_each(m_clients.begin(), m_clients.end(), [&send_packet](std::unique_ptr<Clients>& _client)
							{
								send_packet << _client->m_name << _client->m_client_information.m_ID << _client->m_client_information.m_IP;
							});

						if (tmp_client->m_client_information.m_socket->send(send_packet) == sf::Socket::Done)
						{
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
				else
				{
					tmp_client.reset();
					std::cout << "Failed to connect the client." << std::endl;
				}
			}

			std::for_each(m_clients.begin(), m_clients.end(), [&m_selector](std::unique_ptr<Clients>& _client)
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

		m_sending_timer += Tools::get_delta_time();

		if (m_sending_timer > 0.00833333f)
		{
			sf::Packet m_client_packet;

			m_client_packet << Clients::INFO_TYPE_SERVER_SIDE::CLIENT_INFORMATION << static_cast<INT_TYPE>(m_clients.size());

			std::for_each(m_clients.begin(), m_clients.end(), [&m_client_packet](std::unique_ptr<Clients>& _client)
				{
					m_client_packet << _client->m_client_information.m_ID << _client->m_position;
				});

			std::for_each(m_clients.begin(), m_clients.end(), [&](std::unique_ptr<Clients>& _client)
				{
					_client->m_client_information.m_socket->send(m_client_packet);
				});

			m_sending_timer = 0.f;
		}
	}

    return EXIT_SUCCESS;

}