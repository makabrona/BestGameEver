#include <iostream>
#include <raylib.h>
#include <vector>
#include <cstdlib>
#include <ctime>

using namespace std;

enum GameScreens { Main, Play, GameOver };
GameScreens actualScreen = Main;

class Vector2d {
public:

	float x;
	float y;

	float CalculateMagnitude() {
		float magnitudeSquared = x * x + y * y;
		float magnitude = sqrtf(magnitudeSquared);
		return magnitude;
	}

	Vector2d SetVectorOffset(Vector2d inVectorToAdd) {
		float outVectorX = x + inVectorToAdd.x;
		float outVectorY = y + inVectorToAdd.y;
		Vector2d outVector{ outVectorX, outVectorY };

		return outVector;
	}

	Vector2d ScaleVector(float inScalar) {
		float scaledXVector = x * inScalar;
		float scaledYVector = y * inScalar;
		Vector2d scaledVector{ scaledXVector, scaledYVector };

		return scaledVector;
	}

	Vector2d VectorTowardsTarget(Vector2d inTargetVector) {
		float targetedXVector = inTargetVector.x - x;
		float targetedYVector = inTargetVector.y - y;
		Vector2d targetedVector{ targetedXVector , targetedYVector };

		return targetedVector;
	}

	float DistanceToTarget(Vector2d inTargetedVector) {
		Vector2d targetedVector = VectorTowardsTarget(inTargetedVector);
		float distanceToTargetedVector = targetedVector.CalculateMagnitude();

		return distanceToTargetedVector;
	}

	Vector2d NormalizeVector() {
		if (CalculateMagnitude() == 0)
		{
			return { 0, 0 };
		}

		float normalizedXVector = x / CalculateMagnitude();
		float normalizedYVector = y / CalculateMagnitude();
		Vector2d normalizedVector{ normalizedXVector,normalizedYVector };

		return normalizedVector;
	}

	Vector2d CosineMovement(Vector2d inCenter, float inAmplitude, float inAngle) {
		float x = inCenter.x + inAmplitude * cosf(inAngle);
		float y = inCenter.y;

		return { x, y };
	}

	Vector2d CircularMotion(Vector2d inCenter, float inRadius, float inAngle) {
		float cosX = inCenter.x + inRadius * cosf(inAngle);
		float sinY = inCenter.y + inRadius * sinf(inAngle);

		return { cosX, sinY };
	}
};

class Player {
public:

	// Player atributes
	float size = 20.f;
	float speed = 250.f;
	Vector2d position;
	int score = 0;
	int lives = 3;

	// Tongue atributes
	float tongueSpeed = 200.f;
	float tongueMaxLength = 100.f;
	float tongueCurrentLength = 0.f;
	Vector2d tongueDirection = { 1, 0 };
	Vector2d tongueEnd = { 0,0 };                     // extreme of the tongue
	Vector2d tongueRetract;
	bool isTongueOut = false;               // if tongue is active or not
	bool isTongueExtending = false;         // if tongue is extending or retracting

	void Controller() {
		Vector2d offsetPosition{ 0.f, 0.f };
		if (IsKeyDown(KEY_W)) {
			offsetPosition.y -= 1;
			tongueDirection = { 0, -1 };
		}
		else if (IsKeyDown(KEY_S)) {
			offsetPosition.y += 1;
			tongueDirection = { 0, 1 };
		}
		else if (IsKeyDown(KEY_A)) {
			offsetPosition.x -= 1;
			tongueDirection = { -1, 0 };
		}
		else if (IsKeyDown(KEY_D)) {
			offsetPosition.x += 1;
			tongueDirection = { 1, 0 };
		}

		offsetPosition = offsetPosition.NormalizeVector();

		position = position.SetVectorOffset(offsetPosition.ScaleVector(speed * GetFrameTime()));
	}

	void Tongue(float deltaTime) {
		if (IsKeyPressed(KEY_SPACE) && (!isTongueOut)) {
			isTongueOut = true;
			isTongueExtending = true;
			tongueCurrentLength = 0.f;
			tongueDirection = tongueDirection.NormalizeVector();
		}

		if (isTongueOut) {
			if (isTongueExtending) {
				tongueCurrentLength += tongueSpeed * deltaTime;

				if (tongueCurrentLength >= tongueMaxLength) {
					tongueCurrentLength = tongueMaxLength;
					isTongueExtending = false;
				}
			}
			else {
				tongueCurrentLength -= tongueSpeed * deltaTime;

				if (tongueCurrentLength <= 0.f) {
					tongueCurrentLength = 0.f;
					isTongueOut = false;
				}
			}

			tongueEnd = position.SetVectorOffset(tongueDirection.ScaleVector(tongueCurrentLength));
		}
	}

