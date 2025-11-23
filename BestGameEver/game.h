#pragma once
#include "Vector2d.h"
#include <raylib.h>

enum GameScreens { Main, Play, Level, GameOver};

	// points
	extern GameScreens actualScreen;
	extern int pointsGained;
	extern float pointsGainedTimer;
	extern bool drawPointsGained;
	extern Vector2d pointsPosition;
