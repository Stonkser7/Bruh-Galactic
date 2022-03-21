#pragma once
#include <SFML/Graphics.hpp>
#include "GameWindow.h"

using namespace sf;

struct SplittingBulletData {
	int defaultDamage;

	int defaultRadius;

	Vector2f defaultSpeed;
	float speedVariation;

	Clock fireTimer;
	int fireDelayAsMilliseconds;

	int excessFireDelay;

	Texture texture;
};

class SplittingBullet {
public:
	CircleShape shape;
	Vector2f speed;
	int damage;
	void move();
	bool isOutOfScreen(GameWindow* gwindow);
};