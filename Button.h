#pragma once
#include <SFML/Graphics.hpp>

using namespace sf;

class Game;
using ButtonAction = void(Game::*)();

struct Button {
	Text title;
	ButtonAction action;
	RectangleShape buttonBackground;
};