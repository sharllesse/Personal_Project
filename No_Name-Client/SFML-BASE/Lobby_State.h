#pragma once
#include "State.h"

#include "Button.h"
#include "Clients.h"

class Lobby_State : public State
{
public:
    Lobby_State(WindowManager& _window, StateStack* stackState);
    ~Lobby_State();

    enum class LOBBY_STATE
    {
        LSNULL = -1,
        INLOBBY,
        INCHOOSE,
        INNAMESELECT
    };
private:
    void virtual init();
    void virtual update();
    void virtual render();
    void virtual pushState(char data);

    void load_button(LOBBY_STATE _lobby_state);

    bool enter_name(std::string& _string, int _max_size, bool _replace_space);

    std::unique_ptr<Clients> m_main_client;
    std::string tmp_client_name;

    Button_map m_buttons;

    LOBBY_STATE m_lobby_state;
};