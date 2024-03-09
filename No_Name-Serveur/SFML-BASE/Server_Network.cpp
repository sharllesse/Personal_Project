#include "Server_Network.h"

Room::Room() :
	m_id(0u), m_projectiles_shooted(0u), m_sending_timer(0.f), m_port(0u), m_room_is_finish(false), m_time_out_timer(0.f), m_clients_ID(nullptr), m_main_server_console_ptr(nullptr), m_room_state(ROOM_STATE::RSNULL), m_client_want_to_leave_room(false), m_clients_main_server(nullptr), m_host_id(0u)
{
}

Room::Room(std::shared_ptr<Clients>& _room_clients, std::list<us>& _clients_ID, std::list<std::shared_ptr<Clients>>& _main_server_client, Console& _main_console, us _port) :
	m_id(0u), m_projectiles_shooted(0u), m_sending_timer(0.f), m_port(_port), m_room_is_finish(false), m_time_out_timer(0.f), m_client_want_to_leave_room(false)
{
	m_name = _room_clients->get_name();
	m_host_id = _room_clients->get_client_information().m_ID;

	m_clients.push_back(_room_clients);

	m_main_server_console_ptr = &_main_console;

	m_clients_ID = &_clients_ID;
	m_clients_main_server = &_main_server_client;

	m_room_state = ROOM_STATE::ROOM;

	if (m_listener.listen(_port) == sf::Socket::Done)
	{
		m_listener.setBlocking(false);
		m_selector.add(m_listener);

		//m_selector_thread = std::thread(&Room::update_selector, this);
		m_update_thread = std::thread(&Room::update, this);
	}
}

Room::~Room()
{
	m_room_is_finish = true;

	m_update_thread.join();
	//m_selector_thread.join();

	m_clients.clear();
	m_projectiles.clear();
}

void Room::update()
{
	//This is the base update of the room that as been lauch in a thread.
	while (!m_room_is_finish)
	{
		//This is basically a new server that work on his own.
		this->restart_clock();

		//m_time_out_timer is here to be sure that we need or the room to be deleted.
		if (!static_cast<unsigned>(m_clients.size()))
			m_time_out_timer += this->get_delta_time();
		else
			m_time_out_timer = 0.f;

		//This is here temporary or maybe definitely when will move i will test if its cause lag or not.
		//For now im going to put it here to avoid creating 2 thread per room.
		if (m_selector.wait(sf::seconds(0.001f)))
		{
			this->verify_connection();

			this->receive();
		}

		//this->update_projectiles();

		this->client_timed_out();

		this->send();
	}
}

//USELESS FOR NOW
/*void Room::update_selector()
{
	//update_selector is another update that as been lauch in thread.
	//Why did i lauch it in another thread its because the wait of a selector
	//may cause lag that the server to stop.

	//For now im going to put the selector wait in the normal update to test it.
	while (!m_room_is_finish)
	{
		if (m_selector.wait(sf::seconds(0.001f)))
		{
			this->verify_connection();

			this->receive();
		}
	}
}*/

//FAIRE EN SORTE QUE QUAND ON EST TIMED OUT ON SOIT REMIS DANS LE LOBBY.
//STOCKER LES UTILISATEUR QUI SONT TIMED OUT ET SI IL REJOINGNE ALORS QU'IL SONT SUPPRIMER 
//ON PEUT QUAND MEME SE CONNECTER ET SI LA ROOM ET DEJA COMMENCé ON LUI ENVOIE UNE INFO COMME QUOI
//IL DOIT RETOURNé AU LOBBY.

