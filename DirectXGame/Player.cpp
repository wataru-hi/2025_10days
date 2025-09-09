#include "Player.h"
#include "MapChip.h"
#include <algorithm>
#include <numbers>

using namespace KamataEngine;
using namespace MathUtility;

void Player::Initialize() {
	model = std::unique_ptr<Model>(Model::CreateFromOBJ("player"));

	worldTransform.Initialize();
	worldTransform.scale_ = Vector3{3.0f, 3.0f, 3.0f};
	worldTransform.translation_ = Vector3{0.0f, 5.0f, 0.0f};
	worldTransform.rotation_.y = std::numbers::pi_v<float> / 2.0f;

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

	CollisionMapInfo collisionMapInfo = {};
	collisionMapInfo.move = velocity_;
	collisionMapInfo.landing = false;
	collisionMapInfo.hitWall = false;

	if (!isGhostMode) {
		CheckMapCollision(collisionMapInfo);
		worldTransform.translation_ += collisionMapInfo.move;

		// 天井接触による落下開始
		if (collisionMapInfo.ceiling) {
			velocity_.y = 0;
		}
		// 壁接触による減速
		if (collisionMapInfo.hitWall) {
			velocity_.x *= (1.0f - kAttenuation);
		}
	} else {
		ghostWorldTransform.translation_ += velocity_;
	}

	if (turnTimer_) {
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
		if (isGhostMode) {

			ghostWorldTransform.translation_ = worldTransform.translation_;
			velocity_.y = 0.0f;
		}
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
	if (std::abs(velocity_.x) <= 0.01f) {
		velocity_.x = 0.0f;
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

Vector3 Player::CornerPosition(const Vector3& center, Corner corner) {
	Vector3 offsetTable[] = {
	    {+kWidth / 2.0f, -kHeight / 2.0f, 0}, // kRightBottom
	    {-kWidth / 2.0f, -kHeight / 2.0f, 0}, // kLeftBottom
	    {+kWidth / 2.0f, +kHeight / 2.0f, 0}, // kRightTop
	    {-kWidth / 2.0f, +kHeight / 2.0f, 0}  // kLeftTop
	};

	return center + offsetTable[static_cast<uint32_t>(corner)];
}

void Player::CheckMapCollision(CollisionMapInfo info) {
	CheckMapCollisionUp(info);
	CheckMapCollisionDown(info);
	CheckMapCollisionRight(info);
	CheckMapCollisionLeft(info);
}

void Player::CheckMapCollisionUp(CollisionMapInfo info) {
	if (info.move.y <= 0) {
		return;
	}

	std::array<Vector3, static_cast<size_t>(Corner::kNumCorner)> positionsNew;
	for (uint32_t i = 0; i < positionsNew.size(); ++i) {
		positionsNew[i] = CornerPosition(worldTransform.translation_ + info.move, static_cast<Corner>(i));
	}

	MapChipType mapChipType;
	MapChipType mapChipTypeNext;
	bool hit = false;
	MapChip::IndexSet indexSet;

	// 右上
	indexSet = mapChipData->GetMapChipIndexSetByPosition(positionsNew[static_cast<uint32_t>(Corner::kRightTop)]);
	mapChipType = mapChipData->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	mapChipTypeNext = mapChipData->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex - 1);
	if (mapChipType == MapChipType::Block && mapChipTypeNext != MapChipType::Block) {
		hit = true;
	}

	// 左上
	indexSet = mapChipData->GetMapChipIndexSetByPosition(positionsNew[static_cast<uint32_t>(Corner::kLeftTop)]);
	mapChipType = mapChipData->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	mapChipTypeNext = mapChipData->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex - 1);
	if (mapChipType == MapChipType::Block && mapChipTypeNext != MapChipType::Block) {
		hit = true;
	}

	if (hit) {
		MapChip::IndexSet indexSetNow = mapChipData->GetMapChipIndexSetByPosition(worldTransform.translation_ + Vector3(0, kHeight / 2.0f, 0));
		if (indexSetNow.yIndex != indexSet.yIndex) {
			indexSet = mapChipData->GetMapChipIndexSetByPosition(worldTransform.translation_ + info.move + Vector3(0, kHeight / 2.0f, 0));
			MapChip::Rect rect = mapChipData->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
			info.move.y = my_max(0.0f, rect.bottom - worldTransform.translation_.y - (kHeight / 2.0f + kBlank));
			info.ceiling = true;
		}
	}
}

void Player::CheckMapCollisionDown(CollisionMapInfo info) {
	if (info.move.y >= 0) {
		return;
	}

	std::array<Vector3, static_cast<size_t>(Corner::kNumCorner)> positionsNew;
	for (uint32_t i = 0; i < positionsNew.size(); ++i) {
		positionsNew[i] = CornerPosition(worldTransform.translation_ + info.move, static_cast<Corner>(i));
	}

	MapChipType mapChipType;
	MapChipType mapChipTypeNext;
	bool hit = false;
	MapChip::IndexSet indexSet;

	// 右下
	indexSet = mapChipData->GetMapChipIndexSetByPosition(positionsNew[static_cast<uint32_t>(Corner::kRightBottom)]);
	mapChipType = mapChipData->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	mapChipTypeNext = mapChipData->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex + 1);
	if (mapChipType == MapChipType::Block && mapChipTypeNext != MapChipType::Block) {
		hit = true;
	}

	// 左下
	indexSet = mapChipData->GetMapChipIndexSetByPosition(positionsNew[static_cast<uint32_t>(Corner::kLeftBottom)]);
	mapChipType = mapChipData->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	mapChipTypeNext = mapChipData->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex + 1);
	if (mapChipType == MapChipType::Block && mapChipTypeNext != MapChipType::Block) {
		hit = true;
	}

	if (hit) {
		MapChip::IndexSet indexSetNow = mapChipData->GetMapChipIndexSetByPosition(worldTransform.translation_ + Vector3(0, -kHeight / 2.0f, 0));
		if (indexSetNow.yIndex != indexSet.yIndex) {
			indexSet = mapChipData->GetMapChipIndexSetByPosition(worldTransform.translation_ + info.move + Vector3(0, -kHeight / 2.0f, 0));
			MapChip::Rect rect = mapChipData->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
			info.move.y = my_min(0.0f, rect.top - worldTransform.translation_.y + (kHeight / 2.0f + kBlank));
			info.landing = true;
		}
	}
}

