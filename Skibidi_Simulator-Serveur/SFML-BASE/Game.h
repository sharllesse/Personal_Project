#include "Server_State.h"

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

