#include "Player.h"
#include "MapChip.h"
#include <algorithm>
#include <numbers>

using namespace std;
using namespace KamataEngine;
using namespace MathUtility;

void Player::Initialize(MapChip* mapChip) {
	// 通常モード用のモデルを読み込む
	normalModeModel = Model::CreateFromOBJ("player");

	// ゴーストモード用のモデルを読み込む（例: "ghostPlayer"）
	// 同じモデルを使う場合は、normalModeModelをそのまま使用
	ghostModeModel = Model::CreateFromOBJ("ghost");

	worldTransform.Initialize();
	worldTransform.scale_ = Vector3{1.5f, 1.5f, 1.5f};
	worldTransform.translation_ = Vector3{5.0f, 10.0f, 0.0f};
	worldTransform.rotation_.y = std::numbers::pi_v<float> / 2.0f;

	color = new ObjectColor();
	color->Initialize();
	color->SetColor(GhostModeColor);

	mapChipData = std::unique_ptr<MapChip>(mapChip);
}

void Player::Update() {
	if (Input::GetInstance()->PushKey(DIK_R))
		PosRestart();

	CheckKey();
	InputMove();

	CollisionMapInfo collisionMapInfo = {};
	collisionMapInfo.move = velocity_;
	collisionMapInfo.landing = false;
	collisionMapInfo.hitWall = false;
	collisionMapInfo.ceiling = false;

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

		// onGroundの更新
		UpdateOnGround(collisionMapInfo);
	} else {
		worldTransform.translation_ += velocity_;

		// ゴーストモード専用の重力を適用してゆっくり落下させる
		velocity_ += Vector3(0, -kGhostGravityAcceleration / 60.0f, 0);
		velocity_.y = my_max(velocity_.y, -kLimitFallSpeed); // 落下速度の最大値も適用
	}

	if (turnTimer_) {
		turnTimer_ = (std::max)(turnTimer_ - (1.0f / 60.0f), 0.0f);

		float destionRotationYtable[] = {std::numbers::pi_v<float> / 2.0f, std::numbers::pi_v<float> * 3.0f / 2.0f};
		float destiationRotationY = destionRotationYtable[static_cast<uint32_t>(lrDirection_)];

		float time = turnTimer_ / kTimeTurn_;

		float easeY = EaseInOut(destiationRotationY, turnFirstRotationY_, time);
		// if (isGhostMode)
		//	ghostWorldTransform.rotation_.y = easeY; // 削除
		// else
		//	worldTransform.rotation_.y = easeY;
		worldTransform.rotation_.y = easeY; // 常に worldTransform を更新
	}

#ifdef _DEBUG
	ImGui::Begin("player");
	ImGui::DragFloat3("rot", &worldTransform.translation_.x, 0.1f);
	ImGui::DragFloat3("pos", &worldTransform.translation_.x, 0.01f);
	ImGui::DragFloat3("velocity", &velocity_.x, 0.01f);
	ImGui::Checkbox("Ghost Mode", &isGhostMode);
	ImGui::Checkbox("On Ground", &onGround_);
	ImGui::Checkbox("isHitWall", &collisionMapInfo.hitWall);
	ImGui::Checkbox("isLanding", &collisionMapInfo.landing);
	ImGui::DragFloat3("move", &collisionMapInfo.move.x);
	ImGui::End();
#endif
	if (worldTransform.translation_.y < -10.0f) {
		PosRestart();
	}
	if (mapChipData->GetMapChipType(worldTransform.translation_) == MapChipType::Goal) {
		isGoal_ = true;
	}

	worldTransform.UpdateMatirx();
}

void Player::Draw(const Camera& camera) {
	// ゴーストモード時は半透明なモデルを描画
	if (isGhostMode) {
		ghostModeModel->Draw(worldTransform, camera, color);
	}
	// 通常モード時は通常のモデルを描画
	else {
		normalModeModel->Draw(worldTransform, camera);
	}
}

