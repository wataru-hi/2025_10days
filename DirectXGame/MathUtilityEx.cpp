#pragma once

#include <KamataEngine.h>
#include <numbers>

namespace KamataEngine {

namespace MathUtility {

float EaseInOut(float x1, float x2, float t) {
	float easedT = -(std::cosf(std::numbers::pi_v<float> * t) - 1.0f) / 2.0f;
	return Lerp(x1, x2, easedT);
}

float my_min(float a, float b) { return (a < b) ? a : b; }
float my_max(float a, float b) { return (a > b) ? a : b; }

Vector3 Lerp(const Vector3& v1, const Vector3& v2, float t)
{
	return Vector3(Lerp(v1.x, v2.x, t), Lerp(v1.y, v2.y, t), Lerp(v1.z, v2.z, t));
}

} // namespace MathUtility
} // namespace KamataEngine