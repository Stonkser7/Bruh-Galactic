#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include "GameWindow.h"
#include "OrdinaryBullet.h"
#include "SplittingBullet.h"
#include "SplittedBullet.h"
#include "RayBullet.h"
#include "GrenadeBullet.h"
using namespace sf;
using namespace std;

enum BULLETTYPE { BULT_ORDINARY, BULT_SPLITTING, BULT_SPLITTED, BULT_RAY, BULT_GRENADE };

struct Ammunition {
	vector <OrdinaryBullet> ordinaryBullets;
	vector <SplittingBullet> splittingBullets;
	vector <SplittedBullet> splittedBullets;
	vector <RayBullet> rayBullets;
	vector <GrenadeBullet> grenadeBullets;
};
struct AmmoData {
	OrdinaryBulletData ordinaryBulletData;
	SplittingBulletData splittingBulletData;
	RayBulletData rayBulletData;
	GrenadeBulletData grenadeBulletData;
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
	Texture grenadeBulletScopeTexture;

	Clock deltaTime;	//sets the dependency of player gameplay on time, not on fps

public:
	RectangleShape playerShape;
	RectangleShape scope;
	AdditionalScope additionalScope;
	Ammunition ammo;
	AmmoData ammoData;
	Clock excessFireClock;

	float getDeltaTime();

	void initPlayer(GameWindow* gwindow);
	void initAmmunition();
	void setHPAmount(int requiredAmount = 3);
	int getHPAmount();
	BULLETTYPE getSelectedBulletType();
	void rotateToMouse(Vector2f coords);
	void controlPlayer(GameWindow* gwindow);
	void updateAdditionalScopePart();
	void move(Vector2f offset);
	void fire();
	void splitBullet(SplittingBullet* splittingBullet);
	void splitBullet(SplittedBullet* splittedBullet);
	void deleteBullet(BULLETTYPE bulletType, int index);
	void checkForBulletSwap();
};