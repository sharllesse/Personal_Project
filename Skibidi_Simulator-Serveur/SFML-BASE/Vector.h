#pragma once
#include "SFML/Graphics.hpp"

template<class T>
class Vector2 : public sf::Vector2<T>
{
public:
	Vector2() : sf::Vector2<T>(static_cast<T>(0.), static_cast<T>(0.)) {}
	Vector2(T _x, T _y) : sf::Vector2<T>(_x, _y) {}
	~Vector2() {}

	/*template<typename T>
	Vector2<V> operator+(const Vector2<T> v)
	{
		return Vector2<V>(x + static_cast<V>(v.x), y + static_cast<V>(v.y));
	}

	template<typename T>
	Vector2<V> operator-(const Vector2<T> v)
	{
		return Vector2<V>(x - static_cast<V>(v.x), y - static_cast<V>(v.y));
	}

	Vector2<V> operator*(const float f)
	{
		return Vector2<V>(x * f, y * f);
	}

	Vector2<V> operator/(const float f)
	{
		return Vector2<V>(x / f, y / f);
	}

	template<typename T>
	bool operator==(const Vector2<T> v)
	{
		if (v.x == x && v.y == y)
			return true;

		return false;
	}

	template<typename T>
	bool operator!=(const Vector2<T> v)
	{
		if (v.x == x && v.y == y)
			return false;

		return true;
	}*/

	template<typename T>
	void lerp(Vector2<T> g, float t)
	{
		this->x = (1 - t) * this->x + t * g.x;
		this->y = (1 - t) * this->y + t * g.y;
	}

	template<typename T>
	inline float dotProduct(Vector2<T> v)
	{
		return this->x * v.x + this->y * v.y;
	}

	float getNorme()
	{
		if (this->x == static_cast<V>(0.) && this->y == static_cast<V>(0.))
			return static_cast<V>(0.);

		return std::sqrt((this->x * this->x) + (this->y * this->y));
	}

	template<typename T>
	inline float getDistance(Vector2<T> v)
	{
		return std::sqrt((v.x - this->x) * (v.x - this->x) + (v.y - this->y) * (v.y - this->y));
	}

	template<typename T>
	inline float getAngle(Vector2<T> v)
	{
		return acos(dotProduct(v) / (getNorme() * v.getNorme())) * (180.f / 3.14159265359f);
	}

	Vector2<V> normalize()
	{
		if (this->x == static_cast<V>(0.) && this->y == static_cast<V>(0.))
			return Vector2<V>;

		return *this / getNorme();
	}

	void normalize(bool _normalize_it_self)
	{
		if (this->x == static_cast<V>(0.) && this->y == static_cast<V>(0.))
			*this = Vector2<V>;

		*this = *this / getNorme();
	}

	sf::Vector2<T> truncate(sf::Vector2<T> _v, float _m, bool _truncate_it_self = false)
	{
		sf::Vector2<T> tmp_vector(*this + _v);

		if (tmp_vector != sf::Vector2<V>())
		{
			float i = _m / tmp_vector.getNorme();

			i = (i > 1.f) ? 1.f : i;

			if (_truncate_it_self)
				*this = tmp_vector * i;

			return tmp_vector * i;
		}

		return sf::Vector2f();
	}

	sf::Vector2<T> truncate(float _m, bool _truncate_it_self = false)
	{
		sf::Vector2<T> tmp_vector(*this);

		if (tmp_vector != sf::Vector2<V>())
		{
			float i = _m / tmp_vector.getNorme();

			i = (i > 1.f) ? 1.f : i;

			if (_truncate_it_self)
				*this = tmp_vector * i;

			return tmp_vector * i;
		}

		return sf::Vector2f();
	}
};