#pragma once
#include "State.h"

#include "Server_Network.h"

class Server_State : public State
{
public:
    Server_State(WindowManager& _window, StateStack* stackState);
private:
    void virtual init();
    void virtual update();
    void virtual render();
    void virtual pushState(char data);

    sf::View m_game_view;
};

