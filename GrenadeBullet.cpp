#include "GrenadeBullet.h"

void GrenadeBullet::move() {
	if (abs(destinationCoords.x - static_cast<int>(shape.getPosition().x)) < abs(speed.x) && abs(destinationCoords.y - static_cast<int>(shape.getPosition().y)) < abs(speed.y)) {
		state = GBS_EXPLOSING;
		explosionWave.setRadius(shape.getRadius());
		explosionWave.setOrigin(shape.getRadius(), shape.getRadius());
		explosionWave.setPosition(shape.getPosition());
		explosionWave.setFillColor(Color(139, 0, 0, 40));
		explosionWave.setOutlineColor(Color(255, 69, 0));
		explosionWave.setOutlineThickness(10);
	}
	if (state == GBS_FLYING) {
		// X COORD
		if (abs(destinationCoords.x - static_cast<int>(shape.getPosition().x)) > abs(speed.x)) {
			shape.move(speed.x, 0);
			firstDamageArea.move(speed.x, 0);
			secondDamageArea.move(speed.x, 0);
			thirdDamageArea.move(speed.x, 0);
		}
		// Y COORD
		if (abs(destinationCoords.y - static_cast<int>(shape.getPosition().y)) > abs(speed.y)) {
			shape.move(0, speed.y);
			firstDamageArea.move(0, speed.y);
			secondDamageArea.move(0, speed.y);
			thirdDamageArea.move(0, speed.y);
		}
	}
}

void GrenadeBullet::explosionAnimation() {
	if (explosionWave.getRadius() < thirdDamageArea.getRadius()) {
		explosionWave.setRadius(explosionWave.getRadius() + 17);
		explosionWave.setOrigin(explosionWave.getRadius(), explosionWave.getRadius());
	}
	else {
		state = GBS_DELETE;
	}
}