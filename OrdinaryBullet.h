#pragma once
#include <SFML/Graphics.hpp>
#include "GameWindow.h"

using namespace sf;

struct OrdinaryBulletData {
	int damage;

	Vector2f defaultSpeed;
	float speedVariation;

	Clock fireTimer;
	int fireDelayAsMilliseconds;

	int excessFireDelay;

	Texture texture;
};

class OrdinaryBullet {
public:
	RectangleShape shape;
	Vector2f speed;
	void move();
	bool isOutOfScreen(GameWindow* gwindow);
};