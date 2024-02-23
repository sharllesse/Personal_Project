#pragma once
#include "State.h"

#include "Button.h"

class Options_State : public State
{
public:
    Options_State(WindowManager& _window, StateList* stackState);
    ~Options_State();
private:
    void virtual init();
    void virtual update();
    void virtual render();
    void virtual pushState(char data);

    Button_map m_buttons;
};