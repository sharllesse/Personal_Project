#pragma once
#include "SFML_ENGINE/Tools.h"
#include "SFML_ENGINE/WindowManager.h"
#include "SFML_ENGINE/RessourcesManager.h"

#define button_idle_color sf::Color(200,200,200,255)
#define button_hover_color sf::Color(238,199,17,255)
#define button_active_color sf::Color(128,128,128,255)

class Button
{
public:
	enum BUTTON_STATE
	{
		BSNULL = -1,
		BUTTON_IDLE,
		BUTTON_HOVER,
		BUTTON_PRESSED
	};

	enum LANGUAGE 
	{ 
		FRENCH,
		ENGLISH
	};

private:
	BUTTON_STATE m_button_state;
	sf::RectangleShape m_button_shape;
	sf::Text m_button_text;
	bool m_locked;

public:
	Button();
	Button(sf::Vector2f _position, sf::Vector2f _size, const sf::Font& _font, std::string _english_text, std::string _french_text, LANGUAGE _language, int _font_size = 25, bool _locked = false);
	~Button();

	////////////////////////////////////////////////////////////
	/// \brief Return if the button_states is BUTTON_PRESSED or false
	////////////////////////////////////////////////////////////
	const bool isPressed() const;

	////////////////////////////////////////////////////////////
	/// \brief update the position of the shape and the text
	/// \param pos is the new position of the shape
	////////////////////////////////////////////////////////////
	void setPosition(sf::Vector2f _position);

	////////////////////////////////////////////////////////////
	/// \brief update the size
	/// \param size is the new size of the shape
	////////////////////////////////////////////////////////////
	void setSize(sf::Vector2f size);

	////////////////////////////////////////////////////////////
	/// \brief Update the string in the text (and the position)
	/// \param string will be the new string
	////////////////////////////////////////////////////////////
	void setText(std::string string);

	////////////////////////////////////////////////////////////
	/// \brief Update the size of the text
	/// \param data will be the new size
	////////////////////////////////////////////////////////////
	void setTextSize(int data);

	////////////////////////////////////////////////////////////
	/// \brief Update the button
	/// \param mousePos will be used to check if the new is in the shape
	////////////////////////////////////////////////////////////
	void update(const sf::Vector2f mousePos);

	////////////////////////////////////////////////////////////
	/// \brief draw the shape and the text
	/// \param target will be the window to draw
	////////////////////////////////////////////////////////////
	void render(WindowManager& _window);

	BUTTON_STATE& getButtonState() { return m_button_state; }
};

typedef std::map<std::string, Button> Button_map;