void Player::CheckMapCollisionRight(CollisionMapInfo info) {
	if (info.move.x <= 0) {
		return;
	}

	std::array<Vector3, static_cast<size_t>(Corner::kNumCorner)> positionsNew;
	for (uint32_t i = 0; i < positionsNew.size(); ++i) {
		positionsNew[i] = CornerPosition(worldTransform.translation_ + info.move, static_cast<Corner>(i));
	}

	MapChipType mapChipType;
	MapChipType mapChipTypeNext;
	bool hit = false;
	MapChip::IndexSet indexSet;

	// 右上
	indexSet = mapChipData->GetMapChipIndexSetByPosition(positionsNew[static_cast<uint32_t>(Corner::kRightTop)]);
	mapChipType = mapChipData->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	mapChipTypeNext = mapChipData->GetMapChipTypeByIndex(indexSet.xIndex - 1, indexSet.yIndex);
	if (mapChipType == MapChipType::Block && mapChipTypeNext != MapChipType::Block) {
		hit = true;
	}

	// 右下
	indexSet = mapChipData->GetMapChipIndexSetByPosition(positionsNew[static_cast<uint32_t>(Corner::kRightBottom)]);
	mapChipType = mapChipData->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	mapChipTypeNext = mapChipData->GetMapChipTypeByIndex(indexSet.xIndex - 1, indexSet.yIndex);
	if (mapChipType == MapChipType::Block && mapChipTypeNext != MapChipType::Block) {
		hit = true;
	}

	if (hit) {
		MapChip::IndexSet indexSetNow = mapChipData->GetMapChipIndexSetByPosition(worldTransform.translation_ + Vector3(kWidth / 2.0f, 0, 0));
		if (indexSetNow.xIndex != indexSet.xIndex) {
			indexSet = mapChipData->GetMapChipIndexSetByPosition(worldTransform.translation_ + info.move + Vector3(kWidth / 2.0f, 0, 0));
			MapChip::Rect rect = mapChipData->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
			info.move.x = my_max(0.0f, rect.left - worldTransform.translation_.x - (kWidth / 2.0f + kBlank));
			info.hitWall = true;
		}
	}
}

void Player::CheckMapCollisionLeft(CollisionMapInfo info) {
	if (info.move.x >= 0) {
		return;
	}

	std::array<Vector3, static_cast<size_t>(Corner::kNumCorner)> positionsNew;
	for (uint32_t i = 0; i < positionsNew.size(); ++i) {
		positionsNew[i] = CornerPosition(worldTransform.translation_ + info.move, static_cast<Corner>(i));
	}

	MapChipType mapChipType;
	MapChipType mapChipTypeNext;
	bool hit = false;
	MapChip::IndexSet indexSet;

	// 左上
	indexSet = mapChipData->GetMapChipIndexSetByPosition(positionsNew[static_cast<uint32_t>(Corner::kLeftTop)]);
	mapChipType = mapChipData->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	mapChipTypeNext = mapChipData->GetMapChipTypeByIndex(indexSet.xIndex + 1, indexSet.yIndex);
	if (mapChipType == MapChipType::Block && mapChipTypeNext != MapChipType::Block) {
		hit = true;
	}

	// 左下
	indexSet = mapChipData->GetMapChipIndexSetByPosition(positionsNew[static_cast<uint32_t>(Corner::kLeftBottom)]);
	mapChipType = mapChipData->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	mapChipTypeNext = mapChipData->GetMapChipTypeByIndex(indexSet.xIndex + 1, indexSet.yIndex);
	if (mapChipType == MapChipType::Block && mapChipTypeNext != MapChipType::Block) {
		hit = true;
	}

	if (hit) {
		MapChip::IndexSet indexSetNow = mapChipData->GetMapChipIndexSetByPosition(worldTransform.translation_ + Vector3(-kWidth / 2.0f, 0, 0));
		if (indexSetNow.xIndex != indexSet.xIndex) {
			indexSet = mapChipData->GetMapChipIndexSetByPosition(worldTransform.translation_ + info.move + Vector3(-kWidth / 2.0f, 0, 0));
			MapChip::Rect rect = mapChipData->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
			info.move.x = my_min(0.0f, rect.right - worldTransform.translation_.x + (kWidth / 2.0f + kBlank));
			info.hitWall = true;
		}
	}
}
