#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <string>
#include <cmath>
#include <iomanip>
#include <windows.h>
#include <cassert>
#include <thread>
#include <cstdlib>
#include <ctime>
using namespace sf;
using namespace std;

enum GAMESTATE { GS_PAUSE, GS_PLAY, GS_EXIT, GS_MENU };
enum ENEMYSTATE { ES_MOVING, ES_UNSPAWNED, ES_SPAWN_ANIM, ES_STANDING };
enum ENEMYTYPE { ET_ROMA, ET_ROCK };
enum BULLETTYPE { BULT_ORDINARY, BULT_SPLITTING, BULT_SPLITTED};
enum ROCKENEMYSIDE {S_UP, S_DOWN};

struct GameWindow {
	RenderWindow window;
	int x;
	int y;
	String title;
};

//////////////
//ENEMIES DATA
//////////////
struct RomaEnemiesData {
	bool areActive;
	Texture enemyTexture;
	Texture bulletTexture;
	Vector2f bulletSpeed;
	int defaultRadius;
};
struct RockEnemiesData {
	bool areActive;
	Texture enemyTexture;
	Texture bulletTexture;
	int defaultRadius;
};

struct RockEnemyBullet {
	CircleShape shape;
	Vector2f speed;
};

/////////////////////
//PLAYER BULLETS DATA
/////////////////////
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

////////////////////////
//PLAYER BULLETS CLASSES
////////////////////////
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
		ammo.ordinaryBulletData.defaultSpeed = { 7, 0 };
		ammo.ordinaryBulletData.speedVariation = ((ammo.ordinaryBulletData.defaultSpeed.x / 100) * 70) / 63;

		ammo.splittingBulletData.texture.loadFromFile("Textures\\splittingBulletTexture.png");
		ammo.splittingBulletData.defaultDamage = 25;
		ammo.splittingBulletData.defaultRadius = 25;
		ammo.splittingBulletData.defaultSpeed = { 5, 0 };
		ammo.splittingBulletData.speedVariation = ((ammo.splittingBulletData.defaultSpeed.x / 100) * 70) / 63;

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
			move(0, -1.7);
		}
		if (Keyboard::isKeyPressed(Keyboard::Key::S) && playerShape.getPosition().y + sizeX / 2 < gwindow->y) {
			move(0, 1.7);
		}

		if (Keyboard::isKeyPressed(Keyboard::Key::A) && playerShape.getRotation() > 63) {
			rotateGun("left");
			rotatePlayer(-1);
		}
		if (Keyboard::isKeyPressed(Keyboard::Key::D) && playerShape.getRotation() < 117) {
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
	int minRadius;							//if shape.getRadius <= minRadius then enemy.die();
	void setState(string state) {
		if (state == "ES_MOVING") {
			this->state = ES_MOVING;
		}
		if (state == "ES_UNSPAWNED") {
			this->state = ES_UNSPAWNED;
		}
		if (state == "ES_SPAWN_ANIM") {
			this->state = ES_SPAWN_ANIM;
		}
		if (state == "ES_STANDING") {
			this->state = ES_STANDING;
		}
	}
	ENEMYSTATE getState() {
		return state;
	}
	void takeDamage(int damage) {
		shape.setRadius(shape.getRadius() - damage);
		shape.setOrigin(Vector2f(shape.getRadius(), shape.getRadius()));
	}
	bool isAlive() {
		return shape.getRadius() > minRadius;
	}
	void die() {
		state = ES_UNSPAWNED;
	}
};

