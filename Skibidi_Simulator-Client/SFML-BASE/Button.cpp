#include "Button.h"

#include "Button.h"

Button::Button() : 
	m_button_state(BSNULL), m_locked(false)
{
}

Button::Button(sf::Vector2f _position, sf::Vector2f _size, const sf::Font& _font, std::string _english_text, std::string _french_text, LANGUAGE _language, int _font_size, bool _locked)
{
	m_button_state = BUTTON_IDLE;
	m_locked = _locked;

	this->m_button_shape.setSize(_size);
	this->m_button_shape.setPosition(_position);
	this->m_button_shape.setFillColor(sf::Color(255, 255, 255));

	this->m_button_text.setFont(_font);
	_language == LANGUAGE::ENGLISH ? this->m_button_text.setString(_english_text) : this->m_button_text.setString(_french_text);
	this->m_button_text.setCharacterSize(_font_size);

	this->m_button_text.setPosition
	(
		this->m_button_shape.getPosition().x + (this->m_button_shape.getGlobalBounds().width / 2.f) - this->m_button_text.getGlobalBounds().width / 2.f - this->m_button_text.getLetterSpacing(),
		this->m_button_shape.getPosition().y + (this->m_button_shape.getGlobalBounds().height / 2.f) - this->m_button_text.getGlobalBounds().height / 1.25f - this->m_button_text.getLineSpacing()
	);

	this->m_button_text.setFillColor(button_idle_color);
}

Button::~Button()
{

}

const bool Button::isPressed() const
{
	if (this != nullptr)
	{
		if (this->m_button_state == BUTTON_PRESSED)
			return true;
		return false;
	}

	return false;
}

void Button::setPosition(sf::Vector2f _position)
{
	this->m_button_shape.setPosition(_position);
	this->m_button_text.setPosition
	(
		this->m_button_shape.getPosition().x + (this->m_button_shape.getGlobalBounds().width / 2.f) - this->m_button_text.getGlobalBounds().width / 2.f - this->m_button_text.getLetterSpacing(),
		this->m_button_shape.getPosition().y + (this->m_button_shape.getGlobalBounds().height / 2.f) - this->m_button_text.getGlobalBounds().height / 1.25f - this->m_button_text.getLineSpacing()
	);
}

void Button::setSize(sf::Vector2f size)
{
	this->m_button_shape.setSize(size);
}

void Button::setText(std::string string)
{
	this->m_button_text.setString(string);
	this->m_button_text.setPosition
	(
		this->m_button_shape.getPosition().x + (this->m_button_shape.getGlobalBounds().width / 2.f) - this->m_button_text.getGlobalBounds().width / 2.f - this->m_button_text.getLetterSpacing(),
		this->m_button_shape.getPosition().y + (this->m_button_shape.getGlobalBounds().height / 2.f) - this->m_button_text.getGlobalBounds().height / 1.5f - this->m_button_text.getLineSpacing()
	);
}
void Button::setTextSize(int data)
{
	this->m_button_text.setCharacterSize(data);
}
void Button::update(const sf::Vector2f _mouse_position)
{
	/* Update the state for hover and pressed */
	if (this != nullptr)
	{
		if (!m_locked)
		{
			// Pressed
			if ((this->m_button_shape.getGlobalBounds().contains(_mouse_position) && MOUSE(Left)))
			{
				if (this->m_button_state != BUTTON_PRESSED)
				{
					//GET_MANAGER->playSound("menu_selection");
				}
				this->m_button_state = BUTTON_PRESSED;
				this->m_button_text.setFillColor(button_active_color);
			}
			else if (this->m_button_shape.getGlobalBounds().contains(_mouse_position))
			{
				if (this->m_button_state != BUTTON_HOVER)
				{
					//GET_MANAGER->playSound("menu_navigation");
				}
				this->m_button_state = BUTTON_HOVER;
				this->m_button_text.setFillColor(button_hover_color);
			}
			else
			{
				this->m_button_state = BUTTON_IDLE;
				this->m_button_text.setFillColor(button_idle_color);
			}
		}
	}
}

void Button::render(WindowManager& _window)
{
	if (this != nullptr)
	{
		_window.draw(this->m_button_shape);
		_window.draw(this->m_button_text);
	}
}