void Room::verify_connection()
{
	if (m_selector.isReady(m_listener))
	{
		if (m_room_state == ROOM_STATE::ROOM)
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
							send_packet << _client->get_name() << _client->get_client_information().m_ID << _client->get_client_information().m_IP;
						});

					if (tmp_socket->send(send_packet) == sf::Socket::Done)
					{
						bool tmp_client_has_been_found(false);

						sf::Packet join_packet;

						for (auto client = m_clients.begin(); client != m_clients.end(); client++)
						{
							if ((*client)->get_client_information().m_ID == tmp_id)
							{
								(*client)->get_client_information().m_socket->disconnect();
								(*client)->get_client_information().m_socket = std::move(tmp_socket);

								m_main_server_console_ptr->add_message("Player [NAME][" + (*client)->get_name() + "] [IP][" + (*client)->get_client_information().m_IP + "] [ID][" + std::to_string((*client)->get_client_information().m_ID) + "] connected to a room", Console::Message::INFO);

								(*client)->is_waiting_for_reconnect() = false;

								(*client)->get_client_information().m_socket->setBlocking(false);
								m_selector.add(*(*client)->get_client_information().m_socket);

								tmp_client_has_been_found = true;

								break;
							}
						}

						if (tmp_client_has_been_found)
						{
							join_packet << Clients::INFO_TYPE_SERVER_SIDE::JOIN_INFORMATION << static_cast<INT_TYPE>(m_clients.size());
							std::for_each(m_clients.begin(), m_clients.end(), [&join_packet](std::shared_ptr<Clients>& _client)
								{
									join_packet << _client->get_name() << _client->get_client_information().m_ID << _client->get_client_information().m_IP;
								});

							std::for_each(m_clients.begin(), m_clients.end(), [&join_packet](std::shared_ptr<Clients>& _client)
								{
									_client->send_packet(join_packet);
								});
						}
					}
					else
					{
						tmp_socket.reset();
						m_main_server_console_ptr->add_message("Room [PORT][" + std::to_string(m_port) + "] [ID][" + std::to_string(m_id) + "] failed to send the packet", Console::Message::ERROR);
					}
				}
				else
				{
					tmp_socket.reset();
					m_main_server_console_ptr->add_message("Room [PORT][" + std::to_string(m_port) + "] [ID][" + std::to_string(m_id) + "] failed to receive the packet", Console::Message::ERROR);
				}
			}
			else if (tmp_statue == sf::Socket::Error || tmp_statue == sf::Socket::Disconnected)
			{
				tmp_socket.reset();
				m_main_server_console_ptr->add_message("Room [PORT][" + std::to_string(m_port) + "] [ID][" + std::to_string(m_id) + "] failed to connect the client", Console::Message::ERROR);
			}
		}
	}
}

void Room::client_timed_out()
{
	m_clients.remove_if([this](auto& _client)
		{
			if (_client->is_waiting_for_reconnect())
			{
				_client->get_timed_out_timer() += this->get_delta_time();

				if (_client->get_timed_out_timer() >= 20.f)
				{
					_client->get_client_information().m_socket->disconnect();
					m_clients_ID->remove_if([&_client](us _id) { return _client->get_client_information().m_ID == _id; });
					m_main_server_console_ptr->add_message("Player [NAME][" + _client->get_name() + "] [IP][" + _client->get_client_information().m_IP + "] [ID][" + std::to_string(_client->get_client_information().m_ID) + "] has timed out", Console::Message::INFO);

					return true;
				}
			}
			else
			{
				_client->get_timed_out_timer() = 0.f;
			}

			return false;
		});
}

