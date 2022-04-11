#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include "GameWindow.h"

#include "OrdinaryBullet.h"
#include "SplittingBullet.h"
#include "SplittedBullet.h"
#include "RayBullet.h"
#include "RocketBullet.h"
using namespace sf;
using namespace std;

enum BULLETTYPE { BULT_ORDINARY, BULT_SPLITTING, BULT_SPLITTED, BULT_RAY, BULT_ROCKET };

struct Ammunition {
	vector <OrdinaryBullet> ordinaryBullets;
	vector <SplittingBullet> splittingBullets;
	vector <SplittedBullet> splittedBullets;
	vector <RayBullet> rayBullets;
	vector <RocketBullet> rocketBullets;
};
struct AmmoData {
	OrdinaryBulletData ordinaryBulletData;
	SplittingBulletData splittingBulletData;
	RayBulletData rayBulletData;
	RocketBulletData rocketBulletData;
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

	Vector2f speed;

	BULLETTYPE selectedBullet;

	int HPAmount;

	Texture playertexture3HP;
	Texture playertexture2HP;
	Texture playertexture1HP;

	Texture ordinaryBulletScopeTexture;
	Texture splittingBulletScopeTexture;
	Texture rayBulletScopeTexture;
	Texture rayBulletAdditionalScopeTexture;
	Texture rocketBulletScopeTexture;

	Clock deltaTime;	//sets the dependency of player gameplay on time, not on fps


	void updatePlayerTexture();
	BULLETTYPE getSelectedBulletType();
	void move(Vector2f offset);
public:
	RectangleShape playerShape;
	RectangleShape scope;
	AdditionalScope additionalScope;
	Ammunition ammo;
	AmmoData ammoData;
	Clock excessFireClock;



	void initPlayer(GameWindow* gwindow);
	void initAmmunition();

	float getDeltaTime();

	void takeDamage();
	void rotateTo(Vector2f coords);
	void controlPlayer(GameWindow* gwindow);
	void updateAdditionalScopePart();
	void fire();

	void splitBullet(SplittingBullet* splittingBullet);		//It shouldn't be here, put it to splittingBullet/splittedBullet
	void splitBullet(SplittedBullet* splittedBullet);		////////////////////////////////////////////////////////////////
	void deleteBullet(BULLETTYPE bulletType, int index);	//maaaaaan what are doing here
	void checkForBulletSwap();

	bool isAlive();

	void updateOrdinaryBullets(GameWindow *gwindow);
	void updateSplittingBullets(GameWindow *gwindow);
	void updateSplittedBullets(GameWindow *gwindow);
	void updateRayBullets();
	void updateGrenadeBullets();

	void updateFrame(GameWindow *gwindow);
};