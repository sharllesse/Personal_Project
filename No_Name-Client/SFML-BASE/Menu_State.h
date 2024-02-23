#pragma once
#include "State.h"

#include "Button.h"

class Menu_State : public State
{
public:
    Menu_State(WindowManager& _window, StateList* stackState);
    ~Menu_State();
private:
    void virtual init();
    void virtual update();
    void virtual render();
    void virtual pushState(char data);

    Button_map m_buttons;
};