	void ScreenWrapping() {
		if (position.x > GetScreenWidth()) {
			position.x = 0;
		}
		if (position.x < 0) {
			position.x = GetScreenWidth();
		}
		if (position.y > GetScreenHeight()) {
			position.y = 0;
		}
		if (position.y < 0) {
			position.y = GetScreenHeight();
		}
	}

	void Draw() {
		DrawCircle(position.x, position.y, size, RAYWHITE);

		if (isTongueOut) {
			DrawLine(position.x, position.y, tongueEnd.x, tongueEnd.y, PINK);
		}
	}
};

// Collectable
class Fly {
public:

	Vector2d position;
	float size = 10.f;
	bool isAlive = true;

	void Respawn(int inScreenWidth, int inScreenHeight) {
		float margin = 10.f;

		position = { margin + rand() % (int)(inScreenWidth - 2 * margin),margin + rand() % (int)(inScreenHeight - 2 * margin) };

		isAlive = true;
	}

	void Update(Player& player) {

		if (isAlive) {
			// Collision with Player
			float distanceToPlayer = position.DistanceToTarget(player.position);
			if (distanceToPlayer < size + player.size) {
				isAlive = false;
				player.score++;
				return;
			}

			// Colision with Tongue
			float distanceToTongue = position.DistanceToTarget(player.tongueEnd);
			if (distanceToTongue < size + 5.f) {
				isAlive = false;
				player.score++;
				return;
			}
		}
	}

	void Draw() {
		if (isAlive) {
			DrawCircle(position.x, position.y, size, DARKGREEN);
		}
	}
};

// Enemy
class Bee {
public:

	enum BeeStates { Chasing, Butterfly, Dead };
	BeeStates actualState = Chasing;

	Vector2d position;
	Vector2d startPosition = { 600, 600 };
	float speed = 120.f;
	float size = 20.f;
	bool isAlive = true;
	float spawnDelay = 2.f;
	float butterflyTimer = 0.f;

	void Respawn() {
		position = startPosition;
		isAlive = true;
		spawnDelay = 2.f;
	}

	void Reset() {
		position = startPosition;
		actualState = Chasing;
		butterflyTimer = 0.f;
		spawnDelay = 0.f;
		isAlive = true;
		speed = 120.f;
	}

	void Update(Player& player, float deltaTime) {
		// wait some seconds before moving
		if (spawnDelay > 0.f) {
			spawnDelay -= deltaTime;
			return;
		}

		if (isAlive) {
			if (actualState == Chasing) {
				// Direction towards player
				Vector2d toPlayer = position.VectorTowardsTarget(player.position);
				float distanceToPlayer = toPlayer.CalculateMagnitude();
				Vector2d direction = toPlayer.NormalizeVector();

				// Move towards player
				position = position.SetVectorOffset(direction.ScaleVector(speed * deltaTime));

				// Collision with player
				if (distanceToPlayer < 25.f + player.size) {
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

				// Move players opposite direction
				Vector2d awayFromPlayer = position.VectorTowardsTarget(player.position).ScaleVector(-1.f);
				Vector2d direction = awayFromPlayer.NormalizeVector();
				position = position.SetVectorOffset(direction.ScaleVector(speed * deltaTime));

				// dies if collides with player
				float distanceToPlayer = position.DistanceToTarget(player.position);
				float distanceToTongue = position.DistanceToTarget(player.tongueEnd);
				if (distanceToPlayer < size + player.size || distanceToTongue < size + 5.f) {
					actualState = Dead;
					player.score++;
					Respawn();
				}

				// doesn't go out of the screen
				if (position.x < 10 || position.x > GetScreenWidth() - 10) {

					direction.x *= -1;
				}
				if (position.y < 10 || position.y > GetScreenHeight() - 10) {
					
					direction.y *= -1;
				}
			}

			if (actualState == Dead) {
				// go back to Starting Point
				Vector2d toStartingPoint = position.VectorTowardsTarget(startPosition);
				Vector2d direction = toStartingPoint.NormalizeVector();
				position = position.SetVectorOffset(direction.ScaleVector(speed * deltaTime));

				// go back to chasing mode
				if (position.DistanceToTarget(startPosition) < 1.f) {
					actualState = Chasing;
				}
			}
		}
	}
	void Draw() {
		DrawCircle(position.x, position.y, size, RED);
	}
};

//PowerUp
class Butterfly {
public:
	Vector2d position;
	float size = 15.f;
	bool isAlive = true;

	void Respawn(int screenWidth, int screenHeight) {
		float margin = 20.f;
		position = { margin + rand() % (int)(screenWidth - 2 * margin), margin + rand() % (int)(screenHeight - 2 * margin) };
		isAlive = true;
	}

