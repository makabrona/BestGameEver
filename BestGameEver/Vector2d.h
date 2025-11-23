#pragma once
#include <math.h>

class Vector2d
{
	public:

		float x;
		float y;

		float CalculateMagnitude();

		Vector2d SetVectorOffset(Vector2d inVectorToAdd);

		Vector2d ScaleVector(float inScalar);

		Vector2d VectorTowardsTarget(Vector2d inTargetVector);

		float DistanceToTarget(Vector2d inTargetedVector);

		Vector2d NormalizeVector();

		Vector2d CosineMovement(Vector2d inCenter, float inAmplitude, float inAngle);

		Vector2d CircularMotion(Vector2d inCenter, float inRadius, float inAngle);

		float DotProduct(Vector2d inOtherVector);

		float AngleBetweenVectors(Vector2d inOtherVector);
};

