#include "Server_Network.h"

Room::Room()
{
}

Room::Room(std::shared_ptr<Clients>& _room_clients, std::list<us>* _clients_ID, std::shared_ptr<Console>& _main_console, us _port) :
	m_id(0u), m_projectiles_shooted(0u), m_sending_timer(0.f), m_port(_port), m_room_is_finish(false)
{
	m_clients.push_back(_room_clients);

	m_main_server_console_wptr = _main_console;

	m_clients_ID = _clients_ID;

	m_room_state = ROOM_STATE::ROOM;

	if (m_listener.listen(_port) == sf::Socket::Done)
	{
		m_listener.setBlocking(false);
		m_selector.add(m_listener);

		m_update_thread = std::thread(&Room::update, this);
	}
}

Room::~Room()
{
	m_room_is_finish = true;

	m_update_thread.join();

	m_clients.clear();
	m_projectiles.clear();
}

void Room::update()
{
	while (!m_room_is_finish)
	{
		this->restart_clock();

		if (!static_cast<unsigned>(m_clients.size()))
			m_time_out_timer += this->get_delta_time();
		else
			m_time_out_timer = 0.f;

		if (m_selector.wait(sf::seconds(0.001f)))
		{
			if (m_selector.isReady(m_listener))
			{
				std::unique_ptr<sf::TcpSocket> tmp_socket(std::make_unique<sf::TcpSocket>());
				sf::Socket::Status tmp_statue(m_listener.accept(*tmp_socket));
				if (tmp_statue == sf::Socket::Done)
				{
					sf::Packet receive_packet;
					sf::Packet send_packet;
					
					us tmp_id(0u);

					if (tmp_socket->receive(receive_packet) == sf::Socket::Done)
					{
						receive_packet >> tmp_id;

						send_packet << static_cast<INT_TYPE>(m_clients.size());

						std::for_each(m_clients.begin(), m_clients.end(), [&send_packet](std::shared_ptr<Clients>& _client)
							{
								send_packet << _client->m_name << _client->m_client_information.m_ID << _client->m_client_information.m_IP;
							});

						if (tmp_socket->send(send_packet) == sf::Socket::Done)
						{
							sf::Packet join_packet;

							for (auto client = m_clients.begin(); client != m_clients.end(); client++)
							{
								if ((*client)->m_client_information.m_ID == tmp_id)
								{
									(*client)->m_client_information.m_socket->disconnect();
									(*client)->m_client_information.m_socket = std::move(tmp_socket);

									m_main_server_console_wptr.lock()->add_message("Player [NAME][" + (*client)->m_name + "] [IP][" + (*client)->m_client_information.m_IP + "] [ID][" + std::to_string((*client)->m_client_information.m_ID) + "] connected to a room", Console::Message::INFO);
									(*client)->m_client_information.m_socket->setBlocking(false);
									m_selector.add(*(*client)->m_client_information.m_socket);

									break;
								}
							}

							join_packet << Clients::INFO_TYPE_SERVER_SIDE::JOIN_INFORMATION << static_cast<INT_TYPE>(m_clients.size());
							std::for_each(m_clients.begin(), m_clients.end(), [&join_packet](std::shared_ptr<Clients>& _client)
								{
									join_packet << _client->m_name << _client->m_client_information.m_ID << _client->m_client_information.m_IP;
								});

							std::for_each(m_clients.begin(), m_clients.end(), [&join_packet](std::shared_ptr<Clients>& _client)
								{
									_client->send_packet(join_packet);
								});
						}
						else
						{
							tmp_socket.reset();
							std::cout << "Failed to send the packet." << std::endl;
						}
					}
					else
					{
						tmp_socket.reset();
						std::cout << "Failed to receive the packet." << std::endl;
					}
				}
				else if (tmp_statue == sf::Socket::Error || tmp_statue == sf::Socket::Disconnected)
				{
					tmp_socket.reset();
					std::cout << "Failed to connect the client." << std::endl;
				}
			}
		}

		//FAIRE UN TIMER DE 10 SECONDES SI YA 0 CLIENT DANS LE LOBBY ET PUIS LE DELETE.

		this->send();
	}
	//this->receive();

	//this->update_projectiles();
}

