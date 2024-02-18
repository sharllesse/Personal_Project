#include "Server_State.h"

Server_State::Server_State(WindowManager& _window, StateStack* stackState) : State(_window, stackState)
{
    GET_MANAGER->loadScene("SERVER");
}

void Server_State::init()
{
    //m_game_view = sf::View(sf::Vector2f(m_windowManager.getSize()) / 2.f, sf::Vector2f(m_windowManager.getSize()));

    m_isReady = true;
}

void Server_State::update()
{
    //m_game_view.setCenter(m_environment->getPlayer().getPosition());

    Server->update();
}

void Server_State::render()
{
    m_windowManager.getWindow().setView(m_game_view);
}

void Server_State::pushState(char data)
{
    if (data == 1)
        m_stackState->push(std::make_unique<Server_State>(m_windowManager, m_stackState));
}