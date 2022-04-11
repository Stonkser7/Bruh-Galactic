#include "RayBullet.h"

void RayBullet::fire() {
	if (shape.getSize().x < 1920) {
		shape.setSize(Vector2f(shape.getSize().x + 100, shape.getSize().y));
	}
	else {
		state = RAY_DISAPPEARING;
		delayBeforeDissapear.restart();
	}
}

void RayBullet::dissapear() {
	if (shape.getFillColor().a > 0) {
		shape.setFillColor(Color(shape.getFillColor().r, shape.getFillColor().g, shape.getFillColor().b, shape.getFillColor().a - 1));
	}
	else {
		state = RAY_DELETE;
	}
}