void Room::receive()
{
	if (m_selector.wait(sf::seconds(0.001f)))
	{
		/*std::for_each(m_clients.begin(), m_clients.end(), [&](std::unique_ptr<Clients>& _client)
			{
				if (m_selector.isReady(*_client->m_client_information.m_socket))
				{
					sf::Packet receive_packet;
					if (_client->receive_packet(receive_packet) == sf::Socket::Done)
					{
						Clients::INFO_TYPE_CLIENT_SIDE tmp_info_type(Clients::INFO_TYPE_CLIENT_SIDE::ITCNULL);

						receive_packet >> tmp_info_type;

						if (tmp_info_type == Clients::INFO_TYPE_CLIENT_SIDE::TRANSFORM)
						{
							receive_packet >> _client->m_position >> _client->m_rotation;
						}
						else if (tmp_info_type == Clients::INFO_TYPE_CLIENT_SIDE::SHOOT)
						{
							int tmp_bullet_count(0);
							float tmp_spread(0.f);
							us tmp_id(0u);
							float tmp_speed(0.f);
							sf::Vector2f tmp_start_position;
							float tmp_rotation(0.f);

							receive_packet >> tmp_bullet_count >> tmp_spread >> tmp_id >> tmp_speed >> tmp_start_position >> tmp_rotation;

							for (int i = 0; i < tmp_bullet_count; i++)
								m_projectiles.push_back(std::make_unique<Projectile>(tmp_speed, Tools::Rand(tmp_rotation - tmp_spread, tmp_rotation + tmp_spread), tmp_start_position, tmp_id));

							m_projectiles_shooted += tmp_bullet_count;
						}
					}
				}
			});*/
	}
}

void Room::send()
{
	m_sending_timer += Tools::getDeltaTime();

	if (static_cast<unsigned>(m_clients.size()))
	{
		if (m_sending_timer > 0.00833333f)
		{
			bool tmp_put_one_time(false);

			sf::Packet tmp_client_packet;
			sf::Packet tmp_disconnected_packet;
			//sf::Packet tmp_projectiles_packet;

			//tmp_client_packet << Clients::INFO_TYPE_SERVER_SIDE::CLIENT_INFORMATION << static_cast<INT_TYPE>(m_clients.size());

			//std::for_each(m_clients.begin(), m_clients.end(), [&tmp_client_packet](std::unique_ptr<Clients>& _client)
			//	{
			//		tmp_client_packet << _client->m_client_information.m_ID << _client->m_position << _client->m_rotation;
			//	});

			//tmp_projectiles_packet << Clients::INFO_TYPE_SERVER_SIDE::PROJECTILES_INFORMATION << m_projectiles_shooted;

			//for (auto projectile = m_projectiles.begin(); projectile != m_projectiles.end();)
			//{
			//	tmp_projectiles_packet << (*projectile)->m_player_ID << (*projectile)->m_position << (*projectile)->m_need_to_be_deleted;

			//	if ((*projectile)->m_need_to_be_deleted)
			//		projectile = m_projectiles.erase(projectile);
			//	else
			//		projectile++;
			//}

			for (auto client = m_clients.begin(); client != m_clients.end();)
			{
				sf::Socket::Status tmp_socket_statue((*client)->send_packet(tmp_client_packet));
				if (tmp_socket_statue == sf::Socket::Disconnected || tmp_socket_statue == sf::Socket::Error)
				{
					m_clients_ID->remove_if([&client](us _id) { return (*client)->m_client_information.m_ID == _id; });

					if (!tmp_put_one_time)
						tmp_disconnected_packet << Clients::INFO_TYPE_SERVER_SIDE::DISCONNECTED_INFORMATION;

					tmp_disconnected_packet << (*client)->m_client_information.m_ID;

					tmp_put_one_time = true;

					m_main_server_console_wptr.lock()->add_message("Player [NAME][" + (*client)->m_name + "] [IP][" + (*client)->m_client_information.m_IP + "] [ID][" + std::to_string((*client)->m_client_information.m_ID) + "] disconnected", Console::Message::INFO);

					m_selector.remove(*(*client)->m_client_information.m_socket);
					(*client)->m_client_information.m_socket->disconnect();

					client = m_clients.erase(client);
				}
				else
				{
					//(*client)->send_packet(tmp_projectiles_packet);

					client++;
				}
			}

			if (!tmp_disconnected_packet.endOfPacket())
			{
				std::for_each(m_clients.begin(), m_clients.end(), [&tmp_disconnected_packet](std::shared_ptr<Clients>& _client)
					{
						_client->send_packet(tmp_disconnected_packet);
					});
			}

			m_sending_timer = 0.f;
			m_projectiles_shooted = 0u;
		}
	}
}

