#include <iostream>
#include <raylib.h>
#include <vector>
#include <cstdlib>
#include <ctime>

using namespace std;

enum GameScreens { Main, Play, GameOver };
GameScreens actualScreen = Play;

// Variables
float screenWidth = 800;
float screenHeight = 800;

int main() {

	InitWindow(screenWidth, screenHeight, "BestGameEver");
	SetTargetFPS(60);

	while (!WindowShouldClose()) {

		BeginDrawing();
		ClearBackground(BLACK);

		switch (actualScreen) {
		case Main:
			break;
		case Play:
			break;
		case GameOver:
			break;
		}

		EndDrawing();

	}

	CloseWindow();
	return 0;
}