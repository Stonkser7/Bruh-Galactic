#pragma once
#include <SFML/Graphics.hpp>
#include "GameWindow.h"

using namespace sf;

class SplittedBullet {
private:
	Clock deltaTime;	//sets the dependency of player gameplay on time, not on fps
public:
	CircleShape shape;
	Vector2f speed;
	float acceleration;
	float damage;
	int levelOfSplit;
	void move();
	bool isOutOfScreen(GameWindow* gwindow);
	float getDeltaTime();
};