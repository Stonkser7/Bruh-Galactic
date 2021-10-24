﻿#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <string>
#include <cmath>
#include <iomanip>
using namespace sf;
using namespace std;

enum GAMESTATE { GS_PAUSE, GS_PLAY, GS_EXIT, GS_MENU };
enum ENEMYSTATE { ES_SPAWNED, ES_UNSPAWNED };
enum ENEMYTYPE { ET_ROMA };
enum BULLETSTATE { BULS_FIRED, BULS_INCARTRIDGE };
enum BULLETTYPE { BULT_ORDINARY, BULT_SPLITTING, BULT_SPLITTED, BULT_TWICESPLITTED };

struct GameWindow {
	RenderWindow window;
	int x;
	int y;
	String title;
};
struct RomaEnemiesData {
	bool areRomaEnemiesActive;
	Texture romaEnemyTexture;
	Texture romaBulletTexture;
	Vector2f romaBulletSpeed;
	int defaultRadius;
};
struct OrdinaryBulletData {
	int damage;
	Vector2f defaultSpeed;
	float speedVariation;
	Clock fireDelay;
	Texture texture;
};
struct SplittingBulletData {
	int defaultDamage;
	Vector2f defaultSpeed;
	int defaultRadius;
	float speedVariation;
	Clock fireDelay;
	Texture texture;
};
struct OrdinaryBullet {
	RectangleShape bulletShape;
	Vector2f speed;
	BULLETSTATE state;
};
struct SplittingBullet {
	CircleShape bulletShape;
	Vector2f speed;
	BULLETSTATE state;
	int damage;
};
struct SplittedBullet {
	CircleShape bulletShape;
	Vector2f speed;
	int damage;
};
struct Ammunition {
	OrdinaryBulletData ordinaryBulletData;
	vector <OrdinaryBullet> ordinaryBullets;
	SplittingBulletData splittingBulletData;
	vector <SplittingBullet> splittingBullets;
	vector <SplittedBullet> splittedBullets;
	vector <SplittedBullet> twiceSplittedBullets;
};
class Game;
typedef void (Game::* MenuAction)();
class Menu {
private:
	struct MenuItem {
		Text title;
		MenuAction action;
	};
	int selectedItem;
	bool isMenuTouched;
	Font menuFont;
	Clock delayBetweenMenuPresses;

	Texture menuBackgroundTexture;
	Texture menuItemBackgroundTexture;
public:
	Text caption;
	RectangleShape menuBackground;
	RectangleShape menuItemBackground;
	vector <MenuItem> menuItems;
	void initMenu(GameWindow *gwindow) {
		menuBackgroundTexture.loadFromFile("Textures\\menuBackgroundTexture.jpg");
		menuItemBackgroundTexture.loadFromFile("Textures\\menuItemBackgroundTexture.png");
		menuBackground.setTexture(&menuBackgroundTexture);
		menuBackground.setSize(Vector2f(gwindow->x, gwindow->y));
		menuBackground.setPosition(Vector2f(0, 0));
		menuItemBackground.setTexture(&menuItemBackgroundTexture);
		selectedItem = 0;
		menuFont.loadFromFile("Fonts\\Hacked Cyr.ttf");
		caption.setFont(menuFont);
		caption.setString("BRUH GALACTIC");
		caption.setFillColor(Color(25, 25, 112));
		caption.setPosition(Vector2f(gwindow->x / 2.6, gwindow->y / 2.5));
		caption.setCharacterSize(50);
		caption.getGlobalBounds().height;
		caption.getGlobalBounds().width;
		isMenuTouched = false;
	}
	void addMenuItem(string title, MenuAction action) {
		MenuItem item;
		item.title.setFont(menuFont);
		item.title.setString(title);
		item.title.setFillColor(Color(139, 0, 0));
		item.title.setCharacterSize(30);
		item.title.setPosition(Vector2f(caption.findCharacterPos(2 + menuItems.size()).x, caption.getPosition().y + caption.getGlobalBounds().height * 2 + menuItems.size() * item.title.getGlobalBounds().height * 2));
		item.action = action;
		menuItems.push_back(item);
	}
	void controlMenu() {
		if (Keyboard::isKeyPressed(Keyboard::Key::W) && delayBetweenMenuPresses.getElapsedTime().asMilliseconds() > 200) {
			/*if (!menu.isMenuTouched) {
				menu.isMenuTouched = !menu.isMenuTouched;
			}*/
			selectedItem--;
			if (selectedItem < 0) {
				selectedItem = menuItems.size() - 1;
			}
			delayBetweenMenuPresses.restart();
		}
		if (Keyboard::isKeyPressed(Keyboard::Key::S) && delayBetweenMenuPresses.getElapsedTime().asMilliseconds() > 200) {
			/*if (!menu.isMenuTouched) {
				menu.isMenuTouched = !menu.isMenuTouched;
			}*/
			selectedItem++;
			if (selectedItem > menuItems.size() - 1) {
				selectedItem = 0;
			}
			delayBetweenMenuPresses.restart();
		}
	}
	void selectMenuItem(Game *gameClass) {
		menuItemBackground.setPosition(Vector2f(menuItems[selectedItem].title.getGlobalBounds().left, menuItems[selectedItem].title.getGlobalBounds().top));
		menuItemBackground.setSize(Vector2f(menuItems[selectedItem].title.getGlobalBounds().width, menuItems[selectedItem].title.getGlobalBounds().height));
		if (Keyboard::isKeyPressed(Keyboard::Key::Enter) && delayBetweenMenuPresses.getElapsedTime().asMilliseconds() > 200) {
			(gameClass->*menuItems[selectedItem].action)();
			delayBetweenMenuPresses.restart();
		}
	}
};
class Player {
private:
	int sizeX;
	int sizeY;

