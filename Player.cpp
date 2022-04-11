#include "Player.h"
#include "Collision.h"

//ADDITIONAL SCOPE
void AdditionalScope::toggleActive() {
	isActive = !isActive;
	if (isActive) {
		shape.setFillColor(Color(shape.getFillColor().r, shape.getFillColor().g, shape.getFillColor().b, 255));
	}
}


//PLAYER
float Player::getDeltaTime() {
	return deltaTime.restart().asSeconds();
}
void Player::initPlayer(GameWindow* gwindow) {
	Collision::CreateTextureAndBitmask(playertexture1HP, "Textures\\Player\\playerTexture1HP.jpg");
	Collision::CreateTextureAndBitmask(playertexture2HP, "Textures\\Player\\playerTexture2HP.jpg");
	Collision::CreateTextureAndBitmask(playertexture3HP, "Textures\\Player\\playerTexture3HP.jpg");
	HPAmount = 3;
	updatePlayerTexture();
	ordinaryBulletScopeTexture.loadFromFile("Textures\\Player\\Scopes\\ordinaryBulletScopeTexture.png");
	splittingBulletScopeTexture.loadFromFile("Textures\\Player\\Scopes\\splittingBulletScopeTexture.png");
	rayBulletScopeTexture.loadFromFile("Textures\\Player\\Scopes\\rayBulletScopeTexture.png");
	rayBulletAdditionalScopeTexture.loadFromFile("Textures\\Player\\Scopes\\rayBulletScopeTexture2.png");
	rocketBulletScopeTexture.loadFromFile("Textures\\Player\\Scopes\\rocketBulletScopeTexture.png");
	sizeX = 90;
	sizeY = 50;
	speed = { 450, 450 };
	playerShape.setSize(Vector2f(sizeX, sizeY));
	playerShape.setOrigin(Vector2f(sizeX / 2, sizeY / 2));
	playerShape.setPosition(Vector2f(gwindow->x / 2, gwindow->y / 2));
	scope.setSize(Vector2f(70, 26));
	scope.setOrigin(Vector2f(0, scope.getSize().y / 2));
	scope.setPosition(Vector2f(playerShape.getPosition().x, playerShape.getPosition().y));
	scope.setTextureRect(IntRect(0, 0, 70, 26));
	scope.setTexture(&ordinaryBulletScopeTexture);
	additionalScope.isActive = false;
}
void Player::initAmmunition() {
	selectedBullet = BULT_ORDINARY;
	//ORDINARY BULLET
	Collision::CreateTextureAndBitmask(ammoData.ordinaryBulletData.texture, "Textures\\Player\\Bullets\\ordinaryBullet.jpg");
	ammoData.ordinaryBulletData.damage = 10;
	ammoData.ordinaryBulletData.fireDelayAsMilliseconds = 150;
	ammoData.ordinaryBulletData.excessFireDelay = 0;

	//SPLITTING BULLET
	Collision::CreateTextureAndBitmask(ammoData.splittingBulletData.texture, "Textures\\Player\\Bullets\\splittingBullet.png");
	ammoData.splittingBulletData.defaultDamage = 30;
	ammoData.splittingBulletData.defaultRadius = 25;
	ammoData.splittingBulletData.fireDelayAsMilliseconds = 600;
	ammoData.splittingBulletData.excessFireDelay = 0;

	//RAY BULLET
	Collision::CreateTextureAndBitmask(ammoData.rayBulletData.texture, "Textures\\Player\\Bullets\\rayBullet.png");
	ammoData.rayBulletData.texture.setSmooth(true);
	ammoData.rayBulletData.damage = 120;
	ammoData.rayBulletData.delayBeforeDissapearAsMilliseconds = 100;
	ammoData.rayBulletData.fireDelayAsMilliseconds = 1400;
	ammoData.rayBulletData.excessFireDelay = 0;

	//ROCKET BULLET
	ammoData.rocketBulletData.texture.loadFromFile("Textures\\Player\\Bullets\\rocketBullet.png");
	Collision::CreateTextureAndBitmask(ammoData.rocketBulletData.firstAreaTexture, "Textures\\Player\\Bullets\\rocketFirstDamageArea.png");
	Collision::CreateTextureAndBitmask(ammoData.rocketBulletData.secondAreaTexture, "Textures\\Player\\Bullets\\rocketSecondDamageArea.png");
	Collision::CreateTextureAndBitmask(ammoData.rocketBulletData.thirdAreaTexture, "Textures\\Player\\Bullets\\rocketThirdDamageArea.png");
	ammoData.rocketBulletData.firstAreaTexture.setSmooth(true);
	ammoData.rocketBulletData.secondAreaTexture.setSmooth(true);
	ammoData.rocketBulletData.thirdAreaTexture.setSmooth(true);

	ammoData.rocketBulletData.fireDelayAsMilliseconds = 1200;

	ammoData.rocketBulletData.firstLevelDamage = 70;
	ammoData.rocketBulletData.secondLevelDamage = 30;
	ammoData.rocketBulletData.thirdLevelDamage = 15;

	ammoData.rocketBulletData.size = { 30, 90 };

	ammoData.rocketBulletData.firstAreaRaduis = 100;
	ammoData.rocketBulletData.secondAreaRaduis = 200;
	ammoData.rocketBulletData.thirdAreaRaduis = 350;

	ammoData.rocketBulletData.excessFireDelay = 0;

	ammo.ordinaryBullets.clear();
	ammo.splittingBullets.clear();
	ammo.splittedBullets.clear();
	ammo.rayBullets.clear();
	ammo.rocketBullets.clear();
}
void Player::updatePlayerTexture() {
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
void Player::takeDamage() {
	HPAmount--;
	updatePlayerTexture();
}
BULLETTYPE Player::getSelectedBulletType() {
	return selectedBullet;
}
void Player::rotateTo(Vector2f coords) {
	playerShape.setRotation((atan2(playerShape.getPosition().y - coords.y, playerShape.getPosition().x - coords.x)) * 180 / 3.14159265 - 90);
	scope.setRotation(playerShape.getRotation() - 90);
	additionalScope.shape.setRotation(playerShape.getRotation() - 90);
}
void Player::controlPlayer(GameWindow* gwindow) {
	float deltaT = getDeltaTime();
	if (Mouse::isButtonPressed(Mouse::Button::Left)) {
		fire();
	}
	if (Keyboard::isKeyPressed(Keyboard::Key::W) && playerShape.getPosition().y - sizeX / 2 > 0) {
		move({ 0, -speed.y * deltaT });
	}
	if (Keyboard::isKeyPressed(Keyboard::Key::S) && playerShape.getPosition().y + sizeX / 2 < gwindow->y) {
		move({ 0, speed.y * deltaT });
	}
	if (Keyboard::isKeyPressed(Keyboard::Key::A) && playerShape.getPosition().x - sizeY / 2 > 0) {
		move({ -speed.x * deltaT, 0 });
	}
	if (Keyboard::isKeyPressed(Keyboard::Key::D) && playerShape.getPosition().x + sizeY / 2 < gwindow->x) {
		move({ speed.x * deltaT, 0 });
	}
}
void Player::updateAdditionalScopePart() {
	switch (selectedBullet) {
	case BULT_RAY:
		if (additionalScope.isActive) {
			if (additionalScope.shape.getFillColor().a > 0) {
				additionalScope.shape.setFillColor(Color(additionalScope.shape.getFillColor().r, additionalScope.shape.getFillColor().g, additionalScope.shape.getFillColor().b, additionalScope.shape.getFillColor().a - 1));
			}
			else {
				additionalScope.toggleActive();
			}
		}
		break;
	}
}
void Player::move(Vector2f offset) {
	playerShape.move(offset);
	scope.move(offset);
	additionalScope.shape.move(offset);
}
void Player::fire() {
	switch (selectedBullet) {
	case BULT_ORDINARY:
		if (ammoData.ordinaryBulletData.fireTimer.getElapsedTime().asMilliseconds() - ammoData.ordinaryBulletData.excessFireDelay > ammoData.ordinaryBulletData.fireDelayAsMilliseconds) {
			OrdinaryBullet bullet;
			bullet.shape.setSize(Vector2f(32.f, 19.f));
			bullet.shape.setTexture(&ammoData.ordinaryBulletData.texture);
			bullet.shape.setOrigin(Vector2f(bullet.shape.getSize().x / 2.f, bullet.shape.getSize().y / 2.f));
			bullet.shape.setPosition(Vector2f(playerShape.getPosition().x, (rand() % static_cast<int>(sizeX)) + playerShape.getPosition().y - playerShape.getOrigin().x));
			bullet.shape.setRotation(playerShape.getRotation() - 90);
			bullet.acceleration = 2300;
			bullet.speed.x = bullet.acceleration * cos((playerShape.getRotation() - 90) * 3.14 / 180);
			bullet.speed.y = bullet.acceleration * sin((playerShape.getRotation() - 90) * 3.14 / 180);
			ammo.ordinaryBullets.push_back(bullet);
			ammoData.ordinaryBulletData.fireTimer.restart();
			ammoData.ordinaryBulletData.excessFireDelay = 0;
			return;
		}
		break;
	case BULT_SPLITTING:
		if (ammoData.splittingBulletData.fireTimer.getElapsedTime().asMilliseconds() - ammoData.splittingBulletData.excessFireDelay > ammoData.splittingBulletData.fireDelayAsMilliseconds) {
			SplittingBullet bullet;
			bullet.shape.setRadius(ammoData.splittingBulletData.defaultRadius);
			bullet.shape.setTexture(&ammoData.splittingBulletData.texture);
			bullet.shape.setOrigin(Vector2f(ammoData.splittingBulletData.defaultRadius, ammoData.splittingBulletData.defaultRadius));
			bullet.shape.setPosition(Vector2f(playerShape.getPosition().x, playerShape.getPosition().y));
			bullet.shape.setRotation(playerShape.getRotation());
			bullet.damage = ammoData.splittingBulletData.defaultDamage;
			bullet.acceleration = 1200;
			bullet.speed.x = bullet.acceleration * cos((playerShape.getRotation() - 90) * 3.14 / 180);
			bullet.speed.y = bullet.acceleration * sin((playerShape.getRotation() - 90) * 3.14 / 180);
			ammo.splittingBullets.push_back(bullet);
			ammoData.splittingBulletData.fireTimer.restart();
			ammoData.splittingBulletData.excessFireDelay = 0;
			return;
		}
		break;
	case BULT_RAY:
		if (ammoData.rayBulletData.fireTimer.getElapsedTime().asMilliseconds() - ammoData.rayBulletData.excessFireDelay > ammoData.rayBulletData.fireDelayAsMilliseconds) {
			RayBullet bullet;
			bullet.shape.setSize(Vector2f(0, 20));
			bullet.shape.setTexture(&ammoData.rayBulletData.texture);
			bullet.shape.setOrigin(Vector2f(0, bullet.shape.getSize().y / 2));
			bullet.shape.setRotation(playerShape.getRotation() - 90);
			bullet.shape.setPosition(Vector2f(playerShape.getPosition()));
			bullet.state = RAY_FIRING;
			additionalScope.toggleActive();
			ammo.rayBullets.push_back(bullet);
			ammoData.rayBulletData.fireTimer.restart();
			ammoData.rayBulletData.excessFireDelay = 0;
			return;
		}
		break;
	case BULT_ROCKET:
		if (ammoData.rocketBulletData.fireTimer.getElapsedTime().asMilliseconds() - ammoData.rocketBulletData.excessFireDelay > ammoData.rocketBulletData.fireDelayAsMilliseconds) {
			RocketBullet bullet;
			//setting raduis
			bullet.shape.setSize(ammoData.rocketBulletData.size);
			bullet.firstDamageArea.setRadius(ammoData.rocketBulletData.firstAreaRaduis);
			bullet.secondDamageArea.setRadius(ammoData.rocketBulletData.secondAreaRaduis);
			bullet.thirdDamageArea.setRadius(ammoData.rocketBulletData.thirdAreaRaduis);
			//setting textures
			bullet.shape.setTexture(&ammoData.rocketBulletData.texture);
			bullet.firstDamageArea.setTexture(&ammoData.rocketBulletData.firstAreaTexture);
			bullet.secondDamageArea.setTexture(&ammoData.rocketBulletData.secondAreaTexture);
			bullet.thirdDamageArea.setTexture(&ammoData.rocketBulletData.thirdAreaTexture);
			//setting origins
			bullet.shape.setOrigin(ammoData.rocketBulletData.size.x / 2, ammoData.rocketBulletData.size.y / 2);
			bullet.firstDamageArea.setOrigin(ammoData.rocketBulletData.firstAreaRaduis, ammoData.rocketBulletData.firstAreaRaduis);
			bullet.secondDamageArea.setOrigin(ammoData.rocketBulletData.secondAreaRaduis, ammoData.rocketBulletData.secondAreaRaduis);
			bullet.thirdDamageArea.setOrigin(ammoData.rocketBulletData.thirdAreaRaduis, ammoData.rocketBulletData.thirdAreaRaduis);
			//setting rotation
			bullet.shape.setRotation(playerShape.getRotation());
			//setting position
			bullet.shape.setPosition(playerShape.getPosition());
			bullet.firstDamageArea.setPosition(playerShape.getPosition());
			bullet.secondDamageArea.setPosition(playerShape.getPosition());
			bullet.thirdDamageArea.setPosition(playerShape.getPosition());

			bullet.destinationCoords = Mouse::getPosition();
			bullet.speed.x = (static_cast<float>(bullet.destinationCoords.x) - bullet.shape.getPosition().x);	//speed * deltaT == 1secs to destination
			bullet.speed.y = (static_cast<float>(bullet.destinationCoords.y) - bullet.shape.getPosition().y);	////////////////////////////////////////////////
			bullet.state = ROCKET_MOVE;
			ammo.rocketBullets.push_back(bullet);
			ammoData.rocketBulletData.fireTimer.restart();
			ammoData.rocketBulletData.excessFireDelay = 0;
		}
		break;
	}
}
void Player::splitBullet(SplittingBullet* splittingBullet) {
	SplittedBullet splittedBullet;
	splittedBullet.shape.setTexture(&ammoData.splittingBulletData.texture);
	splittedBullet.shape.setPosition(splittingBullet->shape.getPosition());
	splittedBullet.shape.setRadius(splittingBullet->shape.getRadius() / 1.5);
	splittedBullet.damage = splittingBullet->damage / 1.5;
	splittedBullet.acceleration = splittingBullet->acceleration;
	splittedBullet.levelOfSplit = 1;


	splittedBullet.shape.setRotation(splittingBullet->shape.getRotation());
	splittedBullet.shape.rotate(-9);
	splittedBullet.speed.x = splittedBullet.acceleration * cos((splittedBullet.shape.getRotation() - 90) * 3.14 / 180);
	splittedBullet.speed.y = splittedBullet.acceleration * sin((splittedBullet.shape.getRotation() - 90) * 3.14 / 180);
	ammo.splittedBullets.push_back(splittedBullet);

	splittedBullet.shape.setRotation(splittingBullet->shape.getRotation());
	splittedBullet.shape.rotate(9);
	splittedBullet.speed.x = splittedBullet.acceleration * cos((splittedBullet.shape.getRotation() - 90) * 3.14 / 180);
	splittedBullet.speed.y = splittedBullet.acceleration * sin((splittedBullet.shape.getRotation() - 90) * 3.14 / 180);
	ammo.splittedBullets.push_back(splittedBullet);
}
void Player::splitBullet(SplittedBullet* splittedBullet) {
	if (splittedBullet->levelOfSplit < 2) {
		SplittedBullet twiceSplittedBullet;
		twiceSplittedBullet.shape.setTexture(&ammoData.splittingBulletData.texture);
		twiceSplittedBullet.shape.setPosition(splittedBullet->shape.getPosition());
		twiceSplittedBullet.shape.setRadius(splittedBullet->shape.getRadius() / 1.5);
		twiceSplittedBullet.damage = splittedBullet->damage / 1.5;
		twiceSplittedBullet.acceleration = splittedBullet->acceleration;
		twiceSplittedBullet.levelOfSplit = splittedBullet->levelOfSplit + 1;


		twiceSplittedBullet.shape.setRotation(splittedBullet->shape.getRotation());
		twiceSplittedBullet.shape.rotate(-13);
		twiceSplittedBullet.speed.x = twiceSplittedBullet.acceleration * cos((twiceSplittedBullet.shape.getRotation() - 90) * 3.14 / 180);
		twiceSplittedBullet.speed.y = twiceSplittedBullet.acceleration * sin((twiceSplittedBullet.shape.getRotation() - 90) * 3.14 / 180);
		ammo.splittedBullets.push_back(twiceSplittedBullet);

		twiceSplittedBullet.shape.setRotation(splittedBullet->shape.getRotation());
		twiceSplittedBullet.shape.rotate(13);
		twiceSplittedBullet.speed.x = twiceSplittedBullet.acceleration * cos((twiceSplittedBullet.shape.getRotation() - 90) * 3.14 / 180);
		twiceSplittedBullet.speed.y = twiceSplittedBullet.acceleration * sin((twiceSplittedBullet.shape.getRotation() - 90) * 3.14 / 180);
		ammo.splittedBullets.push_back(twiceSplittedBullet);
	}
}
void Player::deleteBullet(BULLETTYPE bulletType, int index) {
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
	case BULT_ROCKET:
		ammo.rocketBullets.erase(ammo.rocketBullets.begin() + index);
		break;
	}
}
void Player::checkForBulletSwap() {
	if (Keyboard::isKeyPressed(Keyboard::Num1) && selectedBullet != BULT_ORDINARY) {
		selectedBullet = BULT_ORDINARY;
		scope.setSize(Vector2f(70, 26));
		scope.setOrigin(Vector2f(0, scope.getSize().y / 2));
		scope.setPosition(Vector2f(playerShape.getPosition().x, playerShape.getPosition().y));
		scope.setTextureRect(IntRect(0, 0, 70, 26));
		scope.setTexture(&ordinaryBulletScopeTexture);
		additionalScope.isActive = false;
	}
	if (Keyboard::isKeyPressed(Keyboard::Num2) && selectedBullet != BULT_SPLITTING) {
		selectedBullet = BULT_SPLITTING;
		scope.setSize(Vector2f(70, 30));
		scope.setOrigin(Vector2f(0, scope.getSize().y / 2));
		scope.setPosition(Vector2f(playerShape.getPosition().x, playerShape.getPosition().y));
		scope.setTextureRect(IntRect(0, 0, 70, 30));
		scope.setTexture(&splittingBulletScopeTexture);
		additionalScope.isActive = false;
	}
	if (Keyboard::isKeyPressed(Keyboard::Num3) && selectedBullet != BULT_RAY) {
		selectedBullet = BULT_RAY;
		scope.setSize(Vector2f(76, 38));
		scope.setOrigin(Vector2f(0, scope.getSize().y / 2));
		scope.setPosition(playerShape.getPosition());
		scope.setTextureRect(IntRect(0, 0, 76, 38));
		scope.setTexture(&rayBulletScopeTexture);
		additionalScope.shape.setSize(Vector2f(76, 38));
		additionalScope.shape.setOrigin(Vector2f(0, additionalScope.shape.getSize().y / 2));
		additionalScope.shape.setPosition(playerShape.getPosition());
		additionalScope.shape.setTextureRect(IntRect(0, 0, 76, 38));
		additionalScope.shape.setTexture(&rayBulletAdditionalScopeTexture);
		additionalScope.isActive = false;
	}
	if (Keyboard::isKeyPressed(Keyboard::Num4) && selectedBullet != BULT_ROCKET) {
		selectedBullet = BULT_ROCKET;
		scope.setSize(Vector2f(70, 24));
		scope.setOrigin(Vector2f(0, scope.getSize().y / 2));
		scope.setPosition(playerShape.getPosition());
		scope.setTextureRect(IntRect(0, 0, 70, 24));
		scope.setTexture(&rocketBulletScopeTexture);
		additionalScope.isActive = false;
	}
}
bool Player::isAlive() {
	return HPAmount > 0;
}

