#include "Clients.h"

int main()
{
    sf::RenderWindow window(sf::VideoMode(1920, 1080), "Arkanoid");
    sf::Event events;

    std::unique_ptr<Clients> m_client = std::make_unique<Clients>("Charles", sf::Vector2f(100, 100), 200.f);

    if (!m_client->connect("127.0.0.1", 8000u))
        return CONNECTION_FAIL;

    while (window.isOpen())
    {
        Tools::restart_clock();

        while (window.pollEvent(events))
        {
            if (events.type == sf::Event::Closed)
                window.close();
        }

        m_client->update(window);

        window.clear();

        m_client->draw(window);

        window.display();
    }

    return EXIT_SUCCESS;
}