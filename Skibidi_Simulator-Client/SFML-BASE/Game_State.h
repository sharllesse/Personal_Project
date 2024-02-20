#pragma once
#include "State.h"

#include "Clients.h"

class Game_State : public State
{
public:
    Game_State(WindowManager& _window, StateStack* stackState);
    ~Game_State();
private:
    void virtual init();
    void virtual update();
    void virtual render();
    void virtual pushState(char data);

    sf::View m_game_view;

    std::unique_ptr<Clients> m_main_client;
};