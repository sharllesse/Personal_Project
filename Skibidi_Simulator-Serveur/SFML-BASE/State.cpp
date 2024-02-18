#include "State.h"

State::State(WindowManager& _window, std::stack<std::unique_ptr<State>>* stackState) : m_windowManager(_window), m_stackState(stackState), m_isReady(false), m_initIsStarted(false)
{
    static bool firstTime = true;
    if (firstTime)
    {
        m_windowManager.setSFX_Volume(50);
        m_windowManager.setMusic_Volume(50);
        m_windowManager.setMovie_Volume(50); 
        firstTime = false;
    }
}

void State::update()
{
}

//----------------------------------------------
/*Exemple d'utilisation d'une class State*/
/*Test::Test(WindowManager& _window, StateStack* stackState) : State(_window, stackState)
{
    GET_MANAGER->loadScene("Exemple");
}

void Test::init()
{
    m_game_view = sf::View(sf::Vector2f(m_windowManager.getSize()) / 2.f, sf::Vector2f(m_windowManager.getSize()));

    m_isReady = true;
}

void Test::update()
{
    Server->update();

    //if (KEY(R) and m_windowManager.timer() > 0.2f)
    //{
    //    m_windowManager.resetTimer();
    //    pushState(1);
    //}

    //m_game_view.setCenter(m_environment->getPlayer().getPosition());

    //m_environment->update(m_windowManager);
}

void Test::render()
{
    m_windowManager.getWindow().setView(m_game_view);
}

void Test::pushState(char data)
{
    if (data == 1)
        m_stackState->push(std::make_unique<Test>(m_windowManager, m_stackState));
}
//----------------------------------------------*/
