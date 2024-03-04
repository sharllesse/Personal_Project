#include "Clients.h"

Clients::Clients() :
	m_name("No Name"), m_speed(0.f), m_sending_timer(0.f), m_game_is_finish(false), m_shooted(false), m_shoot_timer(0.f), m_rotation(0.f), m_client_information(), m_waiting_for_reconnect(false)
{
}

Clients::Clients(std::string _name, sf::Vector2f _position, float _speed) :
	m_name(_name), m_position(_position), m_speed(_speed), m_sending_timer(0.f), m_game_is_finish(false), m_shooted(false), m_shoot_timer(0.f), m_rotation(0.f), m_client_information(std::string(""), 0u, true), m_waiting_for_reconnect(false)
{
    m_all_clients.setSize(sf::Vector2f(50, 50));
    m_all_clients.setOrigin(m_all_clients.getSize() / 2.f);

    m_all_projectiles.setRadius(20);
    m_all_projectiles.setOrigin(sf::Vector2f(20, 20));

	m_receive_thread = std::thread(&Clients::receive, this);

    m_aim_line = sf::VertexArray(sf::Lines);
    m_aim_line.append(sf::Vertex(sf::Vector2f(0, 0), sf::Color::Red));
    m_aim_line.append(sf::Vertex(sf::Vector2f(0, 0), sf::Color::Red));
}

Clients::Clients(std::string _name, us _ID, std::string _IP) :
	m_name(_name), m_speed(0.f), m_sending_timer(0.f), m_game_is_finish(false), m_shooted(false), m_shoot_timer(0.f), m_rotation(0.f), m_client_information(_IP, _ID, true), m_waiting_for_reconnect(false)
{
}

Clients::~Clients()
{
    m_game_is_finish = true;

    if (m_receive_thread.joinable())
        m_receive_thread.join();

    m_selector.clear();

    m_clients.clear();
}

bool Clients::connect_to_lobby(std::string _IP, us _port, float _time_out)
{
    if (this->m_client_information.m_socket->connect(_IP, _port, sf::seconds(_time_out)) == sf::Socket::Done)
    {
        sf::Packet send_packet;
        sf::Packet receive_packet;

        this->m_client_information.m_IP = this->m_client_information.m_socket->getRemoteAddress().getLocalAddress().toString();

        send_packet << false << this->m_name << this->m_client_information.m_IP;

        if (this->send_packet(send_packet) == sf::Socket::Done)
        {
            if (this->receive_packet(receive_packet) == sf::Socket::Done)
            {
                INT_TYPE tmp_client_size(0);

                std::string tmp_name("");
                us tmp_ID(0u);
                std::string tmp_IP("");

                receive_packet >> this->m_client_information.m_ID/* >> tmp_client_size*/;

                /*for (int i = 0; i < tmp_client_size; i++)
                {
                    receive_packet >> tmp_name >> tmp_ID >> tmp_IP;
                    this->m_clients.push_back(std::make_unique<Clients>(tmp_name, tmp_ID, tmp_IP));
                }*/

                this->m_client_information.m_socket->setBlocking(false);
                this->m_selector.add(*this->m_client_information.m_socket);

                return true;
            }
            else
            {
                #ifdef _DEBUG
                    std::cout << "Failed to receive the ID." << std::endl;
                #endif // _DEBUG
                return false;
            }
        }
        else
        {
            #ifdef _DEBUG
                std::cout << "Failed to send the name and the IP." << std::endl;
            #endif // _DEBUG
            return false;
        }
    }

    return false;
}

void Clients::disconnect_from_lobby()
{
    this->m_selector.remove(*this->m_client_information.m_socket);
    this->m_client_information.m_socket->setBlocking(true);
    this->m_client_information.m_socket->disconnect();
}

void Clients::create_room()
{
    sf::Packet tmp_create_room_packet;

    tmp_create_room_packet << Clients::INFO_TYPE_CLIENT_SIDE::CREATE_ROOM;

    this->send_packet(tmp_create_room_packet);
}