void Room::update_projectiles()
{
	std::for_each(m_projectiles.begin(), m_projectiles.end(), [&](std::unique_ptr<Projectile>& _projectiles)
	{
		_projectiles->m_position += _projectiles->m_velocity * Tools::getDeltaTime();

		if (_projectiles->m_start_position.getDistance(_projectiles->m_position) > 1000)
			_projectiles->m_need_to_be_deleted = true;

		std::for_each(m_clients.begin(), m_clients.end(), [&_projectiles](std::shared_ptr<Clients>& _client)
			{
				if (Tools::Collisions::circle_circle(_client->m_position, 25, _projectiles->m_position, 10) && !_projectiles->m_need_to_be_deleted && _projectiles->m_player_ID != _client->m_client_information.m_ID)
					_projectiles->m_need_to_be_deleted = true;
			});
	});
}

void Room::join_room(std::shared_ptr<Clients>& _client)
{
	//m_clients.push_back(std::move(_client));
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Server_Network::Server_Network() :
	m_sending_timer(0.f), m_server_closed(false), m_client_wanto_to_create_room(false)
{
	if (m_listener.listen(PORT) == sf::Socket::Done)
	{
		m_listener.setBlocking(true);
		m_selector.add(m_listener);

		m_console = std::make_shared<Console>();

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

us Server_Network::get_random_ID()
{
	us tmp_int = Tools::Rand(10u, 10000u);

	for (auto client = m_clients_IDs.begin(); client != m_clients_IDs.end();)
	{
		if (tmp_int == *client)
			tmp_int = Tools::Rand(10u, 10000u);
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
				std::shared_ptr<Clients> tmp_client = std::make_shared<Clients>();
				sf::Socket::Status tmp_statue(m_listener.accept(*tmp_client->m_client_information.m_socket));
				if (tmp_statue == sf::Socket::Done)
				{
					sf::Packet send_packet;
					//sf::Packet join_packet;
					sf::Packet receive_packet;

					if (tmp_client->receive_packet(receive_packet) == sf::Socket::Done)
					{
						receive_packet >> tmp_client->m_name >> tmp_client->m_client_information.m_IP;

						tmp_client->m_client_information.m_ID = get_random_ID();

						send_packet << tmp_client->m_client_information.m_ID << static_cast<INT_TYPE>(m_clients.size());

						/*std::for_each(m_clients.begin(), m_clients.end(), [&send_packet](std::unique_ptr<Clients>& _client)
							{
								send_packet << _client->m_name << _client->m_client_information.m_ID << _client->m_client_information.m_IP;
							});*/

						if (tmp_client->send_packet(send_packet) == sf::Socket::Done)
						{
							m_console->add_message("Player [NAME][" + tmp_client->m_name + "] [IP][" + tmp_client->m_client_information.m_IP + "] [ID][" + std::to_string(tmp_client->m_client_information.m_ID) + "] connected to the menu", Console::Message::INFO);

							m_clients_IDs.push_back(tmp_client->m_client_information.m_ID);

							tmp_client->m_client_information.m_socket->setBlocking(false);
							m_selector.add(*tmp_client->m_client_information.m_socket);
							m_clients.push_back(tmp_client);

							/*join_packet << Clients::INFO_TYPE_SERVER_SIDE::JOIN_INFORMATION << static_cast<INT_TYPE>(m_clients.size());
							std::for_each(m_clients.begin(), m_clients.end(), [&join_packet](std::unique_ptr<Clients>& _client)
								{
									join_packet << _client->m_name << _client->m_client_information.m_ID << _client->m_client_information.m_IP;
								});

							std::for_each(m_clients.begin(), m_clients.end(), [&join_packet](std::unique_ptr<Clients>& _client)
								{
									_client->send_packet(join_packet);
								});*/
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
		std::for_each(m_clients.begin(), m_clients.end(), [&](std::shared_ptr<Clients>& _client)
			{
				if (m_selector.isReady(*_client->m_client_information.m_socket))
				{
					sf::Packet receive_packet;
					if (_client->receive_packet(receive_packet) == sf::Socket::Done)
					{
						Clients::INFO_TYPE_CLIENT_SIDE tmp_info_type(Clients::INFO_TYPE_CLIENT_SIDE::ITCNULL);

						receive_packet >> tmp_info_type;

						if (tmp_info_type == Clients::INFO_TYPE_CLIENT_SIDE::CREATE_ROOM)
						{
							m_client_wanto_to_create_room = true;

							m_clients_ID_to_verify.push_back(_client->m_client_information.m_ID);
						}

						/*if (tmp_info_type == Clients::INFO_TYPE_CLIENT_SIDE::TRANSFORM)
						{
							receive_packet >> _client->m_position >> _client->m_rotation;
						}
						else if (tmp_info_type == Clients::INFO_TYPE_CLIENT_SIDE::SHOOT)
						{
							int tmp_bullet_count(0);
							float tmp_spread(0.f);
							us tmp_id(0u);
							float tmp_speed(0.f);
							sf::Vector2f tmp_start_position;
							float tmp_rotation(0.f);

							receive_packet >> tmp_bullet_count >> tmp_spread >> tmp_id >> tmp_speed >> tmp_start_position >> tmp_rotation;

							for (int i = 0; i < tmp_bullet_count; i++)
								m_projectiles.push_back(std::make_unique<Projectile>(tmp_speed, Tools::Rand(tmp_rotation - tmp_spread, tmp_rotation + tmp_spread), tmp_start_position, tmp_id));

							m_projectiles_shooted += tmp_bullet_count;
						};*/
					}
				}
			});
	}
}

void Server_Network::send()
{
	m_rooms.remove_if([&m_console = m_console](const auto& _room)
		{
			if (_room->get_time_out_timer() >= 10.f)
			{
				m_console->add_message("[PORT]" + std::to_string(_room->get_port()) + " [ID]" + std::to_string(_room->get_id()) + " closed", Console::Message::INFO);
				return true;
			}
			else
				return false;
		});

	m_sending_timer += Tools::getDeltaTime();

	if (static_cast<unsigned>(m_clients.size()))
	{
		if (m_sending_timer > 0.00833333f)
		{
			bool tmp_put_one_time(false);

			if (m_client_wanto_to_create_room)
			{
				for (auto ID = m_clients_ID_to_verify.begin(); ID != m_clients_ID_to_verify.end(); ID++)
				{
					for (auto client = m_clients.begin(); client != m_clients.end();)
					{
						if (*ID == (*client)->m_client_information.m_ID)
						{
							//And there we display a message about the client that as been disconnected.
							m_console->add_message("Player [NAME][" + (*client)->m_name + "] [IP][" + (*client)->m_client_information.m_IP + "] [ID][" + std::to_string((*client)->m_client_information.m_ID) + "] disconnected from the menu", Console::Message::INFO);

							sf::Packet tmp_create_room_packet;
							us tmp_port((!m_rooms.size()) ? PORT + 1 : m_rooms.back()->get_port() + 1);

							tmp_create_room_packet << Clients::INFO_TYPE_SERVER_SIDE::LOBBY_TO_ROOM_INFORMATION << tmp_port;

							(*client)->send_packet(tmp_create_room_packet);

							m_selector.remove(*(*client)->m_client_information.m_socket);

							m_rooms.push_back(std::make_unique<Room>(*client, &m_clients_IDs, m_console, tmp_port));

							client = m_clients.erase(client);
						}
						else
						{
							client++;
						}
					}
				}

				m_clients_ID_to_verify.clear();
				m_client_wanto_to_create_room = false;
			}

			//This packet will contain information about the room
			//to be displayed when the client is in the lobby.
			sf::Packet tmp_client_packet;

			//There is the disconnection part.
			//If we send the packet to the client and the socket statue is Disconnected/Error
			//the client will be removed from the list and the selector.
			for (auto client = m_clients.begin(); client != m_clients.end();)
			{
				sf::Socket::Status tmp_socket_statue((*client)->send_packet(tmp_client_packet));
				if (tmp_socket_statue == sf::Socket::Disconnected || tmp_socket_statue == sf::Socket::Error)
				{
					//And there we display a message about the client that as been disconnected.
					m_console->add_message("Player [NAME][" + (*client)->m_name + "] [IP][" + (*client)->m_client_information.m_IP + "] [ID][" + std::to_string((*client)->m_client_information.m_ID) + "] disconnected", Console::Message::INFO);

					m_clients_IDs.remove_if([&client](us _id) { return (*client)->m_client_information.m_ID == _id; });

					m_selector.remove(*(*client)->m_client_information.m_socket);
					(*client)->m_client_information.m_socket->disconnect();

					client = m_clients.erase(client);
				}
				else
				{
					client++;
				}
			}

			m_sending_timer = 0.f;
		}
	}
}

void Server_Network::update()
{
	this->receive();

	this->send();

	m_console->update();
}

void Server_Network::draw(sf::RenderWindow& _window)
{
	m_console->draw(_window, const_cast<sf::View&>(_window.getDefaultView()));
}