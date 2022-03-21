#include <SFML/Graphics.hpp>
#include "Collision.h"
#include <iostream>
#include <vector>
#include <string>
#include <cmath>
#include <iomanip>
#include <ctime>
#include "Player.h"
#include "Enemies.h"
#include "Menu.h"
#include "Pause.h"
using namespace sf;
using namespace std;

enum GAMESTATE { GS_PAUSE, GS_PLAY, GS_EXIT, GS_MENU };
enum ENEMYTYPE { ET_ROMA, ET_ROCK, ET_ELECTRO };

class Game {
private:
	Player player;
	//enemies data
	RomaEnemiesData romaData;
	RockEnemiesData rockData;
	ElectroEnemiesData electroData;
	HealerEnemiesData healerData;
	//enemies
	vector <Enemy::RomaEnemy> romaEnemies;
	vector <Enemy::RockEnemy> rockEnemies;
	vector <Enemy::ElectroEnemy> electroEnemies;
	vector <Enemy::HealerEnemy> healerEnemies;
	//enemy bullets
	vector <RectangleShape> romaBullets;
	vector <RockEnemyBullet> rockBullets;
	vector <ElectroEnemyLightning> electroLightnings;
	vector <HealerEnemyRay> healerRays;
	Texture gameBackgroundTexture;
	RectangleShape gameBackground;
	Clock delayBetweenEscapePresses;
public:
	GameWindow gameWindow;
	Menu menu;
	Pause pause;
	Event event;
	GAMESTATE gameState;
	Game() {
		initWindow();
	}
	void debugging() {
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
		//cout << endl << player.ammo.rayBullets.size();
		//cout << endl << healerEnemies.size();
		/*if (!healerEnemies.empty()) {
			cout << endl << healerEnemies[0].destinationCoords.x << " | " << healerEnemies[0].shape.getPosition().x << setw(5) << healerEnemies[0].destinationCoords.y << " | " << healerEnemies[0].shape.getPosition().y;
		}*/
		//cout << pause.getSelectedButton() << endl;
		//cout << player.ammo.splittedBullets.size() + player.ammo.splittingBullets.size() << endl;
	}

	void initWindow() {
		gameWindow.x = 1920;
		gameWindow.y = 1080;
		gameWindow.title = "BRUH GALACTIC";
		gameWindow.window.create(VideoMode(gameWindow.x, gameWindow.y), gameWindow.title, Style::Fullscreen);
		gameWindow.window.setFramerateLimit(240);
	}

	void initGameBackground() {
		gameBackgroundTexture.loadFromFile("Textures\\background.jpg");
		gameBackground.setSize(Vector2f(gameWindow.x, gameWindow.y));
		gameBackground.setTexture(&gameBackgroundTexture);
	}

	void initEnemies() {
		//INITIALIZATION ROMA ENEMY
		romaData.areActive = true;
		romaData.maxAmount = 10;
		Collision::CreateTextureAndBitmask(romaData.enemyTexture, "Textures\\RomaEnemy.jpg");
		Collision::CreateTextureAndBitmask(romaData.bulletTexture, "Textures\\romaBulletTexture.jpg");
		romaData.bulletSpeed = { -3, 0 };
		romaData.spawnRadius = 45;
		romaEnemies.clear();
		romaBullets.clear();

		//INITIALIZATION ROCK ENEMY
		rockData.areActive = true;
		rockData.maxAmount = 10;
		Collision::CreateTextureAndBitmask(rockData.enemyTexture, "Textures\\rockEnemy.png");
		Collision::CreateTextureAndBitmask(rockData.bulletTexture, "Textures\\rockEnemyBulletTexture.png");
		rockData.spawnRadius = 40;
		rockEnemies.clear();
		rockBullets.clear();

		//INITIALIZATION ELECTRO ENEMY
		electroData.areActive = true;
		electroData.maxAmount = 4;
		Collision::CreateTextureAndBitmask(electroData.enemyTexture, "Textures\\ElectroEnemy.jpg");
		Collision::CreateTextureAndBitmask(electroData.lightningTexture, "Textures\\lightningTexture1.png");
		electroData.spawnRadius = 35;
		electroData.visibleDelayAsMilliseconds = 800;
		electroData.visible_lightningDelayAsMilliseconds = 400;
		electroEnemies.clear();
		electroLightnings.clear();

		//INITIALIZATION HEALER ENEMY
		healerData.areActive = true;
		healerData.maxAmount = 3;
		healerData.heal = 0.07;
		healerData.spawnRadius = 24;
		healerData.healAreaRadius = 230;
		Collision::CreateTextureAndBitmask(healerData.enemyTexture, "Textures\\healerEnemyTexture.png");
		Collision::CreateTextureAndBitmask(healerData.healAreaTexture, "Textures\\healerActionAreaTexture.png");
		healerData.healAreaTexture.setSmooth(true);
		healerData.rayTexture.loadFromFile("Textures\\healerRayTexture.png");
		healerEnemies.clear();
		healerRays.clear();
	}

