#pragma once
#include <KamataEngine.h>

class Player; // Player クラスの宣言 (前方宣言)

class FollowCamera {
public:
	// 初期化関数：追跡するプレイヤーへのポインタを受け取る
	void Initialize(const Player* targetPlayer);

	// 更新関数：毎フレーム呼び出され、カメラ位置を更新する
	void Update();

	// カメラへの参照を返すゲッター
	const KamataEngine::Camera& GetCamera() const { return camera_; }

private:
	KamataEngine::Camera camera_;
	const Player* targetPlayer_ = nullptr; // 追跡対象のプレイヤーへのポインタ

	// カメラのオフセット値
	const KamataEngine::Vector3 kOffset_ = {0.0f, 5.0f, -50.0f};

	// カメラの追従速度
	const float kFollowSpeed_ = 0.1f;
};