void Clients::leave_room()
{
    sf::Packet tmp_create_room_packet;

    tmp_create_room_packet << Clients::INFO_TYPE_CLIENT_SIDE::LEAVE_ROOM;

    this->send_packet(tmp_create_room_packet);
}

void Clients::room_connection(sf::Packet& _packet)
{
    us tmp_port;

    _packet >> tmp_port;

    m_selector.remove(*m_client_information.m_socket);
    m_client_information.m_socket->setBlocking(true);
    m_client_information.m_socket->disconnect();

    if (m_client_information.m_socket->connect(IP, tmp_port) == sf::Socket::Done)
    {
        sf::Packet tmp_id_packet;
        sf::Packet receive_packet;

        tmp_id_packet << m_client_information.m_ID;

        if (this->send_packet(tmp_id_packet) == sf::Socket::Done)
        {
            if (this->receive_packet(receive_packet) == sf::Socket::Done)
            {
                //Faire le recu de tout les autre client dans la room.
                //Faire en sorte que les id soit unique.
                INT_TYPE tmp_client_size(0);

                std::string tmp_name("");
                us tmp_ID(0u);
                std::string tmp_IP("");

                receive_packet >> tmp_client_size;

                for (int i = 0; i < tmp_client_size; i++)
                {
                    receive_packet >> tmp_name >> tmp_ID >> tmp_IP;
                    this->m_clients.push_back(std::make_shared<Clients>(tmp_name, tmp_ID, tmp_IP));
                }

                m_client_information.m_socket->setBlocking(false);
                m_selector.add(*m_client_information.m_socket);
            }
        }
    }

    //PORT
    //IP

    //Le client envoie une info comme quoi il veut créer ca room.
    //Le serveur resoir l'info et Un thread qui contient la room va etre lancé.
    //La room auras un state LOBBY qui va juste verifié les connection et va les prendre et aussi envoyé les info avec les bouton de chacun.
    //Et apres in game on change de state pour update la room en question.
}

void Clients::server_connection(sf::Packet& _packet)
{
    us tmp_port;

    _packet >> tmp_port;

    m_selector.remove(*m_client_information.m_socket);
    m_client_information.m_socket->setBlocking(true);
    m_client_information.m_socket->disconnect();

    if (m_client_information.m_socket->connect(IP, tmp_port) == sf::Socket::Done)
    {
        sf::Packet tmp_id_packet;
        sf::Packet receive_packet;

        tmp_id_packet << true << m_client_information.m_ID;

        if (this->send_packet(tmp_id_packet) == sf::Socket::Done)
        {
            if (this->receive_packet(receive_packet) == sf::Socket::Done)
            {
                //Faire le recu de tout les autre client dans la room.
                //Faire en sorte que les id soit unique.
                INT_TYPE tmp_client_size(0);

                std::string tmp_name("");
                us tmp_ID(0u);
                std::string tmp_IP("");

                receive_packet >> tmp_client_size;

                /*for (int i = 0; i < tmp_client_size; i++)
                {
                    receive_packet >> tmp_name >> tmp_ID >> tmp_IP;
                    this->m_clients.push_back(std::make_shared<Clients>(tmp_name, tmp_ID, tmp_IP));
                }*/

                m_client_information.m_socket->setBlocking(false);
                m_selector.add(*m_client_information.m_socket);
            }
        }
    }
}

void Clients::clients_information(sf::Packet& _packet)
{
    /*INT_TYPE tmp_client_cout(0);

    us tmp_ID(0u);
    sf::Vector2f tmp_position;
    float tmp_rotation;

    _packet >> tmp_client_cout;

    for (auto client = m_clients.begin(); client != m_clients.end();)
    {
        _packet >> tmp_ID;

        if (tmp_ID != this->m_client_information.m_ID)
        {
            _packet >> (*client)->m_position >> (*client)->m_rotation;

            client++;
        }
        else
        {
            _packet >> tmp_position >> tmp_rotation;
        }

        tmp_client_cout--;
    }

    if (tmp_client_cout != 0)
        _packet >> tmp_ID >> tmp_position >> tmp_rotation;*/
}

