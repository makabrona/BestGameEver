#include "Player.h"
#include "game.h"

void Player::Controller() {

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

void Player::Tongue(float deltaTime) {

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

void Player::ScreenLimits(int screenWidth, int screenHeight) {

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

void Player::Draw() {

	DrawCircle(position.x, position.y, size, RAYWHITE);

	if (isTongueOut) {
		DrawLineEx(Vector2{ position.x, position.y }, Vector2{ tongueEnd.x, tongueEnd.y }, 8.f, PINK);
	}
}