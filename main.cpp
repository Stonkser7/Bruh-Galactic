#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <string>
#include <cmath>
#include <iomanip>
using namespace sf;
using namespace std;

enum GAMESTATE { GS_PAUSE,GS_PLAY, GS_EXIT, GS_MENU };
enum ENEMYSTATE { ES_SPAWNED, ES_UNSPAWNED };
enum ENEMYTYPE { ET_ROMA };
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


class OrdinaryBullet {
public:
	RectangleShape shape;
	Vector2f speed;
	void move() {
		shape.move(speed);
	}
	bool isOutOfScreen(GameWindow* gwindow) {
		return shape.getPosition().x >= gwindow->window.getSize().x || shape.getPosition().y >= gwindow->window.getSize().y + shape.getSize().y || shape.getPosition().y <= 0 - shape.getSize().y;
	}
};
class SplittingBullet {
public:
	CircleShape shape;
	Vector2f speed;
	int damage;
	void move() {
		shape.move(speed);
	}
	bool isOutOfScreen(GameWindow *gwindow) {
		return shape.getPosition().x >= gwindow->window.getSize().x || shape.getPosition().y >= gwindow->window.getSize().y || shape.getPosition().y <= 0 - shape.getRadius() * 2;
	}
};
class SplittedBullet {
public:
	CircleShape shape;
	Vector2f speed;
	int damage;
	int levelOfSplit;
	void move() {
		shape.move(speed);
	}
	bool isOutOfScreen(GameWindow* gwindow) {
		return shape.getPosition().x >= gwindow->window.getSize().x || shape.getPosition().y >= gwindow->window.getSize().y || shape.getPosition().y <= 0 - shape.getRadius() * 2;
	}
};



struct Ammunition {
	OrdinaryBulletData ordinaryBulletData;
	vector <OrdinaryBullet> ordinaryBullets;
	SplittingBulletData splittingBulletData;
	vector <SplittingBullet> splittingBullets;
	vector <SplittedBullet> splittedBullets;
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
		ammo.ordinaryBulletData.texture.loadFromFile("Textures\\pchel.jpg");
		ammo.ordinaryBulletData.damage = 10;
		ammo.ordinaryBulletData.defaultSpeed.x = 7; ammo.ordinaryBulletData.defaultSpeed.y = 0;
		ammo.ordinaryBulletData.speedVariation = ((ammo.ordinaryBulletData.defaultSpeed.x / 100) * 50) / 45;

		ammo.splittingBulletData.texture.loadFromFile("Textures\\splittingBulletTexture.png");
		ammo.splittingBulletData.defaultDamage = 25;
		ammo.splittingBulletData.defaultRadius = 25;
		ammo.splittingBulletData.defaultSpeed.x = 3; ammo.splittingBulletData.defaultSpeed.y = 0;
		ammo.splittingBulletData.speedVariation = ((ammo.splittingBulletData.defaultSpeed.x / 100) * 50) / 45;

