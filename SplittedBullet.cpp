#include "SplittedBullet.h"

void SplittedBullet::move() {
	shape.move(speed);
}

bool SplittedBullet::isOutOfScreen(GameWindow* gwindow) {
	return shape.getPosition().x >= gwindow->window.getSize().x || shape.getPosition().y >= gwindow->window.getSize().y || shape.getPosition().y <= 0 - shape.getRadius() * 2;
}