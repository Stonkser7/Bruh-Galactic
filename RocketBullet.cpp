#include "RocketBullet.h"

void RocketBullet::move() {
	float deltaT = getDeltaTime();
	if (isDestinationReached(deltaT)) {
		state = ROCKET_EXPLOSING;
		explosionWave.setRadius(shape.getSize().y / 2);
		explosionWave.setOrigin(explosionWave.getRadius(), explosionWave.getRadius());
		explosionWave.setPosition(shape.getPosition());
		explosionWave.setFillColor(Color(139, 0, 0, 10));
		explosionWave.setOutlineColor(Color(255, 69, 0));
		explosionWave.setOutlineThickness(10);
	}
	else {
		// X COORD
		if (abs(destinationCoords.x - static_cast<int>(shape.getPosition().x)) > abs(speed.x) * deltaT) {
			shape.move(speed.x * deltaT, 0);
			firstDamageArea.move(speed.x * deltaT, 0);
			secondDamageArea.move(speed.x * deltaT, 0);
			thirdDamageArea.move(speed.x * deltaT, 0);
		}
		// Y COORD
		if (abs(destinationCoords.y - static_cast<int>(shape.getPosition().y)) > abs(speed.y) * deltaT) {
			shape.move(0, speed.y * deltaT);
			firstDamageArea.move(0, speed.y * deltaT);
			secondDamageArea.move(0, speed.y * deltaT);
			thirdDamageArea.move(0, speed.y * deltaT);
		}
	}
}

bool RocketBullet::isDestinationReached(float deltaT)
{
	return abs(destinationCoords.x - static_cast<int>(shape.getPosition().x)) < abs(speed.x) * deltaT && abs(destinationCoords.y - static_cast<int>(shape.getPosition().y)) < abs(speed.y) * deltaT;
}

void RocketBullet::explosionAnimation() {
	if (explosionWave.getRadius() < thirdDamageArea.getRadius()) {
		explosionWave.setRadius(explosionWave.getRadius() + 12);
		explosionWave.setOrigin(explosionWave.getRadius(), explosionWave.getRadius());
	}
	else {
		state = ROCKET_DELETE;
	}
}

float RocketBullet::getDeltaTime() {
	return deltaTime.restart().asSeconds();
}