void Room::receive()
{
	std::for_each(m_clients.begin(), m_clients.end(), [&](std::shared_ptr<Clients>& _client)
			{
				if (m_selector.isReady(*_client->get_client_information().m_socket))
				{
					sf::Packet receive_packet;
					if (_client->receive_packet(receive_packet) == sf::Socket::Done)
					{
						Clients::INFO_TYPE_CLIENT_SIDE tmp_info_type(Clients::INFO_TYPE_CLIENT_SIDE::ITCNULL);

						receive_packet >> tmp_info_type;

						if (tmp_info_type == Clients::INFO_TYPE_CLIENT_SIDE::TRANSFORM)
						{
							//receive_packet >> _client->m_position >> _client->m_rotation;
						}
						else if (tmp_info_type == Clients::INFO_TYPE_CLIENT_SIDE::SHOOT)
						{
							/*int tmp_bullet_count(0);
							float tmp_spread(0.f);
							us tmp_id(0u);
							float tmp_speed(0.f);
							sf::Vector2f tmp_start_position;
							float tmp_rotation(0.f);

							receive_packet >> tmp_bullet_count >> tmp_spread >> tmp_id >> tmp_speed >> tmp_start_position >> tmp_rotation;

							for (int i = 0; i < tmp_bullet_count; i++)
								m_projectiles.push_back(std::make_unique<Projectile>(tmp_speed, Tools::Rand(tmp_rotation - tmp_spread, tmp_rotation + tmp_spread), tmp_start_position, tmp_id));

							m_projectiles_shooted += tmp_bullet_count;*/
						}
						else if (tmp_info_type == Clients::INFO_TYPE_CLIENT_SIDE::LEAVE_ROOM)
						{
							m_client_want_to_leave_room = true;
							m_clients_ID_to_verify.push_back(_client->get_client_information().m_ID);
						}
					}
				}
			});
}

void Room::send()
{
	m_sending_timer += Tools::getDeltaTime();

	if (static_cast<unsigned>(m_clients.size()))
	{
		if (m_sending_timer > 0.00833333f)
		{
			sf::Packet tmp_client_packet;

			std::vector<sf::Packet> tmp_packets_container;
			if (m_room_state == ROOM_STATE::GAME)
			{
				/*sf::Packet tmp_projectiles_packet;

				tmp_client_packet << Clients::INFO_TYPE_SERVER_SIDE::CLIENT_INFORMATION << static_cast<INT_TYPE>(m_clients.size());

				std::for_each(m_clients.begin(), m_clients.end(), [&tmp_client_packet](std::unique_ptr<Clients>& _client)
					{
						tmp_client_packet << _client->m_client_information.m_ID << _client->m_position << _client->m_rotation;
					});

				tmp_projectiles_packet << Clients::INFO_TYPE_SERVER_SIDE::PROJECTILES_INFORMATION << m_projectiles_shooted;

				for (auto projectile = m_projectiles.begin(); projectile != m_projectiles.end();)
				{
					tmp_projectiles_packet << (*projectile)->m_player_ID << (*projectile)->m_position << (*projectile)->m_need_to_be_deleted;

					if ((*projectile)->m_need_to_be_deleted)
						projectile = m_projectiles.erase(projectile);
					else
						projectile++;
				}*/
			}
			else if(m_room_state == ROOM_STATE::ROOM)
			{
				tmp_client_packet << Clients::INFO_TYPE_SERVER_SIDE::CLIENT_INFORMATION << static_cast<INT_TYPE>(m_clients.size());

				std::for_each(m_clients.begin(), m_clients.end(), [&tmp_client_packet, &m_host_id = m_host_id](const auto& _client)
					{
						if (m_host_id == _client->get_client_information().m_ID)
							tmp_client_packet << _client->get_client_information().m_ID << _client->get_name() << true;
						else
							tmp_client_packet << _client->get_client_information().m_ID << _client->get_name() << false;
					});
			}

			this->client_leave_room();

			this->client_disconnection(tmp_client_packet, tmp_packets_container);

			m_sending_timer = 0.f;
			m_projectiles_shooted = 0u;
		}
	}
}

