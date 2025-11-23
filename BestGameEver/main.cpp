#include <iostream>
#include <raylib.h>
#include <vector>
#include <cstdlib>
#include <ctime>

#include "Vector2d.h"
#include "Game.h"
#include "Player.h"
#include "Fly.h"
#include "Bee.h"
#include "Butterfly.h"

using namespace std;

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
		bee1.searchTimer = GetFrameTime();
		bee2.searchTimer = GetFrameTime();

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

				if (currentLevel < 3) {
					nextLevel = currentLevel + 1;
					levelScreenTimer = 2.f; // show level screen for 3 seconds
					player.fliesEaten = 0;
					actualScreen = Level;
				}
				else {
					actualScreen = GameOver;
				}
			}

			// Show points gained when eat something

			if (drawPointsGained) {
				pointsGainedTimer -= deltaTime;
				if (pointsGainedTimer <= 0.f) {
					drawPointsGained = false;
				}
			}

			if (drawPointsGained) {
				DrawText(TextFormat("+%i", pointsGained), (int)pointsPosition.x, (int)pointsPosition.y, 20, YELLOW);
			}

			// Show Level
			DrawText(TextFormat("LEVEL %i", currentLevel), halfScreenWidth - 30, 10, 20, WHITE);

			// Show Flies Eaten
			DrawText(TextFormat("Flies eaten: %i", player.fliesEaten), 10, 10, 20, YELLOW);

			// Show Highscore
			DrawText(TextFormat("%i", player.score), 700, 10, 20, YELLOW);

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

			DrawText(TextFormat("SCORE %i", (int)player.score), 210, 360, 20, WHITE);

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