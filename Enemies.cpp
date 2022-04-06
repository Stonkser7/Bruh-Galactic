#include "Enemies.h"

//ROMA BULLET
void RomaEnemyBullet::move() {
	float deltaT = getDeltaTime();
	shape.move(speed * deltaT);
}
bool RomaEnemyBullet::isOutOfScreen(GameWindow *gwindow) {
	return shape.getPosition().x + shape.getSize().x < 0 || shape.getPosition().x > gwindow->x;
}
float RomaEnemyBullet::getDeltaTime() {
	return deltaTime.restart().asSeconds();
}

//ROCK BULLET
void RockEnemyBullet::move() {
	float deltaT = getDeltaTime();
	shape.move(speed * deltaT);
}
bool RockEnemyBullet::isOutOfScreen(GameWindow *gwindow) {
	return shape.getPosition().x + shape.getRadius() < 0 ||
		shape.getPosition().y + shape.getRadius() < 0 ||
		shape.getPosition().y + shape.getRadius() > gwindow->y;
}
float RockEnemyBullet::getDeltaTime() {
	return deltaTime.restart().asSeconds();
}

//HEALER RAY
void HealerEnemyRay::takeTarget(Vector2f coords) {
	shape.setRotation((atan2(coords.y - shape.getPosition().y, coords.x - shape.getPosition().x)) * 180 / 3.14159265);
}

