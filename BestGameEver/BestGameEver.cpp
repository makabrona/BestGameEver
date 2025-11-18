#include <iostream>
#include <raylib.h>
#include <vector>
#include <cstdlib>
#include <ctime>

using namespace std;

enum GameScreens { Main, Play, Level, GameOver };
GameScreens actualScreen = Play;

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

	float DotProduct(Vector2d inOtherVector)
	{
		float xComponentMultiplied = x * inOtherVector.x;
		float yComponentMultiplied = y * inOtherVector.y;
		float dotProduct = xComponentMultiplied + yComponentMultiplied;

		return dotProduct;
	}

	float AngleBetweenVectors(Vector2d inOtherVector)
	{
		float thisLength = CalculateMagnitude();
		float otherLength = inOtherVector.CalculateMagnitude();

		if (thisLength == 0 || otherLength == 0) {
			return 0.f;
		}
		float dotProduct = DotProduct(inOtherVector);
		float cosine = dotProduct / (thisLength * otherLength);

		float degrees = acosf(cosine) * (180 / 3.14);

		return degrees;
	}
};

class Player {
public:

	// Player atributes
	float size = 20.f;
	float speed = 250.f;
	Vector2d position;
	int fliesEaten = 0;
	int lives = 3;
	int score = 0;

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

	void ScreenLimits(int screenWidth, int screenHeight) {
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

	void Draw() {
		DrawCircle(position.x, position.y, size, RAYWHITE);

		if (isTongueOut) {
			DrawLineEx(Vector2{ position.x, position.y }, Vector2{ tongueEnd.x, tongueEnd.y }, 8.f, PINK);
		}
	}
};

// Collectable
class Fly {
public:

	Vector2d position;
	float size = 10.f;
	bool isAlive = true;
	bool isAttachedToTongue = false;

	void Respawn(int inScreenWidth, int inScreenHeight) {
		float margin = 10.f;

		position = { margin + rand() % (int)(inScreenWidth - 2 * margin),margin + rand() % (int)(inScreenHeight - 2 * margin) };

		isAlive = true;
	}

