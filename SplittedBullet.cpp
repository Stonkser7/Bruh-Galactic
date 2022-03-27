#include "SplittedBullet.h"

void SplittedBullet::move() {
	float deltaT = getDeltaTime();
	shape.move(speed * deltaT);
}

bool SplittedBullet::isOutOfScreen(GameWindow* gwindow) {
	return shape.getPosition().x >= gwindow->window.getSize().x || shape.getPosition().y >= gwindow->window.getSize().y || shape.getPosition().y <= 0 - shape.getRadius() * 2;
}

float SplittedBullet::getDeltaTime() {
	return deltaTime.restart().asSeconds();
}