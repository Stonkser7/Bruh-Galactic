#include <SFML/Graphics.hpp>
#include <iostream>
#include <conio.h>
#include <vector>
#include <string>
#include <cmath>
using namespace sf;
using namespace std;

enum GAMESTATE { GS_PAUSE, GS_PLAY, GS_EXIT, GS_MENU };
enum ENEMYSTATE { ENEMY_SPAWNED, ENEMY_UNSPAWNED };
enum BULLETSTATE { BUL_FIRED, BUL_INCARTRIDGE };
enum BULLETTYPE { BUL_ORDINARY };

struct GameWindow {
	RenderWindow window;
	int x;
	int y;
	String title;
};
struct romaEnemiesData {
	bool areRomaEnemiesActive;
	Texture romaEnemyTexture;
	Texture romaBulletTexture;
	Vector2f romaBulletSpeed;
	int defaultRadius;
};
struct OrdinaryBullet {
	RectangleShape bulletShape;
	Vector2f ordinaryBulletSpeed;
	BULLETSTATE state;
};
struct Ammunition {
	int ordinaryBulletDamage;
	Vector2f ordinaryBulletDefaultSpeed;
	float ordinaryBulletSpeedVariation;
	Clock ordinaryFireDelay;
	Texture ordinaryBulletTexture;
	vector <OrdinaryBullet> ordinaryBullets;
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
public:
	RectangleShape playerShape;
	RectangleShape scope;
	Ammunition ammunition;

