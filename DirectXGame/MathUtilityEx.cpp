#pragma once

#include <KamataEngine.h>
#include <numbers>

namespace KamataEngine {

namespace MathUtility {

float EaseInOut(float x1, float x2, float t) {
	float easedT = -(std::cosf(std::numbers::pi_v<float> * t) - 1.0f) / 2.0f;
	return Lerp(x1, x2, easedT);
}

} // namespace MathUtility
} // namespace KamataEngine