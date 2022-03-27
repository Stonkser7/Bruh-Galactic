#include "OrdinaryBullet.h"


void OrdinaryBullet::move() {
	float deltaT = getDeltaTime();
	shape.move(speed * deltaT);
}

bool OrdinaryBullet::isOutOfScreen(GameWindow* gwindow) {
	return shape.getPosition().x >= gwindow->window.getSize().x || shape.getPosition().y >= gwindow->window.getSize().y + shape.getSize().y || shape.getPosition().y <= 0 - shape.getSize().y;
}

float OrdinaryBullet::getDeltaTime() {
	return deltaTime.restart().asSeconds();
}