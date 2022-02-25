#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <string>
#include <cmath>
#include <iomanip>
#include <ctime>
using namespace sf;
using namespace std;

enum GAMESTATE { GS_PAUSE, GS_PLAY, GS_EXIT, GS_MENU };
enum ENEMYSTATE { ES_MOVING, ES_SPAWN_ANIM, ES_STANDING };
enum ENEMYTYPE { ET_ROMA, ET_ROCK, ET_ELECTRO };
enum BULLETTYPE { BULT_ORDINARY, BULT_SPLITTING, BULT_SPLITTED, BULT_RAY};
enum ROCKENEMYSIDE {S_UP, S_DOWN};
enum RAYBULLETSTATE {BS_FIRING, BS_DISAPPEARING, BS_DELETE};

struct GameWindow {
	RenderWindow window;
	unsigned short int x;
	unsigned short int y;
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
	int spawnRadius;
	unsigned int maxAmount;
};
struct RockEnemiesData {
	bool areActive;
	Texture enemyTexture;
	Texture bulletTexture;
	int spawnRadius;
	unsigned int maxAmount;
};
struct ElectroEnemiesData {
	bool areActive;
	Texture enemyTexture;
	Texture lightningTexture;
	int spawnRadius;
	unsigned int maxAmount;
	int visibleDelayAsMilliseconds;
	int visible_lightningDelayAsMilliseconds;
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


/////////////
//PLAYER DATA
/////////////
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
struct RayBulletData {
	int damage;
	Clock fireDelay;
	Texture texture;
	int delayBeforeDissapearAsMilliseconds;
};
////////////////
//PLAYER BULLETS
////////////////
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
class RayBullet {
public:
	RAYBULLETSTATE state;
	RectangleShape shape;
	Clock delayBeforeDissapear;
	void fire() {
		if (shape.getSize().x < 1920) {
			shape.setSize(Vector2f(shape.getSize().x + 100, shape.getSize().y));
		}
		else {
			state = BS_DISAPPEARING;
			delayBeforeDissapear.restart();
		}
	}
	void dissapear() {
		if (shape.getFillColor().a > 0) {
			shape.setFillColor(Color(shape.getFillColor().r, shape.getFillColor().g, shape.getFillColor().b, shape.getFillColor().a - 1));
		}
		else {
			state = BS_DELETE;
		}
	}
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

public:
	RectangleShape playerShape;
	RectangleShape scope;
	Ammunition ammo;
	AmmoData ammoData;

	void initPlayer(GameWindow *gwindow) {
		playertexture3HP.loadFromFile("Textures\\playerTexture3HP.jpg");
		playertexture2HP.loadFromFile("Textures\\playerTexture2HP.jpg");
		playertexture1HP.loadFromFile("Textures\\playerTexture1HP.jpg");
		ordinaryBulletScopeTexture.loadFromFile("Textures\\ordinaryBulletScopeTexture.png");
		splittingBulletScopeTexture.loadFromFile("Textures\\splittingBulletScopeTexture.png");
		rayBulletScopeTexture.loadFromFile("Textures\\rayBulletScopeTexture.png");
		sizeX = 90;
		sizeY = 50;
		playerShape.setSize(Vector2f(sizeX, sizeY));
		playerShape.setOrigin(Vector2f(sizeX / 2, sizeY / 2));
		playerShape.setPosition(Vector2f(gwindow->x / 100 * 5, gwindow->y / 2));
		setHPAmount();
		scope.setSize(Vector2f(70, 26));
		scope.setOrigin(Vector2f(0, scope.getSize().y / 2));
		scope.setPosition(Vector2f(playerShape.getPosition().x, playerShape.getPosition().y));
		scope.setTextureRect(IntRect(0, 0, 70, 26));
		scope.setTexture(&ordinaryBulletScopeTexture);
	}
	
	void initAmmunition() {
		selectedBullet = BULT_ORDINARY;
		ammoData.ordinaryBulletData.texture.loadFromFile("Textures\\pchel.jpg");
		ammoData.ordinaryBulletData.damage = 10;
		ammoData.ordinaryBulletData.defaultSpeed = { 10, 0 };
		ammoData.ordinaryBulletData.speedVariation = ammoData.ordinaryBulletData.defaultSpeed.x / 90;

		ammoData.splittingBulletData.texture.loadFromFile("Textures\\splittingBulletTexture.png");
		ammoData.splittingBulletData.defaultDamage = 30;
		ammoData.splittingBulletData.defaultRadius = 25;
		ammoData.splittingBulletData.defaultSpeed = { 6, 0 };
		ammoData.splittingBulletData.speedVariation = ammoData.splittingBulletData.defaultSpeed.x / 90;

		ammoData.rayBulletData.texture.loadFromFile("Textures\\rayBulletTexture.png");
		ammoData.rayBulletData.texture.setSmooth(true);
		ammoData.rayBulletData.damage = 120;
		ammoData.rayBulletData.delayBeforeDissapearAsMilliseconds = 100;

		ammo.ordinaryBullets.clear();
		ammo.splittingBullets.clear();
		ammo.splittedBullets.clear();
		ammo.rayBullets.clear();
	}
	
