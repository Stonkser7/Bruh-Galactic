#pragma once
#include <SFML/Graphics.hpp>
#include "GameWindow.h"

using namespace sf;

struct SplittingBulletData {
	int defaultDamage;

	int defaultRadius;

	Clock fireTimer;
	int fireDelayAsMilliseconds;

	int excessFireDelay;

	Texture texture;
};

class SplittingBullet {
private:
	Clock deltaTime;	//sets the dependency of player gameplay on time, not on fps
public:
	CircleShape shape;
	Vector2f speed;
	float acceleration;
	float damage;
	void move();
	bool isOutOfScreen(GameWindow* gwindow);
	float getDeltaTime();
};