#include "Lobby_State.h"

#include "Menu_State.h"
#include "Game_State.h"

Lobby_State::Lobby_State(WindowManager& _window, StateStack* stackState) : State(_window, stackState)
{
    GET_MANAGER->loadScene("LOBBY");
}

Lobby_State::~Lobby_State()
{
    m_buttons.clear();
}

void Lobby_State::init()
{
    m_buttons["RETURN"] = Button((sf::Vector2f(m_windowManager.getSize()) / 2.f) + sf::Vector2f(-225.f, 0.f), sf::Vector2f(450.f, 125.f), GET_MANAGER->getFont("arial"), "Return", "Retour", Button::FRENCH, 50);
    m_buttons["CREATE_LOBBY"] = Button((sf::Vector2f(m_windowManager.getSize()) / 2.f) + sf::Vector2f(-225.f, 155.f), sf::Vector2f(450.f, 125.f), GET_MANAGER->getFont("arial"), "Create lobby", "Créer un lobby", Button::FRENCH, 50);
    m_buttons["JOIN_LOBBY"] = Button((sf::Vector2f(m_windowManager.getSize()) / 2.f) + sf::Vector2f(-225.f, 310.f), sf::Vector2f(450.f, 125.f), GET_MANAGER->getFont("arial"), "Join lobby", "Rejoindre un lobby", Button::FRENCH, 50);

    m_isReady = true;
}

void Lobby_State::update()
{
    std::for_each(m_buttons.begin(), m_buttons.end(), [&m_windowManager = m_windowManager](auto& _button)
        { _button.second.update(m_windowManager.getMousePos<sf::Vector2f>()); });

    if (m_windowManager.getWindow().hasFocus())
    {
        if (m_buttons["CREATE_LOBBY"].isPressed() && m_windowManager.timer() > 0.2f)
        {
            this->pushState(2);
            m_windowManager.resetTimer();
        }

        if (m_buttons["JOIN_LOBBY"].isPressed() && m_windowManager.timer() > 0.2f)
        {
            this->pushState(2);
            m_windowManager.resetTimer();
        }

        if (m_buttons["RETURN"].isPressed() && m_windowManager.timer() > 0.2f)
        {
            this->pushState(1);
            m_windowManager.resetTimer();
        }
    }
}

void Lobby_State::render()
{
    std::for_each(m_buttons.begin(), m_buttons.end(), [&m_windowManager = m_windowManager](auto& _button)
        { _button.second.render(m_windowManager); });
}

void Lobby_State::pushState(char data)
{
    if (data == 1)
        m_stackState->push(std::make_unique<Menu_State>(m_windowManager, m_stackState));

    if (data == 2)
        m_stackState->push(std::make_unique<Game_State>(m_windowManager, m_stackState));
}