void Clients::clients_connected(sf::Packet& _packet)
{
    this->m_delete_client.lock();

    this->m_clients.clear();

    INT_TYPE tmp_client_count(0);

    std::string tmp_name("");
    us tmp_ID(0u);
    std::string tmp_IP("");

    _packet >> tmp_client_count;

    for (int i = 0; i < tmp_client_count; i++)
    {
        _packet >> tmp_name >> tmp_ID >> tmp_IP;

        if (this->m_client_information.m_ID != tmp_ID)
            this->m_clients.push_back(std::make_unique<Clients>(tmp_name, tmp_ID, tmp_IP));
    }

    _packet.clear();

    this->m_delete_client.unlock();
}

void Clients::clients_disconnected(sf::Packet& _packet)
{
    this->m_delete_client.lock();

   /* while (!_packet.endOfPacket())
    {
        us tmp_ID(0u);

        _packet >> tmp_ID;

        this->m_clients.erase(std::remove_if(this->m_clients.begin(), this->m_clients.end(), [tmp_ID](std::unique_ptr<Clients>& _client)
            {
                if (_client->m_client_information.m_ID == tmp_ID)
                    return true;
                else
                    return false;
            }));
    }*/

    this->m_delete_client.unlock();
}

void Clients::projectiles_information(sf::Packet& _packet)
{
    int projectiles_shooted(0);

    _packet >> projectiles_shooted;

    for (int i = 0; i < projectiles_shooted; i++)
    {
        m_projectiles.push_back(std::make_unique<Projectile>());
    }

    for (auto projectile = m_projectiles.begin(); projectile != m_projectiles.end();)
    {
        _packet >> (*projectile)->get_player_ID() >> (*projectile)->get_position() >> (*projectile)->get_deleted();

        if ((*projectile)->get_deleted())
        {
            m_delete_projectiles.lock();
            projectile = m_projectiles.erase(projectile);
            m_delete_projectiles.unlock();
        }
        else
            projectile++;
    }
}

void Clients::receive()
{
    while (!this->m_game_is_finish)
    {
        if (this->m_selector.wait(sf::seconds(0.001f)))
        {
            if (this->m_selector.isReady(*this->m_client_information.m_socket))
            {
                sf::Packet tmp_receive_packet;
                if (this->receive_packet(tmp_receive_packet) == sf::Socket::Done)
                {
                    Clients::INFO_TYPE_SERVER_SIDE tmp_info_type(Clients::INFO_TYPE_SERVER_SIDE::ITSNULL);

                    tmp_receive_packet >> tmp_info_type;

                    if (tmp_info_type == Clients::INFO_TYPE_SERVER_SIDE::LOBBY_TO_ROOM_INFORMATION)
                    {
                        this->room_connection(tmp_receive_packet);
                    }
                    if (tmp_info_type == Clients::INFO_TYPE_SERVER_SIDE::ROOM_TO_LOBBY_INFORMATION)
                    {
                        this->server_connection(tmp_receive_packet);
                    }
                    else if (tmp_info_type == Clients::INFO_TYPE_SERVER_SIDE::JOIN_INFORMATION)
                    {
                        this->clients_connected(tmp_receive_packet);
                    }

                    /*if (tmp_info_type == Clients::INFO_TYPE_SERVER_SIDE::CLIENT_INFORMATION)
                    {
                        this->clients_information(tmp_receive_packet);
                    }
                    else if (tmp_info_type == Clients::INFO_TYPE_SERVER_SIDE::JOIN_INFORMATION)
                    {
                        this->clients_connected(tmp_receive_packet);
                    }
                    else if (tmp_info_type == Clients::INFO_TYPE_SERVER_SIDE::DISCONNECTED_INFORMATION)
                    {
                        this->clients_disconnected(tmp_receive_packet);
                    }
                    else if (tmp_info_type == Clients::INFO_TYPE_SERVER_SIDE::PROJECTILES_INFORMATION)
                    {
                        this->projectiles_information(tmp_receive_packet);
                    }*/
                }
            }
        }
    }
}