void Player::InputMove() {
	if (isPushGhostModeKey) {
		isGhostMode = !isGhostMode;
		if (isGhostMode) {
			velocity_.y = 0.0f;
		}
	}

	if (isGhostMode) {
		// ゴーストモード時の水平移動
		if (isPushRightMoveKey) {
			velocity_.x += kAttenuation;
			// ... (ターン処理)
		} else if (isPushLeftMoveKey) {
			velocity_.x -= kAttenuation;
			// ... (ターン処理)
		} else {
			velocity_.x *= (1.0f - kDeceleration);
		}
		velocity_.x = std::clamp(velocity_.x, -kLimitRusSpeed, kLimitRusSpeed);

	} else { // 通常モードの処理
		if (onGround_) {
			if (isPushRightMoveKey) {
				velocity_.x += kAttenuation;
				if (lrDirection_ != LRDirection::kRight) {
					lrDirection_ = LRDirection::kRight;
					turnTimer_ = kTimeTurn_;
					turnFirstRotationY_ = worldTransform.rotation_.y;
				}
			} else if (isPushLeftMoveKey) {
				velocity_.x -= kAttenuation;
				if (lrDirection_ != LRDirection::kLeft) {
					lrDirection_ = LRDirection::kLeft;
					turnTimer_ = kTimeTurn_;
					turnFirstRotationY_ = worldTransform.rotation_.y;
				}
			} else {
				velocity_.x *= (1.0f - kDeceleration);
			}

			velocity_.x = std::clamp(velocity_.x, -kLimitRusSpeed, kLimitRusSpeed);

			if (std::abs(velocity_.x) <= 0.01f) {
				velocity_.x = 0.0f;
			}
			// 通常モードでのジャンプ処理はここに残す
			if (isPushUpMoveKey) {
				velocity_.y += kJumpAcceleration / 60.0f;
			}
		} else {
			velocity_ += Vector3(0, -kGravityAcceleration / 60.0f, 0);
			velocity_.y = my_max(velocity_.y, -kLimitFallSpeed);
		}
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

void Player::UpdateOnGround(const CollisionMapInfo& info) {
	if (onGround_) {
		// ジャンプ開始
		if (velocity_.y > 0.0f) {
			onGround_ = false;
		} else {
			Vector3 newPos = worldTransform.translation_ + info.move;
			bool ground = false;

			// 右下点の判定
			Vector3 rightBottomPos = CornerPosition(newPos, Corner::kRightBottom);
			if (mapChipData->GetMapChipType(rightBottomPos + Vector3(0, -kGroundSearchHeight, 0)) == MapChipType::Block) {
				ground = true;
			}
			// 左下点の判定
			Vector3 leftBottomPos = CornerPosition(newPos, Corner::kLeftBottom);
			if (mapChipData->GetMapChipType(leftBottomPos + Vector3(0, -kGroundSearchHeight, 0)) == MapChipType::Block) {
				ground = true;
			}

			// 落下開始
			if (!ground) {
				onGround_ = false;
			}
		}
	} else {
		// 着地
		if (info.landing) {
			velocity_.x *= (1.0f - kAttenuationLanding);
			velocity_.y = 0.0f;
			onGround_ = true;
		}
	}
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

void Player::CheckMapCollision(CollisionMapInfo& info) {
	CheckMapCollisionUp(info);
	CheckMapCollisionDown(info);
	CheckMapCollisionRight(info);
	CheckMapCollisionLeft(info);
}

void Player::CheckMapCollisionUp(CollisionMapInfo& info) {
	if (info.move.y <= 0) {
		return;
	}

	Vector3 nextPos = worldTransform.translation_ + info.move;
	bool hit = false;
	MapChip::MapChipIndex indexSet;
	MapChip::Rect rect{};

	// 右上と左上の角の位置
	Vector3 rightTopCorner = CornerPosition(nextPos, Corner::kRightTop);
	Vector3 leftTopCorner = CornerPosition(nextPos, Corner::kLeftTop);

	// 右上点の判定
	indexSet = mapChipData->GetMapChipIndex(rightTopCorner);
	if (mapChipData->GetMapChipType(indexSet) == MapChipType::Block) {
		hit = true;
		rect = mapChipData->GetMapRect(rightTopCorner);
	}
	// 左上点の判定
	indexSet = mapChipData->GetMapChipIndex(leftTopCorner);
	if (mapChipData->GetMapChipType(indexSet) == MapChipType::Block) {
		hit = true;
		rect = mapChipData->GetMapRect(leftTopCorner);
	}

	if (hit) {
		info.move.y = my_max(0.0f, rect.bottom - (worldTransform.translation_.y + kHeight / 2.0f + kBlank));
		info.ceiling = true;
	}
}

void Player::CheckMapCollisionDown(CollisionMapInfo& info) {
	if (info.move.y >= 0) {
		return;
	}

	Vector3 nextPos = worldTransform.translation_ + info.move;
	bool hit = false;
	MapChip::MapChipIndex indexSet;
	MapChip::Rect rect{};

	// 右下と左下の角の位置
	Vector3 rightBottomCorner = CornerPosition(nextPos, Corner::kRightBottom);
	Vector3 leftBottomCorner = CornerPosition(nextPos, Corner::kLeftBottom);

	// 右下点の判定
	indexSet = mapChipData->GetMapChipIndex(rightBottomCorner);
	if (mapChipData->GetMapChipType(indexSet) == MapChipType::Block) {
		hit = true;
		rect = mapChipData->GetMapRect(rightBottomCorner);
	}
	// 左下点の判定
	indexSet = mapChipData->GetMapChipIndex(leftBottomCorner);
	if (mapChipData->GetMapChipType(indexSet) == MapChipType::Block) {
		hit = true;
		rect = mapChipData->GetMapRect(leftBottomCorner);
	}

	if (hit) {
		info.move.y = my_min(0.0f, rect.top - (worldTransform.translation_.y - kHeight / 2.0f - kBlank));
		info.landing = true;
	}
}

void Player::CheckMapCollisionRight(CollisionMapInfo& info) {
	if (info.move.x <= 0) {
		return;
	}

	Vector3 nextPos = worldTransform.translation_ + info.move;
	bool hit = false;
	MapChip::MapChipIndex indexSet;
	MapChip::Rect rect{};

	// 右上と右下の角の位置
	Vector3 rightTopCorner = CornerPosition(nextPos, Corner::kRightTop);
	Vector3 rightBottomCorner = CornerPosition(nextPos, Corner::kRightBottom);

	// 右上点の判定
	indexSet = mapChipData->GetMapChipIndex(rightTopCorner);
	if (mapChipData->GetMapChipType(indexSet) == MapChipType::Block) {
		hit = true;
		rect = mapChipData->GetMapRect(rightTopCorner);
	}
	// 右下点の判定
	indexSet = mapChipData->GetMapChipIndex(rightBottomCorner);
	if (mapChipData->GetMapChipType(indexSet) == MapChipType::Block) {
		hit = true;
		rect = mapChipData->GetMapRect(rightBottomCorner);
	}

	if (hit) {
		info.move.x = my_min(0.0f, rect.left - (worldTransform.translation_.x + kWidth / 2.0f + kBlank));
		info.hitWall = true;
	}
}

void Player::CheckMapCollisionLeft(CollisionMapInfo& info) {
	if (info.move.x >= 0) {
		return;
	}

	Vector3 nextPos = worldTransform.translation_ + info.move;
	bool hit = false;
	MapChip::MapChipIndex indexSet;
	MapChip::Rect rect{};

	// 左上と左下の角の位置
	Vector3 leftTopCorner = CornerPosition(nextPos, Corner::kLeftTop);
	Vector3 leftBottomCorner = CornerPosition(nextPos, Corner::kLeftBottom);

	// 左上点の判定
	indexSet = mapChipData->GetMapChipIndex(leftTopCorner);
	if (mapChipData->GetMapChipType(indexSet) == MapChipType::Block) {
		hit = true;
		rect = mapChipData->GetMapRect(leftTopCorner);
	}
	// 左下点の判定
	indexSet = mapChipData->GetMapChipIndex(leftBottomCorner);
	if (mapChipData->GetMapChipType(indexSet) == MapChipType::Block) {
		hit = true;
		rect = mapChipData->GetMapRect(leftBottomCorner);
	}

	if (hit) {
		info.move.x = my_max(0.0f, rect.right - (worldTransform.translation_.x - kWidth / 2.0f - kBlank));
		info.hitWall = true;
	}
}

void Player::PosRestart() { worldTransform.translation_ = {5.0f, 10.0f, 0.0f}; }
