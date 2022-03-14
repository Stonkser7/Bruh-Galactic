#include "Player.h"
#include "Collision.h"
void OrdinaryBullet::move() {
	shape.move(speed);
}
bool OrdinaryBullet::isOutOfScreen(GameWindow* gwindow) {
	return shape.getPosition().x >= gwindow->window.getSize().x || shape.getPosition().y >= gwindow->window.getSize().y + shape.getSize().y || shape.getPosition().y <= 0 - shape.getSize().y;
}

void SplittingBullet::move() {
	shape.move(speed);
}
bool SplittingBullet::isOutOfScreen(GameWindow* gwindow) {
	return shape.getPosition().x >= gwindow->window.getSize().x || shape.getPosition().y >= gwindow->window.getSize().y || shape.getPosition().y <= 0 - shape.getRadius() * 2;
} 

void SplittedBullet::move() {
	shape.move(speed);
}
bool SplittedBullet::isOutOfScreen(GameWindow* gwindow) {
	return shape.getPosition().x >= gwindow->window.getSize().x || shape.getPosition().y >= gwindow->window.getSize().y || shape.getPosition().y <= 0 - shape.getRadius() * 2;
}

void RayBullet::fire() {
	if (shape.getSize().x < 1920) {
		shape.setSize(Vector2f(shape.getSize().x + 100, shape.getSize().y));
	}
	else {
		state = BS_DISAPPEARING;
		delayBeforeDissapear.restart();
	}
}
void RayBullet::dissapear() {
	if (shape.getFillColor().a > 0) {
		shape.setFillColor(Color(shape.getFillColor().r, shape.getFillColor().g, shape.getFillColor().b, shape.getFillColor().a - 1));
	}
	else {
		state = BS_DELETE;
	}
}


void AdditionalScope::toggleActive() {
	isActive = !isActive;
	if (isActive) {
		shape.setFillColor(Color(shape.getFillColor().r, shape.getFillColor().g, shape.getFillColor().b, 255));
	}
}


