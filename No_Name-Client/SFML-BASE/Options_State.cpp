#include "Options_State.h"

#include "Menu_State.h"

Options_State::Options_State(WindowManager& _window, StateList* stackState) : State(_window, stackState)
{
    GET_MANAGER->loadScene("OPTION");
}

Options_State::~Options_State()
{
    m_buttons.clear();
}

void Options_State::init()
{
    //m_buttons["PLAY"] = Button((sf::Vector2f(m_windowManager.getSize()) / 2.f) + sf::Vector2f(-225.f, 0.f), sf::Vector2f(450.f, 125.f), GET_MANAGER->getFont("arial"), "Play", "Jouer", Button::FRENCH, 50);
    m_buttons["FULLSCREEN"] = Button((sf::Vector2f(m_windowManager.getSize()) / 2.f) + sf::Vector2f(-225.f, 155.f), sf::Vector2f(450.f, 125.f), GET_MANAGER->getFont("arial"), "Fullscreen", "Plein Ecran", Button::LANGUAGE::FRENCH, 50);
    m_buttons["QUIT"] = Button((sf::Vector2f(m_windowManager.getSize()) / 2.f) + sf::Vector2f(-225.f, 310.f), sf::Vector2f(450.f, 125.f), GET_MANAGER->getFont("arial"), "Quit", "Quitter", Button::LANGUAGE::FRENCH, 50);

    m_isReady = true;
}

void Options_State::update()
{
    std::for_each(m_buttons.begin(), m_buttons.end(), [&m_windowManager = m_windowManager](auto& _button)
        { _button.second.update(m_windowManager.getMousePos<sf::Vector2f>()); });

    //if (m_buttons["PLAY"].isPressed() && m_windowManager.timer() > 0.2f)
    //{
    //    this->pushState(1);
    //    m_windowManager.resetTimer();
    //}

    if (m_buttons["FULLSCREEN"].isPressed() && m_windowManager.timer() > 0.2f)
    {
        m_windowManager.toggleFullscreen();
        m_windowManager.resetTimer();
    }

    if (m_buttons["QUIT"].isPressed() && m_windowManager.timer() > 0.2f)
    {
        this->pushState(1);
        m_windowManager.resetTimer();
    }
}

void Options_State::render()
{
    std::for_each(m_buttons.begin(), m_buttons.end(), [&m_windowManager = m_windowManager](auto& _button)
        { _button.second.render(m_windowManager); });
}

void Options_State::pushState(char data)
{
    m_needToBeDeleted = true;
}