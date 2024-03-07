#include "Server_State.h"

Server_State::Server_State(WindowManager& _window, StateStack* stackState) : State(_window, stackState)
{
    GET_MANAGER->loadScene("SERVER");
}

void Server_State::init()
{
    m_game_view = sf::View(sf::Vector2f(m_windowManager.getSize()) / 2.f, sf::Vector2f(m_windowManager.getSize()));

    m_server = std::make_unique<Server_Network>();

    m_shape.setSize(sf::Vector2f(960.f - 6.f, 486.f));
    m_shape.setOrigin(sf::Vector2f((960.f - 6.f) / 2.f, 486.f / 2.f));
    m_shape.setPosition(sf::Vector2f(960.f / 2.f, (486.f / 2.f) + 3.f));
    m_shape.setFillColor(sf::Color(255u, 255u, 255u, 150u));
    m_shape.setOutlineThickness(3.f);
    m_shape.setOutlineColor(sf::Color(255u, 255u, 255u, 100u));

    m_isReady = true;
}

void Server_State::update()
{
    //m_game_view.setCenter(m_environment->getPlayer().getPosition());

    m_server->update();
}

void Server_State::render()
{
    //m_windowManager.getWindow().setView(m_game_view);

    m_windowManager.draw(m_shape);
    
    m_server->draw(m_windowManager.getWindow());
}

void Server_State::pushState(char data)
{
    if (data == 1)
        m_stackState->push(std::make_unique<Server_State>(m_windowManager, m_stackState));
}