	void Update(Player& player) {
		// Sticks to the tongue when it is catched
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
			// Collision with Player
			float distanceToPlayer = position.DistanceToTarget(player.position);
			if (distanceToPlayer < size + player.size) {
				isAlive = false;
				player.fliesEaten++;
				return;
			}

			// Colision with Tongue
			float distanceToTongue = position.DistanceToTarget(player.tongueEnd);
			if (distanceToTongue < size + 5.f && player.isTongueExtending) {
				isAttachedToTongue = true;
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

	// Bee attributes (variables)
	Vector2d position;
	Vector2d startPosition = { 600, 600 };
	int type = 1;	// type 1 just follows player, type 2 searches...
	float speed = 120.f;
	float size = 20.f;
	float spawnDelay = 2.f;
	bool isAlive = true;

	// for searching and detecting the player
	Vector2d fowardVector{ 1, 0 };
	float detectionRange = 300.f;
	float fovAngle = 60.f;
	bool isDetected{ false };
	bool isSearching{ false };
	Vector2d lastSeenPosition;
	float searchTimer = GetFrameTime();

	// for butterfly mode
	bool isEscaping = false;
	float butterflyTimer = 0.f;

	void Respawn(float inSpawnDelay = 2.f) {
		position = startPosition;
		isAlive = false;
		spawnDelay = inSpawnDelay;
		actualState = Chasing;
	}

	void Reset() {
		position = startPosition;
		actualState = Chasing;
		butterflyTimer = 0.f;
		spawnDelay = 0.f;
		isAlive = true;
		speed = 120.f;
	}

	void ScreenLimits(int screenWidth, int screenHeight) {
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

	void Update(Player& player, float deltaTime) {
		// wait some seconds before moving
		if (spawnDelay > 0.f) {
			spawnDelay -= deltaTime;
			if (spawnDelay <= 0.f) {
				spawnDelay = 0.f;
				isAlive = true;
			}
			else {
				return;
			}
		}

		if (isAlive) {
			if (actualState == Chasing) {
				switch (type) {
				case 1:
					// BEE 1: Follows player
				{
					// Direction towards player
					Vector2d toPlayer = position.VectorTowardsTarget(player.position);
					Vector2d toPlayerDirection = toPlayer.NormalizeVector(); // normalize
					position = position.SetVectorOffset(toPlayerDirection.ScaleVector(speed * deltaTime)); //move towards player
				}
				break;
				case 2:
					// BEE 2: Patrol + follow
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
					player.fliesEaten++;
					player.score += 222;
					DrawText("+222", position.x, position.y, 20, YELLOW);
					Respawn();
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

void DrawMap() {
	DrawCircle(400, 400, 50, ORANGE);
	DrawRectangle(388, 574, 20, 80, GRAY);
	DrawRectangle(557, 635, 90, 20, GRAY);
	DrawRectangle(119, 574, 90, 20, GRAY);
	DrawRectangle(596, 347, 20, 80, GRAY);
	DrawRectangle(139, 574, 20, 80, GRAY);
	DrawRectangle(115, 115, 80, 20, GRAY);
	DrawRectangle(585, 115, 80, 20, GRAY);
}

int main() {
	int screenWidth = 800;
	int screenHeight = 800;

	float halfScreenWidth = (float)(screenWidth / 2);
	float halfScreenHeight = (float)(screenHeight / 2);

	int previousLevel = 0;
	int currentLevel = 1;
	int nextLevel = 1;
	float levelScreenTimer = 0.f;
	int fliesToWin;

	// Score Text (winnin points when eat butterfly or bee)
	bool getPoints = false;
	float getPointsTimer = 0.f;
	int getPointsValue = 0;

	// Player Setup
	Player player;
	player.position = { 400, 700 };

	// Collectable Setup
	int amountOfFlies = 3;
	vector<Fly>  flyContainer(amountOfFlies);
	Fly fly;

	for (Fly& fly : flyContainer) {
		fly.Respawn(screenWidth, screenHeight);
	}

	// Enemy Setup
	Vector2d beeStartPosition = { halfScreenWidth, halfScreenHeight };
	// Level 1
	Bee bee1;
	bee1.type = 1;
	bee1.startPosition = beeStartPosition;
	bee1.spawnDelay = 1.f;
	bee1.Respawn();

	// Level 2
	Bee bee2;
	bee2.type = 2;
	bee2.startPosition = beeStartPosition;
	bee2.spawnDelay = 2.f;
	bee2.Respawn();

	// PowerUp Setup ()
	vector<Butterfly> butterflyContainer;
	
	InitWindow(screenWidth, screenHeight, "BestGameEver");
	SetTargetFPS(60);

	while (!WindowShouldClose()) {
		float deltaTime = GetFrameTime();

		BeginDrawing();
		ClearBackground(BLACK);

		if (currentLevel != previousLevel) {
			switch (currentLevel) {
			case 1:
				fliesToWin = 6;
				bee2.Respawn(2.f);
				break;

			case 2:
				fliesToWin = 10;
				bee1.Respawn(2.f);
				bee2.Respawn(0.8f);

				break;

			case 3:
				fliesToWin = 20;
				bee1.Respawn(2.f);
				bee2.Respawn(0.5);
				break;
			}
			previousLevel = currentLevel;
		}

		switch (actualScreen) {
		case Main:

			DrawText("PRESS SPACE TO START", 210, 400, 30, WHITE);

			if (IsKeyDown(KEY_SPACE)) {
				actualScreen = Level;
				nextLevel = currentLevel;
				levelScreenTimer = 2.f;
			}

			break;
		case Play:

			// Player Update
			player.Controller();
			player.Tongue(deltaTime);
			player.ScreenLimits(screenWidth, screenHeight);
			player.Draw();

			// Flies Update
			for (Fly& fly : flyContainer) {
				fly.Update(player);
				fly.Draw();

				if (!fly.isAlive) {
					fly.Respawn(screenWidth, screenHeight);
				}
			}

			//// Butterfly Update

			for (Butterfly& butterfly : butterflyContainer) {
				if (butterfly.Update(player)) {

					bee1.actualState = Bee::Butterfly;
					bee1.butterflyTimer = 5.f;

					bee2.actualState = Bee::Butterfly;
					bee2.butterflyTimer = 5.f;

					

				}
				butterfly.Draw();
			}

			// Bees Update
			if (currentLevel >= 1) {
				bee2.Update(player, deltaTime);
				bee2.Draw();
			}
			if (currentLevel >= 2) {
				bee1.Update(player, deltaTime);
				bee1.Draw();
			}

			// Level Update

			if (player.fliesEaten >= fliesToWin && actualScreen != Level) {
				nextLevel = currentLevel + 1;
				levelScreenTimer = 2.f; // show level screen for 3 seconds
				player.fliesEaten = 0;
				actualScreen = Level;
			}

			if (currentLevel == 4) {
				actualScreen = GameOver;
			}

			//Draw Map
			DrawMap();

			// Show Level
			DrawText(TextFormat("LEVEL %i", currentLevel), halfScreenWidth - 30, 10, 20, WHITE);

			// Show Score
			DrawText(TextFormat("Flies eaten: %i", player.fliesEaten), 10, 10, 20, YELLOW);

			// Show Lives
			for (int i = 0; i < player.lives; i++) {
				DrawCircle(30 + i * 40, 50, 15, RED);
			}

			

			break;

		case Level:
			// Show this screen when level up
			ClearBackground(BLACK);

			DrawText(TextFormat("LEVEL %i", nextLevel), halfScreenWidth - 30, halfScreenHeight, 50, WHITE);
			DrawText(TextFormat("EAT %i FLIES", fliesToWin), halfScreenWidth - 30, halfScreenHeight + 50, 20, WHITE);
			DrawText(TextFormat("Lives left %i", player.lives), halfScreenWidth - 30, halfScreenHeight + 70, 20, WHITE);

			// show level screen for 2 seconds
			levelScreenTimer -= deltaTime;
			if (levelScreenTimer <= 0.f) {
				currentLevel = nextLevel;
				player.fliesEaten = 0;
				actualScreen = Play;

				butterflyContainer.clear();
				int numButterflies = 0;

				switch (currentLevel) {
				case 1: 
					numButterflies = 0;
					break;
				case 2:
					numButterflies = 2;
					break;
				case 3:
					numButterflies = 3;
					break;
					
				}

				butterflyContainer.resize(numButterflies);
				for (Butterfly& butterfly : butterflyContainer) {
					butterfly.Respawn(screenWidth, screenHeight);
				}
			}

			break;

		case GameOver:

			DrawText("GAME OVER", 210, 260, 60, WHITE);
			DrawText("PRESS SPACE TO RESTART", 200, 420, 30, WHITE);

			if (IsKeyPressed(KEY_SPACE)) {
				actualScreen = Play;
			}

			break;
		}

		EndDrawing();
	}

	CloseWindow();
	return 0;
}