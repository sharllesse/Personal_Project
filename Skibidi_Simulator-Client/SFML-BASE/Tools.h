#pragma once

#include "SFML/Audio.hpp"
#include "SFML/Network.hpp"
#include "SFML/Graphics.hpp"

#include <string>
#include <fstream>
#include <iostream>
#include <sstream>

#include <vector>
#include <list>

#include <memory>
#include <assert.h>
#include <random>
#include <stack>
#include <stdlib.h>
#include <concepts>

namespace Tools
{
	float get_delta_time();

	void restart_clock();

	template <class _Type>
	_Type rand(_Type _min, _Type _max)
	{
		std::random_device rd;
		std::mt19937 mt(rd());
		std::uniform_real_distribution<double> dist(_min, _max + 1);

		return static_cast<_Type>(dist(mt));
	}

	// POINT/CIRCLE
	bool point_circle(sf::Vector2<float> _mpos, sf::Vector2<float> _cpos, float _r);

	// CIRCLE/CIRCLE
	bool circle_circle(sf::Vector2<float> _c1pos, float _c1r, sf::Vector2<float> _c2pos, float _c2r);

	// POINT/RECTANGLE
	bool point_rect(sf::Vector2<float> _mpos, sf::FloatRect _rect);

	// RECTANGLE/RECTANGLE
	bool rect_rect(sf::FloatRect _rect1, sf::FloatRect _rect2);

	// CIRCLE/RECTANGLE
	bool circle_rect(sf::Vector2<float> cpos, float radius, sf::FloatRect rect);
}