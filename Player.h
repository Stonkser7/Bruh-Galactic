#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include "GameWindow.h"
using namespace sf;
using namespace std;

enum BULLETTYPE { BULT_ORDINARY, BULT_SPLITTING, BULT_SPLITTED, BULT_RAY };
enum RAYBULLETSTATE { BS_FIRING, BS_DISAPPEARING, BS_DELETE };


class OrdinaryBullet {
public:
	RectangleShape shape;
	Vector2f speed;
	void move();
	bool isOutOfScreen(GameWindow* gwindow);
};
class SplittingBullet {
public:
	CircleShape shape;
	Vector2f speed;
	int damage;
	void move();
	bool isOutOfScreen(GameWindow* gwindow);
};
class SplittedBullet {
public:
	CircleShape shape;
	Vector2f speed;
	int damage;
	int levelOfSplit;
	void move();
	bool isOutOfScreen(GameWindow* gwindow);
};
class RayBullet {
public:
	RAYBULLETSTATE state;
	RectangleShape shape;
	Clock delayBeforeDissapear;
	void fire();
	void dissapear();
};

struct OrdinaryBulletData {
	int damage;
	Vector2f defaultSpeed;
	float speedVariation;
	Clock fireDelay;
	int fireDelayAsMilliseconds;
	int excessFireTime;
	Texture texture;
};
struct SplittingBulletData {
	int defaultDamage;
	Vector2f defaultSpeed;
	int defaultRadius;
	float speedVariation;
	Clock fireDelay;
	int fireDelayAsMilliseconds;
	int excessFireTime;
	Texture texture;
};
struct RayBulletData {
	int damage;
	Clock fireDelay;
	int fireDelayAsMilliseconds;
	int excessFireTime;
	Texture texture;
	int delayBeforeDissapearAsMilliseconds;
};

struct Ammunition {
	vector <OrdinaryBullet> ordinaryBullets;
	vector <SplittingBullet> splittingBullets;
	vector <SplittedBullet> splittedBullets;
	vector <RayBullet> rayBullets;
};
struct AmmoData {
	OrdinaryBulletData ordinaryBulletData;
	SplittingBulletData splittingBulletData;
	RayBulletData rayBulletData;
};


struct AdditionalScope {
	RectangleShape shape;
	bool isActive;
	void toggleActive();
};

class Player {
private:
	float sizeX;
	float sizeY;

	BULLETTYPE selectedBullet;

	int HPAmount;

	Texture playertexture3HP;
	Texture playertexture2HP;
	Texture playertexture1HP;
	Texture ordinaryBulletScopeTexture;
	Texture splittingBulletScopeTexture;
	Texture rayBulletScopeTexture;
	Texture rayBulletAdditionalScopeTexture;

public:
	RectangleShape playerShape;
	RectangleShape scope;
	AdditionalScope additionalScope;
	Ammunition ammo;
	AmmoData ammoData;
	Clock excessFireClock;

	void initPlayer(GameWindow* gwindow);
	void initAmmunition();
	void setHPAmount(int requiredAmount = 3);
	int getHPAmount();
	BULLETTYPE getSelectedBulletType();
	void rotateToMouse(Vector2f coords);
	void controlPlayer(GameWindow* gwindow);
	void updateAdditionalScopePart();
	void move(Vector2f offset);
	void rotateGun();
	void fire();
	void splitBullet(SplittingBullet* splittingBullet);
	void splitBullet(SplittedBullet* splittedBullet);
	void deleteBullet(BULLETTYPE bulletType, int index);
	void checkForBulletSwap();
};