	void initPlayer(GameWindow *gwindow) {
		playertexture3HP.loadFromFile("Textures\\playerTexture3HP.jpg");
		playertexture2HP.loadFromFile("Textures\\playerTexture2HP.jpg");
		playertexture1HP.loadFromFile("Textures\\playerTexture1HP.jpg");
		ordinaryBulletScopeTexture.loadFromFile("Textures\\ordinaryBulletScopeTexture.png");
		sizeX = 75.f;
		sizeY = 40.f;
		playerShape.setOrigin(Vector2f(sizeX / 2, sizeY / 2));
		playerShape.setSize(Vector2f(sizeX, sizeY));
		playerShape.setRotation(90);
		playerShape.setPosition(Vector2f(60.f, gwindow->y / 2));
		setHP();
		scope.setSize(Vector2f(55, 7));
		scope.setPosition(Vector2f(playerShape.getPosition().x, playerShape.getPosition().y - scope.getSize().y / 2));
		scope.setTexture(&ordinaryBulletScopeTexture);
		scope.setRotation(playerShape.getRotation() - 90);
	}
	void initAmmunition() {
		selectedBullet = BUL_ORDINARY;
		ammunition.ordinaryBullets.resize(15);
		ammunition.ordinaryBulletTexture.loadFromFile("Textures\\pchel.jpg");
		ammunition.ordinaryBulletDefaultSpeed.x = 7; ammunition.ordinaryBulletDefaultSpeed.y = 0;
		ammunition.ordinaryBulletSpeedVariation = ((ammunition.ordinaryBulletDefaultSpeed.x / 100) * 50) / 45;
		for (int i = 0; i < ammunition.ordinaryBullets.size(); i++) {
			ammunition.ordinaryBullets[i].bulletShape.setTexture(&ammunition.ordinaryBulletTexture);
			ammunition.ordinaryBullets[i].bulletShape.setSize(Vector2f(30.f, 17.f));
			ammunition.ordinaryBulletDamage = 10;
			ammunition.ordinaryBullets[i].ordinaryBulletSpeed = ammunition.ordinaryBulletDefaultSpeed;
			ammunition.ordinaryBullets[i].state = BUL_INCARTRIDGE;
		}
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
			playerShape.rotate(-1);
			scope.rotate(-1);
			if (playerShape.getRotation() < 90) {
				ammunition.ordinaryBulletDefaultSpeed.x -= ammunition.ordinaryBulletSpeedVariation; ammunition.ordinaryBulletDefaultSpeed.y -= ammunition.ordinaryBulletSpeedVariation;
			}
			else {
				ammunition.ordinaryBulletDefaultSpeed.x += ammunition.ordinaryBulletSpeedVariation; ammunition.ordinaryBulletDefaultSpeed.y -= ammunition.ordinaryBulletSpeedVariation;
			}
		}
		if (Keyboard::isKeyPressed(Keyboard::Key::D) && playerShape.getRotation() < 135) {
			playerShape.rotate(1);
			scope.rotate(1);
			if (playerShape.getRotation() > 90) {
				ammunition.ordinaryBulletDefaultSpeed.x -= ammunition.ordinaryBulletSpeedVariation; ammunition.ordinaryBulletDefaultSpeed.y += ammunition.ordinaryBulletSpeedVariation;
			}
			else {
				ammunition.ordinaryBulletDefaultSpeed.x += ammunition.ordinaryBulletSpeedVariation; ammunition.ordinaryBulletDefaultSpeed.y += ammunition.ordinaryBulletSpeedVariation;
			}
		}
	}
	void move(float offsetX, float offsetY) {
		playerShape.move(offsetX, offsetY);
		scope.move(offsetX, offsetY);
	}
	void fire() {
		switch (selectedBullet) {
		case BUL_ORDINARY:
			for (int i = 0; i < ammunition.ordinaryBullets.size(); i++) {
				if (ammunition.ordinaryBullets[i].state == BUL_INCARTRIDGE && ammunition.ordinaryFireDelay.getElapsedTime().asMilliseconds() > 150 /*Delay between shots*/) {
					ammunition.ordinaryBullets[i].bulletShape.setRotation(playerShape.getRotation() - 90);
					ammunition.ordinaryBullets[i].bulletShape.setPosition(Vector2f(playerShape.getPosition().x, playerShape.getPosition().y - ammunition.ordinaryBullets[i].bulletShape.getSize().y / 2));
					ammunition.ordinaryBullets[i].ordinaryBulletSpeed.x = ammunition.ordinaryBulletDefaultSpeed.x; ammunition.ordinaryBullets[i].ordinaryBulletSpeed.y = ammunition.ordinaryBulletDefaultSpeed.y;
					ammunition.ordinaryBullets[i].state = BUL_FIRED;
					ammunition.ordinaryFireDelay.restart();
					return;
				}
			}
		}
	}
	void moveBullets(GameWindow *gwindow) {
		for (int i = 0; i < ammunition.ordinaryBullets.size(); i++) {
			if (ammunition.ordinaryBullets[i].state == BUL_FIRED) {
				ammunition.ordinaryBullets[i].bulletShape.move(Vector2f(ammunition.ordinaryBullets[i].ordinaryBulletSpeed));
			}
			if (ammunition.ordinaryBullets[i].bulletShape.getPosition().x >= gwindow->window.getSize().x ||
				ammunition.ordinaryBullets[i].bulletShape.getPosition().y >= gwindow->window.getSize().y + ammunition.ordinaryBullets[i].bulletShape.getSize().y ||
				ammunition.ordinaryBullets[i].bulletShape.getPosition().y <= 0 - ammunition.ordinaryBullets[i].bulletShape.getSize().y) {
				ammunition.ordinaryBullets[i].state = BUL_INCARTRIDGE;
			}
		}
	}
	void checkForBulletSwap() {
		if (Keyboard::isKeyPressed(Keyboard::Num1)) {
			selectedBullet = BUL_ORDINARY;
			scope.setSize(Vector2f(55, 7));
			scope.setPosition(Vector2f(playerShape.getPosition().x, playerShape.getPosition().y - scope.getSize().y / 2));
			scope.setTexture(&ordinaryBulletScopeTexture);
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
			state = ENEMY_SPAWNED;
		}
		if(requestedState == "ENEMY_UNSPAWNED") {
			state = ENEMY_UNSPAWNED;
		}
	}
	ENEMYSTATE getState() {
		return state;
	}
	void move() {
		if (state == ENEMY_SPAWNED) {
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
		if (state == ENEMY_SPAWNED && fireDelay.getElapsedTime().asMilliseconds() >= 2000) {
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
};
class Game {
private:
	Player player;
	Texture gameBackgroundTexture;
	RectangleShape gameBackground;
	Clock delayBetweenEscapePresses;
	romaEnemiesData romaEnemiesData;
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
		gameBackgroundTexture.loadFromFile("Textures\\background.jpeg");
		gameBackground.setTexture(&gameBackgroundTexture);
		gameBackground.setSize(Vector2f(gameWindow.x, gameWindow.y));
	}
	void initEnemies() {
		romaEnemiesData.areRomaEnemiesActive = true;
		romaEnemiesData.romaEnemyTexture.loadFromFile("Textures\\roma.jpg");
		romaEnemiesData.romaBulletTexture.loadFromFile("Textures\\romaBulletTexture.jpg");
		romaEnemies.clear();
		romaEnemies.resize(10);
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
	void checkForBulletsDamage() {
		for (int i = 0; i < player.ammunition.ordinaryBullets.size(); i++) {
			if (player.ammunition.ordinaryBullets[i].state == BUL_FIRED) {
				for (int j = 0; j < romaEnemies.size(); j++) {
					if (player.ammunition.ordinaryBullets[i].bulletShape.getGlobalBounds().intersects(romaEnemies[j].shape.getGlobalBounds()) && romaEnemies[j].getState() == ENEMY_SPAWNED) {
						player.ammunition.ordinaryBullets[i].state = BUL_INCARTRIDGE;
						romaEnemies[j].shape.setRadius(romaEnemies[j].shape.getRadius() - player.ammunition.ordinaryBulletDamage);
						romaEnemies[j].shape.setOrigin(Vector2f(romaEnemies[j].shape.getRadius(), romaEnemies[j].shape.getRadius()));
						if (romaEnemies[j].shape.getRadius() <= 10) {
							romaEnemies[j].setState("ENEMY_UNSPAWNED");
						}
					}
					for (int k = 0; k < romaEnemies[j].romaBullets.size(); k++) {
						if (player.ammunition.ordinaryBullets[i].bulletShape.getGlobalBounds().intersects(romaEnemies[j].romaBullets[k].getGlobalBounds())) {
							player.ammunition.ordinaryBullets[i].state = BUL_INCARTRIDGE;
							romaEnemies[j].romaBullets.erase(romaEnemies[j].romaBullets.begin() + k);
						}
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
		return (romaEnemiesData.areRomaEnemiesActive == true && rand() % 300 == 1);
	}
	void spawnRomaEnemy() {
		for (int i = 0; i < romaEnemies.size(); i++) {
			if (romaEnemies[i].getState() == ENEMY_UNSPAWNED) {
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
		for (int i = 0; i < player.ammunition.ordinaryBullets.size(); i++) {
			if (player.ammunition.ordinaryBullets[i].state == BUL_FIRED) {
				gameWindow.window.draw(player.ammunition.ordinaryBullets[i].bulletShape);
			}
		}
		gameWindow.window.draw(player.scope);
		gameWindow.window.draw(player.playerShape);
		for (int i = 0; i < romaEnemies.size(); i++) {
			if (romaEnemies[i].getState() == ENEMY_SPAWNED) {
				gameWindow.window.draw(romaEnemies[i].shape);
			}
			for (int j = 0; j < romaEnemies[i].romaBullets.size(); j++) {
				gameWindow.window.draw(romaEnemies[i].romaBullets[j]);
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