#pragma once
#include <SFML/Graphics.hpp>
using namespace sf;

struct GameWindow {
	RenderWindow window;
	unsigned short x;
	unsigned short y;
	String title;
};