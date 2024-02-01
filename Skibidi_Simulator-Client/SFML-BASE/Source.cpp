#include "menu.h"

int main()
{
    sf::RenderWindow window(sf::VideoMode(1280, 720), "Arkanoid");
    
    game MainGame;

    menu MainMenu;

    changeState(MENU);

    //Init
    initTools(window);

  

    while (window.isOpen())
    {
        restartClock();
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        //Update
        if (GetState() == GAME)
            MainGame.update();
        else if (GetState() == MENU)
            MainMenu.update(window);
            

        //Display
        window.clear();
        if (GetState() == GAME)
            MainGame.draw(window);
        else if (GetState() == MENU)
            MainMenu.display(window);

        window.display();
    }

    return 0;

}