	BULLETTYPE selectedBullet;

	int HPAmount;

	Texture playertexture3HP;
	Texture playertexture2HP;
	Texture playertexture1HP;
	Texture ordinaryBulletScopeTexture;
	Texture splittingBulletScopeTexture;
public:
	RectangleShape playerShape;
	RectangleShape scope;
	Ammunition ammo;

	void initPlayer(GameWindow *gwindow) {
		playertexture3HP.loadFromFile("Textures\\playerTexture3HP.jpg");
		playertexture2HP.loadFromFile("Textures\\playerTexture2HP.jpg");
		playertexture1HP.loadFromFile("Textures\\playerTexture1HP.jpg");
		ordinaryBulletScopeTexture.loadFromFile("Textures\\ordinaryBulletScopeTexture.png");
		splittingBulletScopeTexture.loadFromFile("Textures\\splittingBulletScopeTexture.png");
		sizeX = 75;
		sizeY = 40;
		playerShape.setOrigin(Vector2f(sizeX / 2, sizeY / 2));
		playerShape.setSize(Vector2f(sizeX, sizeY));
		playerShape.setRotation(90);
		playerShape.setPosition(Vector2f(60.f, gwindow->y / 2));
		setHP();
		scope.setSize(Vector2f(55, 15));
		scope.setOrigin(Vector2f(0, scope.getSize().y / 2));
		scope.setPosition(Vector2f(playerShape.getPosition().x, playerShape.getPosition().y));
		scope.setTextureRect(IntRect(0, 0, 55, 15));
		scope.setTexture(&ordinaryBulletScopeTexture);
		scope.setRotation(playerShape.getRotation() - 90);
	}
	void initAmmunition() {
		selectedBullet = BULT_ORDINARY;
		ammo.ordinaryBullets.resize(10);
		ammo.ordinaryBulletData.texture.loadFromFile("Textures\\pchel.jpg");
		ammo.ordinaryBulletData.damage = 10;
		ammo.ordinaryBulletData.defaultSpeed.x = 7; ammo.ordinaryBulletData.defaultSpeed.y = 0;
		ammo.ordinaryBulletData.speedVariation = ((ammo.ordinaryBulletData.defaultSpeed.x / 100) * 50) / 45;
		for (int i = 0; i < ammo.ordinaryBullets.size(); i++) {
			ammo.ordinaryBullets[i].bulletShape.setTexture(&ammo.ordinaryBulletData.texture);
			ammo.ordinaryBullets[i].bulletShape.setSize(Vector2f(30.f, 17.f));
			ammo.ordinaryBullets[i].state = BULS_INCARTRIDGE;
		}
		ammo.splittingBullets.resize(5);
		ammo.splittingBulletData.texture.loadFromFile("Textures\\splittingBulletTexture.png");
		ammo.splittingBulletData.defaultDamage = 26;
		ammo.splittingBulletData.defaultRadius = 26;
		ammo.splittingBulletData.defaultSpeed.x = 3; ammo.splittingBulletData.defaultSpeed.y = 0;
		ammo.splittingBulletData.speedVariation = ((ammo.splittingBulletData.defaultSpeed.x / 100) * 50) / 45;
		for(int i = 0; i < ammo.splittingBullets.size(); i++) {
			ammo.splittingBullets[i].damage = ammo.splittingBulletData.defaultDamage;
			ammo.splittingBullets[i].bulletShape.setTexture(&ammo.splittingBulletData.texture);
			ammo.splittingBullets[i].bulletShape.setRadius(ammo.splittingBulletData.defaultRadius);
			ammo.splittingBullets[i].bulletShape.setOrigin(Vector2f(ammo.splittingBulletData.defaultRadius, ammo.splittingBulletData.defaultRadius));
			ammo.splittingBullets[i].state = BULS_INCARTRIDGE;
		}
		ammo.splittedBullets.clear();
		ammo.twiceSplittedBullets.clear();
	}
	void setHP(int requiredAmount = 3) {
		HPAmount = requiredAmount;
		switch (HPAmount) {
		case 3:
			playerShape.setTexture(&playertexture3HP);
			break;
		case 2:
			playerShape.setTexture(&playertexture2HP);
			break;
		case 1:
			playerShape.setTexture(&playertexture1HP);
			break;
		}
	}
	int getHPAmount() {
		return HPAmount;
	}
	void controlPlayer(GameWindow *gwindow) {
		if (Mouse::isButtonPressed(Mouse::Button::Left)) {
			fire();
		}
		if (Keyboard::isKeyPressed(Keyboard::Key::W) && playerShape.getPosition().y - sizeX / 2 > 0) {
			move(0, -1.9);
		}
		if (Keyboard::isKeyPressed(Keyboard::Key::S) && playerShape.getPosition().y + sizeX / 2 < gwindow->y) {
			move(0, 1.9);
		}

		if (Keyboard::isKeyPressed(Keyboard::Key::A) && playerShape.getRotation() > 45) {
			rotateGun("left");
			rotatePlayer(-1);
		}
		if (Keyboard::isKeyPressed(Keyboard::Key::D) && playerShape.getRotation() < 135) {
			rotateGun("right");
			rotatePlayer(1);
		}
	}
	void move(float offsetX, float offsetY) {
		playerShape.move(offsetX, offsetY);
		scope.move(offsetX, offsetY);
	}
	void rotatePlayer(float angle) {
		playerShape.rotate(angle);
		scope.rotate(angle);
	}
	void rotateGun(string direction) {
		if (direction == "left") {
			if (playerShape.getRotation() <= 90) {
				ammo.ordinaryBulletData.defaultSpeed.x -= ammo.ordinaryBulletData.speedVariation;
				ammo.splittingBulletData.defaultSpeed.x -= ammo.splittingBulletData.speedVariation;
			}
			else {
				ammo.ordinaryBulletData.defaultSpeed.x += ammo.ordinaryBulletData.speedVariation;
				ammo.splittingBulletData.defaultSpeed.x += ammo.splittingBulletData.speedVariation;
			}
			ammo.ordinaryBulletData.defaultSpeed.y -= ammo.ordinaryBulletData.speedVariation;
			ammo.splittingBulletData.defaultSpeed.y -= ammo.splittingBulletData.speedVariation;
		}
		if (direction == "right") {
			if (playerShape.getRotation() >= 90) {
				ammo.ordinaryBulletData.defaultSpeed.x -= ammo.ordinaryBulletData.speedVariation;
				ammo.splittingBulletData.defaultSpeed.x -= ammo.splittingBulletData.speedVariation;
			}
			else {
				ammo.ordinaryBulletData.defaultSpeed.x += ammo.ordinaryBulletData.speedVariation;
				ammo.splittingBulletData.defaultSpeed.x += ammo.splittingBulletData.speedVariation;
			}
			ammo.ordinaryBulletData.defaultSpeed.y += ammo.ordinaryBulletData.speedVariation;
			ammo.splittingBulletData.defaultSpeed.y += ammo.splittingBulletData.speedVariation;
		}
	}
	void fire() {
		switch (selectedBullet) {
		case BULT_ORDINARY:
			for (int i = 0; i < ammo.ordinaryBullets.size(); i++) {
				if (ammo.ordinaryBullets[i].state == BULS_INCARTRIDGE && ammo.ordinaryBulletData.fireDelay.getElapsedTime().asMilliseconds() > 150 /*Delay between shots*/) {
					ammo.ordinaryBullets[i].bulletShape.setRotation(playerShape.getRotation() - 90);
					ammo.ordinaryBullets[i].bulletShape.setPosition(Vector2f(playerShape.getPosition().x, (rand() % sizeX) + playerShape.getPosition().y - playerShape.getOrigin().x - ammo.ordinaryBullets[i].bulletShape.getSize().y / 2));
					ammo.ordinaryBullets[i].speed.x = ammo.ordinaryBulletData.defaultSpeed.x; ammo.ordinaryBullets[i].speed.y = ammo.ordinaryBulletData.defaultSpeed.y;
					ammo.ordinaryBullets[i].state = BULS_FIRED;
					ammo.ordinaryBulletData.fireDelay.restart();
					return;
				}
			}
			break;
		case BULT_SPLITTING:
			for (int i = 0; i < ammo.splittingBullets.size(); i++) {
				if (ammo.splittingBullets[i].state == BULS_INCARTRIDGE && ammo.splittingBulletData.fireDelay.getElapsedTime().asMilliseconds() > 300) {
					ammo.splittingBullets[i].bulletShape.setRotation(playerShape.getRotation());
					ammo.splittingBullets[i].bulletShape.setPosition(Vector2f(playerShape.getPosition().x, playerShape.getPosition().y));
					ammo.splittingBullets[i].speed = ammo.splittingBulletData.defaultSpeed;
					ammo.splittingBullets[i].state = BULS_FIRED;
					ammo.splittingBulletData.fireDelay.restart();
				}
			}
			break;
		}
	}
	void moveBullets(GameWindow *gwindow) {
		//ORDINARY BULLETS
		for (int i = 0; i < ammo.ordinaryBullets.size(); i++) {
			if (ammo.ordinaryBullets[i].state == BULS_FIRED) {
				ammo.ordinaryBullets[i].bulletShape.move(Vector2f(ammo.ordinaryBullets[i].speed));
			}
			if (isBulletOutOfScreen(gwindow, BULT_ORDINARY, i)) {
				ammo.ordinaryBullets[i].state = BULS_INCARTRIDGE;
			}
		}
		//SPLITTING BULLETS
		for (int i = 0; i < ammo.splittingBullets.size(); i++) {
			if (ammo.splittingBullets[i].state == BULS_FIRED) {
				ammo.splittingBullets[i].bulletShape.move(Vector2f(ammo.splittingBullets[i].speed));
			}
			if (isBulletOutOfScreen(gwindow, BULT_SPLITTING, i)) {
				ammo.splittingBullets[i].state = BULS_INCARTRIDGE;
			}
		}
		//SPLITTED BULLETS
		for (int i = 0; i < ammo.splittedBullets.size(); i++) {
			ammo.splittedBullets[i].bulletShape.move(ammo.splittedBullets[i].speed);
			if (isBulletOutOfScreen(gwindow, BULT_SPLITTED, i)) {
				ammo.splittedBullets.erase(ammo.splittedBullets.begin() + i);
			}
		}
		//TWICE SPLITTED BULLETS
		for (int i = 0; i < ammo.twiceSplittedBullets.size(); i++) {
			ammo.twiceSplittedBullets[i].bulletShape.move(ammo.twiceSplittedBullets[i].speed);
			if (isBulletOutOfScreen(gwindow, BULT_TWICESPLITTED, i)) {
				ammo.twiceSplittedBullets.erase(ammo.twiceSplittedBullets.begin() + i);
			}
		}
	}
	void splitBullet(SplittingBullet *splittingBullet) {
		SplittedBullet splittedBullet1;
		SplittedBullet splittedBullet2;
		splittedBullet1.bulletShape.setTexture(&ammo.splittingBulletData.texture);
		splittedBullet1.bulletShape.setPosition(splittingBullet->bulletShape.getPosition());
		splittedBullet1.bulletShape.setRadius(splittingBullet->bulletShape.getRadius() / 1.5);
		splittedBullet1.damage = splittingBullet->damage / 1.5;
		splittedBullet1.speed = splittingBullet->speed;
		splittedBullet1.bulletShape.setRotation(splittingBullet->bulletShape.getRotation());
		for (int i = 0; i < 13; i++) {
			if (splittedBullet1.bulletShape.getRotation() <= 90) {
				splittedBullet1.speed.x -= ammo.splittingBulletData.speedVariation;
			}
			else {
				splittedBullet1.speed.x += ammo.splittingBulletData.speedVariation;
			}
			splittedBullet1.speed.y -= ammo.splittingBulletData.speedVariation;
			splittedBullet1.bulletShape.rotate(-1);
		}

		ammo.splittedBullets.push_back(splittedBullet1);

		splittedBullet2.bulletShape.setTexture(&ammo.splittingBulletData.texture);
		splittedBullet2.bulletShape.setPosition(splittingBullet->bulletShape.getPosition());
		splittedBullet2.bulletShape.setRadius(splittingBullet->bulletShape.getRadius() / 1.5);
		splittedBullet2.damage = splittingBullet->damage / 1.5;
		splittedBullet2.speed = splittingBullet->speed;
		splittedBullet2.bulletShape.setRotation(splittingBullet->bulletShape.getRotation());
		for (int i = 0; i < 13; i++) {
			if (splittingBullet->bulletShape.getRotation() >= 90) {
				splittedBullet2.speed.x -= ammo.splittingBulletData.speedVariation;
			}
			else {
				splittedBullet2.speed.x += ammo.splittingBulletData.speedVariation;
			}
			splittedBullet2.speed.y += ammo.splittingBulletData.speedVariation;
			splittedBullet2.bulletShape.rotate(1);
		}

		ammo.splittedBullets.push_back(splittedBullet2);
	}
	void splitBullet(SplittedBullet *splittedBullet) {
		SplittedBullet twiceSplittedBullet1;
		SplittedBullet twiceSplittedBullet2;
		twiceSplittedBullet1.bulletShape.setTexture(&ammo.splittingBulletData.texture);
		twiceSplittedBullet1.bulletShape.setPosition(splittedBullet->bulletShape.getPosition());
		twiceSplittedBullet1.bulletShape.setRadius(splittedBullet->bulletShape.getRadius() / 1.5);
		twiceSplittedBullet1.damage = splittedBullet->damage / 1.5;
		twiceSplittedBullet1.speed = splittedBullet->speed;
		twiceSplittedBullet1.bulletShape.setRotation(splittedBullet->bulletShape.getRotation());
		for (int i = 0; i < 13; i++) {
			if (twiceSplittedBullet1.bulletShape.getRotation() <= 90) {
				twiceSplittedBullet1.speed.x -= ammo.splittingBulletData.speedVariation;
			}
			else {
				twiceSplittedBullet1.speed.x += ammo.splittingBulletData.speedVariation;
			}
			twiceSplittedBullet1.speed.y -= ammo.splittingBulletData.speedVariation;
			twiceSplittedBullet1.bulletShape.rotate(-1);
		}

		ammo.twiceSplittedBullets.push_back(twiceSplittedBullet1);

		twiceSplittedBullet2.bulletShape.setTexture(&ammo.splittingBulletData.texture);
		twiceSplittedBullet2.bulletShape.setPosition(splittedBullet->bulletShape.getPosition());
		twiceSplittedBullet2.bulletShape.setRadius(splittedBullet->bulletShape.getRadius() / 1.5);
		twiceSplittedBullet2.damage = splittedBullet->damage / 1.5;
		twiceSplittedBullet2.speed = splittedBullet->speed;
		twiceSplittedBullet2.bulletShape.setRotation(splittedBullet->bulletShape.getRotation());
		for (int i = 0; i < 13; i++) {
			if (twiceSplittedBullet2.bulletShape.getRotation() >= 90) {
				twiceSplittedBullet2.speed.x -= ammo.splittingBulletData.speedVariation;
			}
			else {
				twiceSplittedBullet2.speed.x += ammo.splittingBulletData.speedVariation;
			}
			twiceSplittedBullet2.speed.y += ammo.splittingBulletData.speedVariation;
			twiceSplittedBullet2.bulletShape.rotate(1);
		}

		ammo.twiceSplittedBullets.push_back(twiceSplittedBullet2);
	}
	bool isBulletOutOfScreen(GameWindow *gwindow, BULLETTYPE type, int index) {
		switch (type) {
		case BULT_ORDINARY:
			return ammo.ordinaryBullets[index].bulletShape.getPosition().x >= gwindow->window.getSize().x ||
				ammo.ordinaryBullets[index].bulletShape.getPosition().y >= gwindow->window.getSize().y + ammo.ordinaryBullets[index].bulletShape.getSize().y ||
				ammo.ordinaryBullets[index].bulletShape.getPosition().y <= 0 - ammo.ordinaryBullets[index].bulletShape.getSize().y;
			break;
		case BULT_SPLITTING:
			return ammo.splittingBullets[index].bulletShape.getPosition().x >= gwindow->window.getSize().x ||
				ammo.splittingBullets[index].bulletShape.getPosition().y >= gwindow->window.getSize().y ||
				ammo.splittingBullets[index].bulletShape.getPosition().y <= 0 - ammo.splittingBullets[index].bulletShape.getRadius() * 2;
			break;
		case BULT_SPLITTED:
			return ammo.splittedBullets[index].bulletShape.getPosition().x >= gwindow->window.getSize().x ||
				ammo.splittedBullets[index].bulletShape.getPosition().y >= gwindow->window.getSize().y ||
				ammo.splittedBullets[index].bulletShape.getPosition().y <= 0 - ammo.splittedBullets[index].bulletShape.getRadius() * 2;
			break;
		case BULT_TWICESPLITTED:
			return ammo.twiceSplittedBullets[index].bulletShape.getPosition().x >= gwindow->window.getSize().x ||
				ammo.twiceSplittedBullets[index].bulletShape.getPosition().y >= gwindow->window.getSize().y ||
				ammo.twiceSplittedBullets[index].bulletShape.getPosition().y <= 0 - ammo.twiceSplittedBullets[index].bulletShape.getRadius() * 2;
			break;
		}
	}
	void checkForBulletSwap() {
		if (Keyboard::isKeyPressed(Keyboard::Num1)) {
			selectedBullet = BULT_ORDINARY;
			scope.setSize(Vector2f(55, 15));
			scope.setOrigin(Vector2f(0, scope.getSize().y / 2));
			scope.setPosition(Vector2f(playerShape.getPosition().x, playerShape.getPosition().y));
			scope.setTextureRect(IntRect(0, 0, 55, 15));
			scope.setTexture(&ordinaryBulletScopeTexture);
			scope.setRotation(playerShape.getRotation() - 90);
		}
		if (Keyboard::isKeyPressed(Keyboard::Num2)) {
			selectedBullet = BULT_SPLITTING;
			scope.setSize(Vector2f(55, 29));
			scope.setOrigin(Vector2f(0, scope.getSize().y / 2));
			scope.setPosition(Vector2f(playerShape.getPosition().x, playerShape.getPosition().y));
			scope.setTextureRect(IntRect(0, 0, 55, 29));
			scope.setTexture(&splittingBulletScopeTexture);
			scope.setRotation(playerShape.getRotation() - 90);
		}
	}
};
class RomaEnemy {
private:
	ENEMYSTATE state;
	Clock fireDelay;
public:
	CircleShape shape;
	vector <RectangleShape> romaBullets;
	void setState(string requestedState) {
		if (requestedState == "ENEMY_SPAWNED") {
			state = ES_SPAWNED;
		}
		if(requestedState == "ENEMY_UNSPAWNED") {
			state = ES_UNSPAWNED;
		}
	}
	ENEMYSTATE getState() {
		return state;
	}
	void move() {
		if (state == ES_SPAWNED) {
			/*if (romaEnemies[i].romaShape.getPosition().y < player.playerShape.getPosition().y) {
				romaEnemies[i].romaShape.move(Vector2f(0, 0.05));
			}
			if (romaEnemies[i].romaShape.getPosition().y > player.playerShape.getPosition().y) {
				romaEnemies[i].romaShape.move(Vector2f(0, -0.05));
			}*/
			// TEST
			//romaEnemies[i].romaShape.rotate(1);
		}
	}
	bool needToShoot() {
		return state == ES_SPAWNED && fireDelay.getElapsedTime().asMilliseconds() >= 1500;
	}
	void fire(Texture *romaBulletTexture) {
		if (needToShoot()) {
			RectangleShape bullet;
			bullet.setTexture(romaBulletTexture);
			bullet.setSize(Vector2f(40, 22));
			bullet.setPosition(Vector2f(shape.getPosition()));
			romaBullets.push_back(bullet);
			fireDelay.restart();
		}
	}
	void moveBullets(Vector2f bulletSpeed) {
		for (int i = 0; i < romaBullets.size(); i++) {
			romaBullets[i].move(bulletSpeed);
			if (romaBullets[i].getPosition().x + romaBullets[i].getSize().x < 0) {
				romaBullets.erase(romaBullets.begin() + i);
			}
		}
	}
	void takeDamage(int damage) {
		shape.setRadius(shape.getRadius() - damage);
		shape.setOrigin(Vector2f(shape.getRadius(), shape.getRadius()));
	}
};
class Game {
private:
	Player player;
	Texture gameBackgroundTexture;
	RectangleShape gameBackground;
	Clock delayBetweenEscapePresses;
	RomaEnemiesData romaEnemiesData;
public:
	GameWindow gameWindow;
	Menu menu;
	Event event;
	GAMESTATE gameState;
	vector <RomaEnemy> romaEnemies;
	void initWindow() {
		gameWindow.x = 1280;
		gameWindow.y = 720;
		gameWindow.title = "BRUH Galactic";
		gameWindow.window.create(VideoMode(gameWindow.x, gameWindow.y), gameWindow.title);
		gameWindow.window.setFramerateLimit(240);
	}
	void initGameBackground() {
		gameBackgroundTexture.loadFromFile("Textures\\background.jpg");
		gameBackground.setTexture(&gameBackgroundTexture);
		gameBackground.setSize(Vector2f(gameWindow.x, gameWindow.y));
	}
	void initEnemies() {
		romaEnemiesData.areRomaEnemiesActive = true;
		romaEnemiesData.romaEnemyTexture.loadFromFile("Textures\\roma.jpg");
		romaEnemiesData.romaBulletTexture.loadFromFile("Textures\\romaBulletTexture.jpg");
		romaEnemies.clear();
		romaEnemies.resize(15);
		/*
		*
		*				OR
		*
		for (int i = 0; i < romaEnemies.size(); i++) {
			romaEnemies[i].romaBullets.clear();
		}


		*/
		romaEnemiesData.romaBulletSpeed.x = -1; romaEnemiesData.romaBulletSpeed.y = 0;
		romaEnemiesData.defaultRadius = 40;
		for (int i = 0; i < romaEnemies.size(); i++) {
			romaEnemies[i].shape.setTexture(&romaEnemiesData.romaEnemyTexture);
			romaEnemies[i].setState("ENEMY_UNSPAWNED");
		}
	}
	void initGame() {
		gameState = GS_PLAY;
		initGameBackground();
		player.initPlayer(&gameWindow);
		player.initAmmunition();
		initEnemies();
	}
	void exitGame() {
		gameState = GS_EXIT;
	}
	bool isBulletTouchEnemy(BULLETTYPE bulletType, ENEMYTYPE enemyType, int bulletIndex, int enemyIndex) {
		switch (bulletType) {
		case BULT_ORDINARY:
			switch (enemyType) {
			case ET_ROMA:
				return player.ammo.ordinaryBullets[bulletIndex].bulletShape.getGlobalBounds().intersects(romaEnemies[enemyIndex].shape.getGlobalBounds());
				break;
			}
			break;
		case BULT_SPLITTING:
			switch (enemyType) {
			case ET_ROMA:
				return player.ammo.splittingBullets[bulletIndex].bulletShape.getGlobalBounds().intersects(romaEnemies[enemyIndex].shape.getGlobalBounds());
				break;
			}
			break;
		case BULT_SPLITTED:
			switch (enemyType) {
			case ET_ROMA:
				return player.ammo.splittedBullets[bulletIndex].bulletShape.getGlobalBounds().intersects(romaEnemies[enemyIndex].shape.getGlobalBounds());
				break;
			}
			break;
		case BULT_TWICESPLITTED:
			switch (enemyType) {
			case ET_ROMA:
				return player.ammo.twiceSplittedBullets[bulletIndex].bulletShape.getGlobalBounds().intersects(romaEnemies[enemyIndex].shape.getGlobalBounds());
				break;
			}
			break;
		}
	}
	bool isBulletTouchEnemyBullet(BULLETTYPE bulletType, ENEMYTYPE enemyType, int bulletIndex, int enemyIndex, int enemyBulletIndex) {
		switch (bulletType) {
		case BULT_ORDINARY:
			switch (enemyType) {
			case ET_ROMA:
				return player.ammo.ordinaryBullets[bulletIndex].bulletShape.getGlobalBounds().intersects(romaEnemies[enemyIndex].romaBullets[enemyBulletIndex].getGlobalBounds());
				break;
			}
			break;
		case BULT_SPLITTING:
			switch (enemyType) {
			case ET_ROMA:
				return player.ammo.splittingBullets[bulletIndex].bulletShape.getGlobalBounds().intersects(romaEnemies[enemyIndex].romaBullets[enemyBulletIndex].getGlobalBounds());
				break;
			}
			break;
		case BULT_SPLITTED:
			switch (enemyType) {
			case ET_ROMA:
				return player.ammo.splittedBullets[bulletIndex].bulletShape.getGlobalBounds().intersects(romaEnemies[enemyIndex].romaBullets[enemyBulletIndex].getGlobalBounds());
				break;
			}
			break;
		case BULT_TWICESPLITTED:
			switch (enemyType) {
			case ET_ROMA:
				return player.ammo.twiceSplittedBullets[bulletIndex].bulletShape.getGlobalBounds().intersects(romaEnemies[enemyIndex].romaBullets[enemyBulletIndex].getGlobalBounds());
				break;
			}
			break;
		}
	}
	bool isEnemyDead(ENEMYTYPE enemyType, int enemyIndex) {
		switch (enemyType) {
		case ET_ROMA:
			return romaEnemies[enemyIndex].shape.getRadius() <= 10;
			break;
		}
	}
	void checkForBulletsDamage() {
		//ORDINARY BULLETS
		for (int i = 0; i < player.ammo.ordinaryBullets.size(); i++) {
			if (player.ammo.ordinaryBullets[i].state == BULS_FIRED) {
				for (int j = 0; j < romaEnemies.size(); j++) {
					if (romaEnemies[j].getState() == ES_SPAWNED) {
						if (isBulletTouchEnemy(BULT_ORDINARY, ET_ROMA, i, j)) {
							player.ammo.ordinaryBullets[i].state = BULS_INCARTRIDGE;
							romaEnemies[j].takeDamage(player.ammo.ordinaryBulletData.damage);
							if (isEnemyDead(ET_ROMA, j)) {
								romaEnemies[j].setState("ENEMY_UNSPAWNED");
							}
						}
					}
					for (int k = 0; k < romaEnemies[j].romaBullets.size(); k++) {
						if (isBulletTouchEnemyBullet(BULT_ORDINARY, ET_ROMA, i, j, k)) {
							player.ammo.ordinaryBullets[i].state = BULS_INCARTRIDGE;
							romaEnemies[j].romaBullets.erase(romaEnemies[j].romaBullets.begin() + k);
						}
					}
				}
			}
		}
		//SPLITTING BULLETS
		for (int i = 0; i < player.ammo.splittingBullets.size(); i++) {
			if (player.ammo.splittingBullets[i].state == BULS_FIRED) {
				for (int j = 0; j < romaEnemies.size(); j++) {
					if (romaEnemies[j].getState() == ES_SPAWNED) {
						if (isBulletTouchEnemy(BULT_SPLITTING, ET_ROMA, i, j)) {
							romaEnemies[j].takeDamage(player.ammo.splittingBullets[i].damage);
							player.splitBullet(&player.ammo.splittingBullets[i]);
							player.ammo.splittingBullets[i].state = BULS_INCARTRIDGE;
							if (isEnemyDead(ET_ROMA, j)) {
								romaEnemies[j].setState("ENEMY_UNSPAWNED");
							}
						}
					}
					if (player.ammo.splittingBullets[i].state == BULS_FIRED) {
						for (int k = 0; k < romaEnemies[j].romaBullets.size(); k++) {
							if (isBulletTouchEnemyBullet(BULT_SPLITTING, ET_ROMA, i, j, k)) {
								player.ammo.splittingBullets[i].state = BULS_INCARTRIDGE;
								player.splitBullet(&player.ammo.splittingBullets[i]);
								romaEnemies[j].romaBullets.erase(romaEnemies[j].romaBullets.begin() + k);
							}
						}
					}
				}
			}
		}
		//SPLITTED BULLETS
		for (int j = 0; j < romaEnemies.size(); j++) {
			for (int i = 0; i < player.ammo.splittedBullets.size(); i++) {
				if (romaEnemies[j].getState() == ES_SPAWNED) {
					if (isBulletTouchEnemy(BULT_SPLITTED, ET_ROMA, i, j)) {
						romaEnemies[j].takeDamage(player.ammo.splittedBullets[i].damage);
						player.splitBullet(&player.ammo.splittedBullets[i]);
						player.ammo.splittedBullets.erase(player.ammo.splittedBullets.begin() + i);
						if (isEnemyDead(ET_ROMA, j)) {
							romaEnemies[j].setState("ENEMY_UNSPAWNED");
						}
						continue;
					}
				}
				for (int k = 0; k < romaEnemies[j].romaBullets.size(); k++) {
					if (isBulletTouchEnemyBullet(BULT_SPLITTED, ET_ROMA, i, j, k)) {
						player.splitBullet(&player.ammo.splittedBullets[i]);
						player.ammo.splittedBullets.erase(player.ammo.splittedBullets.begin() + i);
						romaEnemies[j].romaBullets.erase(romaEnemies[j].romaBullets.begin() + k);
						break;
					}
				}
			}
		}
		//TWICE SPLITTED BULLETS
		for (int j = 0; j < romaEnemies.size(); j++) {
			for (int i = 0; i < player.ammo.twiceSplittedBullets.size(); i++) {
				if (romaEnemies[j].getState() == ES_SPAWNED) {
					if (isBulletTouchEnemy(BULT_TWICESPLITTED, ET_ROMA, i, j)) {
						romaEnemies[j].takeDamage(player.ammo.twiceSplittedBullets[i].damage);
						player.ammo.twiceSplittedBullets.erase(player.ammo.twiceSplittedBullets.begin() + i);
						if (isEnemyDead(ET_ROMA, j)) {
							romaEnemies[j].setState("ENEMY_UNSPAWNED");
						}
						continue;
					}
				}
				for (int k = 0; k < romaEnemies[j].romaBullets.size(); k++) {
					if (isBulletTouchEnemyBullet(BULT_TWICESPLITTED, ET_ROMA, i, j, k)) {
						player.ammo.twiceSplittedBullets.erase(player.ammo.twiceSplittedBullets.begin() + i);
						romaEnemies[j].romaBullets.erase(romaEnemies[j].romaBullets.begin() + k);
						break;
					}
				}
			}
		}
	}
	void checkForPlayerCollisions() {
		for (int i = 0; i < romaEnemies.size(); i++) {
			for (int j = 0; j < romaEnemies[i].romaBullets.size(); j++) {
				if (player.playerShape.getGlobalBounds().intersects(romaEnemies[i].romaBullets[j].getGlobalBounds())) {
					player.setHP(player.getHPAmount() - 1);
					romaEnemies[i].romaBullets.erase(romaEnemies[i].romaBullets.begin() + j);
				}
			}
		}
	}
	bool isRomaEnemyNeedToSpawn() {
		return (romaEnemiesData.areRomaEnemiesActive == true && rand() % 200 == 1);
	}
	void spawnRomaEnemy() {
		for (int i = 0; i < romaEnemies.size(); i++) {
			if (romaEnemies[i].getState() == ES_UNSPAWNED) {
				romaEnemies[i].shape.setRadius(romaEnemiesData.defaultRadius);
				romaEnemies[i].shape.setOrigin(Vector2f(romaEnemiesData.defaultRadius, romaEnemiesData.defaultRadius));
				romaEnemies[i].shape.setPosition(Vector2f(gameWindow.x - romaEnemiesData.defaultRadius, rand() % (gameWindow.y - romaEnemiesData.defaultRadius * 4) + romaEnemiesData.defaultRadius * 2));
				romaEnemies[i].setState("ENEMY_SPAWNED");
				return;
			}
		}
	}
	void checkForEnemiesSpawn() {
		if (isRomaEnemyNeedToSpawn()) {
			spawnRomaEnemy();
		}
	}

