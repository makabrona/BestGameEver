#pragma once
#include "Vector2d.h"
#include "Player.h"
#include "game.h"

class Butterfly
{
public:
	Vector2d position;
	float size = 15.f;
	bool isAlive = true;

	void Respawn(int screenWidth, int screenHeight);

	bool Update(Player& player);

	void Draw();
};