	void setHPAmount(int requiredAmount = 3) {
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

	BULLETTYPE getSelectedBulletType() {
		return selectedBullet;
	}
	
	void rotateToMouse(Vector2f coords) {
		playerShape.setRotation((atan2(playerShape.getPosition().y - coords.y, playerShape.getPosition().x - coords.x)) * 180 / 3.14159265 - 90);
		scope.setRotation(playerShape.getRotation() - 90);
	}
	
	void controlPlayer(GameWindow *gwindow) {
		if (Mouse::isButtonPressed(Mouse::Button::Left)) {
			fire();
		}
		if (Keyboard::isKeyPressed(Keyboard::Key::W) && playerShape.getPosition().y - sizeX / 2 > 0) {
			move({ 0, -1.7 });
		}
		if (Keyboard::isKeyPressed(Keyboard::Key::S) && playerShape.getPosition().y + sizeX / 2 < gwindow->y) {
			move({ 0, 1.7 });
		}
	}
	
	void move(Vector2f offset) {
		playerShape.move(offset);
		scope.move(offset);
	}
	
	void rotateGun() {
		float rotation = playerShape.getRotation();
		switch (getSelectedBulletType()) {
		case BULT_ORDINARY:
			if (rotation >= 0 && rotation <= 90) {
				ammoData.ordinaryBulletData.defaultSpeed.x = rotation * ammoData.ordinaryBulletData.speedVariation;
				ammoData.ordinaryBulletData.defaultSpeed.y = (90 - rotation) * -ammoData.ordinaryBulletData.speedVariation;
			}
			if (rotation > 90 && rotation <= 180) {
				ammoData.ordinaryBulletData.defaultSpeed.x = (180 - rotation) * ammoData.ordinaryBulletData.speedVariation;
				ammoData.ordinaryBulletData.defaultSpeed.y = (rotation - 90) * ammoData.ordinaryBulletData.speedVariation;
			}
			if (rotation > 180 && rotation <= 270) {
				ammoData.ordinaryBulletData.defaultSpeed.x = (rotation - 180) * -ammoData.ordinaryBulletData.speedVariation;
				ammoData.ordinaryBulletData.defaultSpeed.y = (270 - rotation) * ammoData.ordinaryBulletData.speedVariation;
			}
			if (rotation > 270 && rotation < 360) {
				ammoData.ordinaryBulletData.defaultSpeed.x = (360 - rotation) * -ammoData.ordinaryBulletData.speedVariation;
				ammoData.ordinaryBulletData.defaultSpeed.y = (rotation - 270) * -ammoData.ordinaryBulletData.speedVariation;
			}
			break;
		case BULT_SPLITTING:
			if (rotation >= 0 && rotation <= 90) {
				ammoData.splittingBulletData.defaultSpeed.x = rotation * ammoData.splittingBulletData.speedVariation;
				ammoData.splittingBulletData.defaultSpeed.y = (90 - rotation) * -ammoData.splittingBulletData.speedVariation;
			}
			if (rotation > 90 && rotation <= 180) {
				ammoData.splittingBulletData.defaultSpeed.x = (180 - rotation) * ammoData.splittingBulletData.speedVariation;
				ammoData.splittingBulletData.defaultSpeed.y = (rotation - 90) * ammoData.splittingBulletData.speedVariation;
			}
			if (rotation > 180 && rotation <= 270) {
				ammoData.splittingBulletData.defaultSpeed.x = (rotation - 180) * -ammoData.splittingBulletData.speedVariation;
				ammoData.splittingBulletData.defaultSpeed.y = (270 - rotation) * ammoData.splittingBulletData.speedVariation;
			}
			if (rotation > 270 && rotation < 360) {
				ammoData.splittingBulletData.defaultSpeed.x = (360 - rotation) * -ammoData.splittingBulletData.speedVariation;
				ammoData.splittingBulletData.defaultSpeed.y = (rotation - 270) * -ammoData.splittingBulletData.speedVariation;
			}
			break;
		}
	}
	
	void fire() {
		rotateGun();							//rotation only when player is shooting(for optimization)
		switch (selectedBullet) {
		case BULT_ORDINARY:
			if (ammoData.ordinaryBulletData.fireDelay.getElapsedTime().asMilliseconds() > 170 /*Delay between shots*/) {
				OrdinaryBullet bullet;
				bullet.shape.setSize(Vector2f(32.f, 19.f));
				bullet.shape.setTexture(&ammoData.ordinaryBulletData.texture);
				bullet.shape.setOrigin(Vector2f(bullet.shape.getSize().x / 2.f, bullet.shape.getSize().y / 2.f));
				bullet.shape.setPosition(Vector2f(playerShape.getPosition().x, (rand() % static_cast<int>(sizeX)) + playerShape.getPosition().y - playerShape.getOrigin().x));
				bullet.shape.setRotation(playerShape.getRotation() - 90);
				bullet.speed = ammoData.ordinaryBulletData.defaultSpeed;
				ammo.ordinaryBullets.push_back(bullet);
				ammoData.ordinaryBulletData.fireDelay.restart();
				return;
			}
			break;
		case BULT_SPLITTING:
			if (ammoData.splittingBulletData.fireDelay.getElapsedTime().asMilliseconds() > 550 /*Delay between shots*/) {
				SplittingBullet bullet;
				bullet.shape.setRadius(ammoData.splittingBulletData.defaultRadius);
				bullet.shape.setTexture(&ammoData.splittingBulletData.texture);
				bullet.shape.setOrigin(Vector2f(ammoData.splittingBulletData.defaultRadius, ammoData.splittingBulletData.defaultRadius));
				bullet.shape.setPosition(Vector2f(playerShape.getPosition().x, playerShape.getPosition().y));
				bullet.shape.setRotation(playerShape.getRotation());
				bullet.damage = ammoData.splittingBulletData.defaultDamage;
				bullet.speed = ammoData.splittingBulletData.defaultSpeed;
				ammo.splittingBullets.push_back(bullet);
				ammoData.splittingBulletData.fireDelay.restart();
				return;
			}
			break;
		case BULT_RAY:
			if (ammoData.rayBulletData.fireDelay.getElapsedTime().asMilliseconds() > 1200 /*Delay between shots*/) {
				RayBullet bullet;
				bullet.shape.setSize(Vector2f(0, 20));
				bullet.shape.setTexture(&ammoData.rayBulletData.texture);
				bullet.shape.setOrigin(Vector2f(0, bullet.shape.getSize().y / 2));
				bullet.shape.setRotation(playerShape.getRotation() - 90);
				bullet.shape.setPosition(Vector2f(playerShape.getPosition()));
				bullet.state = BS_FIRING;
				ammo.rayBullets.push_back(bullet);
				ammoData.rayBulletData.fireDelay.restart();
				return;
			}
			break;
		}
	}
	
	void splitBullet(SplittingBullet *splittingBullet) {
		SplittedBullet splittedBullet;
		splittedBullet.shape.setTexture(&ammoData.splittingBulletData.texture);
		splittedBullet.shape.setPosition(splittingBullet->shape.getPosition());
		splittedBullet.shape.setRadius(splittingBullet->shape.getRadius() / 1.5);
		splittedBullet.damage = splittingBullet->damage / 1.5;
		splittedBullet.levelOfSplit = 1;


		splittedBullet.speed = splittingBullet->speed;
		splittedBullet.shape.setRotation(splittingBullet->shape.getRotation());
		splittedBullet.shape.rotate(-13);
		float rotation = splittedBullet.shape.getRotation();
		if (rotation >= 0 && rotation <= 90) {
			splittedBullet.speed.x = rotation * ammoData.splittingBulletData.speedVariation;
			splittedBullet.speed.y = (90 - rotation) * -ammoData.splittingBulletData.speedVariation;
		}
		if (rotation > 90 && rotation <= 180) {
			splittedBullet.speed.x = (180 - rotation) * ammoData.splittingBulletData.speedVariation;
			splittedBullet.speed.y = (rotation - 90) * ammoData.splittingBulletData.speedVariation;
		}
		if (rotation > 180 && rotation <= 270) {
			splittedBullet.speed.x = (rotation - 180) * -ammoData.splittingBulletData.speedVariation;
			splittedBullet.speed.y = (270 - rotation) * ammoData.splittingBulletData.speedVariation;
		}
		if (rotation > 270 && rotation < 360) {
			splittedBullet.speed.x = (360 - rotation) * -ammoData.splittingBulletData.speedVariation;
			splittedBullet.speed.y = (rotation - 270) * -ammoData.splittingBulletData.speedVariation;
		}
		ammo.splittedBullets.push_back(splittedBullet);

		splittedBullet.speed = splittingBullet->speed;
		splittedBullet.shape.setRotation(splittingBullet->shape.getRotation());
		splittedBullet.shape.rotate(13);
		rotation = splittedBullet.shape.getRotation();
		if (rotation >= 0 && rotation <= 90) {
			splittedBullet.speed.x = rotation * ammoData.splittingBulletData.speedVariation;
			splittedBullet.speed.y = (90 - rotation) * -ammoData.splittingBulletData.speedVariation;
		}
		if (rotation > 90 && rotation <= 180) {
			splittedBullet.speed.x = (180 - rotation) * ammoData.splittingBulletData.speedVariation;
			splittedBullet.speed.y = (rotation - 90) * ammoData.splittingBulletData.speedVariation;
		}
		if (rotation > 180 && rotation <= 270) {
			splittedBullet.speed.x = (rotation - 180) * -ammoData.splittingBulletData.speedVariation;
			splittedBullet.speed.y = (270 - rotation) * ammoData.splittingBulletData.speedVariation;
		}
		if (rotation > 270 && rotation < 360) {
			splittedBullet.speed.x = (360 - rotation) * -ammoData.splittingBulletData.speedVariation;
			splittedBullet.speed.y = (rotation - 270) * -ammoData.splittingBulletData.speedVariation;
		}
		ammo.splittedBullets.push_back(splittedBullet);
	}
	void splitBullet(SplittedBullet *splittedBullet) {
		if (splittedBullet->levelOfSplit < 2) {
			SplittedBullet twiceSplittedBullet;
			twiceSplittedBullet.shape.setTexture(&ammoData.splittingBulletData.texture);
			twiceSplittedBullet.shape.setPosition(splittedBullet->shape.getPosition());
			twiceSplittedBullet.shape.setRadius(splittedBullet->shape.getRadius() / 1.5);
			twiceSplittedBullet.damage = splittedBullet->damage / 1.5;
			twiceSplittedBullet.levelOfSplit = splittedBullet->levelOfSplit + 1;


			twiceSplittedBullet.speed = splittedBullet->speed;
			twiceSplittedBullet.shape.setRotation(splittedBullet->shape.getRotation());
			twiceSplittedBullet.shape.rotate(-13);
			float rotation = twiceSplittedBullet.shape.getRotation();
			if (rotation >= 0 && rotation <= 90) {
				twiceSplittedBullet.speed.x = rotation * ammoData.splittingBulletData.speedVariation;
				twiceSplittedBullet.speed.y = (90 - rotation) * -ammoData.splittingBulletData.speedVariation;
			}
			if (rotation > 90 && rotation <= 180) {
				twiceSplittedBullet.speed.x = (180 - rotation) * ammoData.splittingBulletData.speedVariation;
				twiceSplittedBullet.speed.y = (rotation - 90) * ammoData.splittingBulletData.speedVariation;
			}
			if (rotation > 180 && rotation <= 270) {
				twiceSplittedBullet.speed.x = (rotation - 180) * -ammoData.splittingBulletData.speedVariation;
				twiceSplittedBullet.speed.y = (270 - rotation) * ammoData.splittingBulletData.speedVariation;
			}
			if (rotation > 270 && rotation < 360) {
				twiceSplittedBullet.speed.x = (360 - rotation) * -ammoData.splittingBulletData.speedVariation;
				twiceSplittedBullet.speed.y = (rotation - 270) * -ammoData.splittingBulletData.speedVariation;
			}
			ammo.splittedBullets.push_back(twiceSplittedBullet);

			twiceSplittedBullet.speed = splittedBullet->speed;
			twiceSplittedBullet.shape.setRotation(splittedBullet->shape.getRotation());
			twiceSplittedBullet.shape.rotate(13);
			rotation = twiceSplittedBullet.shape.getRotation();
			if (rotation >= 0 && rotation <= 90) {
				twiceSplittedBullet.speed.x = rotation * ammoData.splittingBulletData.speedVariation;
				twiceSplittedBullet.speed.y = (90 - rotation) * -ammoData.splittingBulletData.speedVariation;
			}
			if (rotation > 90 && rotation <= 180) {
				twiceSplittedBullet.speed.x = (180 - rotation) * ammoData.splittingBulletData.speedVariation;
				twiceSplittedBullet.speed.y = (rotation - 90) * ammoData.splittingBulletData.speedVariation;
			}
			if (rotation > 180 && rotation <= 270) {
				twiceSplittedBullet.speed.x = (rotation - 180) * -ammoData.splittingBulletData.speedVariation;
				twiceSplittedBullet.speed.y = (270 - rotation) * ammoData.splittingBulletData.speedVariation;
			}
			if (rotation > 270 && rotation < 360) {
				twiceSplittedBullet.speed.x = (360 - rotation) * -ammoData.splittingBulletData.speedVariation;
				twiceSplittedBullet.speed.y = (rotation - 270) * -ammoData.splittingBulletData.speedVariation;
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
		case BULT_RAY:
			ammo.rayBullets.erase(ammo.rayBullets.begin() + index);
			break;
		}
	}
	
	void checkForBulletSwap() {
		if (Keyboard::isKeyPressed(Keyboard::Num1)) {
			selectedBullet = BULT_ORDINARY;
			scope.setSize(Vector2f(70, 26));
			scope.setOrigin(Vector2f(0, scope.getSize().y / 2));
			scope.setPosition(Vector2f(playerShape.getPosition().x, playerShape.getPosition().y));
			scope.setTextureRect(IntRect(0, 0, 70, 26));
			scope.setTexture(&ordinaryBulletScopeTexture);
		}
		if (Keyboard::isKeyPressed(Keyboard::Num2)) {
			selectedBullet = BULT_SPLITTING;
			scope.setSize(Vector2f(70, 30));
			scope.setOrigin(Vector2f(0, scope.getSize().y / 2));
			scope.setPosition(Vector2f(playerShape.getPosition().x, playerShape.getPosition().y));
			scope.setTextureRect(IntRect(0, 0, 70, 30));
			scope.setTexture(&splittingBulletScopeTexture);
		}
		if (Keyboard::isKeyPressed(Keyboard::Num3)) {
			selectedBullet = BULT_RAY;
			scope.setSize(Vector2f(76, 30));
			scope.setOrigin(Vector2f(0, scope.getSize().y / 2));
			scope.setPosition(Vector2f(playerShape.getPosition().x, playerShape.getPosition().y));
			scope.setTextureRect(IntRect(0, 0, 76, 30));
			scope.setTexture(&rayBulletScopeTexture);
		}
	}
};


///////////////
//ENEMY CLASSES
///////////////
class CircleEnemy {
private:
	ENEMYSTATE state;
public:
	CircleEnemy() {
		shape.setOutlineColor(Color(139, 0, 0, 180));
	}
	CircleShape shape;
	Clock fireClock;
	int fireDelayAsMilliseconds;

	void setState(string state) {
		if (state == "ES_MOVING") {
			this->state = ES_MOVING;
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
		shape.setOutlineThickness(shape.getOutlineThickness() - damage);
	}

	bool isAlive() {
		return shape.getRadius() > abs(shape.getOutlineThickness());
	}

};
class RomaEnemy : public CircleEnemy {
public:
	float spawnCoordX;
	int destinationCoordY;
	Texture* bulletTxtrPtr;
	
	void generateDestinationY(GameWindow *gwindow) {
		destinationCoordY = rand() % static_cast<int>((gwindow->y - shape.getRadius() * 2)) + shape.getRadius();
		setState("ES_MOVING");
	}
	
	void move(GameWindow *gwindow) {
		if (static_cast<int>(shape.getPosition().y) != destinationCoordY) {
			if (shape.getPosition().y < destinationCoordY) {
				shape.move(0, 0.4);
			}
			else {
				shape.move(0, -0.4);
			}
		}
		else {
			setState("ES_STANDING");
		}
	}
	
	bool isNeedToFire() {
		return getState() == ES_MOVING && fireClock.getElapsedTime().asMilliseconds() >= fireDelayAsMilliseconds;
	}
	
	RectangleShape fire() {
		RectangleShape bullet;
		bullet.setTexture(bulletTxtrPtr);
		bullet.setSize(Vector2f(38, 20));
		bullet.setPosition(Vector2f(shape.getPosition()));
		fireClock.restart();
		return bullet;
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
class RockEnemy : public CircleEnemy {
public:
	ROCKENEMYSIDE side;
	float destinationCoordY;					//defining when spawnRockEnemy() invoking
	Texture* bulletTxtrPtr;
	Vector2f defaultBulletSpeed;
	float bulletSpeedVariation;
	
	bool isNeedToFire() {
		return getState() == ES_MOVING && fireClock.getElapsedTime().asMilliseconds() >= fireDelayAsMilliseconds || getState() == ES_STANDING && fireClock.getElapsedTime().asMilliseconds() >= fireDelayAsMilliseconds;
	}

	void rotateGun() {
		shape.rotate(-90);
		float rotation = shape.getRotation();
		if (rotation >= 0 && rotation <= 90) {
			defaultBulletSpeed.x = rotation * bulletSpeedVariation;
			defaultBulletSpeed.y = (90 - rotation) * -bulletSpeedVariation;
		}
		if (rotation > 90 && rotation <= 180) {
			defaultBulletSpeed.x = (180 - rotation) * bulletSpeedVariation;
			defaultBulletSpeed.y = (rotation - 90) * bulletSpeedVariation;
		}
		if (rotation > 180 && rotation <= 270) {
			defaultBulletSpeed.x = (rotation - 180) * -bulletSpeedVariation;
			defaultBulletSpeed.y = (270 - rotation) * bulletSpeedVariation;
		}
		if (rotation > 270 && rotation < 360) {
			defaultBulletSpeed.x = (360 - rotation) * -bulletSpeedVariation;
			defaultBulletSpeed.y = (rotation - 270) * -bulletSpeedVariation;
		}
	}
	
	RockEnemyBullet fire() {
		RockEnemyBullet bullet;
		bullet.shape.setTexture(bulletTxtrPtr);
		bullet.shape.setRadius(15);
		bullet.shape.setPosition(Vector2f(shape.getPosition()));
		bullet.shape.setOrigin(bullet.shape.getRadius(), bullet.shape.getRadius());
		rotateGun();
		bullet.speed = defaultBulletSpeed;
		fireClock.restart();
		return bullet;
	}
	
	void takeTarget(Vector2f coords) {
		shape.setRotation((atan2(shape.getPosition().y - coords.y, shape.getPosition().x - coords.x)) * 180 / 3.14159265);
	}
	
	void move() {
		switch (side) {
		case S_UP:
			if (shape.getPosition().y < destinationCoordY) {
				shape.move(-0.2, 0.9);
			}
			else {
				setState("ES_STANDING");
				fireDelayAsMilliseconds = 2700;
				fireClock.restart();
			}
			break;
		case S_DOWN:
			if (shape.getPosition().y > destinationCoordY) {
				shape.move(-0.2, -0.9);
			}
			else {
				setState("ES_STANDING");
				fireDelayAsMilliseconds = 2700;
				fireClock.restart();
			}
			break;
		}
	}
};
class ElectroEnemy : public CircleEnemy {
public:
	ElectroEnemy() {
		visible = true;
	}
	Texture* lightningTxtrPtr;
	float destinationCoordX;
	Clock visibleClock;
	bool visible;

	void toggleVisible() {
		visible = !visible;
		visibleClock.restart();
	}

	bool isNeedToFire() {
		return getState() == ES_STANDING && fireClock.getElapsedTime().asMilliseconds() >= fireDelayAsMilliseconds;
	}

	void moveRandomCoordY(GameWindow *gwindow) {
		shape.setPosition(Vector2f(shape.getPosition().x, rand() % static_cast<int>((gwindow->y - shape.getRadius() * 4)) + shape.getRadius() * 2));
	}

	ElectroEnemyLightning fire(Vector2f coords) {
		ElectroEnemyLightning lightning;
		lightning.shape.setPosition(shape.getPosition());
		lightning.shape.setSize(Vector2f(sqrt(pow(shape.getPosition().x - coords.x, 2) + pow(abs(shape.getPosition().y - coords.y), 2)), shape.getRadius()));
		lightning.shape.setOrigin(lightning.shape.getSize().x, lightning.shape.getSize().y / 2);
		lightning.shape.setRotation((atan2(lightning.shape.getPosition().y - coords.y, lightning.shape.getPosition().x - coords.x)) * 180 / 3.14159265);
		lightning.shape.setTexture(lightningTxtrPtr);
		fireClock.restart();
		lightning.visible_lightningClock.restart();
		return lightning;
	}

	void takeTarget(Vector2f coords) {
		shape.setRotation((atan2(shape.getPosition().y - coords.y, shape.getPosition().x - coords.x)) * 180 / 3.14159265);
		shape.rotate(-90);
	}

	void move() {
		if (shape.getPosition().x >= destinationCoordX) {
			shape.move(-0.9, 0);
		}
		else {
			setState("ES_STANDING");
			visible = true;
			fireClock.restart();
		}
	}
};


class Game {
private:
	Player player;
	RomaEnemiesData romaData;
	RockEnemiesData rockData;
	ElectroEnemiesData electroData;
	Texture gameBackgroundTexture;
	RectangleShape gameBackground;
	Clock delayBetweenEscapePresses;
public:
	GameWindow gameWindow;
	Menu menu;
	Event event;
	GAMESTATE gameState;
	//enemies
	vector <RomaEnemy> romaEnemies;
	vector <RockEnemy> rockEnemies;
	vector <ElectroEnemy> electroEnemies;
	//enemy bullets
	vector <RectangleShape> romaBullets;
	vector <RockEnemyBullet> rockBullets;
	vector <ElectroEnemyLightning> electroLightnings;
	Game() {
		initWindow();
	}
	void debugging() {
		//cout << rockEnemies[0].shape.getRotation() << "\n";
		//cout << endl << romaEnemies[0].destinationY << setw(10) << romaEnemies[0].shape.getPosition().y;
		//cout << "\n" << player.playerShape.getRotation();
		/*if (!rockBullets.empty()) {
			cout << endl << "speed.x : " << rockBullets[0].speed.x << "   " << "speed.y : " << rockBullets[0].speed.y;
		}*/
		/*if (!rockEnemies.empty()) {
			cout << endl << rockEnemies[0].shape.getRotation();
		}*/
		//cout << endl << rockBullets.size();
		//cout << endl << electroLightnings.size();
		/*if (romaEnemies.size() > 0) {
			cout << endl << romaEnemies[0].shape.getOutlineThickness();
		}*/
	}

	void initWindow() {
		gameWindow.x = 1920;
		gameWindow.y = 1080;
		gameWindow.title = "BRUH Galactic";
		gameWindow.window.create(VideoMode(gameWindow.x, gameWindow.y), gameWindow.title, Style::Fullscreen);
		gameWindow.window.setFramerateLimit(240);
	}

	void initGameBackground() {
		gameBackgroundTexture.loadFromFile("Textures\\background.png");
		gameBackground.setTexture(&gameBackgroundTexture);
		gameBackground.setSize(Vector2f(gameWindow.x, gameWindow.y));
	}

	void initEnemies() {
		//INITIALIZATION ROMA ENEMY
		romaData.areActive = true;
		romaData.maxAmount = 7;
		romaData.enemyTexture.loadFromFile("Textures\\RomaEnemy.jpg");
		romaData.bulletTexture.loadFromFile("Textures\\romaBulletTexture.jpg");
		romaData.bulletSpeed = { -3.5, 0 };
		romaData.spawnRadius = 40;
		romaEnemies.clear();
		romaBullets.clear();

		//INITIALIZATION ROCK ENEMY
		rockData.areActive = true;
		rockData.maxAmount = 7;
		rockData.enemyTexture.loadFromFile("Textures\\rockEnemy.png");
		rockData.bulletTexture.loadFromFile("Textures\\rockEnemyBulletTexture.png");
		rockData.spawnRadius = 45;
		rockEnemies.clear();
		rockBullets.clear();

		//INITIALIZATION ELECTRO ENEMY
		electroData.areActive = true;
		electroData.maxAmount = 3;
		electroData.enemyTexture.loadFromFile("Textures\\ElectroEnemy.jpg");
		electroData.lightningTexture.loadFromFile("Textures\\lightningTexture1.png");
		electroData.spawnRadius = 35;
		electroData.visibleDelayAsMilliseconds = 1000;
		electroData.visible_lightningDelayAsMilliseconds = 400;
		electroEnemies.clear();
		electroLightnings.clear();
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

	/////////////////////////////////////////////////					ДОДЕЛЫВАЙ, ИНАЧЕ НЕ ПОЕШЬ(надо оптимизацию коллизии кщё сделать потом)
	void checkForOrdinaryBulletsCollision() {
			//ROMA ENEMIES
		for (int i = 0; i < player.ammo.ordinaryBullets.size(); i++) {
			for (int j = 0; j < romaEnemies.size(); j++) {
				if (player.ammo.ordinaryBullets[i].shape.getGlobalBounds().intersects(romaEnemies[j].shape.getGlobalBounds())) {
					player.deleteBullet(BULT_ORDINARY, i);
					romaEnemies[j].takeDamage(player.ammoData.ordinaryBulletData.damage);
					break;
				}
			}
		}
			//ROMA BULLETS
		for (int i = 0; i < player.ammo.ordinaryBullets.size(); i++) {
			for (int j = 0; j < romaBullets.size(); j++) {
				if (player.ammo.ordinaryBullets[i].shape.getGlobalBounds().intersects(romaBullets[j].getGlobalBounds())) {
					player.deleteBullet(BULT_ORDINARY, i);
					romaBullets.erase(romaBullets.begin() + j);
					break;
				}
			}
		}



			//ROCK ENEMIES
		for (int i = 0; i < player.ammo.ordinaryBullets.size(); i++) {
			for (int j = 0; j < rockEnemies.size(); j++) {
				if (player.ammo.ordinaryBullets[i].shape.getGlobalBounds().intersects(rockEnemies[j].shape.getGlobalBounds())) {
					player.deleteBullet(BULT_ORDINARY, i);
					rockEnemies[j].takeDamage(player.ammoData.ordinaryBulletData.damage);
					break;
				}
			}
		}
			//ROCK BULLETS
		for (int i = 0; i < player.ammo.ordinaryBullets.size(); i++) {
			for (int j = 0; j < rockBullets.size(); j++) {
				if (player.ammo.ordinaryBullets[i].shape.getGlobalBounds().intersects(rockBullets[j].shape.getGlobalBounds())) {
					player.deleteBullet(BULT_ORDINARY, i);
					rockBullets.erase(rockBullets.begin() + j);
					break;
				}
			}
		}
			//ELECTRO ENEMIES
		for (int i = 0; i < player.ammo.ordinaryBullets.size(); i++) {
			for (int j = 0; j < electroEnemies.size(); j++) {
				if (electroEnemies[j].visible) {
					if (player.ammo.ordinaryBullets[i].shape.getGlobalBounds().intersects(electroEnemies[j].shape.getGlobalBounds())) {
						player.deleteBullet(BULT_ORDINARY, i);
						electroEnemies[j].takeDamage(player.ammoData.ordinaryBulletData.damage);
						break;
					}
				}
			}
		}
	}
	
	void checkForSplittingBulletsCollision() {
			//ROMA ENEMIES
		for (int i = 0; i < player.ammo.splittingBullets.size(); i++) {
			for (int j = 0; j < romaEnemies.size(); j++) {
				if (player.ammo.splittingBullets[i].shape.getGlobalBounds().intersects(romaEnemies[j].shape.getGlobalBounds())) {
					romaEnemies[j].takeDamage(player.ammo.splittingBullets[i].damage);
					player.splitBullet(&player.ammo.splittingBullets[i]);
					player.deleteBullet(BULT_SPLITTING, i);
					break;
				}
			}
		}
			//ROMA BULLETS
		for (int i = 0; i < player.ammo.splittingBullets.size(); i++) {
			for (int j = 0; j < romaBullets.size(); j++) {
				if (player.ammo.splittingBullets[i].shape.getGlobalBounds().intersects(romaBullets[j].getGlobalBounds())) {
					player.splitBullet(&player.ammo.splittingBullets[i]);
					player.deleteBullet(BULT_SPLITTING, i);
					romaBullets.erase(romaBullets.begin() + j);
					break;
				}
			}
		}



			//ROCK ENEMY
		for (int i = 0; i < player.ammo.splittingBullets.size(); i++) {
			for (int j = 0; j < rockEnemies.size(); j++) {
				if (player.ammo.splittingBullets[i].shape.getGlobalBounds().intersects(rockEnemies[j].shape.getGlobalBounds())) {
					rockEnemies[j].takeDamage(player.ammo.splittingBullets[i].damage);
					player.splitBullet(&player.ammo.splittingBullets[i]);
					player.deleteBullet(BULT_SPLITTING, i);
					break;
				}
			}
		}
			//ROCK BULLETS
		for (int i = 0; i < player.ammo.splittingBullets.size(); i++) {
			for (int j = 0; j < rockBullets.size(); j++) {
				if (player.ammo.splittingBullets[i].shape.getGlobalBounds().intersects(rockBullets[j].shape.getGlobalBounds())) {
					player.splitBullet(&player.ammo.splittingBullets[i]);
					player.deleteBullet(BULT_SPLITTING, i);
					rockBullets.erase(rockBullets.begin() + j);
					break;
				}
			}
		}
		//ELECTRO ENEMIES
		for (int i = 0; i < player.ammo.splittingBullets.size(); i++) {
			for (int j = 0; j < electroEnemies.size(); j++) {
				if (electroEnemies[j].visible) {
					if (player.ammo.splittingBullets[i].shape.getGlobalBounds().intersects(electroEnemies[j].shape.getGlobalBounds())) {
						electroEnemies[j].takeDamage(player.ammo.splittingBullets[i].damage);
						player.deleteBullet(BULT_SPLITTING, i);
						break;
					}
				}
			}
		}
	}
	
	void checkForSplittedBulletsCollision() {			
			//ROMA ENEMIES
		for (int i = 0; i < player.ammo.splittedBullets.size(); i++) {
			for (int j = 0; j < romaEnemies.size(); j++) {
				if (player.ammo.splittedBullets[i].shape.getGlobalBounds().intersects(romaEnemies[j].shape.getGlobalBounds())) {
					romaEnemies[j].takeDamage(player.ammo.splittedBullets[i].damage);
					player.splitBullet(&player.ammo.splittedBullets[i]);
					player.deleteBullet(BULT_SPLITTED, i);
					break;
				}
			}
		}
			//ROMA BULLETS
		for (int i = 0; i < player.ammo.splittedBullets.size(); i++) {
			for (int j = 0; j < romaBullets.size(); j++) {
				if (player.ammo.splittedBullets[i].shape.getGlobalBounds().intersects(romaBullets[j].getGlobalBounds())) {
					player.splitBullet(&player.ammo.splittedBullets[i]);
					player.deleteBullet(BULT_SPLITTED, i);
					romaBullets.erase(romaBullets.begin() + j);
					break;
				}
			}
		}



			//ROCK ENEMY
		for (int i = 0; i < player.ammo.splittedBullets.size(); i++) {
			for (int j = 0; j < rockEnemies.size(); j++) {
				if (player.ammo.splittedBullets[i].shape.getGlobalBounds().intersects(rockEnemies[j].shape.getGlobalBounds())) {
					rockEnemies[j].takeDamage(player.ammo.splittedBullets[i].damage);
					player.splitBullet(&player.ammo.splittedBullets[i]);
					player.deleteBullet(BULT_SPLITTED, i);
					break;
				}
			}
		}
			//ROCK BULLETS
		for (int i = 0; i < player.ammo.splittedBullets.size(); i++) {
			for (int j = 0; j < rockBullets.size(); j++) {
				if (player.ammo.splittedBullets[i].shape.getGlobalBounds().intersects(rockBullets[j].shape.getGlobalBounds())) {
					player.splitBullet(&player.ammo.splittedBullets[i]);
					player.deleteBullet(BULT_SPLITTED, i);
					rockBullets.erase(rockBullets.begin() + j);
					break;
				}
			}
		}
		//ELECTRO ENEMIES
		for (int i = 0; i < player.ammo.splittedBullets.size(); i++) {
			for (int j = 0; j < electroEnemies.size(); j++) {
				if (electroEnemies[j].visible) {
					if (player.ammo.splittedBullets[i].shape.getGlobalBounds().intersects(electroEnemies[j].shape.getGlobalBounds())) {
						electroEnemies[j].takeDamage(player.ammo.splittedBullets[i].damage);
						player.deleteBullet(BULT_SPLITTED, i);
						break;
					}
				}
			}
		}
	}

	void checkForRayBulletsCollision() {

	}

	void checkForBulletsCollisions() {
		checkForOrdinaryBulletsCollision();
		checkForSplittingBulletsCollision();
		checkForSplittedBulletsCollision();
		checkForRayBulletsCollision();
	}
	
	void checkForPlayerCollisions() {
		//ROMA BULLETS
		for (int i = 0; i < romaBullets.size(); i++) {
			if (player.playerShape.getGlobalBounds().intersects(romaBullets[i].getGlobalBounds())) {
				player.setHPAmount(player.getHPAmount() - 1);
				romaBullets.erase(romaBullets.begin() + i);
			}
		}
		//ROCK BULLETS
		for (int i = 0; i < rockBullets.size(); i++) {
			if (player.playerShape.getGlobalBounds().intersects(rockBullets[i].shape.getGlobalBounds())) {
				player.setHPAmount(player.getHPAmount() - 1);
				rockBullets.erase(rockBullets.begin() + i);
			}
		}
	}
	
	bool isRomaEnemyNeedToSpawn() {
		return (romaData.areActive == true && romaEnemies.size() < romaData.maxAmount && rand() % 700 == 1);
	}

	bool isRockEnemyNeedToSpawn() {
		return (rockData.areActive == true && rockEnemies.size() < rockData.maxAmount && rand() % 900 == 1);
	}

	bool isElectroEnemyNeedToSpawn() {
		return (electroData.areActive == true && electroEnemies.size() < electroData.maxAmount && rand() % 1600 == 1);
	}

	void spawnRomaEnemy() {
		RomaEnemy roma;
		roma.shape.setTexture(&romaData.enemyTexture);
		roma.bulletTxtrPtr = &romaData.bulletTexture;
		roma.fireDelayAsMilliseconds = 5300;
		roma.spawnCoordX = gameWindow.x - romaData.spawnRadius;
		roma.generateDestinationY(&gameWindow);
		roma.shape.setRadius(romaData.spawnRadius);
		roma.shape.setOrigin(Vector2f(romaData.spawnRadius, romaData.spawnRadius));
		roma.shape.setPosition(Vector2f(gameWindow.x + romaData.spawnRadius, rand() % (gameWindow.y - romaData.spawnRadius * 4) + romaData.spawnRadius * 2));
		roma.setState("ES_SPAWN_ANIM");
		romaEnemies.push_back(roma);
	}
	
	void spawnRockEnemy() {
		RockEnemy rock;
		rock.shape.setTexture(&rockData.enemyTexture);
		rock.bulletTxtrPtr = &rockData.bulletTexture;
		rock.fireDelayAsMilliseconds = 800;
		rock.shape.setRotation(0);
		rock.defaultBulletSpeed = { 2.2, 0 };
		rock.bulletSpeedVariation = rock.defaultBulletSpeed.x / 90;
		rock.shape.setRadius(rockData.spawnRadius);
		rock.shape.setOrigin(rockData.spawnRadius, rockData.spawnRadius);
		rock.side = ROCKENEMYSIDE(rand() % 2);
		switch (rock.side) {
		case S_UP:
			rock.shape.setPosition(Vector2f(rand() % (gameWindow.x / 2) + static_cast<float>(gameWindow.x) / 2, 0 - rock.shape.getRadius()));
			rock.destinationCoordY = gameWindow.y - rock.shape.getRadius();
			break;
		case S_DOWN:
			rock.shape.setPosition(Vector2f(rand() % (gameWindow.x / 2) + static_cast<float>(gameWindow.x) / 2, gameWindow.y + rock.shape.getRadius()));
			rock.destinationCoordY = 0 + rock.shape.getRadius();
			break;
		}
		rock.setState("ES_MOVING");
		rockEnemies.push_back(rock);
	}

	void spawnElectroEnemy() {
		ElectroEnemy electro;
		electro.shape.setTexture(&electroData.enemyTexture);
		electro.lightningTxtrPtr = &electroData.lightningTexture;
		electro.fireDelayAsMilliseconds = 1700;
		electro.shape.setRotation(270);
		electro.shape.setRadius(electroData.spawnRadius);
		electro.shape.setOrigin(electroData.spawnRadius, electroData.spawnRadius);
		electro.shape.setPosition(Vector2f(gameWindow.x + electro.shape.getRadius(), rand() % (gameWindow.y - electroData.spawnRadius * 4) + electroData.spawnRadius * 2));
		electro.destinationCoordX = gameWindow.x / 100 * 15;
		electroEnemies.push_back(electro);
	}
	
	void checkForEnemiesSpawn() {
		if (isRomaEnemyNeedToSpawn()) {
			spawnRomaEnemy();
		}
		if (isRockEnemyNeedToSpawn()) {
			spawnRockEnemy();
		}
		if (isElectroEnemyNeedToSpawn()) {
			spawnElectroEnemy();
		}
	}

	void updatePlayer() {
		if (player.getHPAmount() <= 0) {
			gameState = GS_MENU;
		}
		player.controlPlayer(&gameWindow);
		player.rotateToMouse(static_cast<Vector2f>(Mouse::getPosition(gameWindow.window)));
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
		for (int i = 0; i < player.ammo.rayBullets.size(); i++) {
			switch (player.ammo.rayBullets[i].state) {
			case BS_FIRING:
				player.ammo.rayBullets[i].fire();
				break;
			case BS_DISAPPEARING:
				if (player.ammo.rayBullets[i].delayBeforeDissapear.getElapsedTime().asMilliseconds() > player.ammoData.rayBulletData.delayBeforeDissapearAsMilliseconds) {
					player.ammo.rayBullets[i].dissapear();
				}
				break;
			case BS_DELETE:
				player.deleteBullet(BULT_RAY, i);
				i--;
				break;
			}
		}
	}
	void updateEnemies() {
		checkForEnemiesSpawn();
		//ROMA ENEMIES
		for (int i = 0; i < romaEnemies.size(); i++) {
			if (romaEnemies[i].isNeedToFire()) {
				romaBullets.push_back(romaEnemies[i].fire());
			}
			if (romaEnemies[i].getState() == ES_SPAWN_ANIM) {
				romaEnemies[i].spawnAnimation();
			}
			if (romaEnemies[i].getState() == ES_MOVING) {
				romaEnemies[i].move(&gameWindow);
			}
			if (romaEnemies[i].getState() == ES_STANDING) {
				romaEnemies[i].generateDestinationY(&gameWindow);
			}
			if (!romaEnemies[i].isAlive()) {
				romaEnemies.erase(romaEnemies.begin() + i);
			}
		}
		//ROCK ENEMIES
		for (int i = 0; i < rockEnemies.size(); i++) {
			if (rockEnemies[i].isNeedToFire()) {
				rockBullets.push_back(rockEnemies[i].fire());
			}
			if (rockEnemies[i].getState() == ES_MOVING) {
				rockEnemies[i].move();
			}
			rockEnemies[i].takeTarget(player.playerShape.getPosition());
			if (!rockEnemies[i].isAlive()) {
				rockEnemies.erase(rockEnemies.begin() + i);
			}
		}
		//ELECTRO EMEMIES
		for (int i = 0; i < electroEnemies.size(); i++) {
			if (electroEnemies[i].isNeedToFire()) {
				electroLightnings.push_back(electroEnemies[i].fire(player.playerShape.getPosition()));
				player.setHPAmount(player.getHPAmount() - 1);
			}
			if (electroEnemies[i].getState() == ES_MOVING) {
				electroEnemies[i].move();
				if (electroEnemies[i].visibleClock.getElapsedTime().asMilliseconds() >= electroData.visibleDelayAsMilliseconds) {
					electroEnemies[i].toggleVisible();
					electroEnemies[i].moveRandomCoordY(&gameWindow);
				}
			}
			if (electroEnemies[i].getState() == ES_STANDING) {
				electroEnemies[i].takeTarget(player.playerShape.getPosition());
			}
			if (!electroEnemies[i].isAlive()) {
				electroEnemies.erase(electroEnemies.begin() + i);
			}
		}
	}
	void updateEnemyBullets() {
		//ROMA BULLETS
		for (int i = 0; i < romaBullets.size(); i++) {
			romaBullets[i].move(romaData.bulletSpeed);
			if (romaBullets[i].getPosition().x + romaBullets[i].getSize().x < 0) {
				romaBullets.erase(romaBullets.begin() + i);
			}
		}
		//ROCK BULLETS
		for (int i = 0; i < rockBullets.size(); i++) {
			rockBullets[i].shape.move(rockBullets[i].speed);
			if (rockBullets[i].shape.getPosition().x + rockBullets[i].shape.getRadius() < 0 ||
				rockBullets[i].shape.getPosition().y + rockBullets[i].shape.getRadius() < 0 ||
				rockBullets[i].shape.getPosition().y + rockBullets[i].shape.getRadius() > gameWindow.y) {
				rockBullets.erase(rockBullets.begin() + i);
			}
		}
		//ELECTRO LIGHTNINGS
		for (int i = 0; i < electroLightnings.size(); i++) {
			if (electroLightnings[i].visible_lightningClock.getElapsedTime().asMilliseconds() >= electroData.visible_lightningDelayAsMilliseconds) {
				electroLightnings.erase(electroLightnings.begin() + i);
			}
		}
	}
	void drawNewFrame() {
		gameWindow.window.clear();
		gameWindow.window.draw(gameBackground);

		for (int i = 0; i < romaEnemies.size(); i++) {
			gameWindow.window.draw(romaEnemies[i].shape);
		}
		for (int i = 0; i < rockEnemies.size(); i++) {
			gameWindow.window.draw(rockEnemies[i].shape);
		}
		for (int i = 0; i < electroEnemies.size(); i++) {
			if (electroEnemies[i].visible) {
				gameWindow.window.draw(electroEnemies[i].shape);
			}
		}

		for (int i = 0; i < player.ammo.ordinaryBullets.size(); i++) {
			gameWindow.window.draw(player.ammo.ordinaryBullets[i].shape);
		}
		for (int i = 0; i < player.ammo.splittingBullets.size(); i++) {
			gameWindow.window.draw(player.ammo.splittingBullets[i].shape);
		}
		for (int i = 0; i < player.ammo.splittedBullets.size(); i++) {
			gameWindow.window.draw(player.ammo.splittedBullets[i].shape);
		}
		for (int i = 0; i < player.ammo.rayBullets.size(); i++) {
			gameWindow.window.draw(player.ammo.rayBullets[i].shape);
		}

		gameWindow.window.draw(player.scope);
		gameWindow.window.draw(player.playerShape);

		for (int i = 0; i < romaBullets.size(); i++) {
			gameWindow.window.draw(romaBullets[i]);
		}
		for (int i = 0; i < rockBullets.size(); i++) {
			gameWindow.window.draw(rockBullets[i].shape);
		}
		for (int i = 0; i < electroLightnings.size(); i++) {
			gameWindow.window.draw(electroLightnings[i].shape);
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

		updatePlayer();
		updatePlayerBullets();
		updateEnemies();
		updateEnemyBullets();
		checkForPlayerCollisions();
		checkForBulletsCollisions();
		drawNewFrame();

		debugging();


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