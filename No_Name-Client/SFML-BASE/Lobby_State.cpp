#include "Lobby_State.h"

#include "Menu_State.h"
#include "Game_State.h"

Lobby_State::Lobby_State(WindowManager& _window, StateList* stackState) : State(_window, stackState)
{
    GET_MANAGER->loadScene("LOBBY");

    m_lobby_state = LOBBY_STATE::LSNULL;
    tmp_client_name = "";
}

Lobby_State::~Lobby_State()
{
    m_buttons.clear();
}

void Lobby_State::init()
{
    m_lobby_state = LOBBY_STATE::LSNULL;

    this->load_button(LOBBY_STATE::INNAMESELECT);

    m_join_server_background.setFillColor(sf::Color(255, 255, 255, 200));
    m_join_server_background.setOutlineColor(sf::Color(255, 255, 255, 150));
    m_join_server_background.setOutlineThickness(3.f);
    m_join_server_background.setSize(sf::Vector2f((static_cast<float>(m_windowManager.getSize().x) - 450.f) - 33.f, static_cast<float>(m_windowManager.getSize().y) - 20.f));
    m_join_server_background.setPosition(sf::Vector2f(450.f + 23.f, 10.f));

    m_isReady = true;
}

void Lobby_State::update()
{
    std::for_each(m_buttons.begin(), m_buttons.end(), [&m_windowManager = m_windowManager](auto& _button)
        { _button.second.update(m_windowManager.getMousePos<sf::Vector2f>()); });

    //If the client click on the "Enter your name" button he will be set to the INNAMESELECT state.
    if (m_windowManager.getWindow().hasFocus() && m_lobby_state == LOBBY_STATE::LSNULL)
    {
        if (m_buttons["ENTER_NAME"].isPressed() && m_windowManager.timer() > 0.2f)
        {
            m_lobby_state = LOBBY_STATE::INNAMESELECT;

            m_buttons["ENTER_NAME"].set_locked(true);
            m_windowManager.StopEventUpdate(true);

            m_windowManager.resetTimer();
        }
    }

    //Here the client enter his name and the moment he confirm his name he go to the INCHOOSE state.
    if (m_lobby_state == LOBBY_STATE::INNAMESELECT)
    {
        m_buttons["ENTER_NAME"].setText(tmp_client_name);
        if (this->enter_name(tmp_client_name, 10, false))
        {
            m_lobby_state = LOBBY_STATE::INCHOOSE;
            this->load_button(m_lobby_state);

            m_main_client = std::make_unique<Clients>(tmp_client_name, sf::Vector2f(100.f, 100.f), 200.f);
            if (!m_main_client->connect("127.0.0.1", 8000u))
            {
                //this->pushState(1);
            }

            m_windowManager.StopEventUpdate(false);
            m_buttons["ENTER_NAME"].set_locked(false);
        }

        if (m_buttons["RETURN"].isPressed() && m_windowManager.timer() > 0.2f)
        {
            this->pushState(1);
            m_windowManager.resetTimer();
        }
    }

    //m_main_client->update_UI(m_windowManager.getWindow());

    if (m_windowManager.getWindow().hasFocus())
    {
        if (m_lobby_state == LOBBY_STATE::INCHOOSE)
        {
            if (m_buttons["CREATE_LOBBY"].isPressed() && m_windowManager.timer() > 0.2f)
            {
                m_lobby_state = LOBBY_STATE::INLOBBY;
                this->load_button(m_lobby_state);

                //this->pushState(2);
                m_windowManager.resetTimer();
            }

            if (m_buttons["RETURN"].isPressed() && m_windowManager.timer() > 0.2f)
            {
                m_lobby_state = LOBBY_STATE::INNAMESELECT;
                this->load_button(m_lobby_state);
                m_lobby_state = LOBBY_STATE::LSNULL;
                tmp_client_name = "";

                m_windowManager.resetTimer();
            }
        }
        else if (m_lobby_state == LOBBY_STATE::INLOBBY)
        {
            if (m_buttons["QUIT"].isPressed() && m_windowManager.timer() > 0.2f)
            {
                m_lobby_state = LOBBY_STATE::INCHOOSE;
                this->load_button(m_lobby_state);

                //this->pushState(1);
                m_windowManager.resetTimer();
            }
        }
    }
}