	void initGame() {
		gameState = GS_PLAY;
		initGameBackground();
		player.initPlayer(&gameWindow);
		player.initAmmunition();
		initEnemies();
	}

	void pauseGame() {
		gameState = GS_PAUSE;
		player.excessFireClock.restart();
		for (int i = 0; i < romaEnemies.size(); i++) {
			romaEnemies[i].excessFireClock.restart();
		}
		for (int i = 0; i < rockEnemies.size(); i++) {
			rockEnemies[i].excessFireClock.restart();
		}
		for (int i = 0; i < electroEnemies.size(); i++) {
			electroEnemies[i].excessFireClock.restart();
			electroEnemies[i].excessVisibleClock.restart();
		}
		for (int i = 0; i < healerEnemies.size(); i++) {
			healerEnemies[i].excessFireClock.restart();
		}

		for (int i = 0; i < electroLightnings.size(); i++) {
			electroLightnings[i].excessVisible_lightningClock.restart();
		}
	}
	
	void continueGame() {
		gameState = GS_PLAY;
		player.ammoData.ordinaryBulletData.excessFireDelay = player.excessFireClock.getElapsedTime().asMilliseconds();
		player.ammoData.splittingBulletData.excessFireDelay = player.excessFireClock.getElapsedTime().asMilliseconds();
		player.ammoData.rayBulletData.excessFireDelay = player.excessFireClock.getElapsedTime().asMilliseconds();
		player.ammoData.grenadeBulletData.excessFireDelay = player.excessFireClock.getElapsedTime().asMilliseconds();
		for (int i = 0; i < romaEnemies.size(); i++) {
			romaEnemies[i].excessFireTime = romaEnemies[i].excessFireClock.getElapsedTime().asMilliseconds();
		}
		for (int i = 0; i < rockEnemies.size(); i++) {
			rockEnemies[i].excessFireTime = rockEnemies[i].excessFireClock.getElapsedTime().asMilliseconds();
		}
		for (int i = 0; i < electroEnemies.size(); i++) {
			electroEnemies[i].excessFireTime = electroEnemies[i].excessFireClock.getElapsedTime().asMilliseconds();
			electroEnemies[i].excessVisibleTime = electroEnemies[i].excessVisibleClock.getElapsedTime().asMilliseconds();
		}
		for (int i = 0; i < healerEnemies.size(); i++) {
			healerEnemies[i].excessFireTime = healerEnemies[i].excessFireClock.getElapsedTime().asMilliseconds();
		}

		for (int i = 0; i < electroLightnings.size(); i++) {
			electroLightnings[i].excessVisible_lightningTime = electroLightnings[i].excessVisible_lightningClock.getElapsedTime().asMilliseconds();
		}
	}

	void backToMenu() {
		gameState = GS_MENU;
		menu.delayBetweenMenuPresses.restart();
	}

	void exitGame() {
		gameState = GS_EXIT;
	}

	/////////////////////////////////////////////////					ДОДЕЛЫВАЙ, ИНАЧЕ НЕ ПОЕШЬ(надо оптимизацию коллизии ещё сделать потом)
	void checkForOrdinaryBulletsCollision() {
		//ROMA ENEMIES
		for (int i = 0; i < player.ammo.ordinaryBullets.size(); i++) {
			for (int j = 0; j < romaEnemies.size(); j++) {
				if (player.ammo.ordinaryBullets[i].shape.getGlobalBounds().intersects(romaEnemies[j].shape.getGlobalBounds())) {
					player.deleteBullet(BULT_ORDINARY, i);
					romaEnemies[j].takeDamage(player.ammoData.ordinaryBulletData.damage);
					i--;
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
					i--;
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
					i--;
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
					i--;
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
						i--;
						break;
					}
				}
			}
		}



