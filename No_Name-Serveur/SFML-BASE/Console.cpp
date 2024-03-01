#include "Console.h"

Console::Console()
{
	m_message_view = sf::View(sf::Vector2f(1920.f / 2.f, 972.f / 2.f), sf::Vector2f(1920, 972.f));
	m_message_view.setViewport(sf::FloatRect(0.f, 0.009f, 1.f, 0.89f));

	m_messages_text_handle.setFont(GET_MANAGER->getFont("arial"));
	m_messages_text_handle.setCharacterSize(30u);

	m_message_type_handle.setFont(GET_MANAGER->getFont("arial"));
	m_message_type_handle.setCharacterSize(30u);

	m_space_console = "";
}

Console::~Console()
{
}

void Console::update()
{
	//if (KEY(R))
		//m_console_messages.push_front(Message(std::to_string(Tools::Rand(1, 10000)), Message::INFO));
}

void Console::draw(sf::RenderWindow& _window, sf::View& _base_view)
{
	_window.setView(m_message_view);

	unsigned message_index(1u);
	std::for_each(m_console_messages.begin(), m_console_messages.end(), [&](Message& _message)
		{
			if (_message.m_message_type == Message::INFO)
			{
				m_space_console = "						";
				m_message_type_handle.setString("[Server/Info] : ");
				m_message_type_handle.setFillColor(sf::Color::Blue);
			}
			else if (_message.m_message_type == Message::WARN)
			{
				m_space_console = "						";
				m_message_type_handle.setString("[Server/Warn] : ");
				m_message_type_handle.setFillColor(sf::Color::Yellow);
			}
			else if (_message.m_message_type == Message::COMMAND)
			{
				m_space_console = "								   ";
				m_message_type_handle.setString("[Server/Command] : ");
				m_message_type_handle.setFillColor(sf::Color::Cyan);
			}
			else if (_message.m_message_type == Message::ERROR)
			{
				m_space_console = "						  ";
				m_message_type_handle.setString("[Server/Error] : ");
				m_message_type_handle.setFillColor(sf::Color::Red);
			}

			m_message_type_handle.setPosition(sf::Vector2f(10.f, -static_cast<float>(m_messages_text_handle.getCharacterSize() * message_index) + (m_message_view.getSize().y - 10.f)));
	
			m_messages_text_handle.setString(m_space_console + _message.m_message);
			m_messages_text_handle.setPosition(sf::Vector2f(10.f, -static_cast<float>(m_messages_text_handle.getCharacterSize() * message_index) + (m_message_view.getSize().y - 10.f)));

			_window.draw(m_message_type_handle);
			_window.draw(m_messages_text_handle);
			message_index++;
		});


	if (static_cast<int>(m_console_messages.size()) > 100)
		m_console_messages.erase(std::next(m_console_messages.begin(), static_cast<int>(m_console_messages.size()) - 1));

	_window.setView(_base_view);
}
