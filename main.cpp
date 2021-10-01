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
enum ENEMYACTIVITY { ENEMY_ACTIVE, ENEMY_INACTIVE };

struct Textures {
	Texture playertexture3HP;
	Texture playertexture2HP;
	Texture playertexture1HP;

	Texture romaEnemyTexture;
	Texture romaBulletTexture;

	Texture ordinaryBulletTexture;
	Texture ordinaryBulletScopeTexture;

	Texture gameBackgroundTexture;
	Texture menuBackgroundTexture;
	Texture menuItemBackgroundTexture;
};
class Game;
typedef void (Game::* MenuAction)();
struct MenuItem {
	Text title;
	MenuAction action;
};
struct Menu {
	vector <MenuItem> items;
	int selectedItem;
	bool isMenuTouched;
	Text caption;
	Font menuFont;
	RectangleShape menuBackground;
	RectangleShape menuItemBackground;
	Clock delayBetweenMenuPresses;
};
struct GameWindow {
	RenderWindow window;
	int x;
	int y;
	String title;
};
struct Player {
	RectangleShape playerShape;
	RectangleShape scope;
	int HPCount;
	float sizeX;
	float sizeY;
};
struct romaBullet {
	RectangleShape bulletShape;
};
struct romaEnemy {
	CircleShape romaShape;
	ENEMYSTATE state;
	int defaultRadius;
	Clock fireDelay;
};
struct Enemies {
	vector <romaEnemy> roma;
	Vector2f romaBulletSpeed;
	vector <romaBullet> romaBullets;
	ENEMYACTIVITY romaAcivity;
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
class Game {
private:
	Menu menu;
	RectangleShape gameBackground;
	Player player;
	Ammunition ammunition;
	BULLETTYPE ActiveBullet;
	Enemies enemies;
	Textures textures;
	Clock delayBetweenEscapePresses;
public:
	Event event;
	GameWindow gameWindow;
	GAMESTATE gameState;
	void initWindow() {
		gameWindow.x = 1280;
		gameWindow.y = 720;
		gameWindow.title = "BRUH Galactic";
		gameWindow.window.create(VideoMode(gameWindow.x, gameWindow.y), gameWindow.title);
		gameWindow.window.setFramerateLimit(240);
	}
	void initMenu() {
		textures.menuBackgroundTexture.loadFromFile("Textures\\menuBackgroundTexture.jpg");
		textures.menuItemBackgroundTexture.loadFromFile("Textures\\menuItemBackgroundTexture.png");
		menu.menuBackground.setTexture(&textures.menuBackgroundTexture);
		menu.menuBackground.setSize(Vector2f(gameWindow.x, gameWindow.y));
		menu.menuBackground.setPosition(Vector2f(0, 0));
		menu.menuItemBackground.setTexture(&textures.menuItemBackgroundTexture);
		menu.selectedItem = 0;
		menu.menuFont.loadFromFile("Fonts\\Hacked Cyr.ttf");
		menu.caption.setFont(menu.menuFont);
		menu.caption.setString("BRUH GALACTIC");
		menu.caption.setFillColor(Color(25, 25, 112));
		menu.caption.setPosition(Vector2f(gameWindow.x / 2.6, gameWindow.y / 2.5));
		menu.caption.setCharacterSize(50);
		menu.caption.getGlobalBounds().height;
		menu.caption.getGlobalBounds().width;
		menu.isMenuTouched = false;
	}
	void initGameBackground() {
		textures.gameBackgroundTexture.loadFromFile("Textures\\background.jpeg");
		gameBackground.setTexture(&textures.gameBackgroundTexture);
		gameBackground.setSize(Vector2f(gameWindow.x, gameWindow.y));
	}
	void initPlayer() {
		textures.playertexture3HP.loadFromFile("Textures\\playerTexture3HP.jpg");
		textures.playertexture2HP.loadFromFile("Textures\\playerTexture2HP.jpg");
		textures.playertexture1HP.loadFromFile("Textures\\playerTexture1HP.jpg");
		textures.ordinaryBulletScopeTexture.loadFromFile("Textures\\ordinaryBulletScopeTexture.png");
		player.sizeX = 75.f;
		player.sizeY = 40.f;
		player.playerShape.setOrigin(Vector2f(player.sizeX / 2, player.sizeY / 2));
		player.playerShape.setTexture(&textures.playertexture3HP);
		player.playerShape.setSize(Vector2f(player.sizeX, player.sizeY));
		player.playerShape.setRotation(90);
		player.playerShape.setPosition(Vector2f(60.f, gameWindow.y / 2));
		player.HPCount = 3;
		player.scope.setSize(Vector2f(55, 7));
		player.scope.setPosition(Vector2f(player.playerShape.getPosition().x, player.playerShape.getPosition().y - player.scope.getSize().y / 2));
		player.scope.setTexture(&textures.ordinaryBulletScopeTexture);
		player.scope.setRotation(player.playerShape.getRotation() - 90);
	}
	void initAmmunition() {
		ActiveBullet = BUL_ORDINARY;
		ammunition.ordinaryBullets.resize(15);
		textures.ordinaryBulletTexture.loadFromFile("Textures\\pchel.jpg");
		ammunition.ordinaryBulletDefaultSpeed.x = 7; ammunition.ordinaryBulletDefaultSpeed.y = 0;
		ammunition.ordinaryBulletSpeedVariation = ((ammunition.ordinaryBulletDefaultSpeed.x / 100) * 50) / 45;
		for (int i = 0; i < ammunition.ordinaryBullets.size(); i++) {
			ammunition.ordinaryBullets[i].bulletShape.setTexture(&textures.ordinaryBulletTexture);
			ammunition.ordinaryBullets[i].bulletShape.setSize(Vector2f(30.f, 17.f));
			ammunition.ordinaryBulletDamage = 10;
			ammunition.ordinaryBullets[i].ordinaryBulletSpeed = ammunition.ordinaryBulletDefaultSpeed;
			ammunition.ordinaryBullets[i].state = BUL_INCARTRIDGE;
		}
	}
	void initEnemies() {
		enemies.romaAcivity = ENEMY_ACTIVE;
		textures.romaEnemyTexture.loadFromFile("Textures\\roma.jpg");
		textures.romaBulletTexture.loadFromFile("Textures\\romaBulletTexture.jpg");
		enemies.roma.resize(10);
		enemies.romaBullets.clear();
		enemies.romaBulletSpeed.x = -1; enemies.romaBulletSpeed.y = 0;
		for (int i = 0; i < enemies.roma.size(); i++) {
			enemies.roma[i].defaultRadius = 40;
			enemies.roma[i].romaShape.setRadius(enemies.roma[i].defaultRadius);
			enemies.roma[i].romaShape.setOrigin(Vector2f(enemies.roma[i].defaultRadius, enemies.roma[i].defaultRadius));
			enemies.roma[i].romaShape.setTexture(&textures.romaEnemyTexture);
			enemies.roma[i].state = ENEMY_UNSPAWNED;
		}
	}
	void initGame() {
		gameState = GS_PLAY;
		initGameBackground();
		initPlayer();
		initAmmunition();
		initEnemies();
	}
	void exitGame() {
		gameState = GS_EXIT;
	}
	void addMenuItem(string title, MenuAction action) {
		MenuItem item;
		item.title.setFont(menu.menuFont);
		item.title.setString(title);
		item.title.setFillColor(Color(139, 0, 0));
		item.title.setCharacterSize(30);
		item.title.setPosition(Vector2f(menu.caption.findCharacterPos(2 + menu.items.size()).x, menu.caption.getPosition().y + menu.caption.getGlobalBounds().height * 2 + menu.items.size() * item.title.getGlobalBounds().height * 2));
		item.action = action;
		menu.items.push_back(item);
	}
	void controlMenu() {
		if (Keyboard::isKeyPressed(Keyboard::Key::W) && menu.delayBetweenMenuPresses.getElapsedTime().asMilliseconds() > 200) {
			/*if (!menu.isMenuTouched) {
				menu.isMenuTouched = !menu.isMenuTouched;
			}*/
			menu.selectedItem--;
			menu.delayBetweenMenuPresses.restart();
			if (menu.selectedItem < 0) {
				menu.selectedItem = menu.items.size() - 1;
			}
		}
		if (Keyboard::isKeyPressed(Keyboard::Key::S) && menu.delayBetweenMenuPresses.getElapsedTime().asMilliseconds() > 200) {
			/*if (!menu.isMenuTouched) {
				menu.isMenuTouched = !menu.isMenuTouched;
			}*/
			menu.selectedItem++;
			menu.delayBetweenMenuPresses.restart();
			if (menu.selectedItem > menu.items.size() - 1) {
				menu.selectedItem = 0;
			}
		}
	}
	void selectMenuItem() {
		menu.menuItemBackground.setPosition(Vector2f(menu.items[menu.selectedItem].title.getGlobalBounds().left, menu.items[menu.selectedItem].title.getGlobalBounds().top));
		menu.menuItemBackground.setSize(Vector2f(menu.items[menu.selectedItem].title.getGlobalBounds().width, menu.items[menu.selectedItem].title.getGlobalBounds().height));
		if (Keyboard::isKeyPressed(Keyboard::Key::Enter) && menu.delayBetweenMenuPresses.getElapsedTime().asMilliseconds() > 200) {
			(this->*menu.items[menu.selectedItem].action)();
			menu.delayBetweenMenuPresses.restart();
		}
	}
	void moveEnemies() {
		for (int i = 0; i < enemies.roma.size(); i++) {
			if (enemies.roma[i].state == ENEMY_SPAWNED) {
				/*if (enemies.romaEnemies[i].romaShape.getPosition().y < player.playerShape.getPosition().y) {
					enemies.romaEnemies[i].romaShape.move(Vector2f(0, 0.05));
				}
				if (enemies.romaEnemies[i].romaShape.getPosition().y > player.playerShape.getPosition().y) {
					enemies.romaEnemies[i].romaShape.move(Vector2f(0, -0.05));
				}*/
				// TEST
				//enemies.romaEnemies[i].romaShape.rotate(1);
			}
		}
	}
	void enemiesFire() {
		for (int i = 0; i < enemies.roma.size(); i++) {
			if (enemies.roma[i].state == ENEMY_SPAWNED && enemies.roma[i].fireDelay.getElapsedTime().asMilliseconds() >= 2000) {
				romaBullet bullet;
				bullet.bulletShape.setTexture(&textures.romaBulletTexture);
				bullet.bulletShape.setSize(Vector2f(40, 22));
				bullet.bulletShape.setPosition(Vector2f(enemies.roma[i].romaShape.getPosition()));
				enemies.romaBullets.push_back(bullet);
				enemies.roma[i].fireDelay.restart();
			}
		}
	}
	void moveEnemyBullets() {
		for (int i = 0; i < enemies.romaBullets.size(); i++) {
			enemies.romaBullets[i].bulletShape.move(enemies.romaBulletSpeed);
			if (enemies.romaBullets[i].bulletShape.getPosition().x + enemies.romaBullets[i].bulletShape.getSize().x < 0) {
				enemies.romaBullets.erase(enemies.romaBullets.begin() + i);
			}
		}
	}
	void controlPlayer() {
		if (Keyboard::isKeyPressed(Keyboard::Key::W) && player.playerShape.getPosition().y - player.sizeX / 2 > 0) {
			player.playerShape.move(0, -1.9);
			player.scope.move(0, -1.9);
		}
		if (Keyboard::isKeyPressed(Keyboard::Key::S) && player.playerShape.getPosition().y + player.sizeX / 2 < gameWindow.y) {
			player.playerShape.move(0, 1.9);
			player.scope.move(0, 1.9);
		}

		if (Keyboard::isKeyPressed(Keyboard::Key::A) && player.playerShape.getRotation() > 45) {
			player.playerShape.rotate(-1);
			player.scope.rotate(-1);
			if (player.playerShape.getRotation() < 90) {
				ammunition.ordinaryBulletDefaultSpeed.x -= ammunition.ordinaryBulletSpeedVariation; ammunition.ordinaryBulletDefaultSpeed.y -= ammunition.ordinaryBulletSpeedVariation;
			}
			else {
				ammunition.ordinaryBulletDefaultSpeed.x += ammunition.ordinaryBulletSpeedVariation; ammunition.ordinaryBulletDefaultSpeed.y -= ammunition.ordinaryBulletSpeedVariation;
			}
		}
		if (Keyboard::isKeyPressed(Keyboard::Key::D) && player.playerShape.getRotation() < 135) {
			player.playerShape.rotate(1);
			player.scope.rotate(1);
			if (player.playerShape.getRotation() > 90) {
				ammunition.ordinaryBulletDefaultSpeed.x -= ammunition.ordinaryBulletSpeedVariation; ammunition.ordinaryBulletDefaultSpeed.y += ammunition.ordinaryBulletSpeedVariation;
			}
			else {
				ammunition.ordinaryBulletDefaultSpeed.x += ammunition.ordinaryBulletSpeedVariation; ammunition.ordinaryBulletDefaultSpeed.y += ammunition.ordinaryBulletSpeedVariation;
			}
		}
	}
	void chekForBulletSwap() {
		if (Keyboard::isKeyPressed(Keyboard::Num1)) {
			ActiveBullet = BUL_ORDINARY;
			player.scope.setSize(Vector2f(55, 7));
			player.scope.setPosition(Vector2f(player.playerShape.getPosition().x, player.playerShape.getPosition().y - player.scope.getSize().y / 2));
			player.scope.setTexture(&textures.ordinaryBulletScopeTexture);
			player.scope.setRotation(player.playerShape.getRotation() - 90);
		}
	}
	void playerFire() {
		switch (ActiveBullet) {
		case BUL_ORDINARY:
			for (int i = 0; i < ammunition.ordinaryBullets.size(); i++) {
				if (ammunition.ordinaryBullets[i].state == BUL_INCARTRIDGE && ammunition.ordinaryFireDelay.getElapsedTime().asMilliseconds() > 150 /*Delay between shots*/) {
					ammunition.ordinaryBullets[i].bulletShape.setRotation(player.playerShape.getRotation() - 90);
					ammunition.ordinaryBullets[i].bulletShape.setPosition(Vector2f(player.playerShape.getPosition().x, player.playerShape.getPosition().y - ammunition.ordinaryBullets[i].bulletShape.getSize().y / 2));
					ammunition.ordinaryBullets[i].ordinaryBulletSpeed.x = ammunition.ordinaryBulletDefaultSpeed.x; ammunition.ordinaryBullets[i].ordinaryBulletSpeed.y = ammunition.ordinaryBulletDefaultSpeed.y;
					ammunition.ordinaryBullets[i].state = BUL_FIRED;
					ammunition.ordinaryFireDelay.restart();
					return;
				}
			}
		}
	}
	void moveBullets() {
		for (int i = 0; i < ammunition.ordinaryBullets.size(); i++) {
			if (ammunition.ordinaryBullets[i].state == BUL_FIRED) {
				ammunition.ordinaryBullets[i].bulletShape.move(Vector2f(ammunition.ordinaryBullets[i].ordinaryBulletSpeed));
			}
			if (ammunition.ordinaryBullets[i].bulletShape.getPosition().x >= gameWindow.window.getSize().x ||
				ammunition.ordinaryBullets[i].bulletShape.getPosition().y >= gameWindow.window.getSize().y + ammunition.ordinaryBullets[i].bulletShape.getSize().y ||
				ammunition.ordinaryBullets[i].bulletShape.getPosition().y <= 0 - ammunition.ordinaryBullets[i].bulletShape.getSize().y) {
				ammunition.ordinaryBullets[i].state = BUL_INCARTRIDGE;
			}
		}
	}
	void checkForBulletsDamage() {
		for (int i = 0; i < ammunition.ordinaryBullets.size(); i++) {
			if (ammunition.ordinaryBullets[i].state == BUL_INCARTRIDGE) {
				continue;
			}
			else {
				for (int j = 0; j < enemies.roma.size(); j++) {
					if (ammunition.ordinaryBullets[i].bulletShape.getGlobalBounds().intersects(enemies.roma[j].romaShape.getGlobalBounds()) && enemies.roma[j].state == ENEMY_SPAWNED) {
						ammunition.ordinaryBullets[i].state = BUL_INCARTRIDGE;
						enemies.roma[j].romaShape.setRadius(enemies.roma[j].romaShape.getRadius() - ammunition.ordinaryBulletDamage);
						enemies.roma[j].romaShape.setOrigin(Vector2f(enemies.roma[j].romaShape.getRadius(), enemies.roma[j].romaShape.getRadius()));
						if (enemies.roma[j].romaShape.getRadius() <= 10) {
							enemies.roma[j].state = ENEMY_UNSPAWNED;
						}
					}
				}
				for (int j = 0; j < enemies.romaBullets.size(); j++) {
					if (ammunition.ordinaryBullets[i].bulletShape.getGlobalBounds().intersects(enemies.romaBullets[j].bulletShape.getGlobalBounds())) {
						ammunition.ordinaryBullets[i].state = BUL_INCARTRIDGE;
						enemies.romaBullets.erase(enemies.romaBullets.begin() + j);
					}
				}
			}
		}
	}
	void checkForPlayerCollisions() {
		for (int i = 0; i < enemies.romaBullets.size(); i++) {
			if (player.playerShape.getGlobalBounds().intersects(enemies.romaBullets[i].bulletShape.getGlobalBounds())) {
				player.HPCount--;
				switch (player.HPCount) {
				case 3:
					player.playerShape.setTexture(&textures.playertexture3HP);
					break;
				case 2:
					player.playerShape.setTexture(&textures.playertexture2HP);
					break;
				case 1:
					player.playerShape.setTexture(&textures.playertexture1HP);
					break;
				}
				enemies.romaBullets.erase(enemies.romaBullets.begin() + i);
			}
		}
	}
	bool isRomaEnemyNeedToSpawn() {
		return (enemies.romaAcivity == ENEMY_ACTIVE && rand() % 300 == 1);
	}
	void spawnRomaEnemy() {
		for (int i = 0; i < enemies.roma.size(); i++) {
			if (enemies.roma[i].state == ENEMY_UNSPAWNED) {
				enemies.roma[i].romaShape.setRadius(enemies.roma[i].defaultRadius);
				enemies.roma[i].romaShape.setOrigin(Vector2f(enemies.roma[i].defaultRadius, enemies.roma[i].defaultRadius));
				enemies.roma[i].romaShape.setPosition(Vector2f(gameWindow.x - enemies.roma[i].defaultRadius, rand() % (gameWindow.y - enemies.roma[i].defaultRadius * 4) + enemies.roma[i].defaultRadius * 2));
				enemies.roma[i].state = ENEMY_SPAWNED;
				return;
			}
		}
	}
	void checkForEnemySpawn() {
		if (isRomaEnemyNeedToSpawn()) {
			spawnRomaEnemy();
		}
	}