void Player::updateOrdinaryBullets(GameWindow *gwindow) {
	for (int i = 0; i < ammo.ordinaryBullets.size(); i++) {
		if (ammo.ordinaryBullets[i].isOutOfScreen(gwindow)) {
			deleteBullet(BULT_ORDINARY, i);
			i--;
		}
		else {
			ammo.ordinaryBullets[i].move();
		}
	}
}
void Player::updateSplittingBullets(GameWindow *gwindow) {
	for (int i = 0; i < ammo.splittingBullets.size(); i++) {
		if (ammo.splittingBullets[i].isOutOfScreen(gwindow)) {
			deleteBullet(BULT_SPLITTING, i);
			i--;
		}
		else {
			ammo.splittingBullets[i].move();
		}
	}
}
void Player::updateSplittedBullets(GameWindow *gwindow) {
	for (int i = 0; i < ammo.splittedBullets.size(); i++) {
		if (ammo.splittedBullets[i].isOutOfScreen(gwindow)) {
			deleteBullet(BULT_SPLITTED, i);
			i--;
		}
		else {
			ammo.splittedBullets[i].move();
		}
	}
}
void Player::updateRayBullets() {
	for (int i = 0; i < ammo.rayBullets.size(); i++) {
		switch (ammo.rayBullets[i].state) {
		case RAY_FIRING:
			ammo.rayBullets[i].fire();
			break;
		case RAY_DISAPPEARING:
			if (ammo.rayBullets[i].delayBeforeDissapear.getElapsedTime().asMilliseconds() > ammoData.rayBulletData.delayBeforeDissapearAsMilliseconds) {
				ammo.rayBullets[i].dissapear();
			}
			break;
		case RAY_DELETE:
			deleteBullet(BULT_RAY, i);
			i--;
			break;
		}
	}
}
void Player::updateGrenadeBullets() {
	for (int i = 0; i < ammo.rocketBullets.size(); i++) {
		switch (ammo.rocketBullets[i].state) {
		case ROCKET_EXPLOSIONANIM:
			ammo.rocketBullets[i].explosionAnimation();
			break;
		case ROCKET_EXPLOSING:
			ammo.rocketBullets[i].state = ROCKET_EXPLOSIONANIM;
			break;
		case ROCKET_MOVE:
			ammo.rocketBullets[i].move();
			break;
		case ROCKET_DELETE:
			deleteBullet(BULT_ROCKET, i);
			i--;
			break;
		}
	}
}

void Player::updateFrame(GameWindow* gwindow)
{
	controlPlayer(gwindow);
	rotateTo(static_cast<Vector2f>(Mouse::getPosition(gwindow->window)));
	checkForBulletSwap();
	updateAdditionalScopePart();
}
