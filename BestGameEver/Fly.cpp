#include "Fly.h"
#include <raylib.h>
#include <cstdlib>

// Flies spawn in a random position inside the screen
void Fly::Respawn(int inScreenWidth, int inScreenHeight) {

	float margin = 10.f;

	position = { margin + rand() % (int)(inScreenWidth - 2 * margin),margin + rand() % (int)(inScreenHeight - 2 * margin) };

	isAlive = true;

}

void Fly::Update(Player& player) {

	// Flies stick to the tongue when catched (and go to frog's mouth) until tongue is not out anymore
	if (isAttachedToTongue) {

		position = player.tongueEnd;

		if (!player.isTongueOut) {
			isAttachedToTongue = false;
			isAlive = false;
			player.fliesEaten++;
		}
		return;
	}

	if (isAlive) {

		// flies collision with player
		float distanceToPlayer = position.DistanceToTarget(player.position);

		if (distanceToPlayer < size + player.size) {

			isAlive = false;
			player.fliesEaten++;

			// showing points earned on the eaten fly position
			pointsGained = 10;
			player.score += pointsGained;
			pointsPosition = position;
			drawPointsGained = true;
			pointsGainedTimer = 0.5f;

			return;
		}

		// flies collision with tongue
		float distanceToTongue = position.DistanceToTarget(player.tongueEnd);

		if (distanceToTongue < size + 5.f && player.isTongueExtending) {

			isAttachedToTongue = true;

			// showing points earned on the catched fly position
			pointsGained = 10;
			player.score += pointsGained;
			pointsPosition = position;
			drawPointsGained = true;
			pointsGainedTimer = 0.5f;
			player.score += pointsGained;

			return;
		}
	}
}

void Fly::Draw() {

	if (isAlive) {
		DrawCircle(position.x, position.y, size, DARKGREEN);
	}
}