	void updateMenuFrame(Game *gameClass) {
		menu.controlMenu();
		menu.selectMenuItem(gameClass);
		gameWindow.window.clear(Color::Black);
		gameWindow.window.draw(menu.menuBackground);
		gameWindow.window.draw(menu.caption);
		gameWindow.window.draw(menu.menuItemBackground);
		for (int i = 0; i < menu.menuItems.size(); i++) {
			gameWindow.window.draw(menu.menuItems[i].title);
		}
		gameWindow.window.display();
	}

	void updateGameFrame() {
		if (Keyboard::isKeyPressed(Keyboard::Key::Escape)) {
			if (delayBetweenEscapePresses.getElapsedTime().asMilliseconds() >= 1000) {
				gameState = GS_MENU;
				delayBetweenEscapePresses.restart();
			}
		}

		if (player.getHPAmount() <= 0) {
			gameState = GS_MENU;
		}
		player.controlPlayer(&gameWindow);
		player.checkForBulletSwap();
		player.moveBullets(&gameWindow);

		checkForEnemiesSpawn();
		for (int i = 0; i < romaEnemies.size(); i++) {
			romaEnemies[i].fire(&romaEnemiesData.romaBulletTexture);
			romaEnemies[i].moveBullets(romaEnemiesData.romaBulletSpeed);
		}

		checkForPlayerCollisions();
		checkForBulletsDamage();
		gameWindow.window.clear();
		gameWindow.window.draw(gameBackground);
		for (int i = 0; i < player.ammo.ordinaryBullets.size(); i++) {
			if (player.ammo.ordinaryBullets[i].state == BULS_FIRED) {
				gameWindow.window.draw(player.ammo.ordinaryBullets[i].bulletShape);
			}
		}
		for (int i = 0; i < player.ammo.splittingBullets.size(); i++) {
			if (player.ammo.splittingBullets[i].state == BULS_FIRED) {
				gameWindow.window.draw(player.ammo.splittingBullets[i].bulletShape);
			}
		}
		for (int i = 0; i < player.ammo.splittedBullets.size(); i++) {
			gameWindow.window.draw(player.ammo.splittedBullets[i].bulletShape);
		}
		for (int i = 0; i < player.ammo.twiceSplittedBullets.size(); i++) {
			gameWindow.window.draw(player.ammo.twiceSplittedBullets[i].bulletShape);
		}

		
		cout << endl << player.ammo.splittedBullets.size();
		cout << setw(10) << player.ammo.twiceSplittedBullets.size();
		//cout << endl << setw(10) << player.playerShape.getRotation();


		gameWindow.window.draw(player.scope);
		gameWindow.window.draw(player.playerShape);
		for (int i = 0; i < romaEnemies.size(); i++) {
			for (int j = 0; j < romaEnemies[i].romaBullets.size(); j++) {
				gameWindow.window.draw(romaEnemies[i].romaBullets[j]);
			}
			if (romaEnemies[i].getState() == ES_SPAWNED) {
				gameWindow.window.draw(romaEnemies[i].shape);
			}
		}
		gameWindow.window.display();
	}
};
int main() {
	Game BG;
	BG.initWindow();
	BG.menu.initMenu(&BG.gameWindow);
	BG.menu.addMenuItem("PLAY", &Game::initGame);
	BG.menu.addMenuItem("EXIT", &Game::exitGame);
	BG.gameState = GS_MENU;
	while (BG.gameWindow.window.isOpen()) {
		while (BG.gameWindow.window.pollEvent(BG.event)) {
			switch (BG.event.type) {
			case Event::Closed:
				BG.gameWindow.window.close();
				break;
			}
		}
		int num = 1;
		switch (BG.gameState) {
		case GS_MENU:
			BG.updateMenuFrame(&BG);
			break;
		case GS_PLAY:
			BG.updateGameFrame();
			break;
		case GS_EXIT:
			BG.gameWindow.window.close();
			break;
		}
	}
}