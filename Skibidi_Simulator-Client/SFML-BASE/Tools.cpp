#include "Tools.h"

sf::Clock clocks;
sf::Time times;
State state;


void initTools(sf::RenderWindow& _window)
{
	times = clocks.getElapsedTime();
	srand((unsigned)time(NULL));

	_window.setFramerateLimit(60);
}

float GetDeltaTime()
{
	return times.asSeconds();
}

void restartClock()
{
    times = clocks.restart();
}

int rand_int(int _min, int _max)
{
	return rand() % (_max - _min) + _min;
}

double rand_float(float _min, float _max)
{
	return ((rand() / (double)RAND_MAX) * ((double)_max - (double)_min) + (double)_min);
}

sf::CircleShape CreateCircle(sf::Texture& _texture, sf::Vector2f _pos, float _radius, sf::Vector2f _origin, float _rotation, sf::Color _color)
{
	sf::CircleShape circle;
	circle.setOrigin(_origin);
	circle.setPosition(_pos);
	circle.setRadius(_radius);
	circle.setRotation(_rotation);
	circle.setTexture(&_texture);
	circle.setFillColor(_color);
	return circle;
}

sf::CircleShape CreateCircle(sf::Vector2f _pos, float _radius, sf::Vector2f _origin, float _rotation, sf::Color _color)
{
	sf::CircleShape circle;
	circle.setOrigin(_origin);
	circle.setPosition(_pos);
	circle.setRadius(_radius);
	circle.setRotation(_rotation);
	circle.setFillColor(_color);
	return circle;
}

sf::RectangleShape CreateRectangle(sf::Texture& _texture, sf::Vector2f _pos, sf::Vector2f _size, sf::Vector2f _origin, float _rotation, sf::Color _color)
{
	sf::RectangleShape rectangle;
	rectangle.setOrigin(_origin);
	rectangle.setPosition(_pos);
	rectangle.setRotation(_rotation);
	rectangle.setSize(_size);
	rectangle.setTexture(&_texture);
	rectangle.setFillColor(_color);
	return rectangle;
}

sf::RectangleShape CreateRectangle(sf::Vector2f _pos, sf::Vector2f _size, sf::Vector2f _origin, float _rotation, sf::Color _color)
{
	sf::RectangleShape rectangle;
	rectangle.setOrigin(_origin);
	rectangle.setPosition(_pos);
	rectangle.setSize(_size);
	rectangle.setRotation(_rotation);
	rectangle.setFillColor(_color);
	return rectangle;
}

//POINT/CIRCLE
bool pointCircle(sf::Vector2f _mpos, sf::Vector2f _cpos, float _r) {

	// get distance between the point and circle's center
	// using the Pythagorean Theorem
	float distX = _mpos.x - _cpos.x;
	float distY = _mpos.y - _cpos.y;
	float distance = sqrt((distX * distX) + (distY * distY));

	// if the distance is less than the circle's
	// radius the point is inside!
	if (distance <= _r) {
		return true;
	}
	return false;
}

// CIRCLE/CIRCLE
bool circleCircle(sf::Vector2f _c1pos, float _c1r, sf::Vector2f _c2pos, float _c2r) {

	// get distance between the circle's centers
	// use the Pythagorean Theorem to compute the distance
	float distX = _c1pos.x - _c2pos.x;
	float distY = _c1pos.y - _c2pos.y;
	float distance = sqrt((distX * distX) + (distY * distY));

	// if the distance is less than the sum of the circle's
	// radii, the circles are touching!
	if (distance <= _c1r + _c2r) {
		return true;
	}
	return false;
}

// POINT/RECTANGLE
bool pointRect(sf::Vector2f _mpos, sf::FloatRect _rect) {

	// is the point inside the rectangle's bounds?
	if (_mpos.x >= _rect.left &&        // right of the left edge AND
		_mpos.x <= _rect.left + _rect.width &&   // left of the right edge AND
		_mpos.y >= _rect.top &&        // below the top AND
		_mpos.y <= _rect.top + _rect.height) {   // above the bottom
		return true;
	}
	return false;
}

