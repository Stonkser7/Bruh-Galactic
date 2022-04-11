#pragma once
#include "SFML/Graphics.hpp"
#include "GameWindow.h"

using namespace sf;
using namespace std;

enum ENEMYSTATE { ES_MOVING, ES_SPAWN_ANIM, ES_STANDING };

enum ROMAENEMYSIDE { ROMAS_LEFT, ROMAS_RIGHT };

enum ROCKENEMYSIDE { ROCKS_UPLEFT, ROCKS_UPRIGHT, ROCKS_DOWNLEFT, ROCKS_DOWNRIGHT };

enum HEALERENEMYSIDE { HEALERS_UP, HEALERS_DOWN };

//////////////
//ENEMIES DATA
//////////////
struct RomaEnemiesData {
	bool areActive;
	unsigned int maxAmount;
	Texture enemyTexture;
	Texture bulletTexture;
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
struct RomaEnemyBullet {
private:
	Clock deltaTime;	//sets the dependency of player gameplay on time, not on fps
public:
	RectangleShape shape;
	Vector2f speed;
	void move();
	bool isOutOfScreen(GameWindow *gwindow);
	float getDeltaTime();
};
struct RockEnemyBullet {
private:
	Clock deltaTime;	//sets the dependency of player gameplay on time, not on fps
public:
	CircleShape shape;
	Vector2f speed;
	void move();
	bool isOutOfScreen(GameWindow *gwindow);
	float getDeltaTime();
};
struct ElectroEnemyLightning {
	RectangleShape shape;
	Clock visible_lightningClock;
	Clock excessVisible_lightningClock;
	int excessVisible_lightningTime;
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
		Clock deltaTime;	//sets the dependency of player gameplay on time, not on fps
	public:
		CircleEnemy();
		CircleShape shape;
		Clock fireClock;
		int fireDelayAsMilliseconds;
		Clock excessFireClock;
		int excessFireTime;

		void setState(string state);
		ENEMYSTATE getState();
		void takeDamage(int damage);
		void heal(float heal);
		bool isAlive();
		float getDeltaTime();
	};
	class RomaEnemy : public CircleEnemy {
	public:
		ROMAENEMYSIDE side;
		int spawn_coord_x;
		Vector2f speed;
		int destination_coord_y;
		Texture* bulletTxtrPtr;
		int bullet_acceleration;

		void generateDestinationY(GameWindow* gwindow);
		void move(GameWindow* gwindow);
		bool isNeedToFire();
		RomaEnemyBullet fire();
		void spawnAnimation();
	};
	class RockEnemy : public CircleEnemy {
	public:
		ROCKENEMYSIDE spawn_side;
		Vector2f speed;
		Texture* bulletTxtrPtr;
		int bullet_acceleration;

		bool isNeedToFire();
		RockEnemyBullet fire();
		void takeTarget(Vector2f coords);
		void spawnAnimation(GameWindow* gwindow);
		void move();
		void checkForRebound(GameWindow *gwindow);
	};
	class ElectroEnemy : public CircleEnemy {
	public:
		ElectroEnemy();
		Texture* lightningTxtrPtr;
		float destinationCoordX;
		Clock visibleClock;
		bool visible;
		Clock excessVisibleClock;
		int excessVisibleTime;

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
		bool is_heal_enabled;
		HEALERENEMYSIDE side;
		Texture* rayTxtrPtr;
		int spawn_coord_y;
		Vector2i destinationCoords;
		Vector2f speed;

		void spawnAnimation();
		void move();
		void generateDestination(GameWindow* gwindow);
		bool isCanHeal();
	};
}