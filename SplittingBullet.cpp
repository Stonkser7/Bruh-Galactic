#include "SplittingBullet.h"

void SplittingBullet::move() {
	float deltaT = getDeltaTime();
	shape.move(speed * deltaT);
}

bool SplittingBullet::isOutOfScreen(GameWindow* gwindow) {
	return shape.getPosition().x >= gwindow->window.getSize().x || shape.getPosition().y >= gwindow->window.getSize().y || shape.getPosition().y <= 0 - shape.getRadius() * 2;
}

float SplittingBullet::getDeltaTime() {
	return deltaTime.restart().asSeconds();
}