	void updateMenuFrame() {
		controlMenu();
		selectMenuItem();
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

		if (player.HPCount <= 0) {
			gameState = GS_MENU;
		}
		controlPlayer();
		chekForBulletSwap();
		if (Mouse::isButtonPressed(Mouse::Button::Left)) {
			playerFire();
		}
		moveBullets();

		checkForEnemySpawn();
		moveEnemies();
		enemiesFire();
		moveEnemyBullets();

		checkForPlayerCollisions();
		checkForBulletsDamage();
		gameWindow.window.clear();
		gameWindow.window.draw(gameBackground);
		for (int i = 0; i < ammunition.ordinaryBullets.size(); i++) {
			if (ammunition.ordinaryBullets[i].state == BUL_FIRED) {
				gameWindow.window.draw(ammunition.ordinaryBullets[i].bulletShape);
			}
		}
		for (int i = 0; i < enemies.romaBullets.size(); i++) {
			gameWindow.window.draw(enemies.romaBullets[i].bulletShape);
		}
		gameWindow.window.draw(player.scope);
		gameWindow.window.draw(player.playerShape);
		for (int i = 0; i < enemies.roma.size(); i++) {
			if (enemies.roma[i].state == ENEMY_SPAWNED) {
				gameWindow.window.draw(enemies.roma[i].romaShape);
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