void Lobby_State::render()
{
    if (m_lobby_state == LOBBY_STATE::INCHOOSE)
        m_windowManager.draw(m_join_server_background);

    std::for_each(m_buttons.begin(), m_buttons.end(), [&m_windowManager = m_windowManager](auto& _button)
        { _button.second.render(m_windowManager); });
}

void Lobby_State::pushState(char data)
{
    m_needToBeDeleted = true;

    if (data == 1)
        m_stackState->push_front(std::make_unique<Menu_State>(m_windowManager, m_stackState));

    if (data == 2)
        m_stackState->push_front(std::make_unique<Game_State>(m_windowManager, m_stackState));
}

void Lobby_State::load_button(LOBBY_STATE _lobby_state)
{
    if (_lobby_state == LOBBY_STATE::INCHOOSE)
    {
        m_buttons.clear();

        m_buttons["RETURN"] = Button(sf::Vector2f(10, (static_cast<float>(m_windowManager.getSize().y) - 125.f) - 10.f), sf::Vector2f(450.f, 125.f), GET_MANAGER->getFont("arial"), "Return", "Retour", Button::LANGUAGE::FRENCH, 50);
        m_buttons["CREATE_LOBBY"] = Button(sf::Vector2f(10, 10), sf::Vector2f(450.f, 125.f), GET_MANAGER->getFont("arial"), "Create lobby", "Créer un lobby", Button::LANGUAGE::FRENCH, 50);
    }
    else if (_lobby_state == LOBBY_STATE::INLOBBY)
    {
        m_buttons.clear();

        m_buttons["QUIT"] = Button((sf::Vector2f(m_windowManager.getSize()) / 2.f) + sf::Vector2f(-225.f, 0.f), sf::Vector2f(450.f, 125.f), GET_MANAGER->getFont("arial"), "Quit", "Quitter", Button::LANGUAGE::FRENCH, 50);
    }
    else if (_lobby_state == LOBBY_STATE::INNAMESELECT)
    {
        m_buttons.clear();

        m_buttons["ENTER_NAME"] = Button((sf::Vector2f(m_windowManager.getSize()) / 2.f) + sf::Vector2f(-225.f, 0.f), sf::Vector2f(450.f, 125.f), GET_MANAGER->getFont("arial"), "Enter your name", "Entrer votre nom", Button::LANGUAGE::FRENCH, 50);
        m_buttons["RETURN"] = Button((sf::Vector2f(m_windowManager.getSize()) / 2.f) + sf::Vector2f(-225.f, 135.f), sf::Vector2f(450.f, 125.f), GET_MANAGER->getFont("arial"), "Return", "Retour", Button::LANGUAGE::FRENCH, 50);
    }
}

bool Lobby_State::enter_name(std::string& _string, int _max_size, bool _replace_space)
{
    while (m_windowManager.pollEvent())
    {
        if (m_windowManager.getEvent().type == sf::Event::TextEntered)
        {
            if (m_windowManager.getEvent().text.unicode == 13 and _string.size() >= 3) {
                return true;
            }
            else if (m_windowManager.getEvent().text.unicode == 8 && !_string.empty()) {
                _string.pop_back();
            }
            else if (m_windowManager.getEvent().text.unicode == ' ' && !_replace_space && _string.size() < _max_size) {
                _string += ' ';
            }
            else if (m_windowManager.getEvent().text.unicode == ' ' && _replace_space && _string.size() < _max_size) {
                _string += '_';
            }
            else if (m_windowManager.getEvent().text.unicode >= 32 && m_windowManager.getEvent().text.unicode <= 126 && _string.size() < _max_size) {
                _string += static_cast<char>(m_windowManager.getEvent().text.unicode);
            }
        }
        else if (m_windowManager.getEvent().type == sf::Event::Closed)
            m_windowManager.getWindow().close();
    }

    return false;
}