// RECTANGLE/RECTANGLE
bool rectRect(sf::FloatRect _rect1, sf::FloatRect _rect2) {

	// are the sides of one rectangle touching the other?

	if (_rect1.left + _rect1.width >= _rect2.left &&    // r1 right edge past r2 left
		_rect1.left <= _rect2.left + _rect2.width &&    // r1 left edge past r2 right
		_rect1.top + _rect1.height >= _rect2.top &&    // r1 top edge past r2 bottom
		_rect1.top <= _rect2.top + _rect2.height) {    // r1 bottom edge past r2 top
		return true;
	}
	return false;
}

// CIRCLE/RECTANGLE
bool circleRect(sf::Vector2f cpos, float radius, sf::FloatRect rect) {

	// temporary variables to set edges for testing
	float testX = cpos.x;
	float testY = cpos.y;

	// which edge is closest?
	if (cpos.x < rect.left)         testX = rect.left;      // test left edge
	else if (cpos.x > rect.left + rect.width) testX = rect.left + rect.width;   // right edge
	if (cpos.y < rect.top)         testY = rect.top;      // top edge
	else if (cpos.y > rect.top + rect.height) testY = rect.top + rect.height;   // bottom edge

	// get distance from closest edges
	float distX = cpos.x - testX;
	float distY = cpos.y - testY;
	float distance = sqrt((distX * distX) + (distY * distY));

	// Si la distance est inférieure ou égale au rayon, il y a collision
	if (distance <= radius) {
		return true;
	}
	return false;
}

// CIRCLE/RECTANGLE DIRECTION
bool circleRect(sf::Vector2f cpos, float radius, sf::FloatRect rect, Direction _direction) {

	// temporary variables to set edges for testing
	float testX = cpos.x;
	float testY = cpos.y;

	// Ajout de la condition de direction
	if (_direction == UP) {
		testY -= radius;
	}
	else if (_direction == DOWN) {
		testY += radius;
	}
	else if (_direction == LEFT) {
		testX -= radius;
	}
	else if (_direction == RIGHT) {
		testX += radius;
	}

	// which edge is closest?
	if (cpos.x < rect.left)         testX = rect.left;      // test left edge
	else if (cpos.x > rect.left + rect.width) testX = rect.left + rect.width;   // right edge
	if (cpos.y < rect.top)         testY = rect.top;      // top edge
	else if (cpos.y > rect.top + rect.height) testY = rect.top + rect.height;   // bottom edge

	// get distance from closest edges
	float distX = cpos.x - testX;
	float distY = cpos.y - testY;
	float distance = sqrt((distX * distX) + (distY * distY));

	// if the distance is less than the radius, collision!
	if (distance <= radius) {
		return true;
	}
	return false;
}

State GetState()
{
    return state;
}

void changeState(State _state)
{
    switch (_state)
    {
    case MENU:
        std::cout << "Affichage du menu principal..." << std::endl;
        break;
    case OPTIONS:
        std::cout << "Affichage des options..." << std::endl;
        break;
    case GAME:
        std::cout << "Lancement du jeu..." << std::endl;
        break;
    default:
        std::cout << "�tat non valide." << std::endl;
        break;
    }
    state = _state;
}

float GetDistance(sf::Vector2f a_, sf::Vector2f b_)
{
	return sqrt((b_.x - a_.x) * (b_.x - a_.x) + (b_.y - a_.y) * (b_.y - a_.y));
}

sf::Vector2f SubstractVector2f(sf::Vector2f a, sf::Vector2f b)
{
	sf::Vector2f substractVector2 = { a.x - b.x, a.y - b.y };
	return substractVector2;
}

sf::Vector2f DivideVector2f(sf::Vector2f a, float b)
{
	sf::Vector2f divideVector2 = { a.x / b, a.y / b };
	return divideVector2;
}

sf::Vector2f Normalize(sf::Vector2f v)
{
	return DivideVector2f(v, GetNorme(v));
}

float GetNorme(sf::Vector2f a)
{
	return sqrt(a.x * a.x + a.y * a.y);
}