//CIRCLE ENEMY
Enemy::CircleEnemy::CircleEnemy() {
	shape.setOutlineColor(Color(139, 0, 0, 180));
	excessFireTime = 0;
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
float Enemy::CircleEnemy::getDeltaTime() {
	return deltaTime.restart().asSeconds();
}

//ROMA ENEMY
void Enemy::RomaEnemy::generateDestinationY(GameWindow* gwindow) {
	destination_coord_y = rand() % static_cast<int>((gwindow->y - shape.getRadius() * 2)) + shape.getRadius();
	setState("ES_MOVING");
}
void Enemy::RomaEnemy::move(GameWindow* gwindow) {
	float deltaT = getDeltaTime();
	if (abs(destination_coord_y - static_cast<int>(shape.getPosition().y)) > 230 * deltaT) {
		if (shape.getPosition().y < destination_coord_y) {
			shape.move(speed.x * deltaT, speed.y * deltaT);
		}
		else {
			shape.move(speed.x * deltaT, -speed.y * deltaT);
		}
	}
	else {
		setState("ES_STANDING");
	}
}
bool Enemy::RomaEnemy::isNeedToFire() {
	return getState() == ES_MOVING && fireClock.getElapsedTime().asMilliseconds() - excessFireTime >= fireDelayAsMilliseconds;
}
RomaEnemyBullet Enemy::RomaEnemy::fire() {
	RomaEnemyBullet bullet;
	bullet.shape.setTexture(bulletTxtrPtr);
	bullet.shape.setSize(Vector2f(45, 25));
	bullet.shape.setPosition(Vector2f(shape.getPosition()));
	bullet.speed.x = bullet_acceleration * cos(shape.getRotation() * 3.14 / 180);
	bullet.speed.y = bullet_acceleration * sin(shape.getRotation() * 3.14 / 180);
	fireClock.restart();
	excessFireTime = 0;
	return bullet;
}
void Enemy::RomaEnemy::spawnAnimation() {
	float deltaT = getDeltaTime();
	if (abs(spawn_coord_x - static_cast<int>(shape.getPosition().x)) > abs(speed.x) * deltaT) {
		shape.move(speed.x * deltaT, speed.y * deltaT);
	}
	else {
		speed = { 0, 230 };
		setState("ES_STANDING");
		fireClock.restart();
	}
}

//ROCK ENEMY
bool Enemy::RockEnemy::isNeedToFire() {
	return getState() == ES_MOVING && fireClock.getElapsedTime().asMilliseconds() - excessFireTime >= fireDelayAsMilliseconds || getState() == ES_STANDING && fireClock.getElapsedTime().asMilliseconds() - excessFireTime >= fireDelayAsMilliseconds;
}
RockEnemyBullet Enemy::RockEnemy::fire() {
	RockEnemyBullet bullet;
	bullet.shape.setTexture(bulletTxtrPtr);
	bullet.shape.setRadius(15);
	bullet.shape.setPosition(Vector2f(shape.getPosition()));
	bullet.shape.setOrigin(bullet.shape.getRadius(), bullet.shape.getRadius());
	bullet.speed.x = bullet_acceleration * -cos(shape.getRotation() * 3.14 / 180);
	bullet.speed.y = bullet_acceleration * -sin(shape.getRotation() * 3.14 / 180);
	fireClock.restart();
	excessFireTime = 0;
	return bullet;
}
void Enemy::RockEnemy::takeTarget(Vector2f coords) {
	shape.setRotation((atan2(shape.getPosition().y - coords.y, shape.getPosition().x - coords.x)) * 180 / 3.14159265);
}
void Enemy::RockEnemy::move() {
	float deltaT = getDeltaTime();
	if (abs(destinationCoordY - static_cast<int>(shape.getPosition().y)) > speed.y * deltaT) {
		shape.move(speed.x * deltaT, speed.y * deltaT);
	}
	else {
		setState("ES_STANDING");
		fireDelayAsMilliseconds = 3300;
		fireClock.restart();
	}
}

//ELECTRO ENEMY
Enemy::ElectroEnemy::ElectroEnemy() {
	visible = true;
	excessVisibleTime = 0;
}
void Enemy::ElectroEnemy::toggleVisible() {
	visible = !visible;
	visibleClock.restart();
	excessVisibleTime = 0;
}
bool Enemy::ElectroEnemy::isNeedToFire() {
	return getState() == ES_STANDING && fireClock.getElapsedTime().asMilliseconds() - excessFireTime >= fireDelayAsMilliseconds;
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
	excessFireTime = 0;
	lightning.visible_lightningClock.restart();
	lightning.excessVisible_lightningTime = 0;
	return lightning;
}
void Enemy::ElectroEnemy::takeTarget(Vector2f coords) {
	shape.setRotation((atan2(shape.getPosition().y - coords.y, shape.getPosition().x - coords.x)) * 180 / 3.14159265);
	shape.rotate(-90);
}
void Enemy::ElectroEnemy::move() {
	float deltaT = getDeltaTime();
	if (shape.getPosition().x > destinationCoordX) {
		shape.move(-300 * deltaT, 0);
	}
	else {
		setState("ES_STANDING");
		visible = true;
		fireClock.restart();
	}
}

//HEALER ENEMY
void Enemy::HealerEnemy::spawnAnimation() {
	float deltaT = getDeltaTime();
	if (abs(spawn_coord_y - static_cast<int>(shape.getPosition().y)) > speed.y) {
		shape.move(speed.x * deltaT, speed.y * deltaT);
		healArea.move(speed.x * deltaT, speed.y * deltaT);
	}
	else {
		is_heal_enabled = true;
		setState("ES_STANDING");
	}
}
void Enemy::HealerEnemy::move() {
	float deltaT = getDeltaTime();
	if (abs(destinationCoords.x - static_cast<int>(shape.getPosition().x)) < abs(speed.x) * deltaT && abs(destinationCoords.y - static_cast<int>(shape.getPosition().y)) < abs(speed.y) * deltaT) {
		setState("ES_STANDING");
	}
	// X COORD
	if (abs(destinationCoords.x - static_cast<int>(shape.getPosition().x)) > abs(speed.x) * deltaT) {
		shape.move(speed.x * deltaT, 0);
		healArea.move(speed.x * deltaT, 0);
	}
	// Y COORD
	if (abs(destinationCoords.y - static_cast<int>(shape.getPosition().y)) > abs(speed.y) * deltaT) {
		shape.move(0, speed.y * deltaT);
		healArea.move(0, speed.y * deltaT);
	}
}
void Enemy::HealerEnemy::generateDestination(GameWindow* gwindow) {
	destinationCoords.x = rand() % (gwindow->x / 3) + gwindow->x * 2 / 3;
	destinationCoords.y = rand() % static_cast<int>((gwindow->y - shape.getRadius() * 2)) + shape.getRadius();
	speed.x = (destinationCoords.x - shape.getPosition().x) / 2;	//(destinationCoords - shape.getPosition()) / 2 * deltaT == 2secs to destination
	speed.y = (destinationCoords.y - shape.getPosition().y) / 2;	////////////////////////////////////////////////////////////////////////////////////////
	setState("ES_MOVING");
	getDeltaTime();	//just restart the delta timer
}
bool Enemy::HealerEnemy::isCanHeal() {
	return fireClock.getElapsedTime().asMilliseconds() - excessFireTime > fireDelayAsMilliseconds;
}