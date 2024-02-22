#include "Game.h"
#include "Steam/steam_api.h"

Game::Game() : m_windowManager()
{
}

Game::Game(int width, int height, std::string title, bool fullscreen) : m_windowManager(width, height, title, fullscreen)
{
}

Game::~Game()
{
    while (!m_state.empty())
        m_state.pop();
}

void Game::update()
{
    Tools::restartClock();

    m_windowManager.EventUpdate();
    if (!m_state.empty())
    {
        if (!m_state.top().get()->getIsReady() and GET_MANAGER->IsReady())
        {
            if (!m_state.top().get()->getIsStarted())
            {
                std::thread t([&] {m_state.top().get()->init(); });
                t.detach();
                m_state.top().get()->getIsStarted() = true;
            }
        }
        else if (m_state.top().get()->getIsReady())
            m_state.top().get()->update();
    }
}

void Game::render()
{
    m_windowManager.clear(sf::Color::Black);
    if (!m_state.empty() and GET_MANAGER->IsReady() and m_state.top().get()->getIsReady())
        m_state.top().get()->render();
    else if (!GET_MANAGER->IsReady() or !m_state.top().get()->getIsReady())
        GET_MANAGER->showLoadingScreen(m_windowManager.getWindow());
    m_windowManager.display();
}

void Game::runGame()
{
    /*TODO : Modifier la texture de l'�cran de chargement*/
    GET_MANAGER->getLoadingScreen() = Animation(GET_MANAGER->getTexture("loading"), sf::IntRect(0, 0, 140, 170), 0.1f, 8);
    GET_MANAGER->getLoadingScreen().getSprite().setPosition({ (m_windowManager.getWindow().getSize().x - 140.f) / 2, (m_windowManager.getWindow().getSize().y - GET_MANAGER->getLoadingScreen().getSprite().getGlobalBounds().height) / 2 });
    /*TODO : Modifier le state de d�part de l'application*/

    //if (SteamAPI_Init())
    //    SteamInput()->Init(true);
	
    m_state.push(std::make_unique<Menu_State>(m_windowManager, &m_state));

    while (!m_windowManager.isDone() && m_windowManager.getWindow().isOpen())
    {
        if(GET_MANAGER->IsReady())
        {
            GET_MANAGER->setVolumeForAllMusic(m_windowManager.getMusic_Volume());
            GET_MANAGER->setVolumeForAllSound(m_windowManager.getSFX_Volume());
        }

        //SteamInput()->RunFrame();

        update();
        render();
    }

    //SteamAPI_Shutdown();
}