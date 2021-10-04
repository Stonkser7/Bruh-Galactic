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
struct romaEnemy {
	CircleShape romaShape;
	ENEMYSTATE state;
	int defaultRadius;
	Clock fireDelay;
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
	vector <OrdinaryBullet> ordinaryBullets;
};
class Menu {
private:
	class Game;
	typedef void (Game::* MenuAction)();
	struct MenuItem {
		Text title;
		MenuAction action;
	};
	vector <MenuItem> items;
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
	void initMenu(GameWindow gwindow) {
		menuBackgroundTexture.loadFromFile("Textures\\menuBackgroundTexture.jpg");
		menuItemBackgroundTexture.loadFromFile("Textures\\menuItemBackgroundTexture.png");
		menuBackground.setTexture(&menuBackgroundTexture);
		menuBackground.setSize(Vector2f(gwindow.x, gwindow.y));
		menuBackground.setPosition(Vector2f(0, 0));
		menuItemBackground.setTexture(&menuItemBackgroundTexture);
		selectedItem = 0;
		menuFont.loadFromFile("Fonts\\Hacked Cyr.ttf");
		caption.setFont(menuFont);
		caption.setString("BRUH GALACTIC");
		caption.setFillColor(Color(25, 25, 112));
		caption.setPosition(Vector2f(gwindow.x / 2.6, gwindow.y / 2.5));
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
		item.title.setPosition(Vector2f(caption.findCharacterPos(2 + items.size()).x, caption.getPosition().y + caption.getGlobalBounds().height * 2 + items.size() * item.title.getGlobalBounds().height * 2));
		item.action = action;
		items.push_back(item);
	}
	void controlMenu() {
		if (Keyboard::isKeyPressed(Keyboard::Key::W) && delayBetweenMenuPresses.getElapsedTime().asMilliseconds() > 200) {
			/*if (!menu.isMenuTouched) {
				menu.isMenuTouched = !menu.isMenuTouched;
			}*/
			selectedItem--;
			delayBetweenMenuPresses.restart();
			if (selectedItem < 0) {
				selectedItem = items.size() - 1;
			}
		}
		if (Keyboard::isKeyPressed(Keyboard::Key::S) && delayBetweenMenuPresses.getElapsedTime().asMilliseconds() > 200) {
			/*if (!menu.isMenuTouched) {
				menu.isMenuTouched = !menu.isMenuTouched;
			}*/
			selectedItem++;
			delayBetweenMenuPresses.restart();
			if (selectedItem > items.size() - 1) {
				selectedItem = 0;
			}
		}
	}
	void selectMenuItem(Game *gameClass) {
		menuItemBackground.setPosition(Vector2f(items[selectedItem].title.getGlobalBounds().left, items[selectedItem].title.getGlobalBounds().top));
		menuItemBackground.setSize(Vector2f(items[selectedItem].title.getGlobalBounds().width, items[selectedItem].title.getGlobalBounds().height));
		if (Keyboard::isKeyPressed(Keyboard::Key::Enter) && delayBetweenMenuPresses.getElapsedTime().asMilliseconds() > 200) {
			(gameClass->*items[selectedItem].action)();
			delayBetweenMenuPresses.restart();
		}
	}
};
class Player {
private:
	float sizeX;
	float sizeY;

	BULLETTYPE ActiveBullet;

	Texture playertexture3HP;
	Texture playertexture2HP;
	Texture playertexture1HP;
	Texture ordinaryBulletTexture;
	Texture ordinaryBulletScopeTexture;
public:
	RectangleShape playerShape;
	RectangleShape scope;
	int HPAmount;
	Ammunition ammunition;

