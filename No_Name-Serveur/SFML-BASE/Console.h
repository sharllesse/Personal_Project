#pragma once
#include "Network_Tools.h"
#include "SFML_ENGINE/RessourcesManager.h"

class Console
{
public:
	struct Message
	{
		enum MESSAGE_TYPE
		{
			MTNULL = -1,
			INFO,
			WARN,
			COMMAND,
			ERROR
		};

		Message(std::string _message, MESSAGE_TYPE _type) :
			m_message(_message), m_message_type(_type)
		{}

		std::string m_message;

		MESSAGE_TYPE m_message_type;
	};

	Console();
	~Console();

	void add_message(std::string _message, Message::MESSAGE_TYPE _message_type) { m_console_messages.push_front(Message(_message, _message_type)); }

	inline std::list<Message>& get_console_messages() { return m_console_messages; }

	void update();

	void draw(sf::RenderWindow& _window, sf::View& _base_view);
private:
	std::list<Message> m_console_messages;

	sf::Text m_messages_text_handle;
	sf::Text m_message_type_handle;

	std::string m_space_console;

	sf::View m_message_view;
};