void Player::initPlayer(GameWindow* gwindow) {
	Collision::CreateTextureAndBitmask(playertexture1HP, "Textures\\playerTexture1HP.jpg");
	Collision::CreateTextureAndBitmask(playertexture2HP, "Textures\\playerTexture2HP.jpg");
	Collision::CreateTextureAndBitmask(playertexture3HP, "Textures\\playerTexture3HP.jpg");
	ordinaryBulletScopeTexture.loadFromFile("Textures\\ordinaryBulletScopeTexture.png");
	splittingBulletScopeTexture.loadFromFile("Textures\\splittingBulletScopeTexture.png");
	rayBulletScopeTexture.loadFromFile("Textures\\rayBulletScopeTexture.png");
	rayBulletAdditionalScopeTexture.loadFromFile("Textures\\rayBulletScopeTexture2.png");
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
void Player::initAmmunition() {
	selectedBullet = BULT_ORDINARY;
	//ORDINARY BULLET
	Collision::CreateTextureAndBitmask(ammoData.ordinaryBulletData.texture, "Textures\\pchel.jpg");
	ammoData.ordinaryBulletData.damage = 10;
	ammoData.ordinaryBulletData.defaultSpeed = { 12, 0 };
	ammoData.ordinaryBulletData.speedVariation = ammoData.ordinaryBulletData.defaultSpeed.x / 90;
	ammoData.ordinaryBulletData.fireDelayAsMilliseconds = 150;
	ammoData.ordinaryBulletData.excessFireTime = 0;

	//SPLITTING BULLET
	Collision::CreateTextureAndBitmask(ammoData.splittingBulletData.texture, "Textures\\splittingBulletTexture.png");
	ammoData.splittingBulletData.defaultDamage = 30;
	ammoData.splittingBulletData.defaultRadius = 25;
	ammoData.splittingBulletData.defaultSpeed = { 6, 0 };
	ammoData.splittingBulletData.speedVariation = ammoData.splittingBulletData.defaultSpeed.x / 90;
	ammoData.splittingBulletData.fireDelayAsMilliseconds = 600;
	ammoData.splittingBulletData.excessFireTime = 0;

	//RAY BULLET
	Collision::CreateTextureAndBitmask(ammoData.rayBulletData.texture, "Textures\\rayBulletTexture.png");
	ammoData.rayBulletData.texture.setSmooth(true);
	ammoData.rayBulletData.damage = 120;
	ammoData.rayBulletData.delayBeforeDissapearAsMilliseconds = 100;
	ammoData.rayBulletData.fireDelayAsMilliseconds = 1400;
	ammoData.rayBulletData.excessFireTime = 0;

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
		rotateGun();									//rotation only when player is shooting(for optimization)
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
void Player::rotateGun() {
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
void Player::fire() {
	switch (selectedBullet) {
	case BULT_ORDINARY:
		if (ammoData.ordinaryBulletData.fireDelay.getElapsedTime().asMilliseconds() - ammoData.ordinaryBulletData.excessFireTime > ammoData.ordinaryBulletData.fireDelayAsMilliseconds) {
			OrdinaryBullet bullet;
			bullet.shape.setSize(Vector2f(32.f, 19.f));
			bullet.shape.setTexture(&ammoData.ordinaryBulletData.texture);
			bullet.shape.setOrigin(Vector2f(bullet.shape.getSize().x / 2.f, bullet.shape.getSize().y / 2.f));
			bullet.shape.setPosition(Vector2f(playerShape.getPosition().x, (rand() % static_cast<int>(sizeX)) + playerShape.getPosition().y - playerShape.getOrigin().x));
			bullet.shape.setRotation(playerShape.getRotation() - 90);
			bullet.speed = ammoData.ordinaryBulletData.defaultSpeed;
			ammo.ordinaryBullets.push_back(bullet);
			ammoData.ordinaryBulletData.fireDelay.restart();
			ammoData.ordinaryBulletData.excessFireTime = 0;
			return;
		}
		break;
	case BULT_SPLITTING:
		if (ammoData.splittingBulletData.fireDelay.getElapsedTime().asMilliseconds() - ammoData.splittingBulletData.excessFireTime > ammoData.splittingBulletData.fireDelayAsMilliseconds) {
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
			ammoData.splittingBulletData.excessFireTime = 0;
			return;
		}
		break;
	case BULT_RAY:
		if (ammoData.rayBulletData.fireDelay.getElapsedTime().asMilliseconds() - ammoData.rayBulletData.excessFireTime > ammoData.rayBulletData.fireDelayAsMilliseconds) {
			RayBullet bullet;
			bullet.shape.setSize(Vector2f(0, 20));
			bullet.shape.setTexture(&ammoData.rayBulletData.texture);
			bullet.shape.setOrigin(Vector2f(0, bullet.shape.getSize().y / 2));
			bullet.shape.setRotation(playerShape.getRotation() - 90);
			bullet.shape.setPosition(Vector2f(playerShape.getPosition()));
			bullet.state = BS_FIRING;
			additionalScope.toggleActive();
			ammo.rayBullets.push_back(bullet);
			ammoData.rayBulletData.fireDelay.restart();
			ammoData.rayBulletData.excessFireTime = 0;
			return;
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
	splittedBullet.levelOfSplit = 1;


	splittedBullet.speed = splittingBullet->speed;
	splittedBullet.shape.setRotation(splittingBullet->shape.getRotation());
	splittedBullet.shape.rotate(-10);
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
	splittedBullet.shape.rotate(10);
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
void Player::splitBullet(SplittedBullet* splittedBullet) {
	if (splittedBullet->levelOfSplit < 2) {
		SplittedBullet twiceSplittedBullet;
		twiceSplittedBullet.shape.setTexture(&ammoData.splittingBulletData.texture);
		twiceSplittedBullet.shape.setPosition(splittedBullet->shape.getPosition());
		twiceSplittedBullet.shape.setRadius(splittedBullet->shape.getRadius() / 1.5);
		twiceSplittedBullet.damage = splittedBullet->damage / 1.5;
		twiceSplittedBullet.levelOfSplit = splittedBullet->levelOfSplit + 1;


		twiceSplittedBullet.speed = splittedBullet->speed;
		twiceSplittedBullet.shape.setRotation(splittedBullet->shape.getRotation());
		twiceSplittedBullet.shape.rotate(-12);
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
		twiceSplittedBullet.shape.rotate(12);
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
}
