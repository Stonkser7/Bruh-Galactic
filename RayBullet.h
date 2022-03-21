#pragma once
#include <SFML/Graphics.hpp>

using namespace sf;

enum RAYBULLETSTATE { BS_FIRING, BS_DISAPPEARING, BS_DELETE };

struct RayBulletData {
	int damage;

	Clock fireTimer;
	int fireDelayAsMilliseconds;

	int delayBeforeDissapearAsMilliseconds;

	int excessFireDelay;

	Texture texture;
};

class RayBullet {
public:
	RAYBULLETSTATE state;
	RectangleShape shape;
	Clock delayBeforeDissapear;
	void fire();
	void dissapear();
};