	void initPlayer(GameWindow *gwindow) {
		playertexture3HP.loadFromFile("Textures\\playerTexture3HP.jpg");
		playertexture2HP.loadFromFile("Textures\\playerTexture2HP.jpg");
		playertexture1HP.loadFromFile("Textures\\playerTexture1HP.jpg");
		ordinaryBulletScopeTexture.loadFromFile("Textures\\ordinaryBulletScopeTexture.png");
		sizeX = 75.f;
		sizeY = 40.f;
		playerShape.setOrigin(Vector2f(sizeX / 2, sizeY / 2));
		playerShape.setTexture(&playertexture3HP);
		playerShape.setSize(Vector2f(sizeX, sizeY));
		playerShape.setRotation(90);
		playerShape.setPosition(Vector2f(60.f, gwindow->y / 2));
		HPCount = 3;
		scope.setSize(Vector2f(55, 7));
		scope.setPosition(Vector2f(playerShape.getPosition().x, playerShape.getPosition().y - scope.getSize().y / 2));
		scope.setTexture(&ordinaryBulletScopeTexture);
		scope.setRotation(playerShape.getRotation() - 90);
	}
	void initAmmunition() {
		ActiveBullet = BUL_ORDINARY;
		ammunition.ordinaryBullets.resize(15);
		ordinaryBulletTexture.loadFromFile("Textures\\pchel.jpg");
		ammunition.ordinaryBulletDefaultSpeed.x = 7; ammunition.ordinaryBulletDefaultSpeed.y = 0;
		ammunition.ordinaryBulletSpeedVariation = ((ammunition.ordinaryBulletDefaultSpeed.x / 100) * 50) / 45;
		for (int i = 0; i < ammunition.ordinaryBullets.size(); i++) {
			ammunition.ordinaryBullets[i].bulletShape.setTexture(&ordinaryBulletTexture);
			ammunition.ordinaryBullets[i].bulletShape.setSize(Vector2f(30.f, 17.f));
			ammunition.ordinaryBulletDamage = 10;
			ammunition.ordinaryBullets[i].ordinaryBulletSpeed = ammunition.ordinaryBulletDefaultSpeed;
			ammunition.ordinaryBullets[i].state = BUL_INCARTRIDGE;
		}
	}
	void controlPlayer(GameWindow *gwindow) {
		if (Keyboard::isKeyPressed(Keyboard::Key::W) && playerShape.getPosition().y - sizeX / 2 > 0) {
			playerShape.move(0, -1.9);
			scope.move(0, -1.9);
		}
		if (Keyboard::isKeyPressed(Keyboard::Key::S) && playerShape.getPosition().y + sizeX / 2 < gwindow->y) {
			playerShape.move(0, 1.9);
			scope.move(0, 1.9);
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
	void fire() {
		switch (ActiveBullet) {
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
	void chekForBulletSwap() {
		if (Keyboard::isKeyPressed(Keyboard::Num1)) {
			ActiveBullet = BUL_ORDINARY;
			scope.setSize(Vector2f(55, 7));
			scope.setPosition(Vector2f(playerShape.getPosition().x, playerShape.getPosition().y - scope.getSize().y / 2));
			scope.setTexture(&ordinaryBulletScopeTexture);
			scope.setRotation(playerShape.getRotation() - 90);
		}
	}
	void setPlayerHP(int hpAmount) {
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
};
class Enemies {
private:
	Texture romaEnemyTexture;
	Texture romaBulletTexture;

	Vector2f romaBulletSpeed;
	bool isRomaEnemyActive;
public:
	vector <romaEnemy> romaEnemies;
	vector <RectangleShape> romaBullets;
	void initEnemies() {
		isRomaEnemyActive = true;
		romaEnemyTexture.loadFromFile("Textures\\roma.jpg");
		romaBulletTexture.loadFromFile("Textures\\romaBulletTexture.jpg");
		romaEnemies.resize(10);
		romaBullets.clear();
		romaBulletSpeed.x = -1; romaBulletSpeed.y = 0;
		for (int i = 0; i < romaEnemies.size(); i++) {
			romaEnemies[i].defaultRadius = 40;
			romaEnemies[i].romaShape.setRadius(romaEnemies[i].defaultRadius);
			romaEnemies[i].romaShape.setOrigin(Vector2f(romaEnemies[i].defaultRadius, romaEnemies[i].defaultRadius));
			romaEnemies[i].romaShape.setTexture(&romaEnemyTexture);
			romaEnemies[i].state = ENEMY_UNSPAWNED;
		}
	}
	void moveEnemies() {
		for (int i = 0; i < romaEnemies.size(); i++) {
			if (romaEnemies[i].state == ENEMY_SPAWNED) {
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
	}
	void enemiesFire() {
		for (int i = 0; i < romaEnemies.size(); i++) {
			if (romaEnemies[i].state == ENEMY_SPAWNED && romaEnemies[i].fireDelay.getElapsedTime().asMilliseconds() >= 2000) {
				RectangleShape bullet;
				bullet.setTexture(&romaBulletTexture);
				bullet.setSize(Vector2f(40, 22));
				bullet.setPosition(Vector2f(romaEnemies[i].romaShape.getPosition()));
				romaBullets.push_back(bullet);
				romaEnemies[i].fireDelay.restart();
			}
		}
	}
	void moveEnemyBullets() {
		for (int i = 0; i < romaBullets.size(); i++) {
			romaBullets[i].move(romaBulletSpeed);
			if (romaBullets[i].getPosition().x + romaBullets[i].getSize().x < 0) {
				romaBullets.erase(romaBullets.begin() + i);
			}
		}
	}
	void checkForEnemySpawn(GameWindow *gwindow) {
		if (isRomaEnemyNeedToSpawn()) {
			spawnRomaEnemy(gwindow);
		}
	}
	bool isRomaEnemyNeedToSpawn() {
		return (isRomaEnemyActive && rand() % 300 == 1);
	}
	void spawnRomaEnemy(GameWindow *gwindow) {
		for (int i = 0; i < romaEnemies.size(); i++) {
			if (romaEnemies[i].state == ENEMY_UNSPAWNED) {
				romaEnemies[i].romaShape.setRadius(romaEnemies[i].defaultRadius);
				romaEnemies[i].romaShape.setOrigin(Vector2f(romaEnemies[i].defaultRadius, romaEnemies[i].defaultRadius));
				romaEnemies[i].romaShape.setPosition(Vector2f(gwindow->x - romaEnemies[i].defaultRadius, rand() % (gwindow->y - romaEnemies[i].defaultRadius * 4) + romaEnemies[i].defaultRadius * 2));
				romaEnemies[i].state = ENEMY_SPAWNED;
				return;
			}
		}
	}
};
class Game {
private:
	GameWindow gameWindow;
	Menu menu;
	Player player;
	Enemies enemies;
	Texture gameBackgroundTexture;
	RectangleShape gameBackground;
	Clock delayBetweenEscapePresses;
public:
	Event event;
	GAMESTATE gameState;
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
	void initGame() {
		gameState = GS_PLAY;
		initGameBackground();
		player.initPlayer(&gameWindow);
		player.initAmmunition();
		enemies.initEnemies();
	}
	void exitGame() {
		gameState = GS_EXIT;
	}
	void checkForBulletsDamage(Ammunition *ammunition) {
		for (int i = 0; i < ammunition->ordinaryBullets.size(); i++) {
			if (ammunition->ordinaryBullets[i].state == BUL_INCARTRIDGE) {
				continue;
			}
			else {
				for (int j = 0; j < enemies.romaEnemies.size(); j++) {
					if (ammunition->ordinaryBullets[i].bulletShape.getGlobalBounds().intersects(enemies.romaEnemies[j].romaShape.getGlobalBounds()) && enemies.romaEnemies[j].state == ENEMY_SPAWNED) {
						ammunition->ordinaryBullets[i].state = BUL_INCARTRIDGE;
						enemies.romaEnemies[j].romaShape.setRadius(enemies.romaEnemies[j].romaShape.getRadius() - ammunition->ordinaryBulletDamage);
						enemies.romaEnemies[j].romaShape.setOrigin(Vector2f(enemies.romaEnemies[j].romaShape.getRadius(), enemies.romaEnemies[j].romaShape.getRadius()));
						if (enemies.romaEnemies[j].romaShape.getRadius() <= 10) {
							enemies.romaEnemies[j].state = ENEMY_UNSPAWNED;
						}
					}
				}
				for (int j = 0; j < enemies.romaBullets.size(); j++) {
					if (ammunition->ordinaryBullets[i].bulletShape.getGlobalBounds().intersects(enemies.romaBullets[j].bulletShape.getGlobalBounds())) {
						ammunition->ordinaryBullets[i].state = BUL_INCARTRIDGE;
						enemies.romaBullets.erase(enemies.romaBullets.begin() + j);
					}
				}
			}
		}
	}
	void checkForPlayerCollisions() {
		for (int i = 0; i < enemies.romaBullets.size(); i++) {
			if (player.playerShape.getGlobalBounds().intersects(enemies.romaBullets[i].getGlobalBounds())) {
				player.HPAmount--;
				player.setPlayerHP(player.HPAmount);
				enemies.romaBullets.erase(enemies.romaBullets.begin() + i);
			}
		}
	}

	void updateMenuFrame() {
		menu.controlMenu();
		menu.selectMenuItem();
		gameWindow.window.clear(Color::Black);
		gameWindow.window.draw(menu.menuBackground);
		gameWindow.window.draw(menu.caption);
		gameWindow.window.draw(menu.menuItemBackground);
		for (int i = 0; i < menu.items.size(); i++) {
			gameWindow.window.draw(menu.items[i].title);
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

		if (player.HPAmount <= 0) {
			gameState = GS_MENU;
		}
		player.controlPlayer(&gameWindow);
		player.chekForBulletSwap();
		if (Mouse::isButtonPressed(Mouse::Button::Left)) {
			player.fire();
		}
		player.moveBullets(&gameWindow);

		enemies.checkForEnemySpawn(&gameWindow);
		enemies.moveEnemies();
		enemies.enemiesFire();
		enemies.moveEnemyBullets();

		checkForPlayerCollisions();
		checkForBulletsDamage(&player.ammunition);
		gameWindow.window.clear();
		gameWindow.window.draw(gameBackground);
		for (int i = 0; i < player.ammunition.ordinaryBullets.size(); i++) {
			if (player.ammunition.ordinaryBullets[i].state == BUL_FIRED) {
				gameWindow.window.draw(player.ammunition.ordinaryBullets[i].bulletShape);
			}
		}
		for (int i = 0; i < enemies.romaBullets.size(); i++) {
			gameWindow.window.draw(enemies.romaBullets[i]);
		}
		gameWindow.window.draw(player.scope);
		gameWindow.window.draw(player.playerShape);
		for (int i = 0; i < enemies.romaEnemies.size(); i++) {
			if (enemies.romaEnemies[i].state == ENEMY_SPAWNED) {
				gameWindow.window.draw(enemies.romaEnemies[i].romaShape);
			}
		}
		gameWindow.window.display();
	}
};
int main() {
	Game BG;
	BG.initWindow();
	BG.initMenu();
	BG.addMenuItem("PLAY", &Game::initGame);
	BG.addMenuItem("EXIT", &Game::exitGame);
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
			BG.updateMenuFrame();
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