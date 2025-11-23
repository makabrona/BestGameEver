#include "Game.h"
#include <iostream>
using namespace std;

GameScreens actualScreen = Main;

// initialize the global variables
int pointsGained = 0;
float pointsGainedTimer = 0.5f;
bool drawPointsGained = false;
Vector2d pointsPosition;

// background textures
Texture2D backgroundMainMenu;
Texture2D backgroundPlay;
Texture2D backgroundLevelScreen;
Texture2D backgroundGameOver;

// functions to load and unload textures

void LoadTextures() {
	backgroundMainMenu = LoadTexture("C:/Users/Macarena/Documents/GitHub/BestGameEver/x64/Debug/sprites/background_mainmenu.png");
	backgroundPlay = LoadTexture("C:/Users/Macarena/Documents/GitHub/BestGameEver/x64/Debug/sprites/background_blueprint.png");
	backgroundLevelScreen = LoadTexture("C:/Users/Macarena/Documents/GitHub/BestGameEver/x64/Debug/sprites/background_newlevel.png");
	backgroundGameOver = LoadTexture("C:/Users/Macarena/Documents/GitHub/BestGameEver/x64/Debug/sprites/background_gameover.png");
}

void UnloadTextures() {
	UnloadTexture(backgroundMainMenu);
	UnloadTexture(backgroundPlay);
	UnloadTexture(backgroundLevelScreen);
	UnloadTexture(backgroundGameOver);
}