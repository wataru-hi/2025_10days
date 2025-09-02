#include "Player.h"
#include <algorithm>
#include <numbers>

using namespace KamataEngine;
using namespace MathUtility;

void Player::Initialize() {
	model = std::unique_ptr<Model>(Model::CreateFromOBJ("player"));

	worldTransform.Initialize();
	worldTransform.scale_ = Vector3{3.0f, 3.0f, 3.0f};
	worldTransform.rotation_ .y = std::numbers::pi_v<float> / 2.0f;
	
	ghostWorldTransform.Initialize();
	ghostWorldTransform.scale_ = Vector3{3.0f, 3.0f, 3.0f};
	ghostWorldTransform.rotation_.y = std::numbers::pi_v<float> / 2.0f;

	color = new ObjectColor();
	color->Initialize();
	color->SetColor(GhostModeColor);
}

void Player::Update() {
	CheckKey();
	InputMove();

	if (!isGhostMode)
		worldTransform.translation_ += velocity_;
	else
		ghostWorldTransform.translation_ += velocity_;

	if(turnTimer_) {
		// turnTimer_ -= 1.0f / 60.0f;
		turnTimer_ = (std::max)(turnTimer_ - (1.0f / 60.0f), 0.0f);

		float destionRotationYtable[] = {std::numbers::pi_v<float> / 2.0f, std::numbers::pi_v<float> * 3.0f / 2.0f};
		// 状態に応じた角度を取得する
		float destiationRotationY = destionRotationYtable[static_cast<uint32_t>(lrDirection_)];

		float time = turnTimer_ / kTimeTurn_;

		// 自キャラの角度を設定する
		float easeY = EaseInOut(destiationRotationY, turnFirstRotationY_, time);
		if (isGhostMode)
			ghostWorldTransform.rotation_.y = easeY;
		else
			worldTransform.rotation_.y = easeY;
	}

#ifdef _DEBUG
	ImGui::Begin("player");
	ImGui::DragFloat3("rot", &worldTransform.translation_.x, 0.1f);
	ImGui::DragFloat3("pos", &worldTransform.translation_.x, 0.01f);
	ImGui::DragFloat3("velocity", &velocity_.x, 0.01f);

	ImGui::End();
#endif

	worldTransform.UpdateMatirx();
	ghostWorldTransform.UpdateMatirx();
}

void Player::Draw(const Camera& camera) { 
	if (isGhostMode)
		model->Draw(ghostWorldTransform, camera, color);
	
	model->Draw(worldTransform, camera); 
	
}

void Player::InputMove() {
	if (isPushGhostModeKey) {
		isGhostMode = !isGhostMode;
		if (isGhostMode)
			ghostWorldTransform.translation_ = worldTransform.translation_;
	} 

	if (isGhostMode) {
		if (isPushUpMoveKey || isPushDownMoveKey) {
			Vector3 acceleration = {};
			if (isPushUpMoveKey) {
				acceleration.y += kVerticalAttenuation;
			}
			if (isPushDownMoveKey) {
				acceleration.y -= kVerticalAttenuation;
			}
			velocity_.y += acceleration.y;

			velocity_.y = std::clamp(velocity_.y, -kVerticalLimitRusSpeed, kVerticalLimitRusSpeed);
		} else {
			velocity_.y *= (1.0f - kVerticalAttenuation);
		}
	}


	if (isPushRightMoveKey || isPushLeftMoveKey) {
		Vector3 acceleration = {};
		if (isPushRightMoveKey) {
			acceleration.x += kAttenuation;
			if (lrDirection_ != LRDirection::kRight) {
				lrDirection_ = LRDirection::kRight;
				turnTimer_ = kTimeTurn_;
				if (!isGhostMode)
					turnFirstRotationY_ = worldTransform.rotation_.y;
				else
					turnFirstRotationY_ = ghostWorldTransform.rotation_.y;
			}
		}
		if (isPushLeftMoveKey) {
			acceleration.x -= kAttenuation;
			if (lrDirection_ != LRDirection::kLeft) {
				lrDirection_ = LRDirection::kLeft;
				turnTimer_ = kTimeTurn_;
				if (!isGhostMode)
					turnFirstRotationY_ = worldTransform.rotation_.y;
				else
					turnFirstRotationY_ = ghostWorldTransform.rotation_.y;
			}
		}
		velocity_.x += acceleration.x;

		velocity_.x = std::clamp(velocity_.x, -kLimitRusSpeed, kLimitRusSpeed);
	} else {
		velocity_.x *= (1.0f - kDeceleration);
	}

}

void Player::CheckKey() {

	isPushRightMoveKey = false;
	if (Input::GetInstance()->PushKey(DIK_RIGHT) || Input::GetInstance()->PushKey(DIK_D))
		isPushRightMoveKey = true;

	isPushLeftMoveKey = false;
	if (Input::GetInstance()->PushKey(DIK_LEFT) || Input::GetInstance()->PushKey(DIK_A))
		isPushLeftMoveKey = true;

	isPushUpMoveKey = false;
	if (Input::GetInstance()->PushKey(DIK_UP) || Input::GetInstance()->PushKey(DIK_W))
		isPushUpMoveKey = true;

	isPushDownMoveKey = false;
	if (Input::GetInstance()->PushKey(DIK_DOWN) || Input::GetInstance()->PushKey(DIK_S))
		isPushDownMoveKey = true;

	isPushGhostModeKey = false;
	if (Input::GetInstance()->TriggerKey(DIK_Q))
		isPushGhostModeKey = true;
}
