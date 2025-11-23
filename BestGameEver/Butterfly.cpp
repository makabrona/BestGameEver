#include "Butterfly.h"
#include <cstdlib>
#include <raylib.h>

void Butterfly::Respawn(int screenWidth, int screenHeight) {
	float margin = 20.f;
	position = { margin + rand() % (int)(screenWidth - 2 * margin), margin + rand() % (int)(screenHeight - 2 * margin) };
	isAlive = true;
}

bool Butterfly::Update(Player& player) {
	if (isAlive) {
		float distanceToPlayer = position.DistanceToTarget(player.position);
		float distanceToTongue = position.DistanceToTarget(player.tongueEnd);
		// WHEN IS EATEN
		if (distanceToPlayer < size + player.size || distanceToTongue < size + 5.f) {
			isAlive = false;

			pointsGained = 100;
			player.score += pointsGained;
			pointsPosition = position;
			drawPointsGained = true;
			pointsGainedTimer = 0.5f;

			return true;
		}
		else {
			return false;
		}
	}
}
void Butterfly::Draw() {
	if (isAlive) {
		DrawCircle(position.x, position.y, size, PURPLE);
	}
}
