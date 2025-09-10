#include "CameraController.h"
#include "Player.h"
#include "MapChip.h"
#include <algorithm>
using namespace MathUtility;

void CameraController::Initialize() { camera_.Initialize(); }

void CameraController::Update() {
	// 追従対象のワールドトランスフォームを取得
	const WorldTransform& targetWorldTransform = target_->GetWorldTransform();
	const Vector3& targetVelocity = target_->GetVelocity();

	// 追従対象の座標、移動速度とオフセットで目標座標を計算
	destination_ = targetWorldTransform.translation_ + targetVelocity * kVelocityBias + targetOffset_;
	// 座標補間によりゆったり追従
	camera_.translation_ = Lerp(camera_.translation_, destination_, kInterpolationRate);
	// 追従対象が画面外に出ないように補正
	camera_.translation_.x = std::clamp(camera_.translation_.x, targetWorldTransform.translation_.x + targetMargin.left, targetWorldTransform.translation_.x + targetMargin.right);
	camera_.translation_.y = std::clamp(camera_.translation_.y, targetWorldTransform.translation_.y + targetMargin.bottom, targetWorldTransform.translation_.y + targetMargin.top);
	// 移動範囲制限
	camera_.translation_.x = std::clamp(camera_.translation_.x, movableArea_.left, movableArea_.right);
	camera_.translation_.y = std::clamp(camera_.translation_.y, movableArea_.bottom, movableArea_.top);

	camera_.UpdateMatrix();
}

void CameraController::Reset() { // 追従対象のワールドトランスフォームを取得
	const WorldTransform& targetWorldTransform = target_->GetWorldTransform();

	camera_.translation_ = targetWorldTransform.translation_ + targetOffset_;
}
