#include "Game.h"

int main()
{
    Game main_game(1920, 1080, "Client", false);
    main_game.runGame();

    return EXIT_SUCCESS;
}