#include "FolloCamera.h"
#include "Player.h"
#include "MapChip.h"
#include <numbers> // std::numbers::pi_v を使用

using namespace KamataEngine;
using namespace MathUtility;

void FollowCamera::Initialize(const Player* targetPlayer) {
	camera_.Initialize();
	targetPlayer_ = targetPlayer;

	if (targetPlayer_) {
		// 初期位置と回転を設定
		camera_.translation_ = targetPlayer_->GetPosition() + kOffset_;

		Vector3 direction = targetPlayer_->GetPosition() - camera_.translation_;
		camera_.rotation_.y = std::atan2(direction.x, direction.z);

		// 回転のx成分を計算（上下の角度）
		// ピタゴラスの定理でXZ平面上の距離を求める
		float horizontalDistance = std::sqrt(direction.x * direction.x + direction.z * direction.z);
		camera_.rotation_.x = std::atan2(-direction.y, horizontalDistance);
	}
}

void FollowCamera::Update() {
	if (!targetPlayer_) {
		return;
	}

	// プレイヤーの現在位置を取得
	Vector3 playerPos = targetPlayer_->GetPosition();

	// カメラの目標位置を計算
	Vector3 targetCamPos = playerPos + kOffset_;

	// カメラ位置を目標位置へ線形補間
	camera_.translation_ = MathUtility::Lerp(camera_.translation_, targetCamPos, kFollowSpeed_);

	// カメラからプレイヤーへの方向ベクトルを計算
	Vector3 direction = playerPos - camera_.translation_;

	// カメラの回転を更新
	// Y軸（左右）の回転を計算
	float targetRotationY = std::atan2(direction.x, direction.z);
	camera_.rotation_.y = MathUtility::Lerp(camera_.rotation_.y, targetRotationY, kFollowSpeed_);

	// X軸（上下）の回転を計算
	float horizontalDistance = std::sqrt(direction.x * direction.x + direction.z * direction.z);
	float targetRotationX = std::atan2(-direction.y, horizontalDistance);
	camera_.rotation_.x = MathUtility::Lerp(camera_.rotation_.x, targetRotationX, kFollowSpeed_);

	camera_.UpdateMatrix();
}