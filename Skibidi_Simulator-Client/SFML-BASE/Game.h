#include "Game_State.h"
#include "Menu_State.h"
//#include "Lobby_State"

class Game
{
	WindowManager m_windowManager;
	StateStack m_state;

    void update();
    void render();
public:
	Game();
    Game(int width, int height, std::string title, bool fullscreen);
    ~Game();

    void runGame();


};

