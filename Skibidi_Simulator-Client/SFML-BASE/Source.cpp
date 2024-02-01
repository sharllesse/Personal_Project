#include "Clients.h"

int main()
{
    sf::RenderWindow window(sf::VideoMode(1920, 1080), "Arkanoid");
    sf::Event events;

    sf::SocketSelector m_selector;
    std::unique_ptr<Clients> m_client = std::make_unique<Clients>("Charles", sf::Vector2f(100, 100), 200.f);
    std::vector<std::unique_ptr<Clients>> m_clients;

    sf::RectangleShape m_all_clients(sf::Vector2f(50, 50));

    float m_sending_timer(0.f);

    bool game_is_finish(false);

    if (m_client->m_client_information.m_socket->connect("127.0.0.1", 8000u, sf::seconds(5.f)) == sf::Socket::Done)
    {
        sf::Packet send_packet;
        sf::Packet receive_packet;

        m_client->m_client_information.m_IP = m_client->m_client_information.m_socket->getRemoteAddress().getLocalAddress().toString();

        send_packet << m_client->m_name << m_client->m_client_information.m_IP;

        if (m_client->m_client_information.m_socket->send(send_packet) == sf::Socket::Done)
        {
            if (m_client->m_client_information.m_socket->receive(receive_packet) == sf::Socket::Done)
            {
                INT_TYPE tmp_client_size(0);
                
                std::string tmp_name("");
                unsigned short tmp_ID(0u);
                std::string tmp_IP("");

                receive_packet >> m_client->m_client_information.m_ID >> tmp_client_size;

                for (int i = 0; i < tmp_client_size; i++)
                {
                    receive_packet >> tmp_name >> tmp_ID >> tmp_IP;
                    m_clients.push_back(std::make_unique<Clients>(tmp_name, tmp_ID, tmp_IP));
                }

                m_client->m_client_information.m_socket->setBlocking(false);
                m_selector.add(*m_client->m_client_information.m_socket);
            }
            else
            {
                std::cout << "Failed to receive the ID." << std::endl;
                return CONNECTION_FAIL;
            }
        }
        else
        {
            std::cout << "Failed to send the name and the IP." << std::endl;
            return CONNECTION_FAIL;
        }
    }

    std::thread receive_thread([&]()
        {
            while (!game_is_finish)
            {
                if (m_selector.wait(sf::seconds(0.05f)))
                {   
                    if (m_selector.isReady(*m_client->m_client_information.m_socket))
                    {
                        sf::Packet m_receive_packet;
                        if (m_client->m_client_information.m_socket->receive(m_receive_packet) == sf::Socket::Done)
                        {
                            Clients::INFO_TYPE_SERVER_SIDE tmp_info_type(Clients::INFO_TYPE_SERVER_SIDE::ITSNULL);

                            m_receive_packet >> tmp_info_type;
                            
                            if (tmp_info_type == Clients::INFO_TYPE_SERVER_SIDE::CLIENT_INFORMATION)
                            {
                                INT_TYPE tmp_client_cout(0);

                                unsigned short tmp_ID(0u);
                                sf::Vector2f tmp_position;

                                m_receive_packet >> tmp_client_cout;

                                for (auto client = m_clients.begin(); client != m_clients.end();)
                                {
                                    m_receive_packet >> tmp_ID;

                                    if (tmp_ID != m_client->m_client_information.m_ID)
                                    {
                                        m_receive_packet >> (*client)->m_position;

                                        client++;
                                    }
                                    else
                                    {
                                        m_receive_packet >> tmp_position;
                                    }

                                    tmp_client_cout--;
                                }

                                if (tmp_client_cout != 0)
                                    m_receive_packet >> tmp_ID >> tmp_position;
                            }
                            else if (tmp_info_type == Clients::INFO_TYPE_SERVER_SIDE::JOIN_INFORMATION)
                            {
                                m_clients.clear();

                                INT_TYPE tmp_client_count(0);

                                std::string tmp_name("");
                                unsigned short tmp_ID(0u);
                                std::string tmp_IP("");

                                m_receive_packet >> tmp_client_count;

                                for (int i = 0; i < tmp_client_count; i++)
                                {
                                    m_receive_packet >> tmp_name >> tmp_ID >> tmp_IP;

                                    if (m_client->m_client_information.m_ID != tmp_ID)
                                        m_clients.push_back(std::make_unique<Clients>(tmp_name, tmp_ID, tmp_IP));
                                }
                            }
                        }
                    }
                }
            }
        });

    //FAIRE LE LEFT DES CLIENTS.

    while (window.isOpen())
    {
        Tools::restart_clock();

        while (window.pollEvent(events))
        {
            if (events.type == sf::Event::Closed)
                window.close();
        }

        m_sending_timer += Tools::get_delta_time();

        if (m_sending_timer > 0.00833333f)
        {
            sf::Packet sending_transfrom_packet;

            sending_transfrom_packet << Clients::INFO_TYPE_CLIENT_SIDE::TRANSFORM << m_client->m_position;

            m_client->m_client_information.m_socket->send(sending_transfrom_packet);

            m_sending_timer = 0.f;
        }

        if (window.hasFocus())
        {
            if (KEY(Z))
                m_client->m_position.y -= 200.f * Tools::get_delta_time();

            if (KEY(S))
                m_client->m_position.y += 200.f * Tools::get_delta_time();

            if (KEY(Q))
                m_client->m_position.x -= 200.f * Tools::get_delta_time();

            if (KEY(D))
                m_client->m_position.x += 200.f * Tools::get_delta_time();
        }

        window.clear();

        m_all_clients.setFillColor(sf::Color::Red);
        std::for_each(m_clients.begin(), m_clients.end(), [&m_all_clients, &m_client, &window](std::unique_ptr<Clients>& _client)
            {
                if (_client->m_client_information.m_ID != m_client->m_client_information.m_ID)
                {
                    m_all_clients.setPosition(_client->m_position);
                    window.draw(m_all_clients);
                }
            });

        m_all_clients.setFillColor(sf::Color::White);
        m_all_clients.setPosition(m_client->m_position);
        window.draw(m_all_clients);

        window.display();
    }

    game_is_finish = true;

    receive_thread.join();

    return EXIT_SUCCESS;
}