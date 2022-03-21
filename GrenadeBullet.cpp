#include "GrenadeBullet.h"

void GrenadeBullet::move() {
	if (abs(destinationCoords.x - static_cast<int>(shape.getPosition().x)) < abs(speed.x) && abs(destinationCoords.y - static_cast<int>(shape.getPosition().y)) < abs(speed.y)) {
		state = BS_EXPLOSING;
	}
	if (state == BS_FLYING) {
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