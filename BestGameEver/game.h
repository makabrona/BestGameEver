#pragma once
#include "Vector2d.h"
#include <raylib.h>

enum GameScreens { Main, Play, Level, GameOver};
extern GameScreens actualScreen;

// points
extern int pointsGained;
extern float pointsGainedTimer;
extern bool drawPointsGained;
extern Vector2d pointsPosition;

// background textures 
extern Texture2D backgroundMainMenu;
extern Texture2D backgroundPlay;
extern Texture2D backgroundLevelScreen;
extern Texture2D backgroundGameOver;

void LoadTextures();
void UnloadTextures();
