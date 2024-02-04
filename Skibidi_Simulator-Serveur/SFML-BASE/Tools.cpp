#include "Tools.h"

sf::Clock m_clocks;
sf::Time m_times;

sf::Packet& operator>>(sf::Packet& _p, sf::Vector2f& _v)
{
	return _p >> _v.x >> _v.y;
}

sf::Packet& operator>>(sf::Packet& _p, sf::Color& _c)
{
	return _p >> _c.r >> _c.g >> _c.b >> _c.a;
}

sf::Packet& operator<<(sf::Packet& _p, sf::Vector2f& _v)
{
	return _p << _v.x << _v.y;
}

sf::Packet& operator<<(sf::Packet& _p, sf::Color& _c)
{
	return _p << _c.r << _c.g << _c.b << _c.a;
}

float Tools::get_delta_time()
{
	return m_times.asSeconds();
}

void Tools::restart_clock()
{
	m_times = m_clocks.restart();
}

bool Tools::point_circle(sf::Vector2<float> _mpos, sf::Vector2<float> _cpos, float _r)
{
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

bool Tools::circle_circle(sf::Vector2<float> _c1pos, float _c1r, sf::Vector2<float> _c2pos, float _c2r)
{
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

bool Tools::point_rect(sf::Vector2<float> _mpos, sf::FloatRect _rect)
{
	// is the point inside the rectangle's bounds?
	if (_mpos.x >= _rect.left &&        // right of the left edge AND
		_mpos.x <= _rect.left + _rect.width &&   // left of the right edge AND
		_mpos.y >= _rect.top &&        // below the top AND
		_mpos.y <= _rect.top + _rect.height) {   // above the bottom
		return true;
	}
	return false;
}

bool Tools::rect_rect(sf::FloatRect _rect1, sf::FloatRect _rect2)
{
	// are the sides of one rectangle touching the other?

	if (_rect1.left + _rect1.width >= _rect2.left &&    // r1 right edge past r2 left
		_rect1.left <= _rect2.left + _rect2.width &&    // r1 left edge past r2 right
		_rect1.top + _rect1.height >= _rect2.top &&    // r1 top edge past r2 bottom
		_rect1.top <= _rect2.top + _rect2.height) {    // r1 bottom edge past r2 top
		return true;
	}

	return false;
}

bool Tools::circle_rect(sf::Vector2<float> cpos, float radius, sf::FloatRect rect)
{
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

float Tools::get_distance(sf::Vector2f a_, sf::Vector2f b_)
{
	return sqrt((b_.x - a_.x) * (b_.x - a_.x) + (b_.y - a_.y) * (b_.y - a_.y));
}

float Tools::get_norme(sf::Vector2f a)
{
	if (a == sf::Vector2f())
		return 0.f;

	return sqrt(a.x * a.x + a.y * a.y);
}

sf::Vector2f Tools::truncate(sf::Vector2f _v, float _m)
{
	if (_v != sf::Vector2f())
	{
		float i = _m / get_norme(_v);

		i = (i > 1.f) ? 1.f : i;

		return _v * i;
	}

	return _v;
}

sf::Vector2f Tools::normalize(sf::Vector2f v)
{
	if (v == sf::Vector2f())
		return sf::Vector2f();

	return v / get_norme(v);
}