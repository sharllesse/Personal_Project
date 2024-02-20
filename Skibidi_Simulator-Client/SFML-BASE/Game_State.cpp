#include "Game_State.h"

#include "Menu_State.h"
//#include "Lobby_State.h"

Game_State::Game_State(WindowManager& _window, StateStack* stackState) : State(_window, stackState)
{
    GET_MANAGER->loadScene("GAME");
}

Game_State::~Game_State()
{
    m_main_client.release();
}

void Game_State::init()
{
    m_game_view = sf::View(sf::Vector2f(m_windowManager.getSize()) / 2.f, sf::Vector2f(m_windowManager.getSize()));

    m_main_client = std::make_unique<Clients>("Charles", sf::Vector2f(100.f, 100.f), 200.f);
    m_main_client->connect("127.0.0.1", 8000u);

    m_isReady = true;
}

void Game_State::update()
{
    //m_game_view.setCenter(m_environment->getPlayer().getPosition());

    m_main_client->update(m_windowManager.getWindow());
}

void Game_State::render()
{
    //m_windowManager.getWindow().setView(m_game_view);

    m_main_client->draw(m_windowManager.getWindow());
}

void Game_State::pushState(char data)
{
    if (data == 1)
        m_stackState->push(std::make_unique<Menu_State>(m_windowManager, m_stackState));
}