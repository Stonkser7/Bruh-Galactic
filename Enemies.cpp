#include "Enemies.h"

void HealerEnemyRay::takeTarget(Vector2f coords) {
	shape.setRotation((atan2(coords.y - shape.getPosition().y, coords.x - shape.getPosition().x)) * 180 / 3.14159265);
}

Enemy::CircleEnemy::CircleEnemy() {
	shape.setOutlineColor(Color(139, 0, 0, 180));
}
void Enemy::CircleEnemy::setState(string state) {
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
ENEMYSTATE Enemy::CircleEnemy::getState() {
	return state;
}
void Enemy::CircleEnemy::takeDamage(int damage) {
	shape.setOutlineThickness(shape.getOutlineThickness() - damage);
}
void Enemy::CircleEnemy::heal(float heal) {
	shape.setOutlineThickness(shape.getOutlineThickness() + heal);
	if (shape.getOutlineThickness() > 0) {
		shape.setOutlineThickness(0);
	}
}
bool Enemy::CircleEnemy::isAlive() {
	return shape.getRadius() > abs(shape.getOutlineThickness());
}

void Enemy::RomaEnemy::generateDestinationY(GameWindow* gwindow) {
	destinationCoordY = rand() % static_cast<int>((gwindow->y - shape.getRadius() * 2)) + shape.getRadius();
	setState("ES_MOVING");
}
void Enemy::RomaEnemy::move(GameWindow* gwindow) {
	if (abs(destinationCoordY - static_cast<int>(shape.getPosition().y)) > 0.4) {
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
bool Enemy::RomaEnemy::isNeedToFire() {
	return getState() == ES_MOVING && fireClock.getElapsedTime().asMilliseconds() >= fireDelayAsMilliseconds;
}
RectangleShape Enemy::RomaEnemy::fire() {
	RectangleShape bullet;
	bullet.setTexture(bulletTxtrPtr);
	bullet.setSize(Vector2f(45, 25));
	bullet.setPosition(Vector2f(shape.getPosition()));
	fireClock.restart();
	return bullet;
}
void Enemy::RomaEnemy::spawnAnimation() {
	if (shape.getPosition().x != spawnCoordX) {
		shape.move(-1, 0);
	}
	else {
		setState("ES_STANDING");
		fireClock.restart();
	}
}

bool Enemy::RockEnemy::isNeedToFire() {
	return getState() == ES_MOVING && fireClock.getElapsedTime().asMilliseconds() >= fireDelayAsMilliseconds || getState() == ES_STANDING && fireClock.getElapsedTime().asMilliseconds() >= fireDelayAsMilliseconds;
}
void Enemy::RockEnemy::rotateGun() {
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
RockEnemyBullet Enemy::RockEnemy::fire() {
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
void Enemy::RockEnemy::takeTarget(Vector2f coords) {
	shape.setRotation((atan2(shape.getPosition().y - coords.y, shape.getPosition().x - coords.x)) * 180 / 3.14159265);
}
void Enemy::RockEnemy::move() {
	switch (side) {
	case S_UP:
		if (shape.getPosition().y < destinationCoordY) {
			shape.move(-0.3, 0.8);
		}
		else {
			setState("ES_STANDING");
			fireDelayAsMilliseconds = 3500;
			fireClock.restart();
		}
		break;
	case S_DOWN:
		if (shape.getPosition().y > destinationCoordY) {
			shape.move(-0.3, -0.8);
		}
		else {
			setState("ES_STANDING");
			fireDelayAsMilliseconds = 3500;
			fireClock.restart();
		}
		break;
	}
}

Enemy::ElectroEnemy::ElectroEnemy() {
	visible = true;
}
void Enemy::ElectroEnemy::toggleVisible() {
	visible = !visible;
	visibleClock.restart();
}
bool Enemy::ElectroEnemy::isNeedToFire() {
	return getState() == ES_STANDING && fireClock.getElapsedTime().asMilliseconds() >= fireDelayAsMilliseconds;
}
void Enemy::ElectroEnemy::moveRandomCoordY(GameWindow* gwindow) {
	shape.setPosition(Vector2f(shape.getPosition().x, rand() % static_cast<int>((gwindow->y - shape.getRadius() * 4)) + shape.getRadius() * 2));
}
ElectroEnemyLightning Enemy::ElectroEnemy::fire(Vector2f coords) {
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
void Enemy::ElectroEnemy::takeTarget(Vector2f coords) {
	shape.setRotation((atan2(shape.getPosition().y - coords.y, shape.getPosition().x - coords.x)) * 180 / 3.14159265);
	shape.rotate(-90);
}
void Enemy::ElectroEnemy::move() {
	if (shape.getPosition().x >= destinationCoordX) {
		shape.move(-1, 0);
	}
	else {
		setState("ES_STANDING");
		visible = true;
		fireClock.restart();
	}
}

void Enemy::HealerEnemy::spawnAnimation() {
	if (static_cast<int>(shape.getPosition().y) != spawnCoordY) {
		switch (side) {
		case S_UP:
			shape.move(0, 2);
			healArea.move(0, 2);
			break;
		case S_DOWN:
			shape.move(0, -2);
			healArea.move(0, -2);
			break;
		}
	}
	else {
		isHealAreaActive = true;
		setState("ES_STANDING");
	}
}
void Enemy::HealerEnemy::move() {
	// X COORD
	if (abs(destinationCoords.x - static_cast<int>(shape.getPosition().x)) > abs(speed.x)) {
		shape.move(speed.x, 0);
		healArea.move(speed.x, 0);
	}
	// Y COORD
	if (abs(destinationCoords.y - static_cast<int>(shape.getPosition().y)) > abs(speed.y)) {
		shape.move(0, speed.y);
		healArea.move(0, speed.y);
	}
	if (abs(destinationCoords.x - static_cast<int>(shape.getPosition().x)) < abs(speed.x) && abs(destinationCoords.y - static_cast<int>(shape.getPosition().y)) < abs(speed.y)) {
		setState("ES_STANDING");
	}
}
void Enemy::HealerEnemy::generateDestination(GameWindow* gwindow) {
	destinationCoords.x = rand() % (gwindow->x / 3) + gwindow->x * 2 / 3;
	destinationCoords.y = rand() % static_cast<int>((gwindow->y - shape.getRadius() * 2)) + shape.getRadius();
	speed.x = (destinationCoords.x - shape.getPosition().x) / 600.f;	//framerate limit == 240 that means 2.5 secs to destination (240 * 2.5 == 600)
	speed.y = (destinationCoords.y - shape.getPosition().y) / 600.f;	/////////////////////////////////////////////////////////
	setState("ES_MOVING");
}
bool Enemy::HealerEnemy::isCanHeal() {
	return fireClock.getElapsedTime().asMilliseconds() > fireDelayAsMilliseconds;
}