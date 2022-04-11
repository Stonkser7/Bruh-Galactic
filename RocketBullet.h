#pragma once
#include <SFML/Graphics.hpp>

using namespace sf;

enum ROCKETBULLETSTATE { ROCKET_MOVE, ROCKET_EXPLOSING, ROCKET_EXPLOSIONANIM, ROCKET_DELETE };

struct RocketBulletData {
	int firstLevelDamage;
	int secondLevelDamage;
	int thirdLevelDamage;

	Clock fireTimer;
	int fireDelayAsMilliseconds;

	int excessFireDelay;

	Vector2f size;
	int firstAreaRaduis;
	int secondAreaRaduis;
	int thirdAreaRaduis;

	Texture texture;
	Texture firstAreaTexture;
	Texture secondAreaTexture;
	Texture thirdAreaTexture;
};


class RocketBullet {
private:
	Clock deltaTime;	//sets the dependency of player gameplay on time, not on fps
public:
	RectangleShape shape;

	CircleShape firstDamageArea;
	CircleShape secondDamageArea;
	CircleShape thirdDamageArea;

	CircleShape explosionWave;

	ROCKETBULLETSTATE state;

	Vector2f speed;
	Vector2i destinationCoords;

	void move();
	bool isDestinationReached(float deltaT);
	void explosionAnimation();
	float getDeltaTime();
};