void Room::client_leave_room()
{
	if (m_client_want_to_leave_room)
	{
		bool tmp_put_one_time(false);
		sf::Packet tmp_disconnected_packet;

		for (auto ID = m_clients_ID_to_verify.begin(); ID != m_clients_ID_to_verify.end(); ID++)
		{
			for (auto client = m_clients.begin(); client != m_clients.end();)
			{
				if (*ID == (*client)->get_client_information().m_ID)
				{
					//And there we display a message about the client that as been disconnected.
					m_main_server_console_ptr->add_message("Player [NAME][" + (*client)->get_name() + "] [IP][" + (*client)->get_client_information().m_IP + "] [ID][" + std::to_string((*client)->get_client_information().m_ID) + "] disconnected from the room", Console::Message::INFO);

					if (!tmp_put_one_time)
						tmp_disconnected_packet << Clients::INFO_TYPE_SERVER_SIDE::DISCONNECTED_INFORMATION;

					tmp_disconnected_packet << (*client)->get_client_information().m_ID;

					sf::Packet tmp_leave_room_packet;
					us tmp_port(PORT);

					if (m_host_id == (*client)->get_client_information().m_ID && static_cast<unsigned>(m_clients.size()) > 1u)
						m_host_id = (*std::next(client))->get_client_information().m_ID;

					tmp_leave_room_packet << Clients::INFO_TYPE_SERVER_SIDE::ROOM_TO_LOBBY_INFORMATION << tmp_port;

					(*client)->send_packet(tmp_leave_room_packet);

					(*client)->is_waiting_for_reconnect() = true;

					m_selector.remove(*(*client)->get_client_information().m_socket);

					m_clients_main_server->push_back((*client));

					client = m_clients.erase(client);
				}
				else
				{
					client++;
				}
			}
		}

		if (!tmp_disconnected_packet.endOfPacket())
		{
			std::for_each(m_clients.begin(), m_clients.end(), [&tmp_disconnected_packet](std::shared_ptr<Clients>& _client)
				{
					_client->send_packet(tmp_disconnected_packet);
				});
		}

		m_clients_ID_to_verify.clear();
		m_client_want_to_leave_room = false;
	}
}

void Room::client_disconnection(sf::Packet& _packet, std::vector<sf::Packet> _packets_to_send)
{
	bool tmp_put_one_time(false);
	sf::Packet tmp_disconnected_packet;

	for (auto client = m_clients.begin(); client != m_clients.end();)
	{
		if (!(*client)->is_waiting_for_reconnect())
		{
			sf::Socket::Status tmp_socket_statue((*client)->send_packet(_packet));
			if (tmp_socket_statue == sf::Socket::Disconnected || tmp_socket_statue == sf::Socket::Error)
			{
				m_clients_ID->remove_if([&client](us _id) { return (*client)->get_client_information().m_ID == _id; });

				if (m_host_id == (*client)->get_client_information().m_ID && static_cast<unsigned>(m_clients.size()) > 1u)
					m_host_id = (*std::next(m_clients.begin()))->get_client_information().m_ID;

				if (!tmp_put_one_time)
					tmp_disconnected_packet << Clients::INFO_TYPE_SERVER_SIDE::DISCONNECTED_INFORMATION;

				tmp_disconnected_packet << (*client)->get_client_information().m_ID;

				tmp_put_one_time = true;

				m_main_server_console_ptr->add_message("Player [NAME][" + (*client)->get_name() + "] [IP][" + (*client)->get_client_information().m_IP + "] [ID][" + std::to_string((*client)->get_client_information().m_ID) + "] disconnected", Console::Message::INFO);

				m_selector.remove(*(*client)->get_client_information().m_socket);
				(*client)->get_client_information().m_socket->disconnect();

				client = m_clients.erase(client);
			}
			else
			{
				//(*client)->send_packet(tmp_projectiles_packet);

				for (auto& packet : _packets_to_send)
					(*client)->send_packet(packet);

				client++;
			}
		}
		else
		{
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

	if (!_packets_to_send.empty())
		_packets_to_send.clear();
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
				if (Tools::Collisions::circle_circle(_client->get_position(), 25, _projectiles->m_position, 10) && !_projectiles->m_need_to_be_deleted && _projectiles->m_player_ID != _client->get_client_information().m_ID)
					_projectiles->m_need_to_be_deleted = true;
			});
	});
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Server_Network::Server_Network() :
	m_sending_timer(0.f), m_server_closed(false), m_client_want_to_create_room(false), m_client_want_to_join_room(false), m_room_create_count(0u)
{
	if (m_listener.listen(PORT) == sf::Socket::Done)
	{
		m_listener.setBlocking(false);
		m_selector.add(m_listener);

		m_server_thread = std::thread(&Server_Network::server_thread, this);
	}
}

