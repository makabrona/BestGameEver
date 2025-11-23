#pragma once
#include "Vector2d.h"
#include <raylib.h>

class Player
{
public:

	// frog attributes
	float size = 20.f;
	float speed = 250.f;
	Vector2d position;
	int fliesEaten = 0;
	int lives = 3;
	int score = 0;

	// frog's tongue attributes
	float tongueSpeed = 200.f;
	float tongueMaxLength = 100.f;
	float tongueCurrentLength = 0.f;
	Vector2d tongueDirection = { 1, 0 };
	Vector2d tongueEnd = { 0,0 };            // extreme of the tongue
	Vector2d tongueRetract;
	bool isTongueOut = false;               // if tongue is active or not
	bool isTongueExtending = false;         // if tongue is extending or retracting


	void Controller();
	void Tongue(float deltaTime);
	void ScreenLimits(int screenWidth, int screenHeight);
	void Draw();
};

