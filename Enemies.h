#pragma once
#include "SFML/Graphics.hpp"
#include "GameWindow.h"

using namespace sf;
using namespace std;

enum ENEMYSTATE { ES_MOVING, ES_SPAWN_ANIM, ES_STANDING };
enum ENEMYSIDE { S_UP, S_DOWN };

//////////////
//ENEMIES DATA
//////////////
struct RomaEnemiesData {
	bool areActive;
	unsigned int maxAmount;
	Texture enemyTexture;
	Texture bulletTexture;
	Vector2f bulletSpeed;
	int spawnRadius;
};
struct RockEnemiesData {
	bool areActive;
	unsigned int maxAmount;
	Texture enemyTexture;
	Texture bulletTexture;
	int spawnRadius;
};
struct ElectroEnemiesData {
	bool areActive;
	unsigned int maxAmount;
	Texture enemyTexture;
	Texture lightningTexture;
	int spawnRadius;
	int visibleDelayAsMilliseconds;
	int visible_lightningDelayAsMilliseconds;
};
struct HealerEnemiesData {
	bool areActive;
	unsigned int maxAmount;
	int spawnRadius;
	int healAreaRadius;
	Texture enemyTexture;
	Texture healAreaTexture;
	Texture rayTexture;
	float heal;
};
///////////////
//ENEMY BULLETS
///////////////
struct RockEnemyBullet {
	CircleShape shape;
	Vector2f speed;
};
struct ElectroEnemyLightning {
	RectangleShape shape;
	Clock visible_lightningClock;
};
struct HealerEnemyRay {
	RectangleShape shape;
	void takeTarget(Vector2f coords);
};



///////////////
//ENEMY CLASSES
///////////////
namespace Enemy {
	class CircleEnemy {
	private:
		ENEMYSTATE state;
	public:
		CircleEnemy();
		CircleShape shape;
		Clock fireClock;
		int fireDelayAsMilliseconds;

		void setState(string state);

		ENEMYSTATE getState();

		void takeDamage(int damage);

		void heal(float heal);

		bool isAlive();

	};
	class RomaEnemy : public CircleEnemy {
	public:
		float spawnCoordX;
		int destinationCoordY;
		Texture* bulletTxtrPtr;

		void generateDestinationY(GameWindow* gwindow);

		void move(GameWindow* gwindow);

		bool isNeedToFire();

		RectangleShape fire();

		void spawnAnimation();
	};
	class RockEnemy : public CircleEnemy {
	public:
		ENEMYSIDE side;
		float destinationCoordY;					//defining when spawnRockEnemy() invoking
		Texture* bulletTxtrPtr;
		Vector2f defaultBulletSpeed;
		float bulletSpeedVariation;

		bool isNeedToFire();

		void rotateGun();

		RockEnemyBullet fire();

		void takeTarget(Vector2f coords);

		void move();
	};
	class ElectroEnemy : public CircleEnemy {
	public:
		ElectroEnemy();
		Texture* lightningTxtrPtr;
		float destinationCoordX;
		Clock visibleClock;
		bool visible;

		void toggleVisible();

		bool isNeedToFire();

		void moveRandomCoordY(GameWindow* gwindow);

		ElectroEnemyLightning fire(Vector2f coords);

		void takeTarget(Vector2f coords);

		void move();
	};
	class HealerEnemy : public CircleEnemy {
	public:
		CircleShape healArea;
		bool isHealAreaActive;
		ENEMYSIDE side;
		Texture* rayTxtrPtr;
		int spawnCoordY;
		Vector2i destinationCoords;
		Vector2f speed;

		void spawnAnimation();

		void move();

		void generateDestination(GameWindow* gwindow);

		bool isCanHeal();
	};
}