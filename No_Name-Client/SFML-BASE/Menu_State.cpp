#include "Menu_State.h"

//#include "Game_State.h"
#include "Options_State.h"
#include "Lobby_State.h"

Menu_State::Menu_State(WindowManager& _window, StateList* stackState) : State(_window, stackState)
{
    GET_MANAGER->loadScene("MENU");
}

Menu_State::~Menu_State()
{
    m_buttons.clear();
}

void Menu_State::init()
{
    m_buttons["PLAY"] = Button((sf::Vector2f(m_windowManager.getSize()) / 2.f) + sf::Vector2f(-225.f, 0.f), sf::Vector2f(450.f, 125.f), GET_MANAGER->getFont("arial"), "Play", "Jouer", Button::LANGUAGE::FRENCH, 50);
    m_buttons["OPTIONS"] = Button((sf::Vector2f(m_windowManager.getSize()) / 2.f) + sf::Vector2f(-225.f, 155.f), sf::Vector2f(450.f, 125.f), GET_MANAGER->getFont("arial"), "Options", "Options", Button::LANGUAGE::FRENCH, 50);
    m_buttons["QUIT"] = Button((sf::Vector2f(m_windowManager.getSize()) / 2.f) + sf::Vector2f(-225.f, 310.f), sf::Vector2f(450.f, 125.f), GET_MANAGER->getFont("arial"), "Quit", "Quitter", Button::LANGUAGE::FRENCH, 50);

    m_isReady = true;
}

void Menu_State::update()
{
    std::for_each(m_buttons.begin(), m_buttons.end(), [&m_windowManager = m_windowManager](auto& _button) 
        { _button.second.update(m_windowManager.getMousePos<sf::Vector2f>()); });

    if (m_windowManager.getWindow().hasFocus())
    {
        if (m_buttons["PLAY"].isPressed() && m_windowManager.timer() > 0.2f)
        {
            this->pushState(1);
            m_windowManager.resetTimer();
        }

        if (m_buttons["OPTIONS"].isPressed() && m_windowManager.timer() > 0.2f)
        {
            this->pushState(2);
            m_windowManager.resetTimer();
        }

        if (m_buttons["QUIT"].isPressed() && m_windowManager.timer() > 0.2f)
        {
            m_windowManager.getWindow().close();
            m_windowManager.resetTimer();
        }
    }
}

void Menu_State::render()
{
    std::for_each(m_buttons.begin(), m_buttons.end(), [&m_windowManager = m_windowManager](auto& _button) 
        { _button.second.render(m_windowManager); });
}

void Menu_State::pushState(char data)
{
    if (data == 1)
    {
        m_needToBeDeleted = true;
        m_stackState->push_front(std::make_unique<Lobby_State>(m_windowManager, m_stackState));
    }

    if (data == 2)
        m_stackState->push_front(std::make_unique<Options_State>(m_windowManager, m_stackState));
}