///////////////
//ENEMY CLASSES
///////////////
class RomaEnemy : public Enemy{
public:
	int spawnCoordX;
	int destinationCoordY;
	vector <RectangleShape> bullets;
	Texture* bulletTxtrPtr;
	void generateDestinationY(GameWindow *gwindow) {
		destinationCoordY = rand() % static_cast<int>((gwindow->y - shape.getRadius() * 4)) + shape.getRadius() * 2;
	}
	void move(GameWindow *gwindow) {
		if (static_cast<int>((shape.getPosition().y)) != destinationCoordY) {
			if (shape.getPosition().y < destinationCoordY) {
				shape.move(0, 0.5);
			}
			else {
				shape.move(0, -0.5);
			}
		}
		else {
			generateDestinationY(gwindow);
		}
	}
	bool isNeedToShoot() {
		return getState() == ES_MOVING && fireClock.getElapsedTime().asMilliseconds() >= fireDelayAsMilliseconds;
	}
	void fire() {
		RectangleShape bullet;
		bullet.setTexture(bulletTxtrPtr);
		bullet.setSize(Vector2f(40, 22));
		bullet.setPosition(Vector2f(shape.getPosition()));
		bullets.push_back(bullet);
		fireClock.restart();
	}
	void moveBullets(Vector2f *bulletSpeed) {
		for (int i = 0; i < bullets.size(); i++) {
			bullets[i].move(*bulletSpeed);
			if (bullets[i].getPosition().x + bullets[i].getSize().x < 0) {
				bullets.erase(bullets.begin() + i);
			}
		}
	}
	void spawnAnimation() {
		if (shape.getPosition().x != spawnCoordX) {
			shape.move(-1, 0);
		}
		else {
			setState("ES_MOVING");
			fireClock.restart();
		}
	}
};
class RockEnemy : public Enemy {
public:
	ROCKENEMYSIDE side;
	int destinationCoordY;					//defining when spawnRockEnemy() invoking
	vector <RockEnemyBullet> bullets;
	Texture* bulletTxtrPtr;
	Vector2f defaultBulletSpeed;
	float bulletSpeedVariation;
	RectangleShape gunsight;
	bool isNeedToShoot() {
		return getState() == ES_MOVING && fireClock.getElapsedTime().asMilliseconds() >= fireDelayAsMilliseconds || getState() == ES_STANDING && fireClock.getElapsedTime().asMilliseconds() >= fireDelayAsMilliseconds;
	}
	void fire() {
		RockEnemyBullet bullet;
		bullet.shape.setTexture(bulletTxtrPtr);
		bullet.shape.setRadius(15);
		bullet.shape.setPosition(Vector2f(shape.getPosition()));
		bullet.shape.setOrigin(bullet.shape.getRadius(), bullet.shape.getRadius());
		bullet.speed = defaultBulletSpeed;
		bullets.push_back(bullet);
		fireClock.restart();
	}
	void takeTarget(Vector2f coords) {
		while (!gunsight.getGlobalBounds().contains(coords)) {
			if (gunsight.getRotation() <= 270 && gunsight.getRotation() >= 180) {
				defaultBulletSpeed.x += bulletSpeedVariation;
			}
			else if(gunsight.getRotation() <= 359){
				defaultBulletSpeed.x -= bulletSpeedVariation;
			}
			shape.rotate(1);
			gunsight.rotate(1);
		}
	}
	void moveBullets(GameWindow *gwindow) {
		for (int i = 0; i < bullets.size(); i++) {
			bullets[i].shape.move(bullets[i].speed);
			if (bullets[i].shape.getPosition().x + bullets[i].shape.getRadius() < 0 ||
				bullets[i].shape.getPosition().y + bullets[i].shape.getRadius() < 0 ||
				bullets[i].shape.getPosition().y + bullets[i].shape.getRadius() > gwindow->y) {
				bullets.erase(bullets.begin() + i);
			}
		}
	}
	void move() {
		switch (side) {
		case S_UP:
			if (static_cast<int>(shape.getPosition().y) != destinationCoordY) {
				shape.move(-0.5, 1);
				gunsight.move(-0.5, 1);
			}
			else {
				setState("ES_STANDING");
				fireClock.restart();
			}
			break;
		case S_DOWN:
			if (static_cast<int>(shape.getPosition().y) != destinationCoordY) {
				shape.move(-0.5, -1);
				gunsight.move(-0.5, -1);
			}
			else {
				setState("ES_STANDING");
				fireClock.restart();
			}
			break;
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
	RockEnemiesData rockEnemiesData;
public:
	GameWindow gameWindow;
	Menu menu;
	Event event;
	GAMESTATE gameState;
	vector <RomaEnemy> romaEnemies;
	vector <RockEnemy> rockEnemies;
	Game() {
		initWindow();
	}
	void debugging() {
		cout << rockEnemies[0].shape.getRotation() << "\n";
		//cout << endl << player.ammo.ordinaryBullets.size() << setw(5) << player.ammo.splittingBullets.size() << setw(5) << player.ammo.splittedBullets.size();
		//cout << endl << setw(10) << player.playerShape.getRotation();
		//cout << endl << romaEnemies[0].destinationY << setw(10) << romaEnemies[0].shape.getPosition().y;
	}
	void initWindow() {
		gameWindow.x = 1280;
		gameWindow.y = 720;
		gameWindow.title = "BRUH Galactic";
		gameWindow.window.create(VideoMode(gameWindow.x, gameWindow.y), gameWindow.title);
		gameWindow.window.setFramerateLimit(240);
	}
	void initGameBackground() {
		gameBackgroundTexture.loadFromFile("Textures\\background.png");
		gameBackground.setTexture(&gameBackgroundTexture);
		gameBackground.setSize(Vector2f(gameWindow.x, gameWindow.y));
	}
	void initEnemies() {
		//INITIALIZATION ROMA ENEMY
		romaEnemiesData.areActive = true;
		romaEnemiesData.enemyTexture.loadFromFile("Textures\\RomaEnemy.jpg");
		romaEnemiesData.bulletTexture.loadFromFile("Textures\\romaBulletTexture.jpg");
		romaEnemiesData.bulletSpeed = { -1, 0 };
		romaEnemiesData.defaultRadius = 40;
		romaEnemies.clear();
		romaEnemies.resize(5);
		for (int i = 0; i < romaEnemies.size(); i++) {
			romaEnemies[i].shape.setTexture(&romaEnemiesData.enemyTexture);
			romaEnemies[i].bulletTxtrPtr = &romaEnemiesData.bulletTexture;
			romaEnemies[i].fireDelayAsMilliseconds = 1000;
			romaEnemies[i].minRadius = 10;
			romaEnemies[i].spawnCoordX = gameWindow.x - romaEnemiesData.defaultRadius;
			romaEnemies[i].generateDestinationY(&gameWindow);									//
			romaEnemies[i].setState("ES_UNSPAWNED");
		}
		//INITIALIZATION ROCK ENEMY
		rockEnemiesData.areActive = true;
		rockEnemiesData.enemyTexture.loadFromFile("Textures\\rockEnemy.png");
		rockEnemiesData.bulletTexture.loadFromFile("Textures\\rockEnemyBulletTexture.png");
		rockEnemiesData.defaultRadius = 30;
		rockEnemies.clear();
		rockEnemies.resize(10);
		for (int i = 0; i < rockEnemies.size(); i++) {
			rockEnemies[i].shape.setTexture(&rockEnemiesData.enemyTexture);
			rockEnemies[i].bulletTxtrPtr = &rockEnemiesData.bulletTexture;
			rockEnemies[i].fireDelayAsMilliseconds = 500;
			rockEnemies[i].minRadius = 7;
			rockEnemies[i].shape.setRotation(0);
			rockEnemies[i].gunsight.setSize(Vector2f(gameWindow.x, 1));
			rockEnemies[i].gunsight.setOrigin(Vector2f(gameWindow.x, 0.5));
			rockEnemies[i].defaultBulletSpeed = { -1, 0 };
			rockEnemies[i].bulletSpeedVariation = rockEnemies[i].defaultBulletSpeed.x / 90;
			rockEnemies[i].setState("ES_UNSPAWNED");
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
				if (romaEnemies[j].getState() != ES_UNSPAWNED) {
					if (player.ammo.ordinaryBullets[i].shape.getGlobalBounds().intersects(romaEnemies[j].shape.getGlobalBounds())) {
						player.deleteBullet(BULT_ORDINARY, i);
						romaEnemies[j].takeDamage(player.ammo.ordinaryBulletData.damage);
						if (!romaEnemies[j].isAlive()) {
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
				if (romaEnemies[j].getState() != ES_UNSPAWNED) {
					if (player.ammo.splittingBullets[i].shape.getGlobalBounds().intersects(romaEnemies[j].shape.getGlobalBounds())) {
						romaEnemies[j].takeDamage(player.ammo.splittingBullets[i].damage);
						player.splitBullet(&player.ammo.splittingBullets[i]);
						player.deleteBullet(BULT_SPLITTING, i);
						if (!romaEnemies[j].isAlive()) {
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
				if (romaEnemies[j].getState() != ES_UNSPAWNED) {
					if (player.ammo.splittedBullets[i].shape.getGlobalBounds().intersects(romaEnemies[j].shape.getGlobalBounds())) {
						romaEnemies[j].takeDamage(player.ammo.splittedBullets[i].damage);
						player.splitBullet(&player.ammo.splittedBullets[i]);
						player.deleteBullet(BULT_SPLITTED, i);
						if (!romaEnemies[j].isAlive()) {
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
		return (romaEnemiesData.areActive == true && rand() % 300 == 1);
	}
	void spawnRomaEnemy() {
		for (int i = 0; i < romaEnemies.size(); i++) {
			if (romaEnemies[i].getState() == ES_UNSPAWNED) {
				romaEnemies[i].shape.setRadius(romaEnemiesData.defaultRadius);
				romaEnemies[i].shape.setOrigin(Vector2f(romaEnemiesData.defaultRadius, romaEnemiesData.defaultRadius));
				romaEnemies[i].shape.setPosition(Vector2f(gameWindow.x + romaEnemiesData.defaultRadius, rand() % (gameWindow.y - romaEnemiesData.defaultRadius * 4) + romaEnemiesData.defaultRadius * 2));
				romaEnemies[i].setState("ES_SPAWN_ANIM");
				return;
			}
		}
	}
	bool isRockEnemyNeedToSpawn() {
		return (rockEnemiesData.areActive == true && rand() % 300 == 1);
	}
	void spawnRockEnemy() {
		for (int i = 0; i < rockEnemies.size(); i++) {
			if (rockEnemies[i].getState() == ES_UNSPAWNED) {
				rockEnemies[i].shape.setRadius(rockEnemiesData.defaultRadius);
				rockEnemies[i].shape.setOrigin(rockEnemiesData.defaultRadius, rockEnemiesData.defaultRadius);
				rockEnemies[i].side = ROCKENEMYSIDE(rand() % 2);
				switch (rockEnemies[i].side) {
				case S_UP:
					rockEnemies[i].shape.setPosition(Vector2f(rand() % (gameWindow.x / 5) + gameWindow.x / 5 * 4, 0 - rockEnemies[i].shape.getRadius()));
					rockEnemies[i].destinationCoordY = gameWindow.y - rockEnemies[i].shape.getRadius();
					break;
				case S_DOWN:
					rockEnemies[i].shape.setPosition(Vector2f(rand() % (gameWindow.x / 5) + gameWindow.x / 5 * 4, gameWindow.y + rockEnemies[i].shape.getRadius()));
					rockEnemies[i].destinationCoordY = 0 + rockEnemies[i].shape.getRadius();
					break;
				}
				rockEnemies[i].gunsight.setPosition(rockEnemies[i].shape.getPosition());
				rockEnemies[i].setState("ES_MOVING");
				return;
			}
		}
	}
	void checkForEnemiesSpawn() {
		if (isRomaEnemyNeedToSpawn()) {
			spawnRomaEnemy();
		}
		if (isRockEnemyNeedToSpawn()) {
			spawnRockEnemy();
		}
	}

	void updatePlayer() {
		if (player.getHPAmount() <= 0) {
			gameState = GS_MENU;
		}
		player.controlPlayer(&gameWindow);
		player.checkForBulletSwap();
	}
	void updatePlayerBullets() {
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
	}
	void updateEnemies() {
		checkForEnemiesSpawn();
		for (int i = 0; i < romaEnemies.size(); i++) {
			if (romaEnemies[i].isNeedToShoot()) {
				romaEnemies[i].fire();
			}
			if (romaEnemies[i].getState() == ES_SPAWN_ANIM) {
				romaEnemies[i].spawnAnimation();
			}
			if (romaEnemies[i].getState() == ES_MOVING) {
				romaEnemies[i].move(&gameWindow);
			}
			romaEnemies[i].moveBullets(&romaEnemiesData.bulletSpeed);
		}
		for (int i = 0; i < rockEnemies.size(); i++) {
			rockEnemies[i].takeTarget(player.playerShape.getPosition());
			if (rockEnemies[i].isNeedToShoot()) {
				rockEnemies[i].fire();
			}
			if (rockEnemies[i].getState() == ES_MOVING) {
				rockEnemies[i].move();
			}
			rockEnemies[i].moveBullets(&gameWindow);
		}
	}
	void drawNewFrame() {
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

		gameWindow.window.draw(player.scope);
		gameWindow.window.draw(player.playerShape);
		for (int i = 0; i < romaEnemies.size(); i++) {
			for (int j = 0; j < romaEnemies[i].bullets.size(); j++) {
				gameWindow.window.draw(romaEnemies[i].bullets[j]);
			}
			if (romaEnemies[i].getState() != ES_UNSPAWNED) {
				gameWindow.window.draw(romaEnemies[i].shape);
			}
		}
		for (int i = 0; i < rockEnemies.size(); i++) {
			for (int j = 0; j < rockEnemies[i].bullets.size(); j++) {
				gameWindow.window.draw(rockEnemies[i].bullets[j].shape);
			}
			if (rockEnemies[i].getState() != ES_UNSPAWNED) {
				gameWindow.window.draw(rockEnemies[i].shape);
			}
		}
		gameWindow.window.display();
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

		debugging();

		updatePlayer();
		updatePlayerBullets();
		updateEnemies();
		checkForPlayerCollisions();
		checkForBulletsCollisions();
		drawNewFrame();
	}
};
int main() {
	srand(static_cast<unsigned int>(time(0)));
	rand();
	Game BG;
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