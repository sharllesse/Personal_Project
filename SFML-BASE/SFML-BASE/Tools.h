#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <vector>
#include <iostream>
#include <Windows.h>
#include <map>
#include <ctime>
#include <cstdlib>
#include <string>

enum State
{
    MENU,
    GAME,
    EDITOR,
    OPTIONS
};

enum Direction
{
    LEFT,
    RIGHT,
    UP,
    DOWN
};

void initTools(sf::RenderWindow& _window);

float GetDeltaTime();

void restartClock();

int rand_int(int _min, int _max);

double rand_float(float _min, float _max);

sf::CircleShape CreateCircle(sf::Texture& _texture, sf::Vector2f _pos = sf::Vector2f(), float _radius = 0.f, sf::Vector2f _origin = sf::Vector2f(), float _rotation = 0.f, sf::Color _color = sf::Color());

sf::CircleShape CreateCircle(sf::Vector2f _pos = sf::Vector2f(), float _radius = 0.f, sf::Vector2f _origin = sf::Vector2f(), float _rotation = 0.f, sf::Color _color = sf::Color());

sf::RectangleShape CreateRectangle(sf::Texture& _texture, sf::Vector2f _pos = sf::Vector2f(), sf::Vector2f _size = sf::Vector2f(), sf::Vector2f _origin = sf::Vector2f(), float _rotation = 0.f , sf::Color _color = sf::Color(255, 255, 255));

sf::RectangleShape CreateRectangle(sf::Vector2f _pos = sf::Vector2f(), sf::Vector2f _size = sf::Vector2f(), sf::Vector2f _origin = sf::Vector2f(), float _rotation = 0.f, sf::Color _color = sf::Color(255, 255, 255));

//POINT/CIRCLE
bool pointCircle(sf::Vector2f _mpos, sf::Vector2f _cpos, float _r);

// CIRCLE/CIRCLE
bool circleCircle(sf::Vector2f _c1pos, float _c1r, sf::Vector2f _c2pos, float _c2r);

// POINT/RECTANGLE
bool pointRect(sf::Vector2f _mpos, sf::FloatRect _rect);

// RECTANGLE/RECTANGLE
bool rectRect(sf::FloatRect _rect1, sf::FloatRect _rect2);

// CIRCLE/RECTANGLE
bool circleRect(sf::Vector2f cpos, float radius, sf::FloatRect rect);

// CIRCLE/RECTANGLE DIRECTION
bool circleRect(sf::Vector2f cpos, float radius, sf::FloatRect rect, Direction _direction);

State GetState();

void changeState(State _state);

float GetDistance(sf::Vector2f a_, sf::Vector2f b_);

sf::Vector2f SubstractVector2f(sf::Vector2f a, sf::Vector2f b);

sf::Vector2f DivideVector2f(sf::Vector2f a, float b);

sf::Vector2f Normalize(sf::Vector2f v);

float GetNorme(sf::Vector2f a);