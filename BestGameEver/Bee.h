#pragma once
#include "Vector2d.h"
#include "Player.h"
#include "game.h"

class Bee
{
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
	float searchTimer = 0.f;

	// for butterfly mode
	bool isEscaping = false;
	float butterflyTimer = 0.f;

	void Respawn(float inSpawnDelay = 2.f);

	void Reset();

	void ScreenLimits(int screenWidth, int screenHeight);

	void Update(Player& player, float deltaTime);

	void Draw();
};