		//HEALER ENEMIES
		for (int i = 0; i < player.ammo.ordinaryBullets.size(); i++) {
			for (int j = 0; j < healerEnemies.size(); j++) {
				if (player.ammo.ordinaryBullets[i].shape.getGlobalBounds().intersects(healerEnemies[j].shape.getGlobalBounds())) {
					player.deleteBullet(BULT_ORDINARY, i);
					healerEnemies[j].takeDamage(player.ammoData.ordinaryBulletData.damage);
					i--;
					break;
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
					i--;
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
					i--;
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
					i--;
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
					i--;
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
						player.splitBullet(&player.ammo.splittingBullets[i]);
						player.deleteBullet(BULT_SPLITTING, i);
						i--;
						break;
					}
				}
			}
		}



		//HEALER ENEMIES
		for (int i = 0; i < player.ammo.splittingBullets.size(); i++) {
			for (int j = 0; j < healerEnemies.size(); j++) {
				if (player.ammo.splittingBullets[i].shape.getGlobalBounds().intersects(healerEnemies[j].shape.getGlobalBounds())) {
					healerEnemies[j].takeDamage(player.ammo.splittingBullets[i].damage);
					player.splitBullet(&player.ammo.splittingBullets[i]);
					player.deleteBullet(BULT_SPLITTING, i);
					i--;
					break;
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
					i--;
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
					i--;
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
					i--;
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
					i--;
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
						player.splitBullet(&player.ammo.splittedBullets[i]);
						player.deleteBullet(BULT_SPLITTED, i);
						i--;
						break;
					}
				}
			}
		}



		//HEALER ENEMIES
		for (int i = 0; i < player.ammo.splittedBullets.size(); i++) {
			for (int j = 0; j < healerEnemies.size(); j++) {
				if (player.ammo.splittedBullets[i].shape.getGlobalBounds().intersects(healerEnemies[j].shape.getGlobalBounds())) {
					healerEnemies[j].takeDamage(player.ammo.splittedBullets[i].damage);
					player.splitBullet(&player.ammo.splittedBullets[i]);
					player.deleteBullet(BULT_SPLITTED, i);
					i--;
					break;
				}
			}
		}
	}
	void checkForRayBulletsCollision() {
		//ROMA ENEMIES
		for (int i = 0; i < player.ammo.rayBullets.size(); i++) {
			if (player.ammo.rayBullets[i].state == BS_FIRING) {
				for (int j = 0; j < romaEnemies.size(); j++) {
					if (Collision::PixelPerfectTest(player.ammo.rayBullets[i].shape, romaEnemies[j].shape)) {
						romaEnemies[j].takeDamage(player.ammoData.rayBulletData.damage);
					}
				}
			}
		}
		//ROMA BULLETS
		for (int i = 0; i < player.ammo.rayBullets.size(); i++) {
			if (player.ammo.rayBullets[i].state == BS_FIRING) {
				for (int j = 0; j < romaBullets.size(); j++) {
					if (Collision::PixelPerfectTest(player.ammo.rayBullets[i].shape, romaBullets[j])) {
						romaBullets.erase(romaBullets.begin() + j);
					}
				}
			}
		}



		//ROCK ENEMIES
		for (int i = 0; i < player.ammo.rayBullets.size(); i++) {
			if (player.ammo.rayBullets[i].state == BS_FIRING) {
				for (int j = 0; j < rockEnemies.size(); j++) {
					if (Collision::PixelPerfectTest(player.ammo.rayBullets[i].shape, rockEnemies[j].shape)) {
						rockEnemies[j].takeDamage(player.ammoData.rayBulletData.damage);
					}
				}
			}
		}
		//ROCK BULLETS
		for (int i = 0; i < player.ammo.rayBullets.size(); i++) {
			if (player.ammo.rayBullets[i].state == BS_FIRING) {
				for (int j = 0; j < rockBullets.size(); j++) {
					if (Collision::PixelPerfectTest(player.ammo.rayBullets[i].shape, rockBullets[j].shape)) {
						rockBullets.erase(rockBullets.begin() + j);
					}
				}
			}
		}

		//ELECTRO ENEMIES
		for (int i = 0; i < player.ammo.rayBullets.size(); i++) {
			if (player.ammo.rayBullets[i].state == BS_FIRING) {
				for (int j = 0; j < electroEnemies.size(); j++) {
					if (electroEnemies[j].visible) {
						if (Collision::PixelPerfectTest(player.ammo.rayBullets[i].shape, electroEnemies[j].shape)) {
							electroEnemies[j].takeDamage(player.ammoData.rayBulletData.damage);
						}
					}
				}
			}
		}



		//HEALER ENEMIES
		for (int i = 0; i < player.ammo.rayBullets.size(); i++) {
			if (player.ammo.rayBullets[i].state == BS_FIRING) {
				for (int j = 0; j < healerEnemies.size(); j++) {
					if (Collision::PixelPerfectTest(player.ammo.rayBullets[i].shape, healerEnemies[j].shape)) {
						healerEnemies[j].takeDamage(player.ammoData.rayBulletData.damage);
					}
				}
			}
		}
	}
	void checkForGrenadeBulletCollision() {
		//ROMA ENEMIES
		for (int i = 0; i < player.ammo.grenadeBullets.size(); i++) {
			if (player.ammo.grenadeBullets[i].state == BS_EXPLOSING) {
				for (int j = 0; j < romaEnemies.size(); j++) {
					if (Collision::PixelPerfectTest(player.ammo.grenadeBullets[i].firstDamageArea, romaEnemies[j].shape)) {
						romaEnemies[j].takeDamage(player.ammoData.grenadeBulletData.firstLevelDamage);
					}
					if (Collision::PixelPerfectTest(player.ammo.grenadeBullets[i].secondDamageArea, romaEnemies[j].shape)) {
						romaEnemies[j].takeDamage(player.ammoData.grenadeBulletData.secondLevelDamage);
					}
					if (Collision::PixelPerfectTest(player.ammo.grenadeBullets[i].thirdDamageArea, romaEnemies[j].shape)) {
						romaEnemies[j].takeDamage(player.ammoData.grenadeBulletData.thirdLevelDamage);
					}
				}
			}
		}
		//ROMA BULLETS
		for (int i = 0; i < player.ammo.grenadeBullets.size(); i++) {
			if (player.ammo.grenadeBullets[i].state == BS_EXPLOSING) {
				for (int j = 0; j < romaBullets.size(); j++) {
					if (Collision::PixelPerfectTest(player.ammo.grenadeBullets[i].firstDamageArea, romaBullets[j])) {
						romaBullets.erase(romaBullets.begin() + j);
						j--;
						continue;
					}
					if (Collision::PixelPerfectTest(player.ammo.grenadeBullets[i].secondDamageArea, romaBullets[j])) {
						romaBullets.erase(romaBullets.begin() + j);
						j--;
						continue;
					}
					if (Collision::PixelPerfectTest(player.ammo.grenadeBullets[i].thirdDamageArea, romaBullets[j])) {
						romaBullets.erase(romaBullets.begin() + j);
						j--;
						continue;
					}
				}
			}
		}



		//ROCK ENEMIES
		for (int i = 0; i < player.ammo.grenadeBullets.size(); i++) {
			if (player.ammo.grenadeBullets[i].state == BS_EXPLOSING) {
				for (int j = 0; j < rockEnemies.size(); j++) {
					if (Collision::PixelPerfectTest(player.ammo.grenadeBullets[i].firstDamageArea, rockEnemies[j].shape)) {
						rockEnemies[j].takeDamage(player.ammoData.grenadeBulletData.firstLevelDamage);
					}
					if (Collision::PixelPerfectTest(player.ammo.grenadeBullets[i].secondDamageArea, rockEnemies[j].shape)) {
						rockEnemies[j].takeDamage(player.ammoData.grenadeBulletData.secondLevelDamage);
					}
					if (Collision::PixelPerfectTest(player.ammo.grenadeBullets[i].thirdDamageArea, rockEnemies[j].shape)) {
						rockEnemies[j].takeDamage(player.ammoData.grenadeBulletData.thirdLevelDamage);
					}
				}
			}
		}
		//ROCK BULLETS
		for (int i = 0; i < player.ammo.grenadeBullets.size(); i++) {
			if (player.ammo.grenadeBullets[i].state == BS_EXPLOSING) {
				for (int j = 0; j < rockBullets.size(); j++) {
					if (Collision::PixelPerfectTest(player.ammo.grenadeBullets[i].firstDamageArea, rockBullets[j].shape)) {
						rockBullets.erase(rockBullets.begin() + j);
						j--;
						continue;
					}
					if (Collision::PixelPerfectTest(player.ammo.grenadeBullets[i].secondDamageArea, rockBullets[j].shape)) {
						rockBullets.erase(rockBullets.begin() + j);
						j--;
						continue;
					}
					if (Collision::PixelPerfectTest(player.ammo.grenadeBullets[i].thirdDamageArea, rockBullets[j].shape)) {
						rockBullets.erase(rockBullets.begin() + j);
						j--;
						continue;
					}
				}
			}
		}



		//ELECTRO ENEMIES
		for (int i = 0; i < player.ammo.grenadeBullets.size(); i++) {
			if (player.ammo.grenadeBullets[i].state == BS_EXPLOSING) {
				for (int j = 0; j < electroEnemies.size(); j++) {
					if (electroEnemies[j].visible) {
						if (Collision::PixelPerfectTest(player.ammo.grenadeBullets[i].firstDamageArea, electroEnemies[j].shape)) {
							electroEnemies[j].takeDamage(player.ammoData.grenadeBulletData.firstLevelDamage);
						}
						if (Collision::PixelPerfectTest(player.ammo.grenadeBullets[i].secondDamageArea, electroEnemies[j].shape)) {
							electroEnemies[j].takeDamage(player.ammoData.grenadeBulletData.secondLevelDamage);
						}
						if (Collision::PixelPerfectTest(player.ammo.grenadeBullets[i].thirdDamageArea, electroEnemies[j].shape)) {
							electroEnemies[j].takeDamage(player.ammoData.grenadeBulletData.thirdLevelDamage);
						}
					}
				}
			}
		}



		//HEALER ENEMIES
		for (int i = 0; i < player.ammo.grenadeBullets.size(); i++) {
			if (player.ammo.grenadeBullets[i].state == BS_EXPLOSING) {
				for (int j = 0; j < healerEnemies.size(); j++) {
					if (Collision::PixelPerfectTest(player.ammo.grenadeBullets[i].firstDamageArea, healerEnemies[j].shape)) {
						healerEnemies[j].takeDamage(player.ammoData.grenadeBulletData.firstLevelDamage);
					}
					if (Collision::PixelPerfectTest(player.ammo.grenadeBullets[i].secondDamageArea, healerEnemies[j].shape)) {
						healerEnemies[j].takeDamage(player.ammoData.grenadeBulletData.secondLevelDamage);
					}
					if (Collision::PixelPerfectTest(player.ammo.grenadeBullets[i].thirdDamageArea, healerEnemies[j].shape)) {
						healerEnemies[j].takeDamage(player.ammoData.grenadeBulletData.thirdLevelDamage);
					}
				}
			}
		}
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

	void checkForEffectAreaCollisions() {
		//healerEnemies HEAL AREA
		healerRays.clear();
		//roma enemies
		for (int i = 0; i < healerEnemies.size(); i++) {
			if (healerEnemies[i].isHealAreaActive) {
				for (int j = 0; j < romaEnemies.size(); j++) {
					if (romaEnemies[j].shape.getOutlineThickness() < 0) {
						if (Collision::PixelPerfectTest(healerEnemies[i].healArea, romaEnemies[j].shape)) {
							HealerEnemyRay healRay;
							healRay.shape.setTexture(healerEnemies[i].rayTxtrPtr);
							healRay.shape.setPosition(healerEnemies[i].shape.getPosition());
							healRay.shape.setSize(Vector2f(sqrt(pow(healerEnemies[i].shape.getPosition().x - romaEnemies[j].shape.getPosition().x, 2) + pow(healerEnemies[i].shape.getPosition().y - romaEnemies[j].shape.getPosition().y, 2)), 10));
							healRay.shape.setOrigin(Vector2f(0, healRay.shape.getSize().y / 2));
							healRay.takeTarget(romaEnemies[j].shape.getPosition());
							healerRays.push_back(healRay);
							if (healerEnemies[i].isCanHeal()) {
								romaEnemies[j].heal(healerData.heal);
								healerEnemies[i].excessFireTime = 0;
							}
						}
					}
				}
			}
		}
		//rock enemies
		for (int i = 0; i < healerEnemies.size(); i++) {
			if (healerEnemies[i].isHealAreaActive) {
				for (int j = 0; j < rockEnemies.size(); j++) {
					if (rockEnemies[j].shape.getOutlineThickness() < 0) {
						if (Collision::PixelPerfectTest(healerEnemies[i].healArea, rockEnemies[j].shape)) {
							HealerEnemyRay healRay;
							healRay.shape.setTexture(healerEnemies[i].rayTxtrPtr);
							healRay.shape.setPosition(healerEnemies[i].shape.getPosition());
							healRay.shape.setSize(Vector2f(sqrt(pow(healerEnemies[i].shape.getPosition().x - rockEnemies[j].shape.getPosition().x, 2) + pow(healerEnemies[i].shape.getPosition().y - rockEnemies[j].shape.getPosition().y, 2)), 10));
							healRay.shape.setOrigin(Vector2f(0, healRay.shape.getSize().y / 2));
							healRay.takeTarget(rockEnemies[j].shape.getPosition());
							healerRays.push_back(healRay);
							if (healerEnemies[i].isCanHeal()) {
								rockEnemies[j].heal(healerData.heal);
								healerEnemies[i].excessFireTime = 0;
							}
						}
					}
				}
			}
		}
		//electro enemies
		for (int i = 0; i < healerEnemies.size(); i++) {
			if (healerEnemies[i].isHealAreaActive) {
				for (int j = 0; j < electroEnemies.size(); j++) {
					if (electroEnemies[j].shape.getOutlineThickness() < 0) {
						if (electroEnemies[j].visible) {
							if (Collision::PixelPerfectTest(healerEnemies[i].healArea, electroEnemies[j].shape)) {
								HealerEnemyRay healRay;
								healRay.shape.setTexture(healerEnemies[i].rayTxtrPtr);
								healRay.shape.setPosition(healerEnemies[i].shape.getPosition());
								healRay.shape.setSize(Vector2f(sqrt(pow(healerEnemies[i].shape.getPosition().x - electroEnemies[j].shape.getPosition().x, 2) + pow(healerEnemies[i].shape.getPosition().y - electroEnemies[j].shape.getPosition().y, 2)), 10));
								healRay.shape.setOrigin(Vector2f(0, healRay.shape.getSize().y / 2));
								healRay.takeTarget(electroEnemies[j].shape.getPosition());
								healerRays.push_back(healRay);
								if (healerEnemies[i].isCanHeal()) {
									electroEnemies[j].heal(healerData.heal);
									healerEnemies[i].excessFireTime = 0;
								}
							}
						}
					}
				}
			}
		}
		//healer enemies
		for (int i = 0; i < healerEnemies.size(); i++) {
			if (healerEnemies[i].isHealAreaActive) {
				for (int j = 0; j < healerEnemies.size(); j++) {
					if (healerEnemies[j].shape.getOutlineThickness() < 0) {
						if (i != j) {
							if (Collision::PixelPerfectTest(healerEnemies[i].healArea, healerEnemies[j].shape)) {
								HealerEnemyRay healRay;
								healRay.shape.setTexture(healerEnemies[i].rayTxtrPtr);
								healRay.shape.setPosition(healerEnemies[i].shape.getPosition());
								healRay.shape.setSize(Vector2f(sqrt(pow(healerEnemies[i].shape.getPosition().x - healerEnemies[j].shape.getPosition().x, 2) + pow(healerEnemies[i].shape.getPosition().y - healerEnemies[j].shape.getPosition().y, 2)), 10));
								healRay.shape.setOrigin(Vector2f(0, healRay.shape.getSize().y / 2));
								healRay.takeTarget(healerEnemies[j].shape.getPosition());
								healerRays.push_back(healRay);
								if (healerEnemies[i].isCanHeal()) {
									healerEnemies[j].heal(healerData.heal);
									healerEnemies[i].excessFireTime = 0;
								}
							}
						}
					}
				}
			}
		}
	}


	void checkForCollisions() {
		//efrects
		checkForEffectAreaCollisions();
		//player
		checkForPlayerCollisions();
		//bullets
		checkForOrdinaryBulletsCollision();
		checkForSplittingBulletsCollision();
		checkForSplittedBulletsCollision();
		checkForRayBulletsCollision();
		checkForGrenadeBulletCollision();
		//
	}

	
	bool isRomaEnemyNeedToSpawn() {
		return (romaData.areActive == true && romaEnemies.size() < romaData.maxAmount && rand() % 1500 == 1);
	}

	bool isRockEnemyNeedToSpawn() {
		return (rockData.areActive == true && rockEnemies.size() < rockData.maxAmount && rand() % 1800 == 1);
	}

	bool isElectroEnemyNeedToSpawn() {
		return (electroData.areActive == true && electroEnemies.size() < electroData.maxAmount && rand() % 3100 == 1);
	}

	bool isHealerEnemyNeedToSpawn() {
		return (healerData.areActive == true && healerEnemies.size() < healerData.maxAmount && rand() % 2600 == 1);
	}

	void spawnRomaEnemy() {
		Enemy::RomaEnemy roma;
		roma.shape.setTexture(&romaData.enemyTexture);
		roma.bulletTxtrPtr = &romaData.bulletTexture;
		roma.fireDelayAsMilliseconds = 4200;
		roma.spawnCoordX = gameWindow.x - romaData.spawnRadius;
		roma.shape.setRadius(romaData.spawnRadius);
		roma.shape.setOrigin(Vector2f(romaData.spawnRadius, romaData.spawnRadius));
		roma.shape.setPosition(Vector2f(gameWindow.x + romaData.spawnRadius, rand() % (gameWindow.y - romaData.spawnRadius * 4) + romaData.spawnRadius * 2));
		roma.setState("ES_SPAWN_ANIM");
		romaEnemies.push_back(roma);
	}
	
	void spawnRockEnemy() {
		Enemy::RockEnemy rock;
		rock.shape.setTexture(&rockData.enemyTexture);
		rock.bulletTxtrPtr = &rockData.bulletTexture;
		rock.fireDelayAsMilliseconds = 900;
		rock.shape.setRotation(0);
		rock.defaultBulletSpeed = { 2.0, 0 };
		rock.bulletSpeedVariation = rock.defaultBulletSpeed.x / 90;
		rock.shape.setRadius(rockData.spawnRadius);
		rock.shape.setOrigin(rockData.spawnRadius, rockData.spawnRadius);
		rock.side = ENEMYSIDE(rand() % 2);
		switch (rock.side) {
		case S_UP:
			rock.shape.setPosition(Vector2f(rand() % (gameWindow.x / 2) + static_cast<float>(gameWindow.x) * 2 / 3, 0 - rockData.spawnRadius));
			rock.destinationCoordY = gameWindow.y - rock.shape.getRadius();
			break;
		case S_DOWN:
			rock.shape.setPosition(Vector2f(rand() % (gameWindow.x / 2) + static_cast<float>(gameWindow.x) * 2 / 3, gameWindow.y + rockData.spawnRadius));
			rock.destinationCoordY = 0 + rock.shape.getRadius();
			break;
		}
		rock.setState("ES_MOVING");
		rockEnemies.push_back(rock);
	}

	void spawnElectroEnemy() {
		Enemy::ElectroEnemy electro;
		electro.shape.setTexture(&electroData.enemyTexture);
		electro.lightningTxtrPtr = &electroData.lightningTexture;
		electro.fireDelayAsMilliseconds = 1500;
		electro.shape.setRotation(270);
		electro.shape.setRadius(electroData.spawnRadius);
		electro.shape.setOrigin(electroData.spawnRadius, electroData.spawnRadius);
		electro.shape.setPosition(Vector2f(gameWindow.x + electro.shape.getRadius(), rand() % (gameWindow.y - electroData.spawnRadius * 4) + electroData.spawnRadius * 2));
		electro.destinationCoordX = gameWindow.x / 100 * 15;
		electroEnemies.push_back(electro);
	}

	void spawnHealerEnemy() {
		Enemy::HealerEnemy healer;
		healer.shape.setTexture(&healerData.enemyTexture);
		healer.shape.setRadius(healerData.spawnRadius);
		healer.shape.setOrigin(healerData.spawnRadius, healerData.spawnRadius);
		healer.rayTxtrPtr = &healerData.rayTexture;
		healer.fireDelayAsMilliseconds = 100;
		healer.spawnCoordY = gameWindow.y / 2;
		healer.side = ENEMYSIDE(rand() % 2);
		switch (healer.side) {
		case S_UP:
			healer.shape.setPosition(Vector2f(rand() % (gameWindow.x / 3) + gameWindow.x / 2, 0 - healerData.spawnRadius));
			break;
		case S_DOWN:
			healer.shape.setPosition(Vector2f(rand() % (gameWindow.x / 3) + gameWindow.x / 2, gameWindow.y + healerData.spawnRadius));
			break;
		}
		healer.isHealAreaActive = false;
		healer.healArea.setTexture(&healerData.healAreaTexture);
		healer.healArea.setFillColor(Color(healer.healArea.getFillColor().r, healer.healArea.getFillColor().g, healer.healArea.getFillColor().b, 30));
		healer.healArea.setRadius(healerData.healAreaRadius);
		healer.healArea.setOrigin(healerData.healAreaRadius, healerData.healAreaRadius);
		healer.healArea.setPosition(healer.shape.getPosition());
		healer.setState("ES_SPAWN_ANIM");
		healerEnemies.push_back(healer);
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
		if (isHealerEnemyNeedToSpawn()) {
			spawnHealerEnemy();
		}
	}

	void updatePlayer() {
		if (player.getHPAmount() <= 0) {
			gameState = GS_MENU;
		}
		player.controlPlayer(&gameWindow);
		player.rotateToMouse(static_cast<Vector2f>(Mouse::getPosition(gameWindow.window)));
		player.checkForBulletSwap();
		player.updateAdditionalScopePart();
	}
	void updatePlayerBullets() {
		//ORDINARY BULLETS
		for (int i = 0; i < player.ammo.ordinaryBullets.size(); i++) {
			if (player.ammo.ordinaryBullets[i].isOutOfScreen(&gameWindow)) {
				player.deleteBullet(BULT_ORDINARY, i);
			}
			else {
				player.ammo.ordinaryBullets[i].move();
			}
		}
		//SPLITTING BULLETS
		for (int i = 0; i < player.ammo.splittingBullets.size(); i++) {
			if (player.ammo.splittingBullets[i].isOutOfScreen(&gameWindow)) {
				player.deleteBullet(BULT_SPLITTING, i);
			}
			else {
				player.ammo.splittingBullets[i].move();
			}
		}
		//SPLITTED BULLETS
		for (int i = 0; i < player.ammo.splittedBullets.size(); i++) {
			if (player.ammo.splittedBullets[i].isOutOfScreen(&gameWindow)) {
				player.deleteBullet(BULT_SPLITTED, i);
			}
			else {
				player.ammo.splittedBullets[i].move();
			}
		}
		//RAY BULLETS
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
		//GRENADE BULLETS
		for (int i = 0; i < player.ammo.grenadeBullets.size(); i++) {
			switch (player.ammo.grenadeBullets[i].state) {
			case BS_EXPLOSING:
				player.deleteBullet(BULT_GRENADE, i);
				break;
			case BS_FLYING:
				player.ammo.grenadeBullets[i].move();
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
				i--;
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
				i--;
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
				if (electroEnemies[i].visibleClock.getElapsedTime().asMilliseconds() - electroEnemies[i].excessVisibleTime >= electroData.visibleDelayAsMilliseconds) {
					electroEnemies[i].toggleVisible();
					electroEnemies[i].moveRandomCoordY(&gameWindow);
				}
			}
			if (electroEnemies[i].getState() == ES_STANDING) {
				electroEnemies[i].takeTarget(player.playerShape.getPosition());
			}
			if (!electroEnemies[i].isAlive()) {
				electroEnemies.erase(electroEnemies.begin() + i);
				i--;
			}
		}
		//HEALER ENEMIES
		for (int i = 0; i < healerEnemies.size(); i++) {
			if (healerEnemies[i].getState() == ES_SPAWN_ANIM) {
				healerEnemies[i].spawnAnimation();
			}
			if (healerEnemies[i].getState() == ES_MOVING) {
				healerEnemies[i].move();
				healerEnemies[i].heal(0.01);
			}
			if (healerEnemies[i].getState() == ES_STANDING) {
				healerEnemies[i].generateDestination(&gameWindow);
			}
			if (!healerEnemies[i].isAlive()) {
				healerEnemies.erase(healerEnemies.begin() + i);
				i--;
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
			if (electroLightnings[i].visible_lightningClock.getElapsedTime().asMilliseconds() - electroLightnings[i].excessVisible_lightningTime > electroData.visible_lightningDelayAsMilliseconds) {
				electroLightnings.erase(electroLightnings.begin() + i);
			}
		}
	}
	void drawNewGameFrame(bool isClearAndDisplay = true) {
		if (isClearAndDisplay) {	//this code need for updatePauseFrame()
			gameWindow.window.clear();
		}
		gameWindow.window.draw(gameBackground);

		//ENEMIES
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
		for (int i = 0; i < healerEnemies.size(); i++) {
			if (healerEnemies[i].isHealAreaActive) {
				gameWindow.window.draw(healerEnemies[i].healArea);
			}
			gameWindow.window.draw(healerEnemies[i].shape);
		}

		//PLAYER BULLETS
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
		for (int i = 0; i < player.ammo.grenadeBullets.size(); i++) {
			gameWindow.window.draw(player.ammo.grenadeBullets[i].thirdDamageArea);
			gameWindow.window.draw(player.ammo.grenadeBullets[i].secondDamageArea);
			gameWindow.window.draw(player.ammo.grenadeBullets[i].firstDamageArea);
			gameWindow.window.draw(player.ammo.grenadeBullets[i].shape);
		}
			 
		gameWindow.window.draw(player.scope);
		if (player.additionalScope.isActive) {
			gameWindow.window.draw(player.additionalScope.shape);
		}
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
		for (int i = 0; i < healerRays.size(); i++) {
			gameWindow.window.draw(healerRays[i].shape);
		}
		if (isClearAndDisplay) {	//this code need for updatePauseFrame()
			gameWindow.window.display();
		}
	}

	//UPDATE FRAMES
	void updateMenuFrame(Game *gameClass) {
		menu.controlMenu();
		menu.selectMenuButton(gameClass);
		gameWindow.window.clear();
		gameWindow.window.draw(menu.menuBackground);
		gameWindow.window.draw(menu.caption);
		gameWindow.window.draw(menu.buttons[menu.getSelectedButton()].buttonBackground);
		for (int i = 0; i < menu.buttons.size(); i++) {
			gameWindow.window.draw(menu.buttons[i].title);
		}
		gameWindow.window.display();
	}

	void updateGameFrame() {
		if (Keyboard::isKeyPressed(Keyboard::Key::Escape) && delayBetweenEscapePresses.getElapsedTime().asMilliseconds() > 1000) {
			pauseGame();
			delayBetweenEscapePresses.restart();
		}
		checkForCollisions();
		updatePlayer();
		updatePlayerBullets();
		updateEnemies();
		updateEnemyBullets();
		drawNewGameFrame();

		debugging();
	}

	void updatePauseFrame(Game *gameClass) {
		pause.controlPause();
		pause.selectPauseButton(gameClass);
		gameWindow.window.clear();
		drawNewGameFrame(false);
		gameWindow.window.draw(pause.pauseBackgroundBlur);
		gameWindow.window.draw(pause.pauseBox);
		gameWindow.window.draw(pause.caption);
		gameWindow.window.draw(pause.buttons[pause.getSelectedButton()].buttonBackground);
		for (int i = 0; i < pause.buttons.size(); i++) {
			gameWindow.window.draw(pause.buttons[i].title);
		}
		gameWindow.window.display();
	}
};
int main() {
	srand(static_cast<unsigned int>(time(0)));
	rand();
	Game BG;
	BG.menu.initMenu(&BG.gameWindow);
	BG.menu.addMenuButton("PLAY", &Game::initGame);
	BG.menu.addMenuButton("EXIT", &Game::exitGame);
	BG.pause.initPause(&BG.gameWindow);
	BG.pause.addPauseButton("CONTINUE", &Game::continueGame, &BG.gameWindow);
	BG.pause.addPauseButton("MAIN MENU", &Game::backToMenu, &BG.gameWindow);
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
		case GS_PAUSE:
			BG.updatePauseFrame(&BG);
			break;
		case GS_EXIT:
			BG.gameWindow.window.close();
			break;
		}
	}
}