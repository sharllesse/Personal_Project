#pragma once
#include "State.h"

#include "Button.h"

class Lobby_State : public State
{
public:
    Lobby_State(WindowManager& _window, StateStack* stackState);
    ~Lobby_State();
private:
    void virtual init();
    void virtual update();
    void virtual render();
    void virtual pushState(char data);

    Button_map m_buttons;
};