		ammo.ordinaryBullets.clear();
		ammo.splittingBullets.clear();
		ammo.splittedBullets.clear();
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
			if (ammo.ordinaryBulletData.fireDelay.getElapsedTime().asMilliseconds() > 150 /*Delay between shots*/) {
				OrdinaryBullet bullet;
				bullet.shape.setSize(Vector2f(30.f, 17.f));
				bullet.shape.setTexture(&ammo.ordinaryBulletData.texture);
				bullet.shape.setRotation(playerShape.getRotation() - 90);
				bullet.shape.setPosition(Vector2f(playerShape.getPosition().x, (rand() % sizeX) + playerShape.getPosition().y - playerShape.getOrigin().x - bullet.shape.getSize().y / 2));
				bullet.speed = ammo.ordinaryBulletData.defaultSpeed;
				ammo.ordinaryBullets.push_back(bullet);
				ammo.ordinaryBulletData.fireDelay.restart();
				return;
			}
			break;
		case BULT_SPLITTING:
			if (ammo.splittingBulletData.fireDelay.getElapsedTime().asMilliseconds() > 300) {
				SplittingBullet bullet;
				bullet.shape.setRadius(ammo.splittingBulletData.defaultRadius);
				bullet.shape.setTexture(&ammo.splittingBulletData.texture);
				bullet.shape.setOrigin(Vector2f(ammo.splittingBulletData.defaultRadius, ammo.splittingBulletData.defaultRadius));
				bullet.shape.setRotation(playerShape.getRotation());
				bullet.shape.setPosition(Vector2f(playerShape.getPosition().x, playerShape.getPosition().y));
				bullet.damage = ammo.splittingBulletData.defaultDamage;
				bullet.speed = ammo.splittingBulletData.defaultSpeed;
				ammo.splittingBullets.push_back(bullet);
				ammo.splittingBulletData.fireDelay.restart();
				return;
			}
			break;
		}
	}
	void splitBullet(SplittingBullet *splittingBullet) {
		SplittedBullet splittedBullet;
		splittedBullet.shape.setTexture(&ammo.splittingBulletData.texture);
		splittedBullet.shape.setPosition(splittingBullet->shape.getPosition());
		splittedBullet.shape.setRadius(splittingBullet->shape.getRadius() / 1.5);
		splittedBullet.damage = splittingBullet->damage / 1.5;
		splittedBullet.levelOfSplit = 1;


		splittedBullet.speed = splittingBullet->speed;
		splittedBullet.shape.setRotation(splittingBullet->shape.getRotation());
		for (int i = 0; i < 13; i++) {
			if (splittedBullet.shape.getRotation() <= 90) {
				splittedBullet.speed.x -= ammo.splittingBulletData.speedVariation;
			}
			else {
				splittedBullet.speed.x += ammo.splittingBulletData.speedVariation;
			}
			splittedBullet.speed.y -= ammo.splittingBulletData.speedVariation;
			splittedBullet.shape.rotate(-1);
		}

		ammo.splittedBullets.push_back(splittedBullet);

		splittedBullet.speed = splittingBullet->speed;
		splittedBullet.shape.setRotation(splittingBullet->shape.getRotation());
		for (int i = 0; i < 13; i++) {
			if (splittingBullet->shape.getRotation() >= 90) {
				splittedBullet.speed.x -= ammo.splittingBulletData.speedVariation;
			}
			else {
				splittedBullet.speed.x += ammo.splittingBulletData.speedVariation;
			}
			splittedBullet.speed.y += ammo.splittingBulletData.speedVariation;
			splittedBullet.shape.rotate(1);
		}

		ammo.splittedBullets.push_back(splittedBullet);
	}
	void splitBullet(SplittedBullet* splittedBullet) {
		if (splittedBullet->levelOfSplit < 2) {
			SplittedBullet twiceSplittedBullet;
			twiceSplittedBullet.shape.setTexture(&ammo.splittingBulletData.texture);
			twiceSplittedBullet.shape.setPosition(splittedBullet->shape.getPosition());
			twiceSplittedBullet.shape.setRadius(splittedBullet->shape.getRadius() / 1.5);
			twiceSplittedBullet.damage = splittedBullet->damage / 1.5;
			twiceSplittedBullet.levelOfSplit = splittedBullet->levelOfSplit + 1;


			twiceSplittedBullet.speed = splittedBullet->speed;
			twiceSplittedBullet.shape.setRotation(splittedBullet->shape.getRotation());
			for (int i = 0; i < 13; i++) {
				if (twiceSplittedBullet.shape.getRotation() <= 90) {
					twiceSplittedBullet.speed.x -= ammo.splittingBulletData.speedVariation;
				}
				else {
					twiceSplittedBullet.speed.x += ammo.splittingBulletData.speedVariation;
				}
				twiceSplittedBullet.speed.y -= ammo.splittingBulletData.speedVariation;
				twiceSplittedBullet.shape.rotate(-1);
			}

			ammo.splittedBullets.push_back(twiceSplittedBullet);

			twiceSplittedBullet.speed = splittedBullet->speed;
			twiceSplittedBullet.shape.setRotation(splittedBullet->shape.getRotation());
			for (int i = 0; i < 13; i++) {
				if (twiceSplittedBullet.shape.getRotation() >= 90) {
					twiceSplittedBullet.speed.x -= ammo.splittingBulletData.speedVariation;
				}
				else {
					twiceSplittedBullet.speed.x += ammo.splittingBulletData.speedVariation;
				}
				twiceSplittedBullet.speed.y += ammo.splittingBulletData.speedVariation;
				twiceSplittedBullet.shape.rotate(1);
			}

			ammo.splittedBullets.push_back(twiceSplittedBullet);
		}
	}
	void deleteBullet(BULLETTYPE bulletType, int index) {
		switch (bulletType) {
		case BULT_ORDINARY:
			ammo.ordinaryBullets.erase(ammo.ordinaryBullets.begin() + index);
			break;
		case BULT_SPLITTING:
			ammo.splittingBullets.erase(ammo.splittingBullets.begin() + index);
			break;
		case BULT_SPLITTED:
			ammo.splittedBullets.erase(ammo.splittedBullets.begin() + index);
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
class Enemy {
private:
	ENEMYSTATE state;
public:
	CircleShape shape;
	Clock fireClock;
	int fireDelayAsMilliseconds;
	int minRadius;
	void setState(string state) {
		if (state == "ES_SPAWNED") {
			this->state = ES_SPAWNED;
		}
		if (state == "ES_UNSPAWNED") {
			this->state = ES_UNSPAWNED;
		}
	}
	ENEMYSTATE getState() {
		return state;
	}
	bool isNeedToShoot() {
		return state == ES_SPAWNED && fireClock.getElapsedTime().asMilliseconds() >= fireDelayAsMilliseconds;
	}
	void takeDamage(int damage) {
		shape.setRadius(shape.getRadius() - damage);
		shape.setOrigin(Vector2f(shape.getRadius(), shape.getRadius()));
	}
	bool isDead() {
		return shape.getRadius() <= minRadius;
	}
	void die() {
		state = ES_UNSPAWNED;
	}
};
class RomaEnemy : public Enemy{
public:
	vector <RectangleShape> bullets;
	void move() {
		if (getState() == ES_SPAWNED) {
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
	void fire(Texture *romaBulletTexture) {
		RectangleShape bullet;
		bullet.setTexture(romaBulletTexture);
		bullet.setSize(Vector2f(40, 22));
		bullet.setPosition(Vector2f(shape.getPosition()));
		bullets.push_back(bullet);
		fireClock.restart();
	}
	void moveBullets(Vector2f bulletSpeed) {
		for (int i = 0; i < bullets.size(); i++) {
			bullets[i].move(bulletSpeed);
			if (bullets[i].getPosition().x + bullets[i].getSize().x < 0) {
				bullets.erase(bullets.begin() + i);
			}
		}
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
		romaEnemies.resize(5);
		romaEnemiesData.romaBulletSpeed.x = -1; romaEnemiesData.romaBulletSpeed.y = 0;
		romaEnemiesData.defaultRadius = 40;
		for (int i = 0; i < romaEnemies.size(); i++) {
			romaEnemies[i].shape.setTexture(&romaEnemiesData.romaEnemyTexture);
			romaEnemies[i].fireDelayAsMilliseconds = 1500;
			romaEnemies[i].minRadius = 10;
			romaEnemies[i].setState("ES_UNSPAWNED");
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
	void checkForOrdinaryBulletCollisions() {
		for (int j = 0; j < romaEnemies.size(); j++) {
			for (int i = 0; i < player.ammo.ordinaryBullets.size(); i++) {
				if (romaEnemies[j].getState() == ES_SPAWNED) {
					if (player.ammo.ordinaryBullets[i].shape.getGlobalBounds().intersects(romaEnemies[j].shape.getGlobalBounds())) {
						player.deleteBullet(BULT_ORDINARY, i);
						romaEnemies[j].takeDamage(player.ammo.ordinaryBulletData.damage);
						if (romaEnemies[j].isDead()) {
							romaEnemies[j].die();
						}
						break;
					}
				}
				for (int k = 0; k < romaEnemies[j].bullets.size(); k++) {
					if (player.ammo.ordinaryBullets[i].shape.getGlobalBounds().intersects(romaEnemies[j].bullets[k].getGlobalBounds())) {
						player.deleteBullet(BULT_ORDINARY, i);
						romaEnemies[j].bullets.erase(romaEnemies[j].bullets.begin() + k);
						break;
					}
				}
			}
		}
	}
	void checkForSplittingBulletCollisions() {
		for (int j = 0; j < romaEnemies.size(); j++) {
			for (int i = 0; i < player.ammo.splittingBullets.size(); i++) {
				if (romaEnemies[j].getState() == ES_SPAWNED) {
					if (player.ammo.splittingBullets[i].shape.getGlobalBounds().intersects(romaEnemies[j].shape.getGlobalBounds())) {
						romaEnemies[j].takeDamage(player.ammo.splittingBullets[i].damage);
						player.splitBullet(&player.ammo.splittingBullets[i]);
						player.deleteBullet(BULT_SPLITTING, i);
						if (romaEnemies[j].isDead()) {
							romaEnemies[j].die();
						}
						break;
					}
				}
				for (int k = 0; k < romaEnemies[j].bullets.size(); k++) {
					if (player.ammo.splittingBullets[i].shape.getGlobalBounds().intersects(romaEnemies[j].bullets[k].getGlobalBounds())) {
						player.splitBullet(&player.ammo.splittingBullets[i]);
						player.deleteBullet(BULT_SPLITTING, i);
						romaEnemies[j].bullets.erase(romaEnemies[j].bullets.begin() + k);
						break;
					}
				}
			}
		}
	}
	void checkForSplittedBulletCollisions() {
		for (int j = 0; j < romaEnemies.size(); j++) {
			for (int i = 0; i < player.ammo.splittedBullets.size(); i++) {
				if (romaEnemies[j].getState() == ES_SPAWNED) {
					if (player.ammo.splittedBullets[i].shape.getGlobalBounds().intersects(romaEnemies[j].shape.getGlobalBounds())) {
						romaEnemies[j].takeDamage(player.ammo.splittedBullets[i].damage);
						player.splitBullet(&player.ammo.splittedBullets[i]);
						player.deleteBullet(BULT_SPLITTED, i);
						if (romaEnemies[j].isDead()) {
							romaEnemies[j].die();
						}
						break;
					}
				}
				for (int k = 0; k < romaEnemies[j].bullets.size(); k++) {
					if (player.ammo.splittedBullets[i].shape.getGlobalBounds().intersects(romaEnemies[j].bullets[k].getGlobalBounds())) {
						player.splitBullet(&player.ammo.splittedBullets[i]);
						player.deleteBullet(BULT_SPLITTED, i);
						romaEnemies[j].bullets.erase(romaEnemies[j].bullets.begin() + k);
						break;
					}
				}
			}
		}
	}
	void checkForBulletsCollisions() {
		checkForOrdinaryBulletCollisions();
		checkForSplittingBulletCollisions();
		checkForSplittedBulletCollisions();
	}
	void checkForPlayerCollisions() {
		for (int i = 0; i < romaEnemies.size(); i++) {
			for (int j = 0; j < romaEnemies[i].bullets.size(); j++) {
				if (player.playerShape.getGlobalBounds().intersects(romaEnemies[i].bullets[j].getGlobalBounds())) {
					player.setHP(player.getHPAmount() - 1);
					romaEnemies[i].bullets.erase(romaEnemies[i].bullets.begin() + j);
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
				romaEnemies[i].setState("ES_SPAWNED");
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
		for (int i = 0; i < player.ammo.ordinaryBullets.size(); i++) {
			if (player.ammo.ordinaryBullets[i].isOutOfScreen(&gameWindow)) {
				player.deleteBullet(BULT_ORDINARY, i);
			}
			else {
				player.ammo.ordinaryBullets[i].move();
			}
		}
		for (int i = 0; i < player.ammo.splittingBullets.size(); i++) {
			if (player.ammo.splittingBullets[i].isOutOfScreen(&gameWindow)) {
				player.deleteBullet(BULT_SPLITTING, i);
			}
			else {
				player.ammo.splittingBullets[i].move();
			}
		}
		for (int i = 0; i < player.ammo.splittedBullets.size(); i++) {
			if (player.ammo.splittedBullets[i].isOutOfScreen(&gameWindow)) {
				player.deleteBullet(BULT_SPLITTED, i);
			}
			else {
				player.ammo.splittedBullets[i].move();
			}
		}

		checkForEnemiesSpawn();
		for (int i = 0; i < romaEnemies.size(); i++) {
			if (romaEnemies[i].isNeedToShoot()) {
				romaEnemies[i].fire(&romaEnemiesData.romaBulletTexture);
			}
			romaEnemies[i].moveBullets(romaEnemiesData.romaBulletSpeed);
		}

		checkForPlayerCollisions();
		checkForBulletsCollisions();
		gameWindow.window.clear();
		gameWindow.window.draw(gameBackground);
		for (int i = 0; i < player.ammo.ordinaryBullets.size(); i++) {
			gameWindow.window.draw(player.ammo.ordinaryBullets[i].shape);
		}
		for (int i = 0; i < player.ammo.splittingBullets.size(); i++) {
			gameWindow.window.draw(player.ammo.splittingBullets[i].shape);
		}
		for (int i = 0; i < player.ammo.splittedBullets.size(); i++) {
			gameWindow.window.draw(player.ammo.splittedBullets[i].shape);
		}

		
		cout << endl << player.ammo.ordinaryBullets.size() << setw(5) << player.ammo.splittingBullets.size() << setw(5) << player.ammo.splittedBullets.size();
		//cout << endl << setw(10) << player.playerShape.getRotation();


		gameWindow.window.draw(player.scope);
		gameWindow.window.draw(player.playerShape);
		for (int i = 0; i < romaEnemies.size(); i++) {
			for (int j = 0; j < romaEnemies[i].bullets.size(); j++) {
				gameWindow.window.draw(romaEnemies[i].bullets[j]);
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