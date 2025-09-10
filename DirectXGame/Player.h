#pragma once
#include <KamataEngine.h>
#include "AABB.h"

enum class LRDirection {
	kRight,
	kLeft,
};

struct CollisionMapInfo {
	bool ceiling = false;
	bool landing = false;
	bool hitWall = false;
	KamataEngine::Vector3 move;
};

enum class Corner {
	kRightBottom,
	kLeftBottom,
	kRightTop,
	kLeftTop,

	kNumCorner
};

class MapChip;

class Player {
public:
	Player() {};
	~Player() { 
		delete model;
		model = nullptr;
	};

	void Initialize(MapChip* mapChip);
	void Update();
	void Draw(const KamataEngine::Camera& camera);

private:
	void InputMove();
	
    void CheckKey();

	//void UpdateOnGround(const CollisionMapInfo& info);

	KamataEngine::Vector3 CornerPosition(const KamataEngine::Vector3& center, Corner corner);

	void CheckMapCollision(CollisionMapInfo& info);
	void CheckMapCollisionUp(CollisionMapInfo& info);
	void CheckMapCollisionDown(CollisionMapInfo& info);
	void CheckMapCollisionRight(CollisionMapInfo& info);
	void CheckMapCollisionLeft(CollisionMapInfo& info);

	KamataEngine::Model* model;

	KamataEngine::WorldTransform worldTransform;
	KamataEngine::WorldTransform ghostWorldTransform;

	KamataEngine::ObjectColor* color;


	std::unique_ptr<MapChip> mapChipData;


	KamataEngine::Vector3 velocity_ = {};

	LRDirection lrDirection_ = LRDirection::kRight;

	bool isGhostMode;

	static inline const float kAttenuation = 0.08f;       // 横移動の加速度
	static inline const float kLimitRusSpeed = 0.5f;      // 横移動の最大速度
	static inline const float kDeceleration = 0.2f;       // 横移動の減速速度
	static inline const float kVerticalAttenuation = 0.08f;  // 上下移動の加速度（ゴーストモード用）
	static inline const float kVerticalLimitRusSpeed = 0.4f; // 上下移動の最大速度（ゴーストモード用）
	static inline const float kGroundSearchHeight = 0.06f;   // 地面を探知する高さ
	static inline const float kAttenuationLanding = 0.06f;   // 着地時の減速速度
	static inline const float kJumpAcceleration = 20.0f;     // ジャンプ力


	static inline const float kGravityAcceleration = 0.98f; // 重力加速度
	static inline const float kLimitFallSpeed = 0.5f;       // 落下速度の最大値

	static inline const float kWidth = 1.0f;  // プレイヤーの幅
	static inline const float kHeight = 0.8f; // プレイヤーの高さ
	static inline const float kBlank = 0.04f;  // 当たり判定の余白


	float turnTimer_ = 0.0f; 
	float turnFirstRotationY_ = 0.0f;
	static inline const float kTimeTurn_ = 0.3f;

	static inline const KamataEngine::Vector4 GhostModeColor = KamataEngine::Vector4{1.0f, 1.0f, 1.0f, 0.6f};

	bool onGround_;

	bool isPushRightMoveKey;
	bool isPushLeftMoveKey;
	bool isPushUpMoveKey; 
	bool isPushDownMoveKey;



	bool isPushGhostModeKey;
};
