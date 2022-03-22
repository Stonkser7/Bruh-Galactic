#pragma once
#include <SFML/Graphics.hpp>

using namespace sf;

enum GRENADEBULLETSTATE { GBS_FLYING, GBS_EXPLOSING, GBS_EXPLOSIONANIM, GBS_DELETE };

struct GrenadeBulletData {
	int firstLevelDamage;
	int secondLevelDamage;
	int thirdLevelDamage;

	Clock fireTimer;
	int fireDelayAsMilliseconds;

	int excessFireDelay;

	int defaultRadius;
	int firstAreaRaduis;
	int secondAreaRaduis;
	int thirdAreaRaduis;

	Texture texture;
	Texture firstAreaTexture;
	Texture secondAreaTexture;
	Texture thirdAreaTexture;
};


class GrenadeBullet {
public:
	CircleShape shape;

	CircleShape firstDamageArea;
	CircleShape secondDamageArea;
	CircleShape thirdDamageArea;

	CircleShape explosionWave;

	GRENADEBULLETSTATE state;

	Vector2f speed;
	Vector2i destinationCoords;

	void move();
	void explosionAnimation();
};