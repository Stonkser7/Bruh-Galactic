#pragma once
#include <SFML/Graphics.hpp>
#include "GameWindow.h"

using namespace sf;

class SplittedBullet {
public:
	CircleShape shape;
	Vector2f speed;
	int damage;
	int levelOfSplit;
	void move();
	bool isOutOfScreen(GameWindow* gwindow);
};