#include "Bee.h"

void Bee::Respawn(float inSpawnDelay) {
	position = startPosition;
	isAlive = false;
	spawnDelay = inSpawnDelay;
	actualState = Chasing;
}

void Bee::Reset() {
	position = startPosition;
	actualState = Chasing;
	butterflyTimer = 0.f;
	spawnDelay = 0.f;
	isAlive = true;
	speed = 120.f;
}

// So Bees don't go out of the screen (when they escape from the player)
void Bee::ScreenLimits(int screenWidth, int screenHeight) {
	if (position.x > screenWidth - size) {
		position.x = screenWidth - size;
	}
	if (position.x < size) {
		position.x = size;
	}
	if (position.y > screenHeight - size) {
		position.y = screenHeight - size;
	}
	if (position.y < size) {
		position.y = size;
	}
}

void Bee::Update(Player& player, float deltaTime) {

	// wait some seconds before moving
	if (spawnDelay > 0.f) {
		spawnDelay -= deltaTime;
		if (spawnDelay <= 0.f) {
			spawnDelay = 0.f;
			isAlive = true;
		}
	}

	if (isAlive) {
		if (actualState == Chasing) {

			switch (type) {
			case 1:
				// Bee type 1: Follows player's position
			{

				Vector2d toPlayer = position.VectorTowardsTarget(player.position);
				Vector2d toPlayerDirection = toPlayer.NormalizeVector(); // normalize
				position = position.SetVectorOffset(toPlayerDirection.ScaleVector(speed * deltaTime)); //move towards player
			}
			break;
			case 2:
				// Bee type 2: Patrol and follow
			{
				Vector2d toPlayer = position.VectorTowardsTarget(player.position);
				float distanceToPlayer = toPlayer.CalculateMagnitude();
				Vector2d toPlayerDirection = toPlayer.NormalizeVector(); // normalize
				float angleToPlayer = fowardVector.AngleBetweenVectors(toPlayerDirection); // angle between vectors

				//detection
				// check fov and distance
				if (distanceToPlayer < detectionRange && angleToPlayer < fovAngle) {
					isDetected = true;
				}
				else {
					isDetected = false;
				}

				//detected ai behavior
				if (isDetected) {
					//chase player
					lastSeenPosition = player.position;
					fowardVector = toPlayerDirection;
					position = position.SetVectorOffset(fowardVector.ScaleVector(speed * GetFrameTime()));

					if (distanceToPlayer < 30.f) {
						Respawn();
					}

					if (distanceToPlayer > detectionRange && angleToPlayer > fovAngle) {
						isSearching = true;
						position = position.SetVectorOffset(lastSeenPosition.ScaleVector(speed * GetFrameTime())); // nosesiestabien
						if (searchTimer > 3.f) {
							isSearching = false;
						}
					}
				}
				else {
					position = position.SetVectorOffset(fowardVector.ScaleVector(speed * GetFrameTime()));

					// so it doesnt go out of the screen (it turns around like a pingpong ball)

					if (position.x < 10 || position.x > GetScreenWidth() - 10) {
						fowardVector.x *= -1;
					}
					if (position.y < 10 || position.y > GetScreenHeight() - 10) {
						fowardVector.y *= -1;
					}
				}	break;
			}
			}

			// Collision with player (DAMAGE)

			float distanceToPlayer = position.DistanceToTarget(player.position);
			if (distanceToPlayer < size + player.size) {
				player.lives--;
				Respawn();
				if (player.lives <= 0) {
					actualScreen = GameOver;
				}
			}
		}

		if (actualState == Butterfly) {
			// timer
			butterflyTimer -= deltaTime;
			if (butterflyTimer <= 0.f) {
				actualState = Chasing;
			}

			ScreenLimits(GetScreenWidth(), GetScreenHeight());

			// Move players opposite direction
			Vector2d awayFromPlayer = position.VectorTowardsTarget(player.position).ScaleVector(-1.f);
			Vector2d direction = awayFromPlayer.NormalizeVector();
			position = position.SetVectorOffset(direction.ScaleVector(speed * deltaTime));

			// dies if collides with player or tongue
			float distanceToPlayer = position.DistanceToTarget(player.position);
			float distanceToTongue = position.DistanceToTarget(player.tongueEnd);

			if (distanceToPlayer < size + player.size || distanceToTongue < size + 5.f) {

				actualState = Dead;

				pointsGained = 200;
				player.score += pointsGained;
				pointsPosition = position;
				drawPointsGained = true;
				pointsGainedTimer = 0.5f;

			}
		}

		if (actualState == Dead) {

			// change skin to dead bee (angel + transparency)

			// Fly back to the respawn point
			float deadSpeed = 60.f;

			Vector2d toStartingPoint = position.VectorTowardsTarget(startPosition);
			Vector2d direction = toStartingPoint.NormalizeVector();
			position = position.SetVectorOffset(direction.ScaleVector(deadSpeed * deltaTime));

			// go back to chasing mode
			if (position.DistanceToTarget(startPosition) < 1.f) {
				Respawn();
			}
		}
	}
}

void Bee::Draw() {
	DrawCircle(position.x, position.y, size, RED);
}