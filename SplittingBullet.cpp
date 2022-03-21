#include "SplittingBullet.h"

void SplittingBullet::move() {
	shape.move(speed);
}

bool SplittingBullet::isOutOfScreen(GameWindow* gwindow) {
	return shape.getPosition().x >= gwindow->window.getSize().x || shape.getPosition().y >= gwindow->window.getSize().y || shape.getPosition().y <= 0 - shape.getRadius() * 2;
}