#pragma once
#include <KamataEngine.h>

enum class LRDirection {
	kRight,
	kLeft,
};

class Player {
public:
	Player() {};
	~Player() {};

	void Initialize();
	void Update();
	void Draw(const KamataEngine::Camera& camera);

private:
	void InputMove();
	
	void CheckKey();

	std::unique_ptr<KamataEngine::Model> model;

	KamataEngine::WorldTransform worldTransform;
	KamataEngine::WorldTransform ghostWorldTransform;

	KamataEngine::ObjectColor* color;

	KamataEngine::Vector3 velocity_ = {};

	LRDirection lrDirection_ = LRDirection::kRight;

	bool isGhostMode;

	static inline const float kAttenuation = 0.08f;		// 横移動加速度
	static inline const float kLimitRusSpeed = 0.5f;	// 横移動最大速度
	static inline const float kDeceleration = 0.2f;		// 横移動減速速度
	static inline const float kVerticalAttenuation = 0.08f;  // 上下移動加速度
	static inline const float kVerticalLimitRusSpeed = 0.4f; // 上下移動最大速度

	float turnTimer_ = 0.0f;
	float turnFirstRotationY_ = 0.0f;
	static inline const float kTimeTurn_ = 0.3f;

	static inline const KamataEngine::Vector4 GhostModeColor = KamataEngine::Vector4{1.0f, 1.0f, 1.0f, 0.6f};


	bool isPushRightMoveKey;
	bool isPushLeftMoveKey;
	bool isPushUpMoveKey; 
	bool isPushDownMoveKey;

	bool isPushGhostModeKey;
};