void Clients::send()
{
    this->m_sending_timer += Tools::getDeltaTime();

    if (this->m_sending_timer > 0.00833333f)
    {
       /* sf::Packet sending_transfrom_packet;
        sf::Packet sending_shoot_packet;

        sending_transfrom_packet << Clients::INFO_TYPE_CLIENT_SIDE::TRANSFORM << this->m_position << this->m_rotation;

        this->send_packet(sending_transfrom_packet);

        if (m_shooted)
        {
            sending_shoot_packet << Clients::INFO_TYPE_CLIENT_SIDE::SHOOT << 1 << 0.f << m_client_information.m_ID << 1000.f << this->m_position << this->m_rotation;

            this->send_packet(sending_shoot_packet);

            m_shooted = false;
        }*/

        this->m_sending_timer = 0.f;
    }
}

sf::Socket::Status Clients::send_packet(sf::Packet& _packet)
{
    return this->m_client_information.m_socket->send(_packet);
}

sf::Socket::Status Clients::receive_packet(sf::Packet& _packet)
{
    return this->m_client_information.m_socket->receive(_packet);
}

void Clients::update_Game(sf::RenderWindow& _window)
{
    m_shoot_timer += Tools::getDeltaTime();

    if (_window.hasFocus())
    {
        /*this->m_mouse_position = _window.mapPixelToCoords(sf::Mouse::getPosition(_window));

        if (KEY(Z))
            this->m_position.y -= 200.f * Tools::getDeltaTime();

        if (KEY(S))
            this->m_position.y += 200.f * Tools::getDeltaTime();

        if (KEY(Q))
            this->m_position.x -= 200.f * Tools::getDeltaTime();

        if (KEY(D))
            this->m_position.x += 200.f * Tools::getDeltaTime();

        if (MOUSE(Left) && m_shoot_timer > 0.2f)
        {
            m_shooted = true;

            m_shoot_timer = 0.f;
        }
        
        m_rotation = atan2(m_mouse_position.y - m_position.y, m_mouse_position.x - m_position.x) * RAD2DEG;

        this->m_aim_line[0].position = sf::Vector2f(m_position);
        this->m_aim_line[1].position = sf::Vector2f(m_mouse_position);*/
    }

    this->send();
}

void Clients::draw(sf::RenderWindow& _window)
{
    this->draw_clients(_window);
    this->draw_projectiles(_window);
}

void Clients::draw_clients(sf::RenderWindow& _window)
{
    this->m_delete_client.lock();

   /* this->m_all_clients.setFillColor(sf::Color::White);
    std::for_each(this->m_clients.begin(), this->m_clients.end(), [&](std::unique_ptr<Clients>& _client)
        {
            this->m_all_clients.setPosition(_client->m_position);
            this->m_all_clients.setRotation(_client->m_rotation);
            _window.draw(this->m_all_clients);
        });*/

    this->m_delete_client.unlock();

    /*_window.draw(this->m_aim_line);

    this->m_all_clients.setFillColor(sf::Color::Red);
    this->m_all_clients.setPosition(this->m_position);
    this->m_all_clients.setRotation(this->m_rotation);
    _window.draw(this->m_all_clients);*/
}

void Clients::draw_projectiles(sf::RenderWindow& _window)
{
    m_delete_projectiles.lock();
    /*std::for_each(m_projectiles.begin(), m_projectiles.end(), [&](std::unique_ptr<Projectile>& _projectiles)
        {
            m_all_projectiles.setPosition(_projectiles->get_position());
            _window.draw(m_all_projectiles);
        });*/
    m_delete_projectiles.unlock();
}