Server_Network::~Server_Network()
{
	m_server_closed = true;

	if (m_server_thread.joinable())
		m_server_thread.join();

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

void Server_Network::server_thread()
{
	while (!m_server_closed)
	{
		if (m_selector.wait(sf::seconds(0.001f)))
		{
			this->verify_connection();

			this->receive();
		}
	}
}

void Server_Network::verify_connection()
{
	if (m_selector.isReady(m_listener))
	{
		std::shared_ptr<Clients> tmp_client = std::make_shared<Clients>();
		sf::Socket::Status tmp_statue(m_listener.accept(*tmp_client->get_client_information().m_socket));
		if (tmp_statue == sf::Socket::Done)
		{
			sf::Packet send_packet;
			//sf::Packet join_packet;
			sf::Packet receive_packet;

			if (tmp_client->receive_packet(receive_packet) == sf::Socket::Done)
			{
				bool tmp_is_reconnecting(false);

				receive_packet >> tmp_is_reconnecting;

				//The bool tmp_is_reconnecting is here to know if the client is reconnecting from a room or just a new client.
				if (tmp_is_reconnecting)
				{
					us tmp_id(0u);

					receive_packet >> tmp_id;

					//If the client is from a room we use get his id a set a new socket who as been connect to the main server.
					//And we remove the old that was connected to the room.
					for (auto client = m_clients.begin(); client != m_clients.end(); client++)
					{
						if ((*client)->get_client_information().m_ID == tmp_id)
						{
							(*client)->get_client_information().m_socket->disconnect();
							(*client)->get_client_information().m_socket = std::move(tmp_client->get_client_information().m_socket);

							(*client)->is_waiting_for_reconnect() = false;

							//Ne pas oublier que je ne supprime pas le client si il ne se reconnecte pas.

							(*client)->get_client_information().m_socket->setBlocking(false);
							m_selector.add(*(*client)->get_client_information().m_socket);

							send_packet << static_cast<INT_TYPE>(m_rooms.size());

							std::for_each(m_rooms.begin(), m_rooms.end(), [&send_packet](const auto& _room)
								{
									send_packet << _room->get_name() << _room->get_id() << _room->get_port();
								});

							if ((*client)->send_packet(send_packet) == sf::Socket::Done)
							{
								m_console.add_message("Player [NAME][" + (*client)->get_name() + "] [IP][" + (*client)->get_client_information().m_IP + "] [ID][" + std::to_string((*client)->get_client_information().m_ID) + "] got moved from a room to the main server", Console::Message::INFO);
							}

							break;
						}
					}
				}
				else
				{
					//And here its when its a new client so we giving him a new ID that as not been taken.
					//And we get his IP and name.
					receive_packet >> tmp_client->get_name() >> tmp_client->get_client_information().m_IP;

					tmp_client->get_client_information().m_ID = get_random_ID();

					send_packet << tmp_client->get_client_information().m_ID << static_cast<INT_TYPE>(m_rooms.size());

					std::for_each(m_rooms.begin(), m_rooms.end(), [&send_packet](const auto& _room)
						{
							send_packet << _room->get_name() << _room->get_id() << _room->get_port();
						});

					if (tmp_client->send_packet(send_packet) == sf::Socket::Done)
					{
						m_console.add_message("Player [NAME][" + tmp_client->get_name() + "] [IP][" + tmp_client->get_client_information().m_IP + "] [ID][" + std::to_string(tmp_client->get_client_information().m_ID) + "] connected to the menu", Console::Message::INFO);

						m_clients_IDs.push_back(tmp_client->get_client_information().m_ID);

						tmp_client->get_client_information().m_socket->setBlocking(false);
						m_selector.add(*tmp_client->get_client_information().m_socket);
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
						m_console.add_message("Main server failed to send the packet", Console::Message::ERROR);
					}
				}
			}
			else
			{
				tmp_client.reset();
				m_console.add_message("Main server failed to receive the packet", Console::Message::ERROR);
			}
		}
		else if (tmp_statue == sf::Socket::Error || tmp_statue == sf::Socket::Disconnected)
		{
			tmp_client.reset();
			m_console.add_message("Main server failed to connect the client", Console::Message::ERROR);
		}
	}
}

void Server_Network::receive()
{
	std::for_each(m_clients.begin(), m_clients.end(), [&](std::shared_ptr<Clients>& _client)
		{
			if (m_selector.isReady(*_client->get_client_information().m_socket))
			{
				sf::Packet receive_packet;
				if (_client->receive_packet(receive_packet) == sf::Socket::Done)
				{
					Clients::INFO_TYPE_CLIENT_SIDE tmp_info_type(Clients::INFO_TYPE_CLIENT_SIDE::ITCNULL);

					receive_packet >> tmp_info_type;

					if (tmp_info_type == Clients::INFO_TYPE_CLIENT_SIDE::CREATE_ROOM)
					{
						m_client_want_to_create_room = true;
						m_room_create_count++;

						m_clients_ID_to_verify_create.push_back(_client->get_client_information().m_ID);
					}
					else if (tmp_info_type == Clients::INFO_TYPE_CLIENT_SIDE::JOIN_ROOM)
					{
						us tmp_room_id(0u), tmp_room_port(0u);

						receive_packet >> tmp_room_id >> tmp_room_port;

						m_client_want_to_join_room = true;

						m_clients_ID_to_verify_join.push_back(std::make_tuple(_client->get_client_information().m_ID, tmp_room_id, tmp_room_port));
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

void Server_Network::send()
{
	m_sending_timer += Tools::getDeltaTime();

	if (m_sending_timer > 0.00833333f)
	{
		bool tmp_put_one_time(false);

		this->clients_create_room();

		this->clients_join_room();

		//This packet will contain information about the room
		//to be displayed when the client is in the lobby.
		sf::Packet tmp_client_packet;

		//This here to say to the client to create room to display them.

		tmp_client_packet << Clients::INFO_TYPE_SERVER_SIDE::ROOM_INFORMATION << m_room_create_count;

		m_rooms.remove_if([&m_console = m_console, &tmp_client_packet](const auto& _room)
			{
				if (_room->get_time_out_timer() >= 10.f && !_room->get_client_count())
				{
					m_console.add_message("Room [PORT][" + std::to_string(_room->get_port()) + "] [ID][" + std::to_string(_room->get_id()) + "] closed", Console::Message::INFO);
					tmp_client_packet << _room->get_name() << _room->get_id() << _room->get_port() << true;
					return true;
				}
				else
				{
					tmp_client_packet << _room->get_name() << _room->get_id() << _room->get_port() << false;
					return false;
				}
			});

		//if (!static_cast<unsigned>(m_clients.size()))
			this->client_disconnection(tmp_client_packet);

		m_room_create_count = 0u;
		m_sending_timer = 0.f;
	}
}

void Server_Network::clients_create_room()
{
	if (m_client_want_to_create_room)
	{
		for (auto ID = m_clients_ID_to_verify_create.begin(); ID != m_clients_ID_to_verify_create.end(); ID++)
		{
			for (auto client = m_clients.begin(); client != m_clients.end();)
			{
				if (*ID == (*client)->get_client_information().m_ID)
				{
					//And there we display a message about the client that as been disconnected.
					m_console.add_message("Player [NAME][" + (*client)->get_name() + "] [IP][" + (*client)->get_client_information().m_IP + "] [ID][" + std::to_string((*client)->get_client_information().m_ID) + "] disconnected from the menu", Console::Message::INFO);

					sf::Packet tmp_create_room_packet;
					us tmp_port((!m_rooms.size()) ? PORT + 1 : m_rooms.back()->get_port() + 1);

					tmp_create_room_packet << Clients::INFO_TYPE_SERVER_SIDE::LOBBY_TO_ROOM_INFORMATION << tmp_port;

					(*client)->send_packet(tmp_create_room_packet);

					(*client)->is_waiting_for_reconnect() = true;

					m_selector.remove(*(*client)->get_client_information().m_socket);

					m_rooms.push_back(std::make_unique<Room>(*client, m_clients_IDs, m_clients, m_console, tmp_port));

					client = m_clients.erase(client);
				}
				else
				{
					client++;
				}
			}
		}

		m_clients_ID_to_verify_create.clear();
		m_client_want_to_create_room = false;
	}
}

void Server_Network::clients_join_room()
{
	if (m_client_want_to_join_room)
	{
		for (auto ID = m_clients_ID_to_verify_join.begin(); ID != m_clients_ID_to_verify_join.end(); ID++)
		{
			for (auto client = m_clients.begin(); client != m_clients.end();)
			{
				if (std::get<0>(*ID) == (*client)->get_client_information().m_ID)
				{
					auto tmp_room = std::find_if(m_rooms.begin(), m_rooms.end(), [&ID](const auto& _room)
						{
							if (_room->get_port() == std::get<2>(*ID))
								return true;
							else
								return false;
						});
					
					if (tmp_room != m_rooms.end())
					{
						//And there we display a message about the client that as been disconnected.
						m_console.add_message("Player [NAME][" + (*client)->get_name() + "] [IP][" + (*client)->get_client_information().m_IP + "] [ID][" + std::to_string((*client)->get_client_information().m_ID) + "] disconnected from the menu", Console::Message::INFO);

						sf::Packet tmp_join_room_packet;

						//We send him port that need to connect to.
						tmp_join_room_packet << Clients::INFO_TYPE_SERVER_SIDE::LOBBY_TO_ROOM_INFORMATION << tmp_room->get()->get_port();

						(*client)->send_packet(tmp_join_room_packet);

						(*client)->is_waiting_for_reconnect() = true;

						m_selector.remove(*(*client)->get_client_information().m_socket);

						//And then we push him in the room to remove him from the main server.
						tmp_room->get()->get_clients().push_back((*client));

						client = m_clients.erase(client);
					}
					else
					{
						client++;
					}
				}
				else
				{
					client++;
				}
			}
		}

		m_clients_ID_to_verify_join.clear();
		m_client_want_to_join_room = false;
	}
}

void Server_Network::client_disconnection(sf::Packet& _packet)
{
	//There is the disconnection part.
	//If we send the packet to the client and the socket statue is Disconnected/Error
	//the client will be removed from the list and the selector.
	for (auto client = m_clients.begin(); client != m_clients.end();)
	{
		if (!(*client)->is_waiting_for_reconnect())
		{
			sf::Socket::Status tmp_socket_statue((*client)->send_packet(_packet));
			if (tmp_socket_statue == sf::Socket::Disconnected || tmp_socket_statue == sf::Socket::Error)
			{
				//And there we display a message about the client that as been disconnected.
				m_console.add_message("Player [NAME][" + (*client)->get_name() + "] [IP][" + (*client)->get_client_information().m_IP + "] [ID][" + std::to_string((*client)->get_client_information().m_ID) + "] disconnected", Console::Message::INFO);

				m_clients_IDs.remove_if([&client](us _id) { return (*client)->get_client_information().m_ID == _id; });

				m_selector.remove(*(*client)->get_client_information().m_socket);
				(*client)->get_client_information().m_socket->disconnect();

				client = m_clients.erase(client);
			}
			else
			{
				client++;
			}
		}
		else
		{
			client++;
		}
	}
}

void Server_Network::update()
{
	this->send();

	m_console.update();
}

void Server_Network::draw(sf::RenderWindow& _window)
{
	m_console.draw(_window, const_cast<sf::View&>(_window.getDefaultView()));
}