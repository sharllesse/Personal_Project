#include "Game.h"
#include "Steam/steam_api.h"

Game::Game() : m_windowManager()
{
}

Game::Game(int width, int height, std::string title, bool fullscreen) : m_windowManager(width, height, title, fullscreen)
{
    m_fps_counter_font.loadFromFile("../Ressources/ALL/FONTS/arial.ttf");
    m_fps_counter.setFont(m_fps_counter_font);
    m_fps_counter.setCharacterSize(20u);
    m_fps_counter.setFillColor(sf::Color::Green);
}

Game::~Game()
{
    m_state.clear();
}

void Game::update()
{
    Tools::restartClock();
    m_fps_counter.setString(std::to_string(static_cast<int>(Tools::frameRate())));

    m_windowManager.EventUpdate();
    if (!m_state.empty())
    {
        if (!m_state.front().get()->getIsReady() and GET_MANAGER->IsReady())
        {
            if (!m_state.front().get()->getIsStarted())
            {
                std::thread t([&] {m_state.front().get()->init(); });
                t.detach();
                m_state.front().get()->getIsStarted() = true;
            }
        }
        else if (m_state.front().get()->getIsReady())
            m_state.front().get()->update();

        if (static_cast<int>(m_state.size()) > 1)
            m_state.erase(std::remove_if(m_state.begin(), m_state.end(), [](std::unique_ptr<State>& _state)
                {
                    return _state->getNeedToBeDeleted();
                }), m_state.end());
    }
}

void Game::render()
{
    m_windowManager.clear(sf::Color::Black);
    if (!m_state.empty() and GET_MANAGER->IsReady() and m_state.front().get()->getIsReady())
        m_state.front().get()->render();
    else if (!GET_MANAGER->IsReady() or !m_state.front().get()->getIsReady())
        GET_MANAGER->showLoadingScreen(m_windowManager.getWindow());

    m_windowManager.draw(m_fps_counter);
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
	
    m_state.push_front(std::make_unique<Menu_State>(m_windowManager, &m_state));

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