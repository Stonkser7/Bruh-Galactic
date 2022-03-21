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
void Player::initPlayer(GameWindow* gwindow) {
	Collision::CreateTextureAndBitmask(playertexture1HP, "Textures\\playerTexture1HP.jpg");
	Collision::CreateTextureAndBitmask(playertexture2HP, "Textures\\playerTexture2HP.jpg");
	Collision::CreateTextureAndBitmask(playertexture3HP, "Textures\\playerTexture3HP.jpg");
	ordinaryBulletScopeTexture.loadFromFile("Textures\\ordinaryBulletScopeTexture.png");
	splittingBulletScopeTexture.loadFromFile("Textures\\splittingBulletScopeTexture.png");
	rayBulletScopeTexture.loadFromFile("Textures\\rayBulletScopeTexture.png");
	rayBulletAdditionalScopeTexture.loadFromFile("Textures\\rayBulletScopeTexture2.png");
	grenadeBulletScopeTexture.loadFromFile("Textures\\grenadeScopeTexture.png");
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
	additionalScope.isActive = false;
}
void Player::initAmmunition() {
	selectedBullet = BULT_ORDINARY;
	//ORDINARY BULLET
	Collision::CreateTextureAndBitmask(ammoData.ordinaryBulletData.texture, "Textures\\pchel.jpg");
	ammoData.ordinaryBulletData.damage = 10;
	ammoData.ordinaryBulletData.fireDelayAsMilliseconds = 150;
	ammoData.ordinaryBulletData.excessFireDelay = 0;

	//SPLITTING BULLET
	Collision::CreateTextureAndBitmask(ammoData.splittingBulletData.texture, "Textures\\splittingBulletTexture.png");
	ammoData.splittingBulletData.defaultDamage = 30;
	ammoData.splittingBulletData.defaultRadius = 25;
	ammoData.splittingBulletData.fireDelayAsMilliseconds = 600;
	ammoData.splittingBulletData.excessFireDelay = 0;

	//RAY BULLET
	Collision::CreateTextureAndBitmask(ammoData.rayBulletData.texture, "Textures\\rayBulletTexture.png");
	ammoData.rayBulletData.texture.setSmooth(true);
	ammoData.rayBulletData.damage = 120;
	ammoData.rayBulletData.delayBeforeDissapearAsMilliseconds = 100;
	ammoData.rayBulletData.fireDelayAsMilliseconds = 1400;
	ammoData.rayBulletData.excessFireDelay = 0;

	//GRENADE BULLET
	ammoData.grenadeBulletData.texture.loadFromFile("Textures\\grenadeBulletTexture.png");
	Collision::CreateTextureAndBitmask(ammoData.grenadeBulletData.firstAreaTexture, "Textures\\grenadeFirstDamageArea.png");
	Collision::CreateTextureAndBitmask(ammoData.grenadeBulletData.secondAreaTexture, "Textures\\grenadeSecondDamageArea.png");
	Collision::CreateTextureAndBitmask(ammoData.grenadeBulletData.thirdAreaTexture, "Textures\\grenadeThirdDamageArea.png");
	ammoData.grenadeBulletData.firstAreaTexture.setSmooth(true);
	ammoData.grenadeBulletData.secondAreaTexture.setSmooth(true);
	ammoData.grenadeBulletData.thirdAreaTexture.setSmooth(true);

	ammoData.grenadeBulletData.fireDelayAsMilliseconds = 1200;

	ammoData.grenadeBulletData.firstLevelDamage = 60;
	ammoData.grenadeBulletData.secondLevelDamage = 25;
	ammoData.grenadeBulletData.thirdLevelDamage = 10;

	ammoData.grenadeBulletData.defaultRadius = 30;

	ammoData.grenadeBulletData.firstAreaRaduis = 100;
	ammoData.grenadeBulletData.secondAreaRaduis = 200;
	ammoData.grenadeBulletData.thirdAreaRaduis = 350;

	ammoData.grenadeBulletData.excessFireDelay = 0;

	ammo.ordinaryBullets.clear();
	ammo.splittingBullets.clear();
	ammo.splittedBullets.clear();
	ammo.rayBullets.clear();
}
void Player::setHPAmount(int requiredAmount) {
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
int Player::getHPAmount() {
	return HPAmount;
}
BULLETTYPE Player::getSelectedBulletType() {
	return selectedBullet;
}
void Player::rotateToMouse(Vector2f coords) {
	playerShape.setRotation((atan2(playerShape.getPosition().y - coords.y, playerShape.getPosition().x - coords.x)) * 180 / 3.14159265 - 90);
	scope.setRotation(playerShape.getRotation() - 90);
	additionalScope.shape.setRotation(playerShape.getRotation() - 90);
}
void Player::controlPlayer(GameWindow* gwindow) {
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
			bullet.acceleration = 10;
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
			bullet.acceleration = 5;
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
			bullet.state = BS_FIRING;
			additionalScope.toggleActive();
			ammo.rayBullets.push_back(bullet);
			ammoData.rayBulletData.fireTimer.restart();
			ammoData.rayBulletData.excessFireDelay = 0;
			return;
		}
		break;
	case BULT_GRENADE:
		if (ammoData.grenadeBulletData.fireTimer.getElapsedTime().asMilliseconds() - ammoData.grenadeBulletData.excessFireDelay > ammoData.grenadeBulletData.fireDelayAsMilliseconds) {
			GrenadeBullet bullet;
			//setting raduis
			bullet.shape.setRadius(ammoData.grenadeBulletData.defaultRadius);
			bullet.firstDamageArea.setRadius(ammoData.grenadeBulletData.firstAreaRaduis);
			bullet.secondDamageArea.setRadius(ammoData.grenadeBulletData.secondAreaRaduis);
			bullet.thirdDamageArea.setRadius(ammoData.grenadeBulletData.thirdAreaRaduis);
			//setting textures
			bullet.shape.setTexture(&ammoData.grenadeBulletData.texture);
			bullet.firstDamageArea.setTexture(&ammoData.grenadeBulletData.firstAreaTexture);
			bullet.secondDamageArea.setTexture(&ammoData.grenadeBulletData.secondAreaTexture);
			bullet.thirdDamageArea.setTexture(&ammoData.grenadeBulletData.thirdAreaTexture);
			//setting origins
			bullet.shape.setOrigin(ammoData.grenadeBulletData.defaultRadius, ammoData.grenadeBulletData.defaultRadius);
			bullet.firstDamageArea.setOrigin(ammoData.grenadeBulletData.firstAreaRaduis, ammoData.grenadeBulletData.firstAreaRaduis);
			bullet.secondDamageArea.setOrigin(ammoData.grenadeBulletData.secondAreaRaduis, ammoData.grenadeBulletData.secondAreaRaduis);
			bullet.thirdDamageArea.setOrigin(ammoData.grenadeBulletData.thirdAreaRaduis, ammoData.grenadeBulletData.thirdAreaRaduis);
			//setting rotation
			bullet.shape.setRotation(playerShape.getRotation());
			bullet.firstDamageArea.setRotation(playerShape.getRotation());
			bullet.secondDamageArea.setRotation(playerShape.getRotation());
			bullet.thirdDamageArea.setRotation(playerShape.getRotation());
			//setting position
			bullet.shape.setPosition(playerShape.getPosition());
			bullet.firstDamageArea.setPosition(playerShape.getPosition());
			bullet.secondDamageArea.setPosition(playerShape.getPosition());
			bullet.thirdDamageArea.setPosition(playerShape.getPosition());
			bullet.destinationCoords = Mouse::getPosition();
			bullet.speed.x = (static_cast<float>(bullet.destinationCoords.x) - bullet.shape.getPosition().x) / 240;	//framerate limit == 240 that means 1 secs to destination (240 * 1 == 240)
			bullet.speed.y = (static_cast<float>(bullet.destinationCoords.y) - bullet.shape.getPosition().y) / 240;	//////////////////////////////////////////////////////////////////////////
			bullet.state = BS_FLYING;
			ammo.grenadeBullets.push_back(bullet);
			ammoData.grenadeBulletData.fireTimer.restart();
			ammoData.grenadeBulletData.excessFireDelay = 0;
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
	splittedBullet.shape.rotate(-7);
	splittedBullet.speed.x = splittedBullet.acceleration * cos((splittedBullet.shape.getRotation() - 90) * 3.14 / 180);
	splittedBullet.speed.y = splittedBullet.acceleration * sin((splittedBullet.shape.getRotation() - 90) * 3.14 / 180);
	ammo.splittedBullets.push_back(splittedBullet);

	splittedBullet.shape.setRotation(splittingBullet->shape.getRotation());
	splittedBullet.shape.rotate(7);
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
		twiceSplittedBullet.shape.rotate(-10);
		twiceSplittedBullet.speed.x = twiceSplittedBullet.acceleration * cos((twiceSplittedBullet.shape.getRotation() - 90) * 3.14 / 180);
		twiceSplittedBullet.speed.y = twiceSplittedBullet.acceleration * sin((twiceSplittedBullet.shape.getRotation() - 90) * 3.14 / 180);
		ammo.splittedBullets.push_back(twiceSplittedBullet);

		twiceSplittedBullet.shape.setRotation(splittedBullet->shape.getRotation());
		twiceSplittedBullet.shape.rotate(10);
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
	case BULT_GRENADE:
		ammo.grenadeBullets.erase(ammo.grenadeBullets.begin() + index);
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
	if (Keyboard::isKeyPressed(Keyboard::Num4) && selectedBullet != BULT_GRENADE) {
		selectedBullet = BULT_GRENADE;
		scope.setSize(Vector2f(70, 24));
		scope.setOrigin(Vector2f(0, scope.getSize().y / 2));
		scope.setPosition(playerShape.getPosition());
		scope.setTextureRect(IntRect(0, 0, 70, 24));
		scope.setTexture(&grenadeBulletScopeTexture);
		additionalScope.isActive = false;
	}
}
