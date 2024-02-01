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

	std::vector<std::unique_ptr<Clients>> m_clients;

	if (m_listener.listen(8000u) == sf::Socket::Done)
	{
		m_listener.setBlocking(false);
		m_selector.add(m_listener);
	}

	while (true)
	{
		if (m_selector.wait(sf::seconds(0.001f)))
		{
			if (m_selector.isReady(m_listener))
			{
				std::unique_ptr<Clients> tmp_client = std::make_unique<Clients>();
				if (m_listener.accept(*tmp_client->m_client_information.m_socket))
				{
					sf::Packet send_packet;
					sf::Packet receive_packet;

					if (tmp_client->m_client_information.m_socket->receive(receive_packet) == sf::Socket::Done)
					{
						receive_packet >> tmp_client->m_name >> tmp_client->m_client_information.m_IP;

						tmp_client->m_client_information.m_ID = get_random_ID(m_clients, tmp_client);

						send_packet << tmp_client->m_client_information.m_ID;

						if (tmp_client->m_client_information.m_socket->send(send_packet) == sf::Socket::Done)
						{
							tmp_client->m_client_information.m_socket->setBlocking(false);
							m_selector.add(*tmp_client->m_client_information.m_socket);
							m_clients.push_back(std::move(tmp_client));
						}
						else
						{

							tmp_client.reset();
							std::cout << "Failed to send the packet" << std::endl;
						}
					}
					else
					{
						tmp_client.reset();
						std::cout << "Failed to receive the packet" << std::endl;
					}
				}
				else
				{
					tmp_client.reset();
					std::cout << "Failed to connect the client" << std::endl;
				}
			}
		}
	}

    return 0;

}