#include "Tools.h"

int main()
{
    sf::RenderWindow window(sf::VideoMode(1280, 720), "Arkanoid");

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