#include "Clients.h"

int main()
{
    sf::RenderWindow window(sf::VideoMode(1920, 1080), "Arkanoid");

    sf::SocketSelector m_selector;
    std::unique_ptr<Clients> m_client;
    std::vector<std::unique_ptr<Clients>> m_clients;

    if (m_client->m_client_information.m_socket->connect("127.0.0.1", 8000u) == sf::Socket::Done)
    {
        sf::Packet send_packet;
        sf::Packet receive_packet;

        m_client->m_client_information.m_socket->setBlocking(false);

        std::string tmp_ip(m_client->m_client_information.m_socket->getRemoteAddress().getLocalAddress().toString());

        send_packet << m_client->m_name << tmp_ip;

        m_client->m_client_information.m_socket->send(send_packet);

        m_client->m_client_information.m_socket->receive(receive_packet);

        receive_packet >> m_client->m_client_information.m_ID;

        m_selector.add(*m_client->m_client_information.m_socket);
    }

    while (window.isOpen())
    {
        Tools::restart_clock();
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        window.clear();

        window.display();
    }

    return 0;

}