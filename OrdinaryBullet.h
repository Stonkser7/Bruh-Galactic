#pragma once
#include <SFML/Graphics.hpp>
#include "GameWindow.h"

using namespace sf;

struct OrdinaryBulletData {
	int damage;

	Clock fireTimer;
	int fireDelayAsMilliseconds;

	int excessFireDelay;

	Texture texture;
};

class OrdinaryBullet {
public:
	RectangleShape shape;
	Vector2f speed;
	float acceleration;
	void move();
	bool isOutOfScreen(GameWindow* gwindow);
};