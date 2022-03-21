#pragma once
#include <SFML/Graphics.hpp>

using namespace sf;

enum GRENADEBULLETSTATE { BS_FLYING, BS_EXPLOSING };

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

	GRENADEBULLETSTATE state;

	Vector2f speed;
	Vector2i destinationCoords;

	void move();
};