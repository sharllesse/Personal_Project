#pragma once
#include "State.h"

#include "Button.h"
#include "Clients.h"

class Lobby_State : public State
{
public:
    Lobby_State(WindowManager& _window, StateList* stackState);
    ~Lobby_State();
private:
    void virtual init();
    void virtual update();
    void virtual render();
    void virtual pushState(char data);

    void load_button(std::shared_ptr<Clients::CLIENT_STATE> _lobby_state);

    bool enter_name(std::string& _string, int _max_size, bool _replace_space);

    std::shared_ptr<Clients::CLIENT_STATE> m_lobby_state;

    std::unique_ptr<Clients> m_main_client;

    sf::RectangleShape m_join_server_background;

    std::string tmp_client_name;

    Button_map m_buttons;
};