	bool Update(Player& player) {
		if (isAlive) {
			float distanceToPlayer = position.DistanceToTarget(player.position);
			float distanceToTongue = position.DistanceToTarget(player.tongueEnd);

			if (distanceToPlayer < size + player.size || distanceToTongue < size + 5.f) {
				isAlive = false;
				return true;
			}
			else {
				return false;
			}
		}
	}
	void Draw() {
		if (isAlive) {
			DrawCircle(position.x, position.y, size, PURPLE);
		}
	}
};

int main() {
	int screenWidth = 800;
	int screenHeight = 800;

	float halfScreenWidth = (float)(screenWidth / 2);
	float halfScreenHeight = (float)(screenHeight / 2);

	int currentLevel = 1;
	int fliesToWin = 6;

	// Player Setup
	Player player;
	player.position = { halfScreenWidth, halfScreenHeight };

	// Collectable Setup
	int amountOfFlies = 3;
	vector<Fly>  flyContainer(amountOfFlies);
	Fly fly;

	for (Fly& fly : flyContainer) {
		fly.Respawn(screenWidth, screenHeight);
	}

	// Enemy Setup
	int amountOfBees = 1;
	vector<Bee> beeContainer(amountOfBees);
	Bee bee;

	Vector2d beeStartPositions[] = {
		{360, 700},
		{400, 700},
		{440, 700},
	};


	for (int i = 0; i < amountOfBees; i++) {
		beeContainer[i].startPosition = beeStartPositions[i];
		beeContainer[i].Respawn();
	}

	// PowerUp Setup
	int amountOfButterflies = 2;
	vector<Butterfly> butterflyContainer(amountOfButterflies);
	Butterfly butterfly;

	for (Butterfly& butterfly : butterflyContainer) {
		butterfly.Respawn(screenWidth, screenHeight);
	}

	InitWindow(screenWidth, screenHeight, "BestGameEver");
	SetTargetFPS(60);

	while (!WindowShouldClose()) {
		float deltaTime = GetFrameTime();

		BeginDrawing();
		ClearBackground(BLACK);

		switch (actualScreen) {
		case Main:

			DrawText("PRESS SPACE TO START", 210, 400, 30, WHITE);

			if (IsKeyDown(KEY_SPACE)) {
				actualScreen = Play;
			}

			break;
		case Play:

			// Player Update
			player.Controller();
			player.ScreenWrapping();
			player.Tongue(deltaTime);
			player.Draw();

			// Fly Update
			for (Fly& fly : flyContainer) {
				fly.Update(player);
				fly.Draw();

				if (!fly.isAlive) {
					fly.Respawn(screenWidth, screenHeight);
					fly.isAlive = true;
				}
			}


			// Bee Update
			for (Bee& bee : beeContainer) {
				bee.Update(player, deltaTime);
				bee.Draw();
			}

			// Butterfly Update

			for (Butterfly& butterfly : butterflyContainer) {
				if (butterfly.Update(player)) {
					for (Bee& bee : beeContainer) {
						if (bee.actualState == Bee::Chasing) {
							bee.actualState = Bee::Butterfly;
							bee.butterflyTimer = 5.f;
						}
					}
				}
				butterfly.Draw();
			}

			// Level Update

			if (player.score >= fliesToWin) {
				currentLevel++;
				player.score = 0;
			}
			switch (currentLevel) {
			case 2:
				fliesToWin = 10;
				amountOfBees = 2;
				beeContainer.resize(amountOfBees);
				for (int i = 0; i < amountOfBees; i++) {
					beeContainer[i].startPosition = beeStartPositions[i];
					beeContainer[i].Reset();
				}
				break;
			case 3:
				fliesToWin = 25;
				amountOfBees = 3;
				beeContainer.resize(amountOfBees);
				for (int i = 0; i < amountOfBees; i++) {
					beeContainer[i].startPosition = beeStartPositions[i];
					beeContainer[i].Reset();
				}
				break;
				}
			
			// Show Level
			DrawText(TextFormat("LEVEL %i", currentLevel), halfScreenWidth - 30, 10, 20, WHITE);

			// Show Score
			DrawText(TextFormat("Flies eaten: %i", player.score), 10, 10, 20, YELLOW);

			// Show Lives
			for (int i = 0; i < player.lives; i++) {
				DrawCircle(30 + i * 40, 50, 15, RED);
			}

			break;
		case GameOver:

			DrawText("GAME OVER", 210, 260, 60, WHITE);
			DrawText("PRESS SPACE TO RESTART", 200, 420, 30, WHITE);

			if (IsKeyDown(KEY_SPACE)) {
				actualScreen = Play;
			}

			break;
		}

		EndDrawing();
